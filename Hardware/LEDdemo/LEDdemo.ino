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
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Physical constants for black body radiation
const float h = 6.626e-34;  // Planck's constant (J·s)
const float c = 3.0e8;      // Speed of light (m/s)
const float k = 1.381e-23;  // Boltzmann constant (J/K)

// Convolution kernel setup
#define KERNEL_SIZE 17
float cosineKernel[KERNEL_SIZE];
int kernelHalf = KERNEL_SIZE / 2;
float sunSignal[NUM_LEDS];
float convolvedSignal[NUM_LEDS];

struct SkyColor {
  uint8_t r, g, b;
};

// Planck's law for spectral radiance (simplified)
// Returns relative intensity at wavelength lambda (nm) for temperature T (K)
float planckRadiance(float lambda_nm, float T) {
  float lambda = lambda_nm * 1e-9; // Convert nm to meters
  
  // Simplified Planck's law (proportional to actual)
  float exp_term = (h * c) / (lambda * k * T);
  
  // Avoid overflow for very large exponents
  if (exp_term > 50) return 0;
  
  float denominator = exp(exp_term) - 1.0;
  if (denominator < 1e-10) return 0;
  
  return 1.0 / (pow(lambda, 5) * denominator);
}

// Convert color temperature (K) to RGB using black body radiation
SkyColor colorTempToRGB(float temp) {
  // Sample wavelengths for RGB (approximate peak sensitivities)
  float lambda_r = 700; // nm (red)
  float lambda_g = 546; // nm (green)
  float lambda_b = 436; // nm (blue)
  
  // Get relative intensities from Planck's law
  float I_r = planckRadiance(lambda_r, temp);
  float I_g = planckRadiance(lambda_g, temp);
  float I_b = planckRadiance(lambda_b, temp);
  
  // Find maximum for normalization
  float maxI = max(max(I_r, I_g), I_b);
  
  // Normalize to 0-1 range
  float r_norm = I_r / maxI;
  float g_norm = I_g / maxI;
  float b_norm = I_b / maxI;
  
  // Apply gamma correction (2.2) for perceptual uniformity
  r_norm = pow(r_norm, 1.0/2.2);
  g_norm = pow(g_norm, 1.0/2.2);
  b_norm = pow(b_norm, 1.0/2.2);
  
  SkyColor color;
  color.r = (uint8_t)(r_norm * 255);
  color.g = (uint8_t)(g_norm * 255);
  color.b = (uint8_t)(b_norm * 255);
  
  return color;
}

// Get color temperature for sun based on time of day
float getSunColorTemp(float hourFloat) {
  // Sun's actual surface temperature is ~5777K (from article)
  // But atmospheric scattering changes apparent color temperature
  
  if (hourFloat < 6.0 || hourFloat > 18.5) {
    return 0; // No sun
  }
  else if (hourFloat < 7.0) {
    // Sunrise: 2000K (warm orange) to 4000K
    float t = (hourFloat - 6.0);
    return 2000 + 2000 * t;
  }
  else if (hourFloat < 8.0) {
    // Early morning: 4000K to 5000K
    float t = (hourFloat - 7.0);
    return 4000 + 1000 * t;
  }
  else if (hourFloat < 16.0) {
    // Midday: 5500K (slightly warm) to 6000K (neutral white)
    return 5777; // Actual solar surface temperature from article
  }
  else if (hourFloat < 17.5) {
    // Late afternoon: 6000K to 4000K
    float t = (hourFloat - 16.0) / 1.5;
    return 6000 - 2000 * t;
  }
  else {
    // Sunset: 4000K to 1800K (deep red)
    float t = (hourFloat - 17.5);
    return 4000 - 2200 * t;
  }
}

// Get sky ambient color temperature
float getSkyColorTemp(float hourFloat) {
  // Sky color is from Rayleigh scattering + ambient light
  
  if (hourFloat < 5.0 || hourFloat >= 21.0) {
    return 0; // Night - return 0 to use special night color
  }
  else if (hourFloat < 6.0) {
    // Pre-dawn: Deep blue (~10000K) warming up
    return 10000;
  }
  else if (hourFloat < 7.0) {
    // Dawn: Sky warms from scattered light
    float t = (hourFloat - 6.0);
    return 8000 - 2000 * t;
  }
  else if (hourFloat < 10.0) {
    // Morning: Clear sky blue (8000-10000K)
    return 9000;
  }
  else if (hourFloat < 15.0) {
    // Midday: Pure sky blue (10000-12000K)
    return 11000;
  }
  else if (hourFloat < 17.0) {
    // Afternoon: Sky stays blue but warmer
    return 9000;
  }
  else if (hourFloat < 18.0) {
    // Pre-sunset: Sky warming
    float t = (hourFloat - 17.0);
    return 9000 - 3000 * t;
  }
  else if (hourFloat < 19.0) {
    // Sunset: Warm sky colors
    float t = (hourFloat - 18.0);
    return 6000 - 2000 * t;
  }
  else {
    // Dusk: Deep blue again
    return 8000;
  }
}

// Get base sky color with physical modeling
SkyColor getSkyBaseColor(float hourFloat) {
  float skyTemp = getSkyColorTemp(hourFloat);
  
  // Special handling for night (no black body radiation applies)
  if (skyTemp == 0) {
    SkyColor nightColor = {5, 5, 25}; // Deep blue
    return nightColor;
  }
  
  // Use black body radiation for sky color
  SkyColor skyColor = colorTempToRGB(skyTemp);
  
  // Adjust intensity based on time of day
  float intensity = 1.0;
  
  if (hourFloat < 6.0) {
    intensity = 0.1;
  }
  else if (hourFloat < 7.0) {
    intensity = 0.3 + 0.4 * (hourFloat - 6.0);
  }
  else if (hourFloat >= 18.5 && hourFloat < 21.0) {
    intensity = 0.7 - 0.6 * ((hourFloat - 18.5) / 2.5);
  }
  
  skyColor.r *= intensity;
  skyColor.g *= intensity;
  skyColor.b *= intensity;
  
  return skyColor;
}

// Initialize cosine kernel for convolution
void initCosineKernel() {
  float sum = 0;
  for (int i = 0; i < KERNEL_SIZE; i++) {
    int offset = i - kernelHalf;
    cosineKernel[i] = (cos(PI * offset / kernelHalf) + 1.0) / 2.0;
    sum += cosineKernel[i];
  }
  // Normalize kernel
  for (int i = 0; i < KERNEL_SIZE; i++) {
    cosineKernel[i] /= sum;
  }
}

// Create sun signal (impulse at sun position)
void createSunSignal(float sunPos, float hourFloat) {
  // Clear signal
  for (int i = 0; i < NUM_LEDS; i++) {
    sunSignal[i] = 0;
  }
  
  // Sun intensity based on time (follows Stefan-Boltzmann: intensity ∝ T^4)
  float sunTemp = getSunColorTemp(hourFloat);
  if (sunTemp == 0) return;
  
  // Relative intensity (normalized to midday)
  float sunIntensity = pow(sunTemp / 5777.0, 4);
  
  // Limit intensity for sunset/sunrise
  if (hourFloat < 7.0 || hourFloat > 17.5) {
    sunIntensity *= 0.6;
  }
  
  // Place impulse at sun position (with interpolation)
  int sunIdx = (int)sunPos;
  float frac = sunPos - sunIdx;
  
  if (sunIdx >= 0 && sunIdx < NUM_LEDS) {
    sunSignal[sunIdx] = sunIntensity * (1.0 - frac);
  }
  if (sunIdx + 1 >= 0 && sunIdx + 1 < NUM_LEDS) {
    sunSignal[sunIdx + 1] = sunIntensity * frac;
  }
}

// Discrete convolution
void convolveSignal() {
  for (int i = 0; i < NUM_LEDS; i++) {
    convolvedSignal[i] = 0;
    
    for (int k = 0; k < KERNEL_SIZE; k++) {
      int idx = i - k + kernelHalf;
      
      if (idx >= 0 && idx < NUM_LEDS) {
        convolvedSignal[i] += sunSignal[idx] * cosineKernel[k];
      }
    }
  }
}

// Calculate sun position (0 = left, NUM_LEDS-1 = right)
float getSunPosition(float hourFloat) {
  if (hourFloat < 6.0 || hourFloat > 18.5) {
    return -10; // Below horizon
  }
  
  float sunProgress = (hourFloat - 6.0) / 12.5;
  return sunProgress * (NUM_LEDS - 1);
}

// Apply convolved sun glow using black body radiation
void applySunGlow(float sunPos, float hourFloat) {
  float sunTemp = getSunColorTemp(hourFloat);
  if (sunTemp == 0) return;
  
  // Get sun color from black body radiation
  SkyColor sunColor = colorTempToRGB(sunTemp);
  
  // Create and convolve sun signal
  createSunSignal(sunPos, hourFloat);
  convolveSignal();
  
  // Apply convolved glow to LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    float glow = convolvedSignal[i];
    
    if (glow > 0.01) {
      leds[i].r = min(255, (int)(leds[i].r * (1-glow) + sunColor.r * glow));
      leds[i].g = min(255, (int)(leds[i].g * (1-glow) + sunColor.g * glow));
      leds[i].b = min(255, (int)(leds[i].b * (1-glow) + sunColor.b * glow));
    }
  }
}

// Add atmospheric scattering variation
void addAtmosphericVariation(float hourFloat) {
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
  
  // Get base sky color from black body radiation
  SkyColor baseColor = getSkyBaseColor(hourFloat);
  
  // Fill all LEDs with base color
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(baseColor.r, baseColor.g, baseColor.b);
  }
  
  // Get sun position and apply glow using convolution
  float sunPos = getSunPosition(hourFloat);
  applySunGlow(sunPos, hourFloat);
  
  // Add atmospheric variation
  addAtmosphericVariation(hourFloat);
  
  // Update LEDs
  FastLED.show();
  
  // Debug output
  float sunTemp = getSunColorTemp(hourFloat);
  float skyTemp = getSkyColorTemp(hourFloat);
  Serial.printf("Time: %02d:%02d:%02d | Sun: %.0fK | Sky: %.0fK | Pos: %.1f\n", 
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
                sunTemp, skyTemp, sunPos);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize cosine kernel for convolution
  initCosineKernel();
  
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
  
  Serial.println("Physics-based sky simulator started!");
  Serial.println("Using Planck's law for black body radiation colors");
}

void loop() {
  updateSky();
  delay(1000); // Update every second
}