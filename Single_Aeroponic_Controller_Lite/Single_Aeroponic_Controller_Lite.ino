#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int pins[3] = {8, 9, 10}; // C7, C8, C9 solenoids (NO, LOW=open)
const int bypassPin = 11;       // C6 bypass solenoid (NC, LOW=closed)
unsigned long onDurs[3] = {60000, 30000, 120000}; // ms: C7=60s, C8=30s, C9=120s
unsigned long offDurs[3] = {900000, 450000, 1800000}; // ms: C7=900s, C8=450s, C9=1800s
unsigned long timers[3] = {0, 0, 0}; // End times for current state
bool ready[3] = {true, true, true}; // Ready to irrigate
bool irrigating[3] = {false, false, false}; // Irrigation state
const int transDelay = 2000; // Transition delay (ms)

void setup() {
  wdt_enable(WDTO_8S); // 8s watchdog
  lcd.init(); lcd.backlight();
  for(int i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], HIGH); // Solenoids closed
  }
  pinMode(bypassPin, OUTPUT);
  digitalWrite(bypassPin, HIGH); // Bypass open (NC default)
  
  // Load params from EEPROM
  EEPROM.get(0, onDurs);
  EEPROM.get(12, offDurs);
  
  lcd.setCursor(0, 0); lcd.print("R&D Controller");
}

void loop() {
  wdt_reset();
  unsigned long now = millis();
  bool anyOn = irrigating[0] || irrigating[1] || irrigating[2];

  // Update tank states
  for(int i = 0; i < 3; i++) {
    if(irrigating[i] && now >= timers[i]) {
      irrigating[i] = false;
      if(!anyOn) { // Last tank finishing
        digitalWrite(bypassPin, HIGH); // Open bypass
        delay(transDelay);
      }
      digitalWrite(pins[i], HIGH); // Close tank solenoid
      timers[i] = now + offDurs[i]; // Start off period
      ready[i] = false;
    } else if(!irrigating[i] && !ready[i] && now >= timers[i]) {
      ready[i] = true; // Off period done
    }
  }

  // Start next irrigation if none active
  if(!anyOn) {
    for(int i = 0; i < 3; i++) { // Priority: C7 > C8 > C9
      if(ready[i]) {
        digitalWrite(pins[i], LOW); // Open tank solenoid
        delay(transDelay);
        digitalWrite(bypassPin, LOW); // Close bypass
        irrigating[i] = true;
        timers[i] = now + onDurs[i] + transDelay; // Include delay
        ready[i] = false;
        lcd.setCursor(0, 1); lcd.print("C"); lcd.print(7+i); lcd.print(" Irrigating ");
        break;
      }
    }
  }

  // Update LCD
  if(!anyOn) {
    lcd.setCursor(0, 1); lcd.print("Idle            ");
  }
  delay(100); // Throttle loop
}