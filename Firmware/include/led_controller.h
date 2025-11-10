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
    // Constructor
    LEDController();
    
    // Initialization
    bool init();
    
    // Set radar sensor for automatic presence detection
    void setRadarSensor(ld2410* radarSensor);
    
    // Automatic detection control
    void enableAutoDetection(bool enabled);  // Turn ON/OFF automatic detection
    bool isAutoDetectionEnabled();
    
    // Mode control
    void setMode(LEDMode mode);
    LEDMode getMode();
    void setBrightness(uint8_t brightness);
    
    // Main update loop
    void update();
    
    // Effect functions
    void skySimulationEffect();
    void rainEffect();
    void meteorEffect();
    void apocalypseEffect();
    void setCustomColor(uint8_t r, uint8_t g, uint8_t b);
    void off();
    
private:
    // LED array - using static array sized from config.h
    CRGB leds[NUM_LEDS];
    
    // Radar sensor for presence detection
    ld2410* radar = nullptr;
    
    LEDMode currentMode;
    uint8_t brightness;
    CRGB customColor = CRGB(255,255,255); // default white for BASIC mode
    
    // Auto detection state
    bool autoDetectionEnabled = false;
    LEDMode lastManualMode = MODE_BASIC;  // Remember last mode before auto turned off LEDs
    
    // Helper for presence detection
    bool checkPresence();
    
    // Helper functions for sky simulation (formerly sunrise/sunset)
    float getSunColorTemp(float hourFloat);
    struct SkyColor {
        uint8_t r, g, b;
    };
    SkyColor colorTempToRGB(float temp);
    float planckRadiance(float lambda_nm, float T);
    
    // Effect state variables - initialized to prevent garbage values
    unsigned long lastUpdate = 0;
    int effectState = 0;
    
    // Convolution for smooth sky simulation transitions
    // Arrays sized to match NUM_LEDS from config.h
    float sunSignal[NUM_LEDS];
    float convolvedSignal[NUM_LEDS];
    void initConvolution();
    void applySunConvolution(int centerIndex, float amplitude);
    void buildSunKernel(int radius);
    int getSunPositionIndex(float hourFloat);
    float getSunIntensity(float hourFloat);
    
    // Raised-cosine kernel for point-like sun highlight
    static constexpr int MAX_KERNEL_RADIUS = 15; // Maximum safe radius
    int sunKernelRadius = 8; // adjustable highlight width in LEDs (must be <= MAX_KERNEL_RADIUS)
    float sunKernel[NUM_LEDS];
};

#endif // LED_CONTROLLER_H
