/**
 * @file led_controller.h
 * @brief LED effects and control system
 */

#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include <FastLED.h>
#include <ld2410.h>
#include "config.h"

enum LEDMode {
    MODE_OFF,
    MODE_SKY_SIMULATION,
    MODE_RAIN,
    MODE_METEOR,
    MODE_APOCALYPSE,
    MODE_BASIC
};

class LEDController {
public:
    LEDController();
    
    bool init();
    
    void setRadarSensor(ld2410* radarSensor);
    
    void enableAutoDetection(bool enabled);
    bool isAutoDetectionEnabled();
    
    void setMode(LEDMode mode);
    LEDMode getMode();
    void setBrightness(uint8_t brightness);
    
    void update();
    
    void skySimulationEffect();
    void rainEffect();
    void meteorEffect();
    void apocalypseEffect();
    void setCustomColor(uint8_t r, uint8_t g, uint8_t b);
    void off();
    
private:
    CRGB leds[NUM_LEDS];
    
    ld2410* radar = nullptr;
    
    LEDMode currentMode;
    uint8_t brightness;
    CRGB customColor = CRGB(255,255,255);
    
    bool autoDetectionEnabled = false;
    LEDMode lastManualMode = MODE_BASIC;
    
    bool checkPresence();
    
    float getSunColorTemp(float hourFloat);
    struct SkyColor {
        uint8_t r, g, b;
    };
    SkyColor colorTempToRGB(float temp);
    float planckRadiance(float lambda_nm, float T);
    
    unsigned long lastUpdate = 0;
    int effectState = 0;
    
    // Sky simulation arrays
    float sunSignal[NUM_LEDS];
    float convolvedSignal[NUM_LEDS];
    void initConvolution();
    void applySunConvolution(int centerIndex, float amplitude);
    void buildSunKernel(int radius);
    int getSunPositionIndex(float hourFloat);
    float getSunIntensity(float hourFloat);
    
    // Raised-cosine kernel for sun highlight
    static constexpr int MAX_KERNEL_RADIUS = 15;
    int sunKernelRadius = 8;
    float sunKernel[NUM_LEDS];
};

#endif // LED_CONTROLLER_H
