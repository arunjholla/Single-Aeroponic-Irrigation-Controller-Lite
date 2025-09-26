#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>  // Watchdog

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int rndPin = 8;    // R&D solenoid relay
const int bypassPin = 9; // Bypass solenoid relay
int irrDuration = 60;    // Seconds
int irrInterval = 900;   // Seconds (15 min)
int transDelay = 2;      // Transition delay seconds

void setup() {
  wdt_enable(WDTO_8S);  // 8s watchdog
  lcd.init(); lcd.backlight();
  pinMode(rndPin, OUTPUT); digitalWrite(rndPin, HIGH);  // Default: closed
  pinMode(bypassPin, OUTPUT); digitalWrite(bypassPin, HIGH);  // Default: open
  EEPROM.get(0, irrDuration); EEPROM.get(4, irrInterval); EEPROM.get(8, transDelay);
  lcd.print("R&D Controller");
}

void loop() {
  wdt_reset();
  lcd.setCursor(0,1); lcd.print("Irrigating...");
  digitalWrite(rndPin, LOW);          // Open R&D
  delay(transDelay * 1000);
  digitalWrite(bypassPin, LOW);       // Close bypass
  delay(irrDuration * 1000);
  digitalWrite(bypassPin, HIGH);      // Open bypass
  delay(transDelay * 1000);
  digitalWrite(rndPin, HIGH);         // Close R&D
  lcd.setCursor(0,1); lcd.print("Waiting...");
  delay(irrInterval * 1000);
}