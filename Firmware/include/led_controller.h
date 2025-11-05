/**
 * @file led_controller.h
 * @brief LED effects and control system
 */

#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include <FastLED.h>

enum LEDMode {
  MODE_OFF,
  MODE_SUNRISE_SUNSET,
  MODE_RAIN,
  MODE_METEOR,
  MODE_APOCALYPSE,
  MODE_CUSTOM
};

class LEDController {
public:
  LEDController();
  
  // Initialization
  bool init();
  
  // Mode control
  void setMode(LEDMode mode);
  LEDMode getMode();
  void setBrightness(uint8_t brightness);
  
  // Main update loop
  void update();
  
  // Effect functions
  void sunriseSunsetEffect();
  void rainEffect();
  void meteorEffect();
  void apocalypseEffect();
  void setCustomColor(uint8_t r, uint8_t g, uint8_t b);
  void off();
  
private:
  CRGB* leds;
  LEDMode currentMode;
  uint8_t brightness;
  
  // Helper functions for sunrise/sunset
  float getSunColorTemp(float hourFloat);
  struct SkyColor {
    uint8_t r, g, b;
  };
  SkyColor colorTempToRGB(float temp);
  float planckRadiance(float lambda_nm, float T);
  
  // Effect state variables
  unsigned long lastUpdate;
  int effectState;
  
  // Convolution for smooth sunrise/sunset
  float sunSignal[60];
  float convolvedSignal[60];
  void initConvolution();
  void applySunConvolution();
};

#endif // LED_CONTROLLER_H
