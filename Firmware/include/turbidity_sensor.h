/**
 * @file turbidity_sensor.h
 * @brief Turbidity sensor interface for water quality monitoring
 */

#ifndef TURBIDITY_SENSOR_H
#define TURBIDITY_SENSOR_H

#include <Arduino.h>

class TurbiditySensor {
public:
    // Constructor
    TurbiditySensor(uint8_t pin);
    
    // Initialization
    bool init();
    
    // Read sensor data
    float readNTU();           // Read turbidity in NTU (Nephelometric Turbidity Units)
    int readRawValue();        // Read raw analog value (0-4095)
    float readVoltage();       // Read voltage (0-3.3V)
    float readStableVoltage(); // Read voltage with moving average filter
    String getWaterQuality();  // Get qualitative assessment
    
    // Calibration
    void calibrate(float clearWaterVoltage, float dirtyWaterVoltage);
    
private:
    uint8_t sensorPin;
    float clearWaterVoltage;   // Voltage reading in clear water
    float dirtyWaterVoltage;   // Voltage reading in dirty water
    
    // Moving average filter
    static const int FILTER_SIZE = 20;
    float voltageBuffer[FILTER_SIZE];
    int bufferIndex;
    bool bufferFilled;
    
    // Convert voltage to NTU
    float voltageToNTU(float voltage);
    float getAverageVoltage();
};

#endif // TURBIDITY_SENSOR_H
