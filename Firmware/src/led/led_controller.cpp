/**
 * @file led_controller.cpp
 * @brief LED effects implementation
 */

#include <Arduino.h>
#include <FastLED.h>
#include <ld2410.h>
#include "led_controller.h"
#include "config.h"
#include <time.h>

const float h = 6.626e-34;
const float c = 3.0e8;
const float k = 1.381e-23;

LEDController::LEDController() {
    currentMode = MODE_OFF;
    brightness = DEFAULT_BRIGHTNESS;
    effectState = 0;
}

bool LEDController::init() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);
    FastLED.clear();
    FastLED.show();
    
    initConvolution();
    
    Serial.println("[LED] Controller initialized");
    
    return true;
}

void LEDController::setRadarSensor(ld2410* radarSensor) {
    radar = radarSensor;
    Serial.println("[LED] Radar sensor attached");
}

void LEDController::enableAutoDetection(bool enabled) {
    autoDetectionEnabled = enabled;
    if (enabled) {
        lastManualMode = currentMode;
        Serial.println("[LED] Auto detection enabled");
    } else {
        Serial.println("[LED] Auto detection disabled");
    }
}

bool LEDController::isAutoDetectionEnabled() {
    return autoDetectionEnabled;
}

bool LEDController::checkPresence() {
    if (radar == nullptr) {
        return false;
    }
    return radar->presenceDetected();
}

void LEDController::setMode(LEDMode mode) {
    currentMode = mode;
    
    if (mode == MODE_OFF) {
        off();
    }
    
    Serial.print("[LED] Mode changed to: ");
    Serial.println(mode);
}

LEDMode LEDController::getMode() {
    return currentMode;
    }

    void LEDController::setBrightness(uint8_t newBrightness) {
    brightness = newBrightness;
    FastLED.setBrightness(brightness);
    
    Serial.print("[LED] Brightness set to: ");
    Serial.println(brightness);
}

void LEDController::update() {
    // Execute current mode
    switch(currentMode) {
        case MODE_OFF:
        // LEDs stay off
        break;
        case MODE_SKY_SIMULATION:
        skySimulationEffect();
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
        case MODE_BASIC:
        // Ensure LEDs reflect current custom color
        for(int i = 0; i < NUM_LEDS; i++) leds[i] = customColor;
        break;
    }
    
    FastLED.show();
}

void LEDController::off() {
    FastLED.clear();
    FastLED.show();
}

void LEDController::setCustomColor(uint8_t r, uint8_t g, uint8_t b) {
    customColor = CRGB(r,g,b);
    for(int i = 0; i < NUM_LEDS; i++) leds[i] = customColor;
    currentMode = MODE_BASIC;
}

void LEDController::initConvolution() {
    for(int i = 0; i < NUM_LEDS; i++) {
        sunSignal[i] = 0;
        convolvedSignal[i] = 0;
    }
    buildSunKernel(sunKernelRadius);
}

void LEDController::buildSunKernel(int radius) {
    for(int i = 0; i < NUM_LEDS; i++) sunKernel[i] = 0.0f;
    if(radius < 1) { sunKernel[0] = 1.0f; return; }
    for(int d = -radius; d <= radius; d++) {
        float w = 0.5f * (1.0f + cos((PI * d) / radius)); // Hann shape
        sunKernel[d + radius] = w; // temporary contiguous storage if needed
    }
    // Normalize peak and overall energy (optional)
    float maxVal = 0.0f;
    for(int d = -radius; d <= radius; d++) {
        float val = sunKernel[d + radius];
        if(val > maxVal) maxVal = val;
    }
    if(maxVal > 0) {
        for(int d = -radius; d <= radius; d++) sunKernel[d + radius] /= maxVal;
    }
}

int LEDController::getSunPositionIndex(float hourFloat) {
    // Map daylight hours (6 -> 18) to full rotation around cylinder
    // At night we still return a position but intensity will be 0
    float wrappedHour = fmod(hourFloat + 24.0f, 24.0f);
    float daylightSpanStart = 6.0f;
    float daylightSpanEnd = 18.0f; // inclusive-ish
    float span = daylightSpanEnd - daylightSpanStart;
    float t = (wrappedHour - daylightSpanStart) / span; // 0..1 during day
    if(t < 0) t += 1.0f; // before 6 -> treat as night
    // Use continuous rotation irrespective of day/night so sun keeps moving
    float rotationFraction = fmod(wrappedHour / 24.0f, 1.0f); // 0..1 over 24h
    int idx = (int)(rotationFraction * NUM_LEDS) % NUM_LEDS;
    return idx;
}

float LEDController::getSunIntensity(float hourFloat) {
    // Simple elevation curve: zero at night, cosine arch during daylight
    if(hourFloat < 6.0f || hourFloat > 18.0f) return 0.0f;
    float t = (hourFloat - 6.0f) / 12.0f; // 0 at 6h, 1 at 18h
    // Elevation approximated with sine: peak at midday (t=0.5)
    float elev = sin(PI * t); // 0..1
    if(elev < 0) elev = 0;
    return elev;
}

void LEDController::applySunConvolution(int centerIndex, float amplitude) {
    // Clear previous signals
    for(int i = 0; i < NUM_LEDS; i++) {
        sunSignal[i] = 0.0f;
        convolvedSignal[i] = 0.0f;
    }
    sunSignal[centerIndex] = amplitude;
    int R = sunKernelRadius;
    for(int led = 0; led < NUM_LEDS; led++) {
        float acc = 0.0f;
        for(int d = -R; d <= R; d++) {
        int src = (led - d + NUM_LEDS) % NUM_LEDS;
        float k = sunKernel[d + R];
        acc += sunSignal[src] * k;
        }
        convolvedSignal[led] = acc;
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

void LEDController::skySimulationEffect() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    float hourFloat = timeinfo->tm_hour + timeinfo->tm_min / 60.0;
    float sunTemp = getSunColorTemp(hourFloat);
    int sunIndex = getSunPositionIndex(hourFloat);
    float sunIntensity = getSunIntensity(hourFloat);
    applySunConvolution(sunIndex, sunIntensity);
    
    CRGB ambient;
    if (sunTemp > 0) {
        SkyColor skyColor = colorTempToRGB(sunTemp);
        ambient = CRGB(skyColor.r, skyColor.g, skyColor.b);
    } else {
        ambient = CRGB(0, 0, 10);
    }

    SkyColor sunColor = colorTempToRGB(max(sunTemp, 2000.0f));

    for(int i = 0; i < NUM_LEDS; i++) {
        float highlight = convolvedSignal[i];
        float mixR = ambient.r + highlight * (sunColor.r - ambient.r);
        float mixG = ambient.g + highlight * (sunColor.g - ambient.g);
        float mixB = ambient.b + highlight * (sunColor.b - ambient.b);
        leds[i] = CRGB((uint8_t)mixR, (uint8_t)mixG, (uint8_t)mixB);
    }

    static unsigned long lastLog = 0;
    unsigned long nowMs = millis();
    if(nowMs - lastLog > 5000) {
        Serial.print("[LED] Sun idx:"); Serial.print(sunIndex);
        Serial.print(" intensity:"); Serial.print(sunIntensity, 3);
        Serial.print(" temp:"); Serial.println(sunTemp);
        lastLog = nowMs;
    }
}

void LEDController::rainEffect() {
    static unsigned long lastLightning = 0;
    static bool lightningActive = false;
    static int lightningBrightness = 0;
    static int lightningPosition = 0;
    
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(6, 6, 6);
        leds[i].r += random(-1, 2);
        leds[i].g += random(-1, 2);
        leds[i].b += random(-1, 2);
    }
    
    if(random(100) < 30) {
        int pos = random(NUM_LEDS);
        leds[pos] = CRGB(2, 2, 3);
    }
    
    unsigned long now = millis();
    if(!lightningActive && (now - lastLightning > random(3000, 8000))) {
        lightningActive = true;
        lightningBrightness = 180;
        lightningPosition = random(30, 58);
        lastLightning = now;
    }
    
    if(lightningActive) {
        for(int i = 0; i < 3; i++) {
            int ledIndex = (lightningPosition + i) % NUM_LEDS;
            leds[ledIndex] = CRGB(lightningBrightness, lightningBrightness, lightningBrightness + 20);
        }
        
        lightningBrightness -= 5;
        if(lightningBrightness <= 0) {
            lightningActive = false;
        }
    }
}

void LEDController::meteorEffect() {
    static int meteorPos[3] = {0, 20, 40};
    static int meteorSpeed[3] = {2, 3, 2};
    
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i].fadeToBlackBy(64);
    }
    
    for(int m = 0; m < 3; m++) {
        if(meteorPos[m] < NUM_LEDS) {
        leds[meteorPos[m]] = CRGB(255, 200, 100);
        
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
