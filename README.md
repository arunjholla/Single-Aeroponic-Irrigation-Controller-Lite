Irrigation Controller Project
This Arduino-based irrigation controller manages three tanks (C7, C8, C9) with solenoid valves in Bangalore, India. It controls irrigation cycles with specific on (irrigation) and off (idle) durations, displays status on an LCD, and uses a bypass solenoid for system control. Three variants are implemented with different irrigation-to-idle ratios for testing and optimization.
Hardware Requirements

Arduino board (e.g., Uno)
LiquidCrystal_I2C LCD (16x2, address 0x27)
Solenoids: C7, C8, C9 (normally open, LOW=open), C6 bypass (normally closed, LOW=closed)
Pins: C7 (8), C8 (9), C9 (10), Bypass (11)
Relay module for solenoid control

Features

Manages irrigation cycles for three tanks with configurable on/off durations
Displays status (Idle, C7, C8, or C9 irrigation) on a 16x2 LCD
2-second transition delay between cycles
10ms relay stabilization delay
Serial Monitor debugging for state transitions

Variants
The project includes three variants with different irrigation-to-idle ratios for tanks C7, C8, and C9.
Variant 1: Ratio 1:15

C7: 60s on, 900s off (1:15 ratio)
C8: 30s on, 450s off (1:15 ratio)
C9: 120s on, 1800s off (1:15 ratio)
File: original.ino

Variant 2: Ratio 1:3

C7: 30s on, 90s off (1:3 ratio)
C8: 60s on, 180s off (1:3 ratio)
C9: 120s on, 360s off (1:3 ratio)
File: variant1.ino

Variant 3: Ratio 1:7

C7: 30s on, 210s off (1:7 ratio)
C8: 60s on, 420s off (1:7 ratio)
C9: 120s on, 840s off (1:7 ratio)
File: variant2.ino

Notes

EEPROM and watchdog timer were removed from Variants 2 and 3 to address system hang issues during bypass solenoid actuation.
Serial Monitor (9600 baud) is enabled in Variants 2 and 3 for debugging state changes (e.g., tank irrigation, bypass status).
SD card logging is planned for future implementation to replace EEPROM functionality.
All timings are in milliseconds.

Setup

Connect the LCD and solenoids to the specified pins.
Upload the desired variant's .ino file to the Arduino.
Open Serial Monitor (9600 baud) for debugging (Variants 2 and 3).
Monitor the LCD for real-time status updates.
