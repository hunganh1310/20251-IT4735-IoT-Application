#include <FastLED.h>

#define LED_PIN     5       // Data pin to LED strip
#define NUM_LEDS    120     // Change to your number of LEDs
#define LED_TYPE    WS2812B // WS2813 works with same driver macros
#define COLOR_ORDER GRB
#define BRIGHTNESS  160     // Start here; reduce if power limited

CRGB leds[NUM_LEDS];

// Effect selection
enum Effect { RAIN, METEOR, APOCALYPSE };
Effect currentEffect = RAIN;

// Timing & control
unsigned long lastSwitch = 0;
const unsigned long switchInterval = 20000UL; // auto-cycle every 20s (optional)
unsigned long nowMs = 0;

// ---- RAIN effect state ----
struct Drop { int pos; uint8_t hue; uint8_t brightness; bool alive; };
const int MAX_DROPS = 18;
Drop drops[MAX_DROPS];
unsigned long lastRainSpawn = 0;
const unsigned long rainSpawnInterval = 120; // spawn attempt interval (ms)
const uint8_t rainFade = 24; // fade applied each tick

// ---- METEOR effect state ----
int meteorPos = -1;
int meteorDir = 1;
int meteorLength = 16;
unsigned long lastMeteorMove = 0;
const unsigned long meteorSpeed = 20; // ms per pixel step
CRGB meteorColor = CRGB::White;

// ---- APOCALYPSE effect state ----
uint16_t noiseIndex = 0;
unsigned long lastApocTick = 0;
const unsigned long apocTick = 40;
CRGBPalette16 firePal;
bool apocStrobe = false;
unsigned long strobeEnd = 0;

// utility
void setupPalettes() {
  // create a warm "fire-like" palette
    firePal = CRGBPalette16(
        CRGB::Black,
        CRGB(4,4,12),
        CRGB(40,8,0),
        CRGB(120,24,0),
        CRGB(200,80,0),
        CRGB(255,180,50),
        CRGB::White,
        CRGB::White
    );
}

void setup() {
    delay(200);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    randomSeed((uint32_t)esp_random());
    setupPalettes();

    // initialize drops
    for (int i = 0; i < MAX_DROPS; ++i) { drops[i].alive = false; }
}

void loop() {
    nowMs = millis();

  // Optionally auto-cycle effects
    if (nowMs - lastSwitch > switchInterval) {
        lastSwitch = nowMs;
        if (currentEffect == RAIN) currentEffect = METEOR;
        else if (currentEffect == METEOR) currentEffect = APOCALYPSE;
        else currentEffect = RAIN;
    }

    switch (currentEffect) {
        case RAIN: rainEffect(); break;
        case METEOR: meteorEffect(); break;
        case APOCALYPSE: apocalypseEffect(); break;
    }

  FastLED.show();
  // small baseline delay to avoid starving WiFi or other tasks; keep short
  FastLED.delay(16);
}

// ---------------- RAIN ----------------
void spawnRainDrop() {
  // find dead slot
    for (int i = 0; i < MAX_DROPS; ++i) {
        if (!drops[i].alive) {
            drops[i].alive = true;
            drops[i].pos = 0; // top of strip (index 0)
            drops[i].hue = random8(160, 220); // bluish hues
            drops[i].brightness = random8(180, 255);
            return;
        }
    }
}

void rainEffect() {
  // fade whole strip slightly to create trails
    fadeToBlackBy(leds, NUM_LEDS, rainFade);

  // spawn logic
    if (nowMs - lastRainSpawn >= rainSpawnInterval) {
        lastRainSpawn = nowMs;
        if (random8() < 220) { // chance to spawn
            spawnRainDrop();
        }
    }

  // advance drops
    for (int i = 0; i < MAX_DROPS; ++i) {
        if (!drops[i].alive) continue;
        // draw drop
        int p = drops[i].pos;
        if (p >= 0 && p < NUM_LEDS) {
            // draw a little head plus faint trail
            leds[p] += CHSV(drops[i].hue, 220, drops[i].brightness);
            if (p+1 < NUM_LEDS) leds[p+1] += CHSV(drops[i].hue, 160, drops[i].brightness / 2);
        }
        // move
        drops[i].pos += 1 + (random8()<40 ? 1 : 0); // occasional double-step
        // if reached end, mark dead and maybe spawn splash
        if (drops[i].pos >= NUM_LEDS) {
            drops[i].alive = false;
            // splash: scatter a few pixels near bottom
            int base = NUM_LEDS - 3;
            for (int s = 0; s < 6; ++s) {
                int idx = base + random8(0, 3);
                if (idx >= 0 && idx < NUM_LEDS) leds[idx] += CHSV(drops[i].hue, 255, random8(40,160));
            }
        }
    }
}

// ---------------- METEOR ----------------
void meteorEffect() {
    // fade slightly every frame (trail)
    fadeToBlackBy(leds, NUM_LEDS, 64);

    if (nowMs - lastMeteorMove >= meteorSpeed) {
        lastMeteorMove = nowMs;
        // move
        meteorPos += meteorDir;
        if (meteorPos - meteorLength > NUM_LEDS || meteorPos < -10) {
            // reset: start on left or right
            meteorDir = (random8() < 128) ? 1 : -1;
            meteorLength = random8(8, 28);
            meteorPos = (meteorDir == 1) ? -meteorLength : NUM_LEDS + meteorLength;
            meteorColor = CHSV(random8(160,255), 255, 255); // bright white/blue-ish or other
        }
        // draw meteor head + trail
        for (int i = 0; i < meteorLength; ++i) {
            int idx = meteorPos - i*meteorDir;
            if (idx >= 0 && idx < NUM_LEDS) {
            // trailing fade by distance
            uint8_t b = 255 - (i * (200 / max(1, meteorLength)));
            leds[idx] += meteorColor.nscale8_video(b);
            }
        }
    }

    // occasional sparkles behind meteor
    if (random8() < 20) {
        int spark = constrain(meteorPos - random8(0, meteorLength*2)*meteorDir, 0, NUM_LEDS-1);
        leds[spark] += CHSV(random8(0,255), 220, random8(120,255));
    }
}

// ---------------- APOCALYPSE ----------------
void apocalypseEffect() {
    // base ember/fire using Perlin-like noise (simple)
    if (nowMs - lastApocTick >= apocTick) {
        lastApocTick = nowMs;
        noiseIndex += 7;

        for (int i = 0; i < NUM_LEDS; ++i) {
            uint16_t n = inoise8((i * 10) + noiseIndex, noiseIndex/2); // pseudo-noise
            uint8_t index = n >> 2; // scale to 0..63
            uint8_t bri = qadd8(n, random8(30)); // add flicker
            CRGB c = ColorFromPalette(firePal, index, bri, LINEARBLEND);
            // dim toward the top to mimic glowing ground
            float heightFactor = 1.0 - (float)i / (float)NUM_LEDS;
            leds[i] = c.nscale8_video((uint8_t)(bri * (0.5 + 0.5*heightFactor)));
        }

        // occasional ember burst or spark
        if (random8() < 18) {
            int emberPos = random16(NUM_LEDS);
            int spread = random8(4, 12);
            for (int j = -spread; j <= spread; ++j) {
                int idx = emberPos + j;
                if (idx >= 0 && idx < NUM_LEDS) {
                    leds[idx] += CHSV(10 + random8(40), 255, random8(100,255));
                }
            }
        }

        // random electrical glitch: brief bluish-white flashes
        if (random8() < 6 && !apocStrobe) {
            apocStrobe = true;
            strobeEnd = nowMs + random16(80, 220);
            // apply immediate flash
            for (int i = 0; i < NUM_LEDS; i += random8(3,8)) {
                leds[i] = CRGB::White;
            }
        }
    }

  // handle strobe timing
    if (apocStrobe) {
        if (nowMs < strobeEnd) {
            // flash persists, fade between intervals
            fadeToBlackBy(leds, NUM_LEDS, 40);
        } else {
            apocStrobe = false;
        }
    }

  // occasional heavy flicker (like explosions)
    if (random8() < 4) {
        int center = random16(NUM_LEDS);
        int radius = random8(4, 18);
        for (int i = -radius; i <= radius; ++i) {
            int idx = center + i;
            if (idx >= 0 && idx < NUM_LEDS) {
            leds[idx] = leds[idx] + CHSV(random8(10,30), 255, random8(120,255));
            }
        }
    }
}
