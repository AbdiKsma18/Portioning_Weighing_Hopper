#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <HX711.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definisi Pin
#define PIN_POT       A0
#define PIN_TARE      2
#define PIN_START     3
#define PIN_HX_DT     4
#define PIN_HX_SCK    5
#define PIN_LED_GRN   6
#define PIN_LED_RED   7
#define PIN_BUZZER    8
#define PIN_SERVO     9

// Konstanta Katup Servo (Derajat)
const int VALVE_CLOSE = 0;
const int VALVE_SLOW  = 25;
const int VALVE_FAST  = 90;

// Kalibrasi HX711 agar output langsung dalam satuan kg
const float LOADCELL_SCALE = 420.0;

HX711 scale;
Servo hopperGate;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

enum SystemState { IDLE, FAST_FILL, SLOW_FILL, DONE, OVERLOAD };
SystemState state = IDLE;

bool isRunning = false;
float currentWeightKg = 0.0;
float targetWeightKg  = 0.50;
int valveAngle = VALVE_CLOSE;

unsigned long lastReadTime = 0;
unsigned long lastBtnTime = 0;

void beep(int freq, int duration) {
  tone(PIN_BUZZER, freq, duration);
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Header & Status
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("HOPPER:"));
  display.setCursor(48, 0);
  switch (state) {
    case IDLE:      display.print(F("READY")); break;
    case FAST_FILL: display.print(F("FAST MODE")); break;
    case SLOW_FILL: display.print(F("SLOW MODE")); break;
    case DONE:      display.print(F("COMPLETE")); break;
    case OVERLOAD:  display.print(F("OVERLOAD!")); break;
  }
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  // Target (bersih tanpa ambang tambahan)
  display.setCursor(0, 14);
  display.print(F("Target : "));
  display.print(targetWeightKg, 2);
  display.println(F(" kg"));

  // Berat Aktual
  display.setCursor(0, 25);
  display.print(F("Aktual : "));
  display.print(currentWeightKg, 2);
  display.println(F(" kg"));

  // Bukaan Katup
  int valvePct = map(valveAngle, 0, 90, 0, 100);
  display.setCursor(0, 36);
  display.print(F("Katup  : "));
  display.print(valvePct);
  display.println(F(" %"));

  // Progress Bar Pengisian
  display.drawRect(0, 49, 128, 13, SSD1306_WHITE);
  float ratio = constrain(currentWeightKg / max(targetWeightKg, 0.01f), 0.0, 1.0);
  int barWidth = (int)(ratio * 124);
  display.fillRect(2, 51, barWidth, 9, SSD1306_WHITE);

  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_TARE, INPUT_PULLUP);
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_LED_GRN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  hopperGate.attach(PIN_SERVO);
  hopperGate.write(VALVE_CLOSE);

  scale.begin(PIN_HX_DT, PIN_HX_SCK);
  scale.set_scale(LOADCELL_SCALE);
  scale.tare();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  beep(1500, 100);
  updateDisplay();
}

void loop() {
  unsigned long now = millis();

  // 1. Pembacaan Tombol dengan Debounce 200ms
  if (now - lastBtnTime > 200) {
    if (digitalRead(PIN_TARE) == LOW && !isRunning) {
      scale.tare();
      currentWeightKg = 0.0;
      state = IDLE;
      beep(1200, 80);
      lastBtnTime = now;
    }

    if (digitalRead(PIN_START) == LOW) {
      isRunning = !isRunning;
      if (!isRunning) {
        state = IDLE;
        valveAngle = VALVE_CLOSE;
        hopperGate.write(valveAngle);
        beep(800, 120);
      } else {
        beep(1800, 100);
      }
      lastBtnTime = now;
    }
  }

  // 2. Pembacaan Sensor & State Machine (Setiap 100ms)
  if (now - lastReadTime >= 100) {
    lastReadTime = now;

    // Potensiometer mengatur target 0.20 kg s.d. 2.00 kg (kelipatan 0.05 kg)
    int adcVal = analogRead(PIN_POT);
    if (!isRunning) {
      int steps = map(adcVal, 0, 1023, 4, 40);
      targetWeightKg = steps * 0.05;
    }

    // Baca Berat dari HX711
    if (scale.is_ready()) {
      currentWeightKg = scale.get_units(1);
      if (currentWeightKg < 0.0 && currentWeightKg > -0.01) currentWeightKg = 0.0;
    }

    // State Machine Kontrol Katup & Indikator
    if (isRunning) {
      if (currentWeightKg < targetWeightKg * 0.80) {
        state = FAST_FILL;
        valveAngle = VALVE_FAST;
      } else if (currentWeightKg < targetWeightKg) {
        state = SLOW_FILL;
        valveAngle = VALVE_SLOW;
      } else {
        isRunning = false;
        valveAngle = VALVE_CLOSE;
        state = (currentWeightKg > targetWeightKg * 1.05) ? OVERLOAD : DONE;
        beep(2200, 300);
      }
    } else {
      valveAngle = VALVE_CLOSE;
      if (currentWeightKg > targetWeightKg * 1.05) {
        state = OVERLOAD;
      } else if (state != DONE) {
        state = IDLE;
      }
    }

    hopperGate.write(valveAngle);

    digitalWrite(PIN_LED_GRN, (state == DONE || state == IDLE) ? HIGH : LOW);
    digitalWrite(PIN_LED_RED, (state == FAST_FILL || state == SLOW_FILL || state == OVERLOAD) ? HIGH : LOW);

    updateDisplay();
  }
}