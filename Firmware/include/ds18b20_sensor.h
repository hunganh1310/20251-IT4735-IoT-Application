/**
 * @file ds18b20_sensor.h
 * @brief DS18B20 temperature sensor interface
 */

#ifndef DS18B20_SENSOR_H
#define DS18B20_SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Sensor {
public:
    // Constructor
    DS18B20Sensor(uint8_t pin);
    
    // Initialization
    bool init();
    
    // Read sensor data
    float readTemperature();           // Read temperature in Celsius
    float readTemperatureFahrenheit(); // Read temperature in Fahrenheit
    bool isConnected();                // Check if sensor is connected
    int getDeviceCount();              // Get number of connected devices
    
    // Request new temperature reading (non-blocking)
    void requestTemperatures();
    
    // Check if conversion is complete
    bool isConversionComplete();
    
private:
    uint8_t sensorPin;
    OneWire* oneWire;
    DallasTemperature* sensors;
    float lastTemperature;
    bool initialized;
};

#endif // DS18B20_SENSOR_H
