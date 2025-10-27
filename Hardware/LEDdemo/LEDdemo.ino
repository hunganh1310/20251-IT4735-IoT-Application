#include <FastLED.h>
#include <WiFi.h>
#include <time.h>

// LED Configuration
#define LED_PIN 5
#define NUM_LEDS 60
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define BRIGHTNESS 128

CRGB leds[NUM_LEDS];

// Vietnam timezone (UTC+7)
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

// WiFi credentials - replace with your network
const char* ssid = "BXT235";
const char* password = "321Thai@";

// Sky color presets for different times (RGB)
struct SkyColor {
  uint8_t r, g, b;
};

// Color temperature lookup based on time of day
SkyColor getSkyBaseColor(float hourFloat) {
  SkyColor color;
  
  // Night (0-5h, 21-24h): Deep blue
  if (hourFloat < 5.0 || hourFloat >= 21.0) {
    color = {5, 5, 25};
  }
  // Dawn (5-6h): Purple to orange
  else if (hourFloat < 6.0) {
    float t = (hourFloat - 5.0);
    color.r = 80 * t + 20 * (1-t);
    color.g = 40 * t + 5 * (1-t);
    color.b = 80 * t + 40 * (1-t);
  }
  // Sunrise (6-7h): Orange to yellow
  else if (hourFloat < 7.0) {
    float t = (hourFloat - 6.0);
    color.r = 255 * t + 150 * (1-t);
    color.g = 180 * t + 80 * (1-t);
    color.b = 100 * t + 30 * (1-t);
  }
  // Morning (7-10h): Yellow to light blue
  else if (hourFloat < 10.0) {
    float t = (hourFloat - 7.0) / 3.0;
    color.r = 135 * (1-t) + 135 * t;
    color.g = 206 * t + 180 * (1-t);
    color.b = 235 * t + 150 * (1-t);
  }
  // Midday (10-15h): Bright blue sky
  else if (hourFloat < 15.0) {
    color = {135, 206, 235};
  }
  // Afternoon (15-17h): Warm blue
  else if (hourFloat < 17.0) {
    float t = (hourFloat - 15.0) / 2.0;
    color.r = 135 + 40 * t;
    color.g = 206 - 40 * t;
    color.b = 235 - 50 * t;
  }
  // Sunset (17-18.5h): Orange to red
  else if (hourFloat < 18.5) {
    float t = (hourFloat - 17.0) / 1.5;
    color.r = 255;
    color.g = 150 - 70 * t;
    color.b = 100 - 80 * t;
  }
  // Dusk (18.5-21h): Purple to deep blue
  else {
    float t = (hourFloat - 18.5) / 2.5;
    color.r = 100 * (1-t) + 5 * t;
    color.g = 50 * (1-t) + 5 * t;
    color.b = 120 * (1-t) + 25 * t;
  }
  
  return color;
}

// Calculate sun position (0 = left edge, NUM_LEDS-1 = right edge)
// Sun rises at ~6h, sets at ~18h in Vietnam
float getSunPosition(float hourFloat) {
  // Map 6h-18h to 0-NUM_LEDS range
  if (hourFloat < 6.0 || hourFloat > 18.5) {
    return -10; // Sun below horizon
  }
  
  // Sun travels across sky from 6h to 18.5h
  float sunProgress = (hourFloat - 6.0) / 12.5;
  return sunProgress * (NUM_LEDS - 1);
}

// Cosine window function for sun glow
float cosineWindow(float distance, float width) {
  if (abs(distance) > width) return 0;
  return (cos(PI * distance / width) + 1.0) / 2.0;
}

// Apply sun glow to sky
void applySunGlow(float sunPos, float hourFloat) {
  // Sun intensity and color based on time
  SkyColor sunColor;
  float sunWidth = 8.0;
  float sunIntensity = 1.0;
  
  if (hourFloat >= 6.0 && hourFloat < 7.0) {
    // Sunrise: Orange-red
    sunColor = {255, 120, 40};
    sunIntensity = 0.5 + 0.5 * (hourFloat - 6.0);
  }
  else if (hourFloat >= 7.0 && hourFloat < 17.0) {
    // Daytime: Bright yellow-white
    sunColor = {255, 240, 200};
    sunIntensity = 1.0;
  }
  else if (hourFloat >= 17.0 && hourFloat <= 18.5) {
    // Sunset: Deep orange-red
    float t = (hourFloat - 17.0) / 1.5;
    sunColor.r = 255;
    sunColor.g = 100 - 40 * t;
    sunColor.b = 30 - 20 * t;
    sunIntensity = 1.0 - 0.3 * t;
  }
  else {
    return; // No sun glow at night
  }
  
  // Apply cosine window around sun position
  for (int i = 0; i < NUM_LEDS; i++) {
    float distance = i - sunPos;
    float glow = cosineWindow(distance, sunWidth) * sunIntensity;
    
    if (glow > 0) {
      leds[i].r = min(255, (int)(leds[i].r * (1-glow) + sunColor.r * glow));
      leds[i].g = min(255, (int)(leds[i].g * (1-glow) + sunColor.g * glow));
      leds[i].b = min(255, (int)(leds[i].b * (1-glow) + sunColor.b * glow));
    }
  }
}

// Add atmospheric scattering variation
void addAtmosphericVariation(float hourFloat) {
  // Add subtle variation to simulate atmospheric depth
  for (int i = 0; i < NUM_LEDS; i++) {
    float variation = sin(i * 0.3 + millis() * 0.0001) * 0.05 + 1.0;
    leds[i].r *= variation;
    leds[i].g *= variation;
    leds[i].b *= variation;
  }
}

void updateSky() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  
  float hourFloat = timeinfo.tm_hour + timeinfo.tm_min / 60.0 + timeinfo.tm_sec / 3600.0;
  
  // Get base sky color
  SkyColor baseColor = getSkyBaseColor(hourFloat);
  
  // Fill all LEDs with base color
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(baseColor.r, baseColor.g, baseColor.b);
  }
  
  // Get sun position and apply glow
  float sunPos = getSunPosition(hourFloat);
  applySunGlow(sunPos, hourFloat);
  
  // Add atmospheric variation
  addAtmosphericVariation(hourFloat);
  
  // Update LEDs
  FastLED.show();
  
  // Debug output
  Serial.printf("Time: %02d:%02d:%02d | Sun pos: %.1f | Color: R%d G%d B%d\n", 
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
                sunPos, baseColor.r, baseColor.g, baseColor.b);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize LED strip
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Connect to WiFi
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    
    // Configure time
    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      Serial.printf("Current time: %02d:%02d:%02d\n", 
                    timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }
  } else {
    Serial.println("\nWiFi connection failed! Using demo mode...");
  }
  
  Serial.println("Sky simulator started!");
}

void loop() {
  updateSky();
  delay(1000); // Update every second
}