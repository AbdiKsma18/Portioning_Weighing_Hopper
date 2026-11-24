# Automated Portioning & Weighing Hopper

An automated portioning and weighing hopper system built on **Arduino Uno** using a load cell amplifier (**HX711**), a **servo valve**, an **OLED display**, a **potentiometer**, control pushbuttons (`TARE` and `START/STOP`), status LEDs, and a buzzer. Developed and simulated using **PlatformIO** and **Wokwi** in **VS Code**, with all weight parameters standardized in **kilograms (`kg`)**.

## Key Features
- **Two-Stage Filling Control:** Uses `FAST MODE` (100% valve opening) below 80% of the target weight and switches to `SLOW MODE` (reduced valve opening) from 80% up to the target weight to minimize material overshoot.
- **Clear Mode & Status Feedback:** Displays system states (`READY`, `FAST MODE`, `SLOW MODE`, `COMPLETE`, and `OVERLOAD!`), target weight, current weight, valve percentage, and a progress bar on the OLED screen.
- **Weight Unit in `kg`:** Aligns weight parameters directly with the Wokwi HX711 simulation slider for smoother testing and demonstration.
- **Tare & Start/Stop Controls:** Allows zeroing the scale (`TARE`) when idle and toggling filling operation (`START/STOP`).

## Hardware & Pin Mapping
| Component | Pin / Interface | Function |
| :--- | :--- | :--- |
| Potentiometer | `A0` | Adjusts target weight in `kg` (when stopped) |
| TARE Button | `D2` (`INPUT_PULLUP`) | Resets scale baseline to `0.00 kg` when idle |
| START/STOP Button | `D3` (`INPUT_PULLUP`) | Starts or stops the portioning cycle |
| HX711 Load Cell Amp | `DT: D4`, `SCK: D5` | Reads weight measurements in `kg` |
| Green LED | `D6` | Indicates `READY` or `COMPLETE` state |
| Red LED | `D7` | Indicates active filling (`FAST MODE` / `SLOW MODE`) or `OVERLOAD!` |
| Buzzer | `D8` | Provides audio feedback for button presses, completion, and alerts |
| Servo Motor | `D9` | Controls hopper valve opening angle (`0°` closed, `25°` slow, `90°` fast) |
| SSD1306 OLED (128x64) | `SDA: A4`, `SCL: A5` (`I2C`, `0x3C`) | Displays mode, target weight, actual weight, valve %, and progress bar |

## Project Structure
```text
Portioning_Weighing_Hopper/
├── include/
├── lib/
├── src/
│   └── main.cpp
├── test/
├── .gitignore
├── diagram.json
├── platformio.ini
└── wokwi.toml
