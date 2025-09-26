# R&D Aeroponic Controller Lite
A lightweight irrigation controller for a single R&D aeroponic tank, designed for AgResearch Labs' Growize platform. Manages a single tank with an always-on pump and two solenoid valves (R&D and bypass) for precise irrigation cycles.

## Overview
- **Purpose**: Control irrigation for one R&D tank adjacent to a water storage tank.
- **Hardware**: Arduino Uno, 2x single-channel relays, 16x2 LCD I2C, EEPROM.
- **Cycle**: 60s irrigation, 15min off, with 2s transition delays.
- **Safety**: Bypass solenoid wired to normally closed (NC) relay to ensure water flow during power failure or system hang.
- **Build Cost**: ~₹800 (Bangalore, India).

## Features
- Programmable irrigation: 60s on, 15min off (adjustable via serial/EEPROM).
- Dual solenoid control:
  - R&D solenoid: Normally open (NO) relay wiring.
  - Bypass solenoid: Normally closed (NC) for fail-safe operation.
- LCD display for real-time status.
- Watchdog timer to prevent hangs.
- EEPROM for parameter persistence.
- Optional: MQTT integration for Growize dashboard.

## Hardware Setup
- **Arduino Uno**: Core controller.
- **Relays**:
  - Pin 8: R&D solenoid (NO, LOW = open).
  - Pin 9: Bypass solenoid (NC, LOW = closed).
- **LCD**: 16x2 I2C (address 0x27).
- **Power**: 5V adapter (recommend UPS for Bangalore power stability).
- **Optional Sensors**: HC-SR04 (tank level), DHT22 (humidity/temp).

## Software
- **Platform**: Arduino IDE.
- **Libraries**: `EEPROM`, `LiquidCrystal_I2C`, `avr/wdt`.
- **Logic**:
  1. Set R&D pin LOW (open valve).
  2. Wait 2s, set bypass pin LOW (close valve).
  3. Wait 60s (irrigation).
  4. Set bypass pin HIGH (open valve).
  5. Wait 2s, set R&D pin HIGH (close valve).
  6. Wait 15min, repeat.
- **Configuration**: Update durations via serial or EEPROM.

## Installation
1. Connect hardware as per pin assignments.
2. Upload code using Arduino IDE.
3. Flash provided sketch (`rnd_controller_lite.ino`).
4. Monitor via LCD or serial (9600 baud).
5. Test solenoid timing and power backup.

## Usage
- Power on: System initializes, loads params from EEPROM.
- LCD shows "R&D Controller" and cycle status ("Irrigating..." or "Waiting...").
- Adjust cycle params via serial commands (TBD) or reflash EEPROM.
- Ensure bypass solenoid defaults to open on power loss.

## Future Enhancements
- Add sensor support (e.g., tank level, humidity).
- Integrate MQTT for Growize dashboard telemetry.
- Add button interface for on-device param changes.

## Notes
- Test solenoid wiring (NO/NC) before deployment.
- Account for Bangalore’s humidity and power fluctuations.
- Contact AgResearch Labs for Growize integration details.

## License
Proprietary - AgResearch Labs, Bangalore, India.
