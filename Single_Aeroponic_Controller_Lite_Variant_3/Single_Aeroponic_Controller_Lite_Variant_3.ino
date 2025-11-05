#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Section pins (NO, LOW=open)
const int c7Section1Pin = 8;
const int c7Section2Pin = 7;
const int c8Section1Pin = 9;
const int c8Section2Pin = 6;
const int c9Section1Pin = 10;
const int c9Section2Pin = 5;
const int bypassPin = 11; // C6 bypass solenoid (NC, LOW=closed)
// Timing parameters (ms)
unsigned long c7OnDur = 30000; // C7: 30s on
unsigned long c7OffDur = 210000; // C7: 210s off
unsigned long c8OnDur = 60000; // C8: 60s on
unsigned long c8OffDur = 420000; // C8: 420s off
unsigned long c9OnDur = 120000; // C9: 120s on
unsigned long c9OffDur = 840000; // C9: 840s off
const unsigned long transDelay = 2000; // 2s transition
const unsigned long overlapDelay = 2000; // 2s overlap
const unsigned long relayStabilize = 10; // 10ms post-relay delay
// State variables
unsigned long c7Timer = 0, c8Timer = 0, c9Timer = 0;
bool c7Ready = true, c8Ready = true, c9Ready = true;
bool c7Irrigating = false, c8Irrigating = false, c9Irrigating = false;
bool bypassClosed = false;
int currentTank = -1; // -1=Idle, 0=C7, 1=C8, 2=C9
unsigned long transTimer = 0;
unsigned long sectionTimer = 0;
unsigned long halfOnDur = 0;
enum IrrigationState { IDLE, SECTION1, SWITCHING, SECTION2 };
IrrigationState irrState = IDLE;
void setup() {
  Serial.begin(9600); // Enable Serial Monitor for debugging
  Serial.println("R&D Controller Starting...");
  lcd.init(); lcd.backlight();
  pinMode(c7Section1Pin, OUTPUT); digitalWrite(c7Section1Pin, HIGH);
  pinMode(c7Section2Pin, OUTPUT); digitalWrite(c7Section2Pin, HIGH);
  pinMode(c8Section1Pin, OUTPUT); digitalWrite(c8Section1Pin, HIGH);
  pinMode(c8Section2Pin, OUTPUT); digitalWrite(c8Section2Pin, HIGH);
  pinMode(c9Section1Pin, OUTPUT); digitalWrite(c9Section1Pin, HIGH);
  pinMode(c9Section2Pin, OUTPUT); digitalWrite(c9Section2Pin, HIGH);
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
  // Handle section transitions if a tank is irrigating
  if (currentTank != -1) {
    int s1Pin, s2Pin;
    switch (currentTank) {
      case 0: s1Pin = c7Section1Pin; s2Pin = c7Section2Pin; break;
      case 1: s1Pin = c8Section1Pin; s2Pin = c8Section2Pin; break;
      case 2: s1Pin = c9Section1Pin; s2Pin = c9Section2Pin; break;
      default: return;
    }
    if (irrState == SECTION1 && now >= sectionTimer) {
      digitalWrite(s2Pin, LOW);
      delay(relayStabilize);
      irrState = SWITCHING;
      sectionTimer = now + overlapDelay;
      Serial.print("Tank "); Serial.print(currentTank); Serial.println(": Starting overlap");
    } else if (irrState == SWITCHING && now >= sectionTimer) {
      digitalWrite(s1Pin, HIGH);
      delay(relayStabilize);
      irrState = SECTION2;
      sectionTimer = now + halfOnDur;
      Serial.print("Tank "); Serial.print(currentTank); Serial.println(": Switching to Section 2");
    }
  }
  // Check for end of irrigation
  if (c7Irrigating && now >= c7Timer) {
    endIrrigation(0);
    stateChanged = true;
  } else if (!c7Irrigating && !c7Ready && now >= c7Timer) {
    c7Ready = true;
    stateChanged = true;
    Serial.println("C7: Ready");
  }
  if (c8Irrigating && now >= c8Timer) {
    endIrrigation(1);
    stateChanged = true;
  } else if (!c8Irrigating && !c8Ready && now >= c8Timer) {
    c8Ready = true;
    stateChanged = true;
    Serial.println("C8: Ready");
  }
  if (c9Irrigating && now >= c9Timer) {
    endIrrigation(2);
    stateChanged = true;
  } else if (!c9Irrigating && !c9Ready && now >= c9Timer) {
    c9Ready = true;
    stateChanged = true;
    Serial.println("C9: Ready");
  }
  // Start next irrigation if none active and transition complete
  if (!c7Irrigating && !c8Irrigating && !c9Irrigating && now >= transTimer) {
    bool started = false;
    if (c7Ready && !bypassClosed) {
      startIrrigation(0, c7OnDur);
      started = true;
    } else if (c8Ready && !bypassClosed) {
      startIrrigation(1, c8OnDur);
      started = true;
    } else if (c9Ready && !bypassClosed) {
      startIrrigation(2, c9OnDur);
      started = true;
    }
  }
  delay(100);
}
void startIrrigation(int tank, unsigned long onDur) {
  int s1Pin, s2Pin;
  unsigned long *timerPtr;
  bool *irrigatingPtr;
  bool *readyPtr;
  switch (tank) {
    case 0: s1Pin = c7Section1Pin; s2Pin = c7Section2Pin; timerPtr = &c7Timer; irrigatingPtr = &c7Irrigating; readyPtr = &c7Ready; break;
    case 1: s1Pin = c8Section1Pin; s2Pin = c8Section2Pin; timerPtr = &c8Timer; irrigatingPtr = &c8Irrigating; readyPtr = &c8Ready; break;
    case 2: s1Pin = c9Section1Pin; s2Pin = c9Section2Pin; timerPtr = &c9Timer; irrigatingPtr = &c9Irrigating; readyPtr = &c9Ready; break;
    default: return;
  }
  halfOnDur = onDur / 2;
  unsigned long now = millis();
  digitalWrite(s1Pin, LOW);
  delay(relayStabilize);
  transTimer = now + transDelay;
  digitalWrite(bypassPin, LOW);
  delay(relayStabilize);
  *irrigatingPtr = true;
  irrState = SECTION1;
  sectionTimer = now + transDelay + halfOnDur;
  *timerPtr = now + transDelay + onDur + overlapDelay;
  *readyPtr = false;
  bypassClosed = true;
  updateLCD(tank);
  Serial.print("Tank "); Serial.print(tank); Serial.println(": Starting Section 1");
}
void endIrrigation(int tank) {
  int s1Pin, s2Pin;
  unsigned long *timerPtr;
  bool *irrigatingPtr;
  unsigned long offDur;
  switch (tank) {
    case 0: s1Pin = c7Section1Pin; s2Pin = c7Section2Pin; timerPtr = &c7Timer; irrigatingPtr = &c7Irrigating; offDur = c7OffDur; break;
    case 1: s1Pin = c8Section1Pin; s2Pin = c8Section2Pin; timerPtr = &c8Timer; irrigatingPtr = &c8Irrigating; offDur = c8OffDur; break;
    case 2: s1Pin = c9Section1Pin; s2Pin = c9Section2Pin; timerPtr = &c9Timer; irrigatingPtr = &c9Irrigating; offDur = c9OffDur; break;
    default: return;
  }
  unsigned long now = millis();
  *irrigatingPtr = false;
  irrState = IDLE;
  digitalWrite(s1Pin, HIGH);
  digitalWrite(s2Pin, HIGH);
  delay(relayStabilize);
  if (!c7Irrigating && !c8Irrigating && !c9Irrigating) {
    digitalWrite(bypassPin, HIGH);
    delay(relayStabilize);
    transTimer = now + transDelay;
    bypassClosed = false;
    Serial.println("Bypass: Open");
  }
  *timerPtr = now + offDur;
  updateLCD(-1);
  Serial.print("Tank "); Serial.print(tank); Serial.println(": Irrigation Ended");
}