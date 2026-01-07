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
    DS18B20Sensor(uint8_t pin);
    
    bool init();
    
    float readTemperature();
    float readTemperatureFahrenheit();
    bool isConnected();
    int getDeviceCount();
    
    void requestTemperatures();
    bool isConversionComplete();
    
private:
    uint8_t sensorPin;
    OneWire* oneWire;
    DallasTemperature* sensors;
    float lastTemperature;
    bool initialized;
};

#endif // DS18B20_SENSOR_H
