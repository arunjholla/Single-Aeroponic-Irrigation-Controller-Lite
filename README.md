# R&D Aeroponic Controller Lite
A lightweight irrigation controller for three R&D aeroponic tanks (C7, C8, C9) with one water storage tank (C6), designed for AgResearch Labs' Growize platform in Bangalore, India. Manages staggered irrigation cycles with an always-on pump and solenoid valves to test varying irrigation durations.

## Overview
- **Purpose**: Control irrigation for three R&D tanks (C7, C8, C9) with a bypass to C6 storage tank, ensuring no simultaneous irrigation.
- **Hardware**: Arduino Uno, 4x single-channel relays, 16x2 LCD I2C, EEPROM.
- **Irrigation Cycles**:
  - C7: 60s on, 900s off
  - C8: 30s on, 450s off
  - C9: 120s on, 1800s off
- **Safety**: Bypass solenoid (C6) wired normally closed (NC) to ensure water flow to storage during power failure or system hang.
- **Build Cost**: ~₹800 (Bangalore, India).

## Features
- Staggered irrigation to prevent overlaps using priority queuing (C7 > C8 > C9).
- Programmable cycles stored in EEPROM for power failure recovery.
- Solenoid control:
  - C7, C8, C9: Normally open (NO) relays (LOW = open).
  - C6 bypass: Normally closed (NC) relay (LOW = closed).
- 2s transition delays for smooth solenoid switching.
- LCD display for real-time status (tank irrigating or idle).
- Watchdog timer to prevent system hangs.
- Optional: Future MQTT integration for Growize dashboard.

## Hardware Setup
- **Arduino Uno**: Core controller.
- **Relays**:
  - Pin 8: C7 solenoid (NO, LOW = open)
  - Pin 9: C8 solenoid (NO, LOW = open)
  - Pin 10: C9 solenoid (NO, LOW = open)
  - Pin 11: C6 bypass solenoid (NC, LOW = closed)
- **LCD**: 16x2 I2C (address 0x27, pins A4/SDA, A5/SCL).
- **Power**: 5V adapter (recommend UPS for Bangalore power stability).
- **Optional Sensors**: HC-SR04 (tank level), DHT22 (humidity/temp) for future expansion.

## Software
- **Platform**: Arduino IDE.
- **Libraries**: `EEPROM`, `LiquidCrystal_I2C`, `avr/wdt`.
- **Logic**:
  1. Check timers (millis-based) for each tank’s on/off state.
  2. Mark tanks ready after off period completes.
  3. If no tank irrigating, start highest-priority ready tank (C7 > C8 > C9):
     - Open tank solenoid (LOW), wait 2s, close bypass (LOW).
     - After on-duration: Open bypass (HIGH, if no other tanks on), wait 2s, close tank solenoid (HIGH).
     - Start off-timer.
  4. Bypass open (HIGH) when no tanks irrigating; closed (LOW) during irrigation.
- **Configuration**: Durations stored in EEPROM; update via serial or reflash (not implemented in base sketch).

## Installation
1. Connect hardware per pin assignments.
2. Upload code using Arduino IDE.
3. Flash provided sketch (`rnd_aeroponic_controller_lite.ino`).
4. Monitor via LCD or serial (9600 baud).
5. Test solenoid wiring (NO for tanks, NC for bypass) and timing.

## Usage
- Power on: Initializes, loads params from EEPROM, sets bypass open (NC default).
- LCD shows "R&D Controller" and status ("C7 Irrigating", "Idle", etc.).
- Cycles run automatically with no overlaps; priority ensures shorter cycles (C7/C8) run promptly.
- Adjust params via serial or reflash EEPROM if needed.

## Future Enhancements
- Add SD card logging for irrigation events (requires pin adjustments).
- Add sensors (e.g., tank level, humidity) via I2C to avoid pin constraints.
- Integrate MQTT/ESP8266 for Growize dashboard telemetry.
- Add button interface for on-device param changes.

## Notes
- Test solenoid wiring (NO/NC) and timing in Bangalore’s humid conditions.
- Use UPS for power stability; bypass NC ensures fail-safe water return.
- `millis()` sufficient for short-term cycles; reset every ~50 days or add overflow handling for long runs.
- Contact AgResearch Labs for Growize integration details.

## License
Proprietary - AgResearch Labs, Bangalore, India.