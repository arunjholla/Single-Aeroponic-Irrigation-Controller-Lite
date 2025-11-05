Irrigation Controller Project
This Arduino-based irrigation controller manages three tanks (C7, C8, C9) with dual-section solenoids per tank in Bangalore, India. It controls irrigation cycles, splits on-time between sections with 2s overlap, displays status on an LCD, and uses a bypass solenoid. Three versions are implemented with different irrigation-to-idle ratios.
Hardware Requirements

Arduino board (e.g., Uno)
LiquidCrystal_I2C LCD (16x2, address 0x27)
Solenoids: C7 sections (pins 8,7), C8 (9,6), C9 (10,5) (NO, LOW=open); C6 bypass (11, NC, LOW=closed)
Relay module for solenoid control

Features

Manages irrigation for three tanks with configurable on/off durations
Splits tank irrigation: Section 1 (half on-time), 2s overlap (both open), Section 2 (half on-time)
Displays status (Idle, C7, C8, or C9 irrigation) on LCD
2s transition delay between cycles
10ms relay stabilization delay
Serial Monitor debugging for state transitions (e.g., sections, overlaps, bypass)

Versions

Version 1: Ratio 1:15

C7: 60s on (30s/sec + overlap), 900s off
C8: 30s on (15s/sec + overlap), 450s off
C9: 120s on (60s/sec + overlap), 1800s off
File: Single_Aeroponic_Controller_Lite_Variant_1.ino


Version 2: Ratio 1:3

C7: 30s on (15s/sec + overlap), 90s off
C8: 60s on (30s/sec + overlap), 180s off
C9: 120s on (60s/sec + overlap), 360s off
File: Single_Aeroponic_Controller_Lite_Variant_2.ino


Version 3: Ratio 1:7

C7: 30s on (15s/sec + overlap), 210s off
C8: 60s on (30s/sec + overlap), 420s off
C9: 120s on (60s/sec + overlap), 840s off
File: Single_Aeroponic_Controller_Lite_Variant_3.ino



Notes

EEPROM and watchdog timer removed to prevent hangs during solenoid actuation
Serial Monitor (9600 baud) enabled for debugging
SD card logging planned for future
All timings in milliseconds; total irrigation phase: onDur + 2000ms (overlap)

Setup

Connect LCD and solenoids to specified pins.
Upload desired version's .ino file to Arduino.
Open Serial Monitor for debugging.
Monitor LCD for status.
