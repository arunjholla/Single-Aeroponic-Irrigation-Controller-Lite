\# R\&D Aeroponic Controller Lite

A lightweight, non-blocking irrigation controller for three R\&D aeroponic tanks (C7, C8, C9) with one water storage tank (C6), designed for AgResearch Labs' Growize platform in Bangalore, India. Manages staggered irrigation cycles with an always-on pump and solenoid valves to test varying irrigation durations, with full state persistence for power resume.



\## Overview

\- \*\*Purpose\*\*: Control irrigation for three R\&D tanks (C7, C8, C9) with a bypass to C6 storage tank, ensuring no simultaneous irrigation.

\- \*\*Hardware\*\*: Arduino Uno, 4x single-channel relays, 16x2 LCD I2C, EEPROM.

\- \*\*Irrigation Cycles\*\*:

&nbsp; - C7: 60s on, 900s off

&nbsp; - C8: 30s on, 450s off

&nbsp; - C9: 120s on, 1800s off

\- \*\*Safety\*\*: Bypass solenoid (C6) wired normally closed (NC) to ensure water flow to storage during power failure or system hang.

\- \*\*Build Cost\*\*: ~₹800 (Bangalore, India).



\## Features

\- Non-blocking `millis()`-based timing for robust operation and power resume.

\- Staggered irrigation to prevent overlaps using priority queuing (C7 > C8 > C9).

\- Dynamic EEPROM persistence: Saves/loads parameters and full state (timers, flags) on changes/power resume to continue exactly where left off.

\- Solenoid control:

&nbsp; - C7, C8, C9: Normally open (NO) relays (LOW = open).

&nbsp; - C6 bypass: Normally closed (NC) relay (LOW = closed).

\- 2s transition delays for smooth solenoid switching; 10ms stabilization post-relay trigger.

\- LCD display showing real-time status: "C7 - Irrigation", "C8 - Irrigation", "C9 - Irrigation", or "Idle" (when water returns to C6).

\- 8s watchdog timer to prevent system hangs.

\- Optional: Future MQTT integration for Growize dashboard.



\## Hardware Setup

\- \*\*Arduino Uno\*\*: Core controller.

\- \*\*Relays\*\*:

&nbsp; - Pin 8: C7 solenoid (NO, LOW = open)

&nbsp; - Pin 9: C8 solenoid (NO, LOW = open)

&nbsp; - Pin 10: C9 solenoid (NO, LOW = open)

&nbsp; - Pin 11: C6 bypass solenoid (NC, LOW = closed)

\- \*\*LCD\*\*: 16x2 I2C (address 0x27, pins A4/SDA, A5/SCL).

\- \*\*Power\*\*: 5V 2A adapter (separate for relay JD-VCC; recommend UPS for Bangalore power stability). Add 100µF capacitor on Uno VIN/GND.

\- \*\*Optional Sensors\*\*: HC-SR04 (tank level), DHT22 (humidity/temp) for future expansion.

\- \*\*EMI Mitigation\*\*: Flyback diodes on coils; shielded cables; pull-ups (4.7kΩ) on I2C lines.



\## Software

\- \*\*Platform\*\*: Arduino IDE.

\- \*\*Libraries\*\*: `EEPROM`, `LiquidCrystal\_I2C`, `avr/wdt`.

\- \*\*Logic\*\*:

&nbsp; 1. On startup: Load params/state from EEPROM; restore solenoid positions.

&nbsp; 2. Check `millis()` timers for each tank’s on/off state; save state on changes.

&nbsp; 3. Mark tanks ready after off period completes.

&nbsp; 4. If no tank irrigating and transition complete, start highest-priority ready tank (C7 > C8 > C9):

&nbsp;    - Open tank solenoid (LOW), wait 2s, close bypass (LOW).

&nbsp;    - After on-duration: Open bypass (HIGH, if no other tanks on), wait 2s, close tank solenoid (HIGH).

&nbsp;    - Start off-timer; save state.

&nbsp; 5. Bypass open (HIGH) when no tanks irrigating; closed (LOW) during irrigation.

&nbsp; 6. LCD updates on state change to show "C7 - Irrigation", etc., or "Idle".

\- \*\*EEPROM Layout\*\*: Params (0-23 bytes), state (24-79 bytes: remaining times, flags).

\- \*\*Configuration\*\*: Durations/states dynamically saved; initial flash sets defaults.



\## Installation

1\. Connect hardware per pin assignments; ensure relay opto-isolation (JD-VCC jumper removed).

2\. Upload code using Arduino IDE.

3\. Flash provided sketch (`rnd\_aeroponic\_controller\_lite.ino`).

4\. Monitor via LCD (shows "R\&D Controller" on line 1, status on line 2) or serial (9600 baud, uncomment for debug).

5\. Test solenoid wiring (NO for tanks, NC for bypass), timing, and power cycles for resume.



\## Usage

\- Power on: Initializes, loads params/state from EEPROM, restores solenoid states, sets bypass open (NC default).

\- LCD shows "R\&D Controller" (line 1) and status (line 2): "C7 - Irrigation", etc., or "Idle".

\- Cycles run automatically with no overlaps; priority ensures shorter cycles (C7/C8) run promptly.

\- Power outage: On resume, continues from saved state (e.g., remaining 30s in C7 on).

\- Adjust params via serial or reflash EEPROM if needed.



\## Future Enhancements

\- Add SD card logging for irrigation events (requires pin adjustments).

\- Add sensors (e.g., tank level, humidity) via I2C to avoid pin constraints.

\- Integrate MQTT/ESP8266 for Growize dashboard telemetry.

\- Add button interface for on-device param changes.



\## Notes

\- Test solenoid wiring (NO/NC), timing, and LCD visibility in Bangalore’s humid conditions.

\- Use UPS for power stability; bypass NC ensures fail-safe water return.

\- `millis()` sufficient for short-term cycles; handles overflow for ~49 days.

\- EEPROM writes limited (~100k cycles); saves only on state changes.

\- Contact AgResearch Labs for Growize integration details.



\## License

Proprietary - AgResearch Labs, Bangalore, India.

