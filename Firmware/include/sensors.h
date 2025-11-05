/**
 * @file sensors.h
 * @brief Sensor management interface
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// ==================== Sensor Data Structures ====================
struct RadarData {
  bool presenceDetected;
  bool movingDetected;
  bool stationaryDetected;
  uint16_t movingDistance;
  uint8_t movingEnergy;
  uint16_t stationaryDistance;
  uint8_t stationaryEnergy;
  uint32_t timestamp;
};

struct TemperatureData {
  float tempCelsius;
  float tempFahrenheit;
  bool valid;
  uint32_t timestamp;
};

struct TurbidityData {
  float voltage;
  float ntu;
  uint16_t rawADC;
  uint32_t timestamp;
};

struct SensorData {
  RadarData radar;
  TemperatureData temperature;
  TurbidityData turbidity;
};

// ==================== Sensor Manager Class ====================
class SensorManager {
public:
  SensorManager();
  
  // Initialization
  bool initRadar();
  bool initTemperature();
  bool initTurbidity();
  bool initAll();
  
  // Data reading
  RadarData readRadar();
  TemperatureData readTemperature();
  TurbidityData readTurbidity();
  SensorData readAll();
  
  // Utility
  void printSensorData(const SensorData& data);
  String toJSON(const SensorData& data);
  
private:
  bool radarInitialized;
  bool temperatureInitialized;
  bool turbidityInitialized;
  
  // Calibration data for turbidity
  float turbidity_a_coeff;
  float turbidity_b_coeff;
  
  float readFilteredVoltage(int pin, int samples);
};

#endif // SENSORS_H
