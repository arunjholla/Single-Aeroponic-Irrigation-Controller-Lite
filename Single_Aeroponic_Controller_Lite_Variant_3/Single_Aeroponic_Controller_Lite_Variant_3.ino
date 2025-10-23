#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int c7Pin = 8; // C7 solenoid (NO, LOW=open)
const int c8Pin = 9; // C8 solenoid (NO, LOW=open)
const int c9Pin = 10; // C9 solenoid (NO, LOW=open)
const int bypassPin = 11; // C6 bypass solenoid (NC, LOW=closed)
// Timing parameters (ms)
unsigned long c7OnDur = 30000; // C7: 30s on
unsigned long c7OffDur = 210000; // C7: 210s off
unsigned long c8OnDur = 60000; // C8: 60s on
unsigned long c8OffDur = 420000; // C8: 420s off
unsigned long c9OnDur = 120000; // C9: 120s on
unsigned long c9OffDur = 840000; // C9: 840s off
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
  Serial.begin(9600); // Enable Serial Monitor for debugging
  Serial.println("R&D Controller Starting...");
  lcd.init(); lcd.backlight();
  pinMode(c7Pin, OUTPUT); digitalWrite(c7Pin, HIGH);
  pinMode(c8Pin, OUTPUT); digitalWrite(c8Pin, HIGH);
  pinMode(c9Pin, OUTPUT); digitalWrite(c9Pin, HIGH);
  pinMode(bypassPin, OUTPUT); digitalWrite(bypassPin, HIGH);
  lcd.setCursor(0, 0); lcd.print("R&D Controller");
  updateLCD(currentTank);
}
void updateLCD(int tank) {
  if (tank != currentTank) {
    lcd.setCursor(0, 1); lcd.print("                ");
    lcd.setCursor(0, 1);
    if (tank == -1) {
      lcd.print("Idle");
      Serial.println("LCD: Idle");
    } else if (tank == 0) {
      lcd.print("C7 - Irrigation");
      Serial.println("LCD: C7 - Irrigation");
    } else if (tank == 1) {
      lcd.print("C8 - Irrigation");
      Serial.println("LCD: C8 - Irrigation");
    } else if (tank == 2) {
      lcd.print("C9 - Irrigation");
      Serial.println("LCD: C9 - Irrigation");
    }
    currentTank = tank;
  }
}
void loop() {
  unsigned long now = millis();
  bool stateChanged = false;
  if (c7Irrigating && now >= c7Timer) {
    c7Irrigating = false;
    stateChanged = true;
    if (!c8Irrigating && !c9Irrigating) {
      digitalWrite(bypassPin, HIGH);
      delay(relayStabilize);
      transTimer = now + transDelay;
      bypassClosed = false;
      Serial.println("Bypass: Open");
    }
    digitalWrite(c7Pin, HIGH);
    delay(relayStabilize);
    c7Timer = now + c7OffDur;
    c7Ready = false;
    updateLCD(-1);
  } else if (!c7Irrigating && !c7Ready && now >= c7Timer) {
    c7Ready = true;
    stateChanged = true;
    Serial.println("C7: Ready");
  }
  if (c8Irrigating && now >= c8Timer) {
    c8Irrigating = false;
    stateChanged = true;
    if (!c7Irrigating && !c9Irrigating) {
      digitalWrite(bypassPin, HIGH);
      delay(relayStabilize);
      transTimer = now + transDelay;
      bypassClosed = false;
      Serial.println("Bypass: Open");
    }
    digitalWrite(c8Pin, HIGH);
    delay(relayStabilize);
    c8Timer = now + c8OffDur;
    c8Ready = false;
    updateLCD(-1);
  } else if (!c8Irrigating && !c8Ready && now >= c8Timer) {
    c8Ready = true;
    stateChanged = true;
    Serial.println("C8: Ready");
  }
  if (c9Irrigating && now >= c9Timer) {
    c9Irrigating = false;
    stateChanged = true;
    if (!c7Irrigating && !c8Irrigating) {
      digitalWrite(bypassPin, HIGH);
      delay(relayStabilize);
      transTimer = now + transDelay;
      bypassClosed = false;
      Serial.println("Bypass: Open");
    }
    digitalWrite(c9Pin, HIGH);
    delay(relayStabilize);
    c9Timer = now + c9OffDur;
    c9Ready = false;
    updateLCD(-1);
  } else if (!c9Irrigating && !c9Ready && now >= c9Timer) {
    c9Ready = true;
    stateChanged = true;
    Serial.println("C9: Ready");
  }
  if (!c7Irrigating && !c8Irrigating && !c9Irrigating && now >= transTimer) {
    bool started = false;
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
      Serial.println("C7: Irrigating");
      started = true;
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
      Serial.println("C8: Irrigating");
      started = true;
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
      Serial.println("C9: Irrigating");
      started = true;
    }
  }
  delay(100);
}