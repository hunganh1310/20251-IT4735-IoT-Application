/**
 * @file sensors.cpp
 * @brief Sensor management implementation
 */

#include "sensors.h"
#include "config.h"
#include <LD2410.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Global sensor objects
extern HardwareSerial radarSerial;
extern LD2410 radar;
extern OneWire oneWire;
extern DallasTemperature tempSensors;

SensorManager::SensorManager() {
  radarInitialized = false;
  temperatureInitialized = false;
  turbidityInitialized = false;
  
  // Default turbidity calibration (user should calibrate)
  turbidity_a_coeff = -100.0;  // Adjust based on calibration
  turbidity_b_coeff = 200.0;    // Adjust based on calibration
}

bool SensorManager::initRadar() {
  radarSerial.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  
  if (radar.begin(radarSerial, true)) {
    if (DEBUG_MODE) Serial.println("[Sensor] Radar LD2410 initialized");
    radarInitialized = true;
    return true;
  }
  
  if (DEBUG_MODE) Serial.println("[Sensor] Failed to initialize radar");
  return false;
}

bool SensorManager::initTemperature() {
  tempSensors.begin();
  
  int deviceCount = tempSensors.getDeviceCount();
  if (deviceCount > 0) {
    if (DEBUG_MODE) {
      Serial.print("[Sensor] Found ");
      Serial.print(deviceCount);
      Serial.println(" DS18B20 device(s)");
    }
    temperatureInitialized = true;
    return true;
  }
  
  if (DEBUG_MODE) Serial.println("[Sensor] No DS18B20 devices found");
  return false;
}

bool SensorManager::initTurbidity() {
  analogReadResolution(12);
  analogSetPinAttenuation(TURBIDITY_PIN, ADC_11db);
  
  if (DEBUG_MODE) Serial.println("[Sensor] Turbidity sensor initialized");
  turbidityInitialized = true;
  return true;
}

bool SensorManager::initAll() {
  bool success = true;
  
  success &= initRadar();
  success &= initTemperature();
  success &= initTurbidity();
  
  if (DEBUG_MODE) {
    if (success) {
      Serial.println("[Sensor] All sensors initialized successfully");
    } else {
      Serial.println("[Sensor] Some sensors failed to initialize");
    }
  }
  
  return success;
}

RadarData SensorManager::readRadar() {
  RadarData data = {false, false, false, 0, 0, 0, 0, millis()};
  
  if (!radarInitialized) return data;
  
  radar.read();
  
  data.presenceDetected = radar.presenceDetected();
  data.movingDetected = radar.movingTargetDetected();
  data.stationaryDetected = radar.stationaryTargetDetected();
  data.movingDistance = radar.movingTargetDistance();
  data.movingEnergy = radar.movingTargetEnergy();
  data.stationaryDistance = radar.stationaryTargetDistance();
  data.stationaryEnergy = radar.stationaryTargetEnergy();
  
  return data;
}

TemperatureData SensorManager::readTemperature() {
  TemperatureData data = {0.0, 0.0, false, millis()};
  
  if (!temperatureInitialized) return data;
  
  tempSensors.requestTemperatures();
  float tempC = tempSensors.getTempCByIndex(0);
  
  if (tempC != DEVICE_DISCONNECTED_C) {
    data.tempCelsius = tempC;
    data.tempFahrenheit = (tempC * 9.0 / 5.0) + 32.0;
    data.valid = true;
  }
  
  return data;
}

TurbidityData SensorManager::readTurbidity() {
  TurbidityData data = {0.0, 0.0, 0, millis()};
  
  if (!turbidityInitialized) return data;
  
  data.voltage = readFilteredVoltage(TURBIDITY_PIN, 20);
  
  // Calculate NTU using calibration coefficients
  data.ntu = turbidity_a_coeff * data.voltage + turbidity_b_coeff;
  if (data.ntu < 0) data.ntu = 0;
  
  data.rawADC = analogRead(TURBIDITY_PIN);
  
  return data;
}

SensorData SensorManager::readAll() {
  SensorData data;
  data.radar = readRadar();
  data.temperature = readTemperature();
  data.turbidity = readTurbidity();
  return data;
}

float SensorManager::readFilteredVoltage(int pin, int samples) {
  uint32_t sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  float avgRaw = (float)sum / (float)samples;
  return (avgRaw / 4095.0) * 3.3;
}

void SensorManager::printSensorData(const SensorData& data) {
  Serial.println("\n========== Sensor Data ==========");
  
  Serial.println("--- Radar ---");
  Serial.print("Presence: ");
  Serial.println(data.radar.presenceDetected ? "YES" : "NO");
  if (data.radar.movingDetected) {
    Serial.print("Moving: ");
    Serial.print(data.radar.movingDistance);
    Serial.print(" cm, Energy: ");
    Serial.println(data.radar.movingEnergy);
  }
  if (data.radar.stationaryDetected) {
    Serial.print("Stationary: ");
    Serial.print(data.radar.stationaryDistance);
    Serial.print(" cm, Energy: ");
    Serial.println(data.radar.stationaryEnergy);
  }
  
  Serial.println("--- Temperature ---");
  if (data.temperature.valid) {
    Serial.print("Temp: ");
    Serial.print(data.temperature.tempCelsius, 2);
    Serial.print(" °C (");
    Serial.print(data.temperature.tempFahrenheit, 2);
    Serial.println(" °F)");
  } else {
    Serial.println("Invalid reading");
  }
  
  Serial.println("--- Turbidity ---");
  Serial.print("Voltage: ");
  Serial.print(data.turbidity.voltage, 3);
  Serial.print(" V, NTU: ");
  Serial.println(data.turbidity.ntu, 2);
  
  Serial.println("================================\n");
}

String SensorManager::toJSON(const SensorData& data) {
  String json = "{";
  
  // Radar data
  json += "\"radar\":{";
  json += "\"presence\":" + String(data.radar.presenceDetected ? "true" : "false") + ",";
  json += "\"moving\":{";
  json += "\"detected\":" + String(data.radar.movingDetected ? "true" : "false") + ",";
  json += "\"distance\":" + String(data.radar.movingDistance) + ",";
  json += "\"energy\":" + String(data.radar.movingEnergy);
  json += "},";
  json += "\"stationary\":{";
  json += "\"detected\":" + String(data.radar.stationaryDetected ? "true" : "false") + ",";
  json += "\"distance\":" + String(data.radar.stationaryDistance) + ",";
  json += "\"energy\":" + String(data.radar.stationaryEnergy);
  json += "},";
  json += "\"timestamp\":" + String(data.radar.timestamp);
  json += "},";
  
  // Temperature data
  json += "\"temperature\":{";
  json += "\"celsius\":" + String(data.temperature.tempCelsius, 2) + ",";
  json += "\"fahrenheit\":" + String(data.temperature.tempFahrenheit, 2) + ",";
  json += "\"valid\":" + String(data.temperature.valid ? "true" : "false") + ",";
  json += "\"timestamp\":" + String(data.temperature.timestamp);
  json += "},";
  
  // Turbidity data
  json += "\"turbidity\":{";
  json += "\"voltage\":" + String(data.turbidity.voltage, 3) + ",";
  json += "\"ntu\":" + String(data.turbidity.ntu, 2) + ",";
  json += "\"rawADC\":" + String(data.turbidity.rawADC) + ",";
  json += "\"timestamp\":" + String(data.turbidity.timestamp);
  json += "}";
  
  json += "}";
  return json;
}
