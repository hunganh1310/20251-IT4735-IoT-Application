/**
 * @file ds18b20_sensor.cpp
 * @brief DS18B20 temperature sensor implementation
 */

#include "ds18b20_sensor.h"

DS18B20Sensor::DS18B20Sensor(uint8_t pin) {
    sensorPin = pin;
    oneWire = nullptr;
    sensors = nullptr;
    lastTemperature = 0.0;
    initialized = false;
}

bool DS18B20Sensor::init() {
    // Initialize OneWire
    oneWire = new OneWire(sensorPin);
    sensors = new DallasTemperature(oneWire);
    
    // Start the DallasTemperature library
    sensors->begin();
    
    // Check if any devices are connected
    int deviceCount = sensors->getDeviceCount();
    
    if (deviceCount == 0) {
        Serial.println("[DS18B20] ERROR: No devices found on pin " + String(sensorPin));
        return false;
    }
    
    Serial.println("[DS18B20] Sensor initialized on pin " + String(sensorPin));
    Serial.println("[DS18B20] Found " + String(deviceCount) + " device(s)");
    
    // Set resolution to 12-bit (0.0625°C precision)
    sensors->setResolution(12);
    
    initialized = true;
    return true;
}

float DS18B20Sensor::readTemperature() {
    if (!initialized) {
        Serial.println("[DS18B20] ERROR: Sensor not initialized");
        return -127.0;  // Error value
    }
    
    // Request temperature reading
    sensors->requestTemperatures();
    
    // Read temperature from first device (index 0)
    float temperature = sensors->getTempCByIndex(0);
    
    // Check for valid reading
    if (temperature == DEVICE_DISCONNECTED_C) {
        Serial.println("[DS18B20] ERROR: Device disconnected");
        return lastTemperature;  // Return last known good value
    }
    
    lastTemperature = temperature;
    return temperature;
}

float DS18B20Sensor::readTemperatureFahrenheit() {
    float celsius = readTemperature();
    return (celsius * 9.0 / 5.0) + 32.0;
}

bool DS18B20Sensor::isConnected() {
    if (!initialized) {
        return false;
    }
    
    return sensors->getDeviceCount() > 0;
}

int DS18B20Sensor::getDeviceCount() {
    if (!initialized) {
        return 0;
    }
    
    return sensors->getDeviceCount();
}

void DS18B20Sensor::requestTemperatures() {
    if (initialized) {
        sensors->requestTemperatures();
    }
}

bool DS18B20Sensor::isConversionComplete() {
    if (!initialized) {
        return false;
    }
    
    // Check if conversion is complete (takes ~750ms for 12-bit resolution)
    return sensors->isConversionComplete();
}
