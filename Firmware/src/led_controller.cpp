/**
 * @file led_controller.cpp
 * @brief LED effects implementation
 */

#include "led_controller.h"
#include "config.h"
#include <time.h>

// Physical constants for black body radiation
const float h = 6.626e-34;  // Planck's constant (J·s)
const float c = 3.0e8;      // Speed of light (m/s)
const float k = 1.381e-23;  // Boltzmann constant (J/K)

LEDController::LEDController() {
  leds = new CRGB[NUM_LEDS];
  currentMode = MODE_OFF;
  brightness = DEFAULT_BRIGHTNESS;
  lastUpdate = 0;
  effectState = 0;
}

bool LEDController::init() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  FastLED.clear();
  FastLED.show();
  
  initConvolution();
  
  if (DEBUG_MODE) {
    Serial.println("[LED] Controller initialized");
  }
  
  return true;
}

void LEDController::setMode(LEDMode mode) {
  currentMode = mode;
  effectState = 0;
  
  if (mode == MODE_OFF) {
    off();
  }
  
  if (DEBUG_MODE) {
    Serial.print("[LED] Mode changed to: ");
    Serial.println(mode);
  }
}

LEDMode LEDController::getMode() {
  return currentMode;
}

void LEDController::setBrightness(uint8_t newBrightness) {
  brightness = newBrightness;
  FastLED.setBrightness(brightness);
  
  if (DEBUG_MODE) {
    Serial.print("[LED] Brightness set to: ");
    Serial.println(brightness);
  }
}

void LEDController::update() {
  switch(currentMode) {
    case MODE_OFF:
      // Nothing to do
      break;
    case MODE_SUNRISE_SUNSET:
      sunriseSunsetEffect();
      break;
    case MODE_RAIN:
      rainEffect();
      break;
    case MODE_METEOR:
      meteorEffect();
      break;
    case MODE_APOCALYPSE:
      apocalypseEffect();
      break;
    case MODE_CUSTOM:
      // Custom color already set
      break;
  }
  
  FastLED.show();
}

void LEDController::off() {
  FastLED.clear();
  FastLED.show();
}

void LEDController::setCustomColor(uint8_t r, uint8_t g, uint8_t b) {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(r, g, b);
  }
  currentMode = MODE_CUSTOM;
}

// ==================== Sunrise/Sunset Effect ====================
void LEDController::initConvolution() {
  // Initialize cosine kernel for smooth transitions
  for(int i = 0; i < NUM_LEDS; i++) {
    sunSignal[i] = 0;
    convolvedSignal[i] = 0;
  }
}

float LEDController::planckRadiance(float lambda_nm, float T) {
  float lambda = lambda_nm * 1e-9;
  float exp_term = (h * c) / (lambda * k * T);
  
  if (exp_term > 50) return 0;
  
  float denominator = exp(exp_term) - 1.0;
  if (denominator < 1e-10) return 0;
  
  return 1.0 / (pow(lambda, 5) * denominator);
}

LEDController::SkyColor LEDController::colorTempToRGB(float temp) {
  float lambda_r = 700;
  float lambda_g = 546;
  float lambda_b = 436;
  
  float I_r = planckRadiance(lambda_r, temp);
  float I_g = planckRadiance(lambda_g, temp);
  float I_b = planckRadiance(lambda_b, temp);
  
  float maxI = max(max(I_r, I_g), I_b);
  
  float r_norm = I_r / maxI;
  float g_norm = I_g / maxI;
  float b_norm = I_b / maxI;
  
  r_norm = pow(r_norm, 1.0/2.2);
  g_norm = pow(g_norm, 1.0/2.2);
  b_norm = pow(b_norm, 1.0/2.2);
  
  SkyColor color;
  color.r = (uint8_t)(r_norm * 255);
  color.g = (uint8_t)(g_norm * 255);
  color.b = (uint8_t)(b_norm * 255);
  
  return color;
}

float LEDController::getSunColorTemp(float hourFloat) {
  if (hourFloat < 6.0 || hourFloat > 18.5) {
    return 0;
  }
  else if (hourFloat < 7.0) {
    float t = (hourFloat - 6.0);
    return 2000 + 2000 * t;
  }
  else if (hourFloat < 8.0) {
    float t = (hourFloat - 7.0);
    return 4000 + 1500 * t;
  }
  else if (hourFloat < 12.0) {
    return 5500;
  }
  else if (hourFloat < 17.0) {
    return 5500;
  }
  else if (hourFloat < 18.0) {
    float t = (hourFloat - 17.0);
    return 5500 - 1500 * t;
  }
  else {
    float t = (hourFloat - 18.0);
    return 4000 - 2000 * t;
  }
}

void LEDController::sunriseSunsetEffect() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  float hourFloat = timeinfo->tm_hour + timeinfo->tm_min / 60.0;
  float sunTemp = getSunColorTemp(hourFloat);
  
  if (sunTemp > 0) {
    SkyColor skyColor = colorTempToRGB(sunTemp);
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(skyColor.r, skyColor.g, skyColor.b);
    }
  } else {
    // Night time - dark blue
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 10);
    }
  }
}

// ==================== Rain Effect ====================
void LEDController::rainEffect() {
  static unsigned long lastLightning = 0;
  static bool lightningActive = false;
  static int lightningBrightness = 0;
  static int lightningPosition = 0;
  
  // Base stormy sky
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(5, 8, 15);
    leds[i].r += random(-2, 3);
    leds[i].g += random(-2, 3);
    leds[i].b += random(-3, 5);
  }
  
  // Random raindrops
  if(random(100) < 30) {
    int pos = random(NUM_LEDS);
    leds[pos] = CRGB(2, 5, 10);
  }
  
  // Lightning
  unsigned long now = millis();
  if(!lightningActive && (now - lastLightning > random(3000, 8000))) {
    lightningActive = true;
    lightningBrightness = 255;
    lightningPosition = random(NUM_LEDS / 3, NUM_LEDS * 2 / 3);
    lastLightning = now;
  }
  
  if(lightningActive) {
    int spread = 15;
    for(int i = max(0, lightningPosition - spread); 
        i < min(NUM_LEDS, lightningPosition + spread); i++) {
      int distance = abs(i - lightningPosition);
      int brightness = lightningBrightness * (spread - distance) / spread;
      leds[i] += CRGB(brightness, brightness, brightness + 20);
    }
    
    lightningBrightness -= 30;
    if(lightningBrightness <= 0) {
      lightningActive = false;
    }
  }
}

// ==================== Meteor Effect ====================
void LEDController::meteorEffect() {
  static int meteorPos[3] = {0, 20, 40};
  static int meteorSpeed[3] = {2, 3, 2};
  
  // Fade all LEDs
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(64);
  }
  
  // Draw meteors
  for(int m = 0; m < 3; m++) {
    if(meteorPos[m] < NUM_LEDS) {
      leds[meteorPos[m]] = CRGB(255, 200, 100);
      
      // Trail
      for(int j = 1; j < 8; j++) {
        if(meteorPos[m] - j >= 0) {
          leds[meteorPos[m] - j] = CRGB(255/(j+1), 200/(j+1), 100/(j+1));
        }
      }
      
      meteorPos[m] += meteorSpeed[m];
    } else {
      meteorPos[m] = random(-20, 0);
      meteorSpeed[m] = random(2, 4);
    }
  }
  
  delay(50);
}

// ==================== Apocalypse Effect ====================
void LEDController::apocalypseEffect() {
  for(int i = 0; i < NUM_LEDS; i++) {
    int flicker = random(50, 255);
    leds[i] = CRGB(flicker, flicker/4, 0);
  }
  
  // Smoke effect (darker patches)
  if(random(100) < 20) {
    int pos = random(NUM_LEDS);
    int width = random(3, 8);
    for(int i = pos; i < min(pos + width, NUM_LEDS); i++) {
      leds[i].fadeToBlackBy(150);
    }
  }
  
  delay(30);
}
