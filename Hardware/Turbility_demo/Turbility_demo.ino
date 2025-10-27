// Turbidity example for ESP32 (Arduino core)
// Reads analog turbidity sensor (A0) and prints raw ADC, voltage, estimated NTU
// - If your module is 5V, use voltage divider as explained
// - Calibrate V_CLEAN and V_TURBID with known samples

#include <WiFi.h>        // only needed if you enable MQTT part later

// --- CONFIG ---
const int ADC_PIN = 4;         // ADC1 channel (GPIO34 is input-only, good for analog)
const int NUM_SAMPLES = 20;     // number of readings to average
const float VREF = 3.3;         // ADC reference voltage for ESP32 (approx)
const int ADC_MAX = 4095;       // 12-bit resolution

// Calibration constants (example) - YOU MUST CALIBRATE THESE
// Procedure: measure voltage with clean water (V_clean) and with known turbid sample (V_turbid)
// Then derive linear mapping NTU = a * V + b
float V_clean = 2.0;   // measured voltage on clean water (example)
float NTU_clean = 0.0; // NTU for clean water (usually 0)
float V_turbid = 0.5;  // measured voltage on turbid sample (example)
float NTU_turbid = 100.0; // known NTU for that sample

// Derived coefficients (will compute in setup if calibration available)
float a_coeff = 0.0;
float b_coeff = 0.0;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Turbidity sensor example (ESP32)");

  // ADC setup
  analogReadResolution(12);                   // 0..4095
  analogSetPinAttenuation(ADC_PIN, ADC_11db); // allow full-scale ~3.3V range

  // Compute linear mapping a*x + b using two calibration points (V->NTU)
  // If you haven't measured calibration values, keep default a/b (will be 0)
  if (fabs(V_turbid - V_clean) > 0.0001) {
    a_coeff = (NTU_turbid - NTU_clean) / (V_turbid - V_clean);
    b_coeff = NTU_clean - a_coeff * V_clean;
    Serial.printf("Calibration computed: a=%.6f, b=%.6f\n", a_coeff, b_coeff);
    Serial.printf("V_clean=%.3f V -> NTU=%.3f ; V_turbid=%.3f V -> NTU=%.3f\n",
                  V_clean, NTU_clean, V_turbid, NTU_turbid);
  } else {
    Serial.println("Calibration values invalid; set V_clean and V_turbid first.");
  }
}

float readFilteredVoltage() {
  uint32_t sum = 0;
  for (int i = 0; i < NUM_SAMPLES; ++i) {
    int raw = analogRead(ADC_PIN);
    sum += raw;
    delay(10); // small delay between samples
  }
  float avgRaw = (float)sum / (float)NUM_SAMPLES;
  float voltage = (avgRaw / ADC_MAX) * VREF;
  return voltage;
}

void loop() {
  float V = readFilteredVoltage();
  // If you used a voltage divider, account for the divider ratio:
  // For example, if divider halves voltage (R1=R2), actual_V_module = V * (R1+R2)/R2 = V * 2.0
  // float dividerRatio = 2.0; // set to 1.0 if no divider
  float dividerRatio = 1.0; // CHANGE to divider ratio if used
  float V_module = V * dividerRatio;

  float NTU_est = a_coeff * V_module + b_coeff;

  // clamp
  if (NTU_est < 0) NTU_est = 0;

  Serial.printf("ADC Vmeas=%.3f V (scaled=%.3f V) -> NTU ~ %.2f\n", V, V_module, NTU_est);

  // wait between readings
  delay(1500);
}
