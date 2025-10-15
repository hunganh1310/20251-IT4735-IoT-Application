#include <FastLED.h>

// --- LED Configuration ---
#define DATA_PIN 5     // The ESP32 pin connected to the data input of the WS2813 strip
#define NUM_LEDS 60     // Number of LEDs in your strip
#define BRIGHTNESS 128  // Max brightness (adjust this value)

// --- Simulated Environment Parameters ---
CRGB leds[NUM_LEDS];
unsigned long time_ms = 0;
// Cycle is now approx 3.5 hours long, making transitions much slower.
const float TIME_SCALE = 0.0000005; 
// Offset to push the starting phase to simulated midnight (cos(PI) = -1.0),
// ensuring the night color displays immediately upon startup.
const float START_PHASE_OFFSET = M_PI; 

// --- VIETNAM/TROPICAL SKY PARAMETER ADJUSTMENTS ---
// Higher humidity and aerosol content enhance Mie scattering (haze).
// This factor determines how much "whiteness" or haze is mixed into the sky,
// especially near the horizon (low sun_height).
const float HAZE_FACTOR = 0.45; 

// Base Colors adjusted for a Hazy/Tropical Sky
const CRGB COLOR_NOON_SKY = CRGB(0x3070FF); // Slightly less saturated blue
const CRGB COLOR_SUNRISE_SET = CRGB(0xFF7B00); // More Golden-Orange (less deep red)
const CRGB COLOR_NIGHT_SKY = CRGB(0x080020); // Deep Indigo
const CRGB COLOR_ZENITH_BRIGHT = CRGB(0xFFFFFF); // Brightest point

// --- Helper Functions ---

// Calculates the color based on the simulated sun's height (-1.0 to 1.0 for full cycle)
CRGB getSkyColor(float sun_height) {
    // sun_height will range from -1.0 (deep night) to 1.0 (noon)

    if (sun_height > 0.15) {
        // --- DAY TIME (Sun well above horizon) ---
        // Blend between the blue sky (0.15) and the bright zenith (1.0).
        float blend_factor = map(sun_height, 0.15, 1.0, 0.0, 1.0);
        
        // Blend a bright white for intensity with the sky blue for hue.
        CRGB day_color = blend(COLOR_NOON_SKY, COLOR_ZENITH_BRIGHT, blend_factor * 255);
        
        // Introduce haze/whiteness near the horizon (low blend_factor)
        float haze_blend = (1.0 - blend_factor) * HAZE_FACTOR;
        return blend(day_color, CRGB::White, haze_blend * 255);

    } else if (sun_height >= -0.15 && sun_height <= 0.15) {
        // --- SUNRISE/SUNSET (Sun near horizon) ---
        // Red/Orange/Golden dominates due to long light path and high Mie scatter.
        
        // Remap the range [-0.15, 0.15] to [0.0, 1.0] for blending
        float blend_factor = map(sun_height, -0.15, 0.15, 0.0, 1.0);
        
        CRGB temp_color;
        if (blend_factor < 0.5) {
            // From Night (0) to Sunset Peak (0.5)
            temp_color = blend(COLOR_NIGHT_SKY, COLOR_SUNRISE_SET, blend_factor * 510);
        } else {
            // From Sunset Peak (0.5) to Blue Sky (1.0)
            temp_color = blend(COLOR_SUNRISE_SET, COLOR_NOON_SKY, (blend_factor - 0.5) * 510);
        }
        return temp_color;
        
    } else {
        // --- NIGHT TIME (Sun below horizon) ---
        return COLOR_NIGHT_SKY;
    }
}

// Maps a floating point value from one range to another
float map(float value, float in_min, float in_max, float out_min, float out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// --- Main Arduino Sketch Functions ---

void setup() {
    FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    delay(500); 
}

void loop() {
    // 1. Update simulated time
    time_ms = millis();
    // Apply the offset to start the cycle at the desired phase (midnight)
    float cycle_angle = fmod(time_ms * TIME_SCALE + START_PHASE_OFFSET, TWO_PI); 

    // 2. Calculate the "Sun Height" factor
    float sun_height_factor = cos(cycle_angle);

    // 3. Get the resulting sky color
    CRGB current_sky_color = getSkyColor(sun_height_factor);

    // 4. Set all LEDs to the calculated color
    fill_solid(leds, NUM_LEDS, current_sky_color);

    // 5. Display the color and delay
    FastLED.show();
    delay(10); 
}
