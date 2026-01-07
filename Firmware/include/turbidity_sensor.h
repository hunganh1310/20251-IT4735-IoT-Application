/**
 * @file turbidity_sensor.h
 * @brief Turbidity sensor interface for water quality monitoring
 */

#ifndef TURBIDITY_SENSOR_H
#define TURBIDITY_SENSOR_H

#include <Arduino.h>

class TurbiditySensor {
public:
    TurbiditySensor(uint8_t pin);
    
    bool init();
    
    float readNTU();
    int readRawValue();
    float readVoltage();
    float readStableVoltage();
    String getWaterQuality();
    
    void calibrate(float clearWaterVoltage, float dirtyWaterVoltage);
    
private:
    uint8_t sensorPin;
    float clearWaterVoltage;
    float dirtyWaterVoltage;
    
    static const int FILTER_SIZE = 20;
    float voltageBuffer[FILTER_SIZE];
    int bufferIndex;
    bool bufferFilled;
    
    float voltageToNTU(float voltage);
    float getAverageVoltage();
};

#endif // TURBIDITY_SENSOR_H
