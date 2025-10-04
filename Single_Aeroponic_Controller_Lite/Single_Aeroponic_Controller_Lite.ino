#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int c7Pin = 8;     // C7 solenoid (NO, LOW=open)
const int c8Pin = 9;     // C8 solenoid (NO, LOW=open)
const int c9Pin = 10;    // C9 solenoid (NO, LOW=open)
const int bypassPin = 11; // C6 bypass solenoid (NC, LOW=closed)
// Timing parameters (ms)
unsigned long c7OnDur = 60000;   // C7: 60s on
unsigned long c7OffDur = 900000; // C7: 900s off
unsigned long c8OnDur = 30000;   // C8: 30s on
unsigned long c8OffDur = 450000; // C8: 450s off
unsigned long c9OnDur = 120000;  // C9: 120s on
unsigned long c9OffDur = 1800000; // C9: 1800s off
const unsigned long transDelay = 2000; // 2s transition
const unsigned long relayStabilize = 10; // 10ms post-relay delay
// State variables
unsigned long c7Timer = 0, c8Timer = 0, c9Timer = 0;
bool c7Ready = true, c8Ready = true, c9Ready = true;
bool c7Irrigating = false, c8Irrigating = false, c9Irrigating = false;
bool bypassClosed = false;
int currentTank = -1; // -1=Idle, 0=C7, 1=C8, 2=C9
unsigned long transTimer = 0;
void setup() {
// Serial.begin(9600); // Uncomment for debugging
wdt_enable(WDTO_8S); // Enable 8s watchdog
lcd.init(); lcd.backlight();
// Initialize pins
pinMode(c7Pin, OUTPUT); digitalWrite(c7Pin, HIGH); // C7 closed
pinMode(c8Pin, OUTPUT); digitalWrite(c8Pin, HIGH); // C8 closed
pinMode(c9Pin, OUTPUT); digitalWrite(c9Pin, HIGH); // C9 closed
pinMode(bypassPin, OUTPUT); digitalWrite(bypassPin, HIGH); // Bypass open (NC)
// Load params from EEPROM
EEPROM.get(0, c7OnDur); EEPROM.get(4, c7OffDur);
EEPROM.get(8, c8OnDur); EEPROM.get(12, c8OffDur);
EEPROM.get(16, c9OnDur); EEPROM.get(20, c9OffDur);
// Initialize LCD
lcd.setCursor(0, 0); lcd.print("R&D Controller");
lcd.setCursor(0, 1); lcd.print("Idle            ");
}
void updateLCD(int tank) {
if (tank != currentTank) {
lcd.setCursor(0, 1); lcd.print("                ");
lcd.setCursor(0, 1);
if (tank == -1) {
lcd.print("Idle            ");
} else if (tank == 0) {
lcd.print("C7 - Irrigation ");
} else if (tank == 1) {
lcd.print("C8 - Irrigation ");
} else if (tank == 2) {
lcd.print("C9 - Irrigation ");
}
currentTank = tank;
// Serial.print("LCD: "); Serial.println(tank == -1 ? "Idle" : tank == 0 ? "C7" : tank == 1 ? "C8" : "C9");
}
}
void loop() {
wdt_reset();
unsigned long now = millis();
// Update tank states
if (c7Irrigating && now >= c7Timer) {
c7Irrigating = false;
if (!c8Irrigating && !c9Irrigating) {
digitalWrite(bypassPin, HIGH);
delay(relayStabilize);
transTimer = now + transDelay;
bypassClosed = false;
// Serial.println("Bypass: Open");
}
digitalWrite(c7Pin, HIGH);
delay(relayStabilize);
c7Timer = now + c7OffDur;
c7Ready = false;
updateLCD(-1);
} else if (!c7Irrigating && !c7Ready && now >= c7Timer) {
c7Ready = true;
}
if (c8Irrigating && now >= c8Timer) {
c8Irrigating = false;
if (!c7Irrigating && !c9Irrigating) {
digitalWrite(bypassPin, HIGH);
delay(relayStabilize);
transTimer = now + transDelay;
bypassClosed = false;
// Serial.println("Bypass: Open");
}
digitalWrite(c8Pin, HIGH);
delay(relayStabilize);
c8Timer = now + c8OffDur;
c8Ready = false;
updateLCD(-1);
} else if (!c8Irrigating && !c8Ready && now >= c8Timer) {
c8Ready = true;
}
if (c9Irrigating && now >= c9Timer) {
c9Irrigating = false;
if (!c7Irrigating && !c8Irrigating) {
digitalWrite(bypassPin, HIGH);
delay(relayStabilize);
transTimer = now + transDelay;
bypassClosed = false;
// Serial.println("Bypass: Open");
}
digitalWrite(c9Pin, HIGH);
delay(relayStabilize);
c9Timer = now + c9OffDur;
c9Ready = false;
updateLCD(-1);
} else if (!c9Irrigating && !c9Ready && now >= c9Timer) {
c9Ready = true;
}
// Start next irrigation if none active and transition complete
if (!c7Irrigating && !c8Irrigating && !c9Irrigating && now >= transTimer) {
if (c7Ready && !bypassClosed) {
digitalWrite(c7Pin, LOW);
delay(relayStabilize);
transTimer = now + transDelay;
digitalWrite(bypassPin, LOW);
delay(relayStabilize);
c7Irrigating = true;
c7Timer = now + transDelay + c7OnDur;
c7Ready = false;
bypassClosed = true;
updateLCD(0);
// Serial.println("C7: Irrigating");
} else if (c8Ready && !bypassClosed) {
digitalWrite(c8Pin, LOW);
delay(relayStabilize);
transTimer = now + transDelay;
digitalWrite(bypassPin, LOW);
delay(relayStabilize);
c8Irrigating = true;
c8Timer = now + transDelay + c8OnDur;
c8Ready = false;
bypassClosed = true;
updateLCD(1);
// Serial.println("C8: Irrigating");
} else if (c9Ready && !bypassClosed) {
digitalWrite(c9Pin, LOW);
delay(relayStabilize);
transTimer = now + transDelay;
digitalWrite(bypassPin, LOW);
delay(relayStabilize);
c9Irrigating = true;
c9Timer = now + transDelay + c9OnDur;
c9Ready = false;
bypassClosed = true;
updateLCD(2);
// Serial.println("C9: Irrigating");
}
}
delay(100); // Throttle loop
}