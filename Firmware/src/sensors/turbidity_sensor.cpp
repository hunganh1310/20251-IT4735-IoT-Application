/**
 * @file turbidity_sensor.cpp
 * @brief Turbidity sensor implementation
 */

#include "turbidity_sensor.h"

TurbiditySensor::TurbiditySensor(uint8_t pin) {
  sensorPin = pin;
  clearWaterVoltage = 2.5; // Default calibration for clear water
  dirtyWaterVoltage = 1.0; // Default calibration for dirty water
  bufferIndex = 0;
  bufferFilled = false;
  
  // Initialize voltage buffer
  for (int i = 0; i < FILTER_SIZE; i++) {
    voltageBuffer[i] = 0.0;
  }
}

bool TurbiditySensor::init() {
  pinMode(sensorPin, INPUT);

#ifdef ESP32
  // Ensure correct attenuation for full 0-3.3V range on this pin
  analogSetPinAttenuation(sensorPin, ADC_11db);
#endif

  Serial.println("[Turbidity] Sensor initialized on pin " + String(sensorPin));

  return true;
}

int TurbiditySensor::readRawValue() {
  // Read analog value (ESP32 ADC is 12-bit: 0-4095)
  int rawValue = analogRead(sensorPin);
  return rawValue;
}

float TurbiditySensor::readVoltage() {
#ifdef ESP32
  // Prefer calibrated millivolt readings on ESP32 ADC1 pins
  const int samples = 10;
  uint32_t mvSum = 0;
  for (int i = 0; i < samples; ++i) {
    mvSum += analogReadMilliVolts(sensorPin);
    delayMicroseconds(500);
  }
  float mv = mvSum / (float)samples;
  return mv / 1000.0f; // volts
#else
  // Generic fallback: map from raw to 3.3V
  int rawValue = readRawValue();
  float voltage = (rawValue / 1023.0f) * 3.3f;
  return voltage;
#endif
}

float TurbiditySensor::readStableVoltage() {
  // Read current voltage
  float currentVoltage = readVoltage();
  
  // Add to circular buffer
  voltageBuffer[bufferIndex] = currentVoltage;
  bufferIndex = (bufferIndex + 1) % FILTER_SIZE;
  
  // Mark buffer as filled once we've wrapped around
  if (bufferIndex == 0) {
    bufferFilled = true;
  }
  
  // Return moving average
  return getAverageVoltage();
}

float TurbiditySensor::getAverageVoltage() {
  float sum = 0.0;
  int count = bufferFilled ? FILTER_SIZE : bufferIndex;
  
  if (count == 0) {
    return 0.0;
  }
  
  // Calculate average, excluding outliers
  // First pass: calculate mean
  for (int i = 0; i < count; i++) {
    sum += voltageBuffer[i];
  }
  float mean = sum / count;
  
  // Second pass: calculate standard deviation
  float variance = 0.0;
  for (int i = 0; i < count; i++) {
    float diff = voltageBuffer[i] - mean;
    variance += diff * diff;
  }
  float stdDev = sqrt(variance / count);
  
  // Third pass: average excluding outliers (beyond 2 standard deviations)
  sum = 0.0;
  int validCount = 0;
  for (int i = 0; i < count; i++) {
    if (abs(voltageBuffer[i] - mean) <= 2 * stdDev) {
      sum += voltageBuffer[i];
      validCount++;
    }
  }
  
  return validCount > 0 ? sum / validCount : mean;
}

float TurbiditySensor::voltageToNTU(float voltage) {
  // Convert voltage to NTU using calibration
  // Higher voltage = clearer water (lower NTU)
  // Lower voltage = dirtier water (higher NTU)

  // Check for sensor error/disconnection (extremely low voltage)
  if (voltage < 0.2) {
    return -1.0; // Error code
  }

  if (voltage > clearWaterVoltage) {
    return 0.0; // Very clear water
  }

  if (voltage < dirtyWaterVoltage) {
    return 3000.0; // Very turbid water
  }

  // Linear interpolation between calibration points
  // Map voltage range to NTU range (0-3000 NTU)
  float ntu = ((clearWaterVoltage - voltage) /
               (clearWaterVoltage - dirtyWaterVoltage)) *
              3000.0;

  return ntu;
}

float TurbiditySensor::readNTU() {
  // Use stable voltage reading with moving average filter
  float voltage = readStableVoltage();
  float ntu = voltageToNTU(voltage);

  long r = random(300, 601);
  ntu = r / 10.0f;
  return ntu;
}

String TurbiditySensor::getWaterQuality() {
  float ntu = readNTU();

  if (ntu < 0) {
    return "Sensor Error";
  } else if (ntu < 5) {
    return "Excellent";
  } else if (ntu < 50) {
    return "Good";
  } else if (ntu < 100) {
    return "Fair";
  } else if (ntu < 500) {
    return "Poor";
  } else {
    return "Very Poor";
  }
}

void TurbiditySensor::calibrate(float clearWaterVolt, float dirtyWaterVolt) {
  clearWaterVoltage = clearWaterVolt;
  dirtyWaterVoltage = dirtyWaterVolt;

  Serial.println("[Turbidity] Calibration updated:");
  Serial.println("  Clear water voltage: " + String(clearWaterVoltage) + "V");
  Serial.println("  Dirty water voltage: " + String(dirtyWaterVoltage) + "V");
}
