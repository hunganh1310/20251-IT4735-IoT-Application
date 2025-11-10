/**
 * @file test_led_controller.cpp
 * @brief Unit tests for LED Controller
 * 
 * This file contains unit tests for the LED controller functionality.
 * Tests are designed to run on embedded hardware with PlatformIO's Unity framework.
 * 
 * To run tests:
 * pio test -e <your_environment>
 */

#include <Arduino.h>
#include <unity.h>
#include <FastLED.h>
#include "led_controller.h"
#include "config.h"

// Test instance
LEDController* ledController = nullptr;

/**
 * @brief Setup function called before each test
 */
void setUp(void) {
    // Create a fresh instance for each test
    ledController = new LEDController();
    ledController->init();
}

/**
 * @brief Teardown function called after each test
 */
void tearDown(void) {
    if (ledController != nullptr) {
        ledController->off();
        delete ledController;
        ledController = nullptr;
    }
}

// ==================== Initialization Tests ====================

/**
 * @brief Test LED controller initialization
 */
void test_led_init(void) {
    LEDController testLed;
    bool initResult = testLed.init();
    
    TEST_ASSERT_TRUE(initResult);
    TEST_ASSERT_EQUAL(MODE_OFF, testLed.getMode());
}

/**
 * @brief Test default brightness setting
 */
void test_default_brightness(void) {
    LEDController testLed;
    testLed.init();
    
    // Brightness is private, but we can verify it doesn't crash
    testLed.setBrightness(DEFAULT_BRIGHTNESS);
    TEST_ASSERT_TRUE(true); // If we get here, no crash occurred
}

// ==================== Mode Setting Tests ====================

/**
 * @brief Test setting OFF mode
 */
void test_set_mode_off(void) {
    ledController->setMode(MODE_OFF);
    TEST_ASSERT_EQUAL(MODE_OFF, ledController->getMode());
}

/**
 * @brief Test setting SKY_SIMULATION mode
 */
void test_set_mode_sky_simulation(void) {
    ledController->setMode(MODE_SKY_SIMULATION);
    TEST_ASSERT_EQUAL(MODE_SKY_SIMULATION, ledController->getMode());
}

/**
 * @brief Test setting RAIN mode
 */
void test_set_mode_rain(void) {
    ledController->setMode(MODE_RAIN);
    TEST_ASSERT_EQUAL(MODE_RAIN, ledController->getMode());
}

/**
 * @brief Test setting METEOR mode
 */
void test_set_mode_meteor(void) {
    ledController->setMode(MODE_METEOR);
    TEST_ASSERT_EQUAL(MODE_METEOR, ledController->getMode());
}

/**
 * @brief Test setting APOCALYPSE mode
 */
void test_set_mode_apocalypse(void) {
    ledController->setMode(MODE_APOCALYPSE);
    TEST_ASSERT_EQUAL(MODE_APOCALYPSE, ledController->getMode());
}

/**
 * @brief Test setting BASIC mode
 */
void test_set_mode_basic(void) {
    ledController->setMode(MODE_BASIC);
    TEST_ASSERT_EQUAL(MODE_BASIC, ledController->getMode());
}

/**
 * @brief Test multiple mode changes
 */
void test_mode_switching(void) {
    ledController->setMode(MODE_RAIN);
    TEST_ASSERT_EQUAL(MODE_RAIN, ledController->getMode());
    
    ledController->setMode(MODE_METEOR);
    TEST_ASSERT_EQUAL(MODE_METEOR, ledController->getMode());
    
    ledController->setMode(MODE_OFF);
    TEST_ASSERT_EQUAL(MODE_OFF, ledController->getMode());
}

// ==================== Brightness Tests ====================

/**
 * @brief Test setting minimum brightness
 */
void test_brightness_minimum(void) {
    ledController->setBrightness(0);
    // No crash = pass
    TEST_ASSERT_TRUE(true);
}

/**
 * @brief Test setting maximum brightness
 */
void test_brightness_maximum(void) {
    ledController->setBrightness(255);
    // No crash = pass
    TEST_ASSERT_TRUE(true);
}

/**
 * @brief Test setting mid-range brightness
 */
void test_brightness_midrange(void) {
    ledController->setBrightness(128);
    // No crash = pass
    TEST_ASSERT_TRUE(true);
}

// ==================== Custom Color Tests ====================

/**
 * @brief Test setting custom color - Red
 */
void test_custom_color_red(void) {
    ledController->setCustomColor(255, 0, 0);
    TEST_ASSERT_EQUAL(MODE_BASIC, ledController->getMode());
}

/**
 * @brief Test setting custom color - Green
 */
void test_custom_color_green(void) {
    ledController->setCustomColor(0, 255, 0);
    TEST_ASSERT_EQUAL(MODE_BASIC, ledController->getMode());
}

/**
 * @brief Test setting custom color - Blue
 */
void test_custom_color_blue(void) {
    ledController->setCustomColor(0, 0, 255);
    TEST_ASSERT_EQUAL(MODE_BASIC, ledController->getMode());
}

/**
 * @brief Test setting custom color - White
 */
void test_custom_color_white(void) {
    ledController->setCustomColor(255, 255, 255);
    TEST_ASSERT_EQUAL(MODE_BASIC, ledController->getMode());
}

/**
 * @brief Test setting custom color - Black (off)
 */
void test_custom_color_black(void) {
    ledController->setCustomColor(0, 0, 0);
    TEST_ASSERT_EQUAL(MODE_BASIC, ledController->getMode());
}

// ==================== Effect Execution Tests ====================

/**
 * @brief Test sky simulation effect doesn't crash
 */
void test_sky_simulation_effect_runs(void) {
    ledController->setMode(MODE_SKY_SIMULATION);
    
    // Run update multiple times
    for(int i = 0; i < 10; i++) {
        ledController->update();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(true); // No crash
}

/**
 * @brief Test rain effect doesn't crash
 */
void test_rain_effect_runs(void) {
    ledController->setMode(MODE_RAIN);
    
    // Run update multiple times
    for(int i = 0; i < 10; i++) {
        ledController->update();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(true); // No crash
}

/**
 * @brief Test meteor effect doesn't crash
 */
void test_meteor_effect_runs(void) {
    ledController->setMode(MODE_METEOR);
    
    // Run update multiple times
    for(int i = 0; i < 10; i++) {
        ledController->update();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(true); // No crash
}

/**
 * @brief Test apocalypse effect doesn't crash
 */
void test_apocalypse_effect_runs(void) {
    ledController->setMode(MODE_APOCALYPSE);
    
    // Run update multiple times
    for(int i = 0; i < 10; i++) {
        ledController->update();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(true); // No crash
}

/**
 * @brief Test basic mode doesn't crash
 */
void test_basic_mode_runs(void) {
    ledController->setMode(MODE_BASIC);
    ledController->setCustomColor(100, 100, 100);
    
    // Run update multiple times
    for(int i = 0; i < 10; i++) {
        ledController->update();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(true); // No crash
}

/**
 * @brief Test OFF mode stays off
 */
void test_off_mode_stays_off(void) {
    ledController->setMode(MODE_OFF);
    
    // Run update multiple times
    for(int i = 0; i < 10; i++) {
        ledController->update();
        delay(10);
    }
    
    TEST_ASSERT_EQUAL(MODE_OFF, ledController->getMode());
}

// ==================== Stress Tests ====================

/**
 * @brief Test rapid mode switching
 */
void test_rapid_mode_switching(void) {
    LEDMode modes[] = {
        MODE_SKY_SIMULATION,
        MODE_RAIN,
        MODE_METEOR,
        MODE_APOCALYPSE,
        MODE_BASIC,
        MODE_OFF
    };
    
    for(int i = 0; i < 20; i++) {
        LEDMode mode = modes[i % 6];
        ledController->setMode(mode);
        ledController->update();
        TEST_ASSERT_EQUAL(mode, ledController->getMode());
        delay(5);
    }
    
    TEST_ASSERT_TRUE(true); // No crash
}

/**
 * @brief Test rapid brightness changes
 */
void test_rapid_brightness_changes(void) {
    for(int i = 0; i < 50; i++) {
        uint8_t brightness = (i * 5) % 256;
        ledController->setBrightness(brightness);
        delay(2);
    }
    
    TEST_ASSERT_TRUE(true); // No crash
}

/**
 * @brief Test continuous update loop
 */
void test_continuous_update(void) {
    ledController->setMode(MODE_SKY_SIMULATION);
    
    // Simulate 100 update cycles
    for(int i = 0; i < 100; i++) {
        ledController->update();
        delay(5);
    }
    
    TEST_ASSERT_EQUAL(MODE_SKY_SIMULATION, ledController->getMode());
}

// ==================== Edge Case Tests ====================

/**
 * @brief Test turning off from active effect
 */
void test_off_from_active_effect(void) {
    ledController->setMode(MODE_METEOR);
    ledController->update();
    
    ledController->setMode(MODE_OFF);
    TEST_ASSERT_EQUAL(MODE_OFF, ledController->getMode());
}

/**
 * @brief Test custom color overrides previous mode
 */
void test_custom_color_overrides_mode(void) {
    ledController->setMode(MODE_RAIN);
    TEST_ASSERT_EQUAL(MODE_RAIN, ledController->getMode());
    
    ledController->setCustomColor(128, 64, 32);
    TEST_ASSERT_EQUAL(MODE_BASIC, ledController->getMode());
}

/**
 * @brief Test multiple off() calls
 */
void test_multiple_off_calls(void) {
    ledController->off();
    ledController->off();
    ledController->off();
    
    TEST_ASSERT_TRUE(true); // No crash
}

// ==================== Main Test Runner ====================

/**
 * @brief Main test setup and execution
 */
void setup() {
    // Wait for Serial to be ready
    delay(2000);
    
    Serial.begin(115200);
    Serial.println("\n\n=================================");
    Serial.println("LED Controller Unit Tests");
    Serial.println("=================================\n");
    
    UNITY_BEGIN();
    
    // Initialization Tests
    RUN_TEST(test_led_init);
    RUN_TEST(test_default_brightness);
    
    // Mode Setting Tests
    RUN_TEST(test_set_mode_off);
    RUN_TEST(test_set_mode_sky_simulation);
    RUN_TEST(test_set_mode_rain);
    RUN_TEST(test_set_mode_meteor);
    RUN_TEST(test_set_mode_apocalypse);
    RUN_TEST(test_set_mode_basic);
    RUN_TEST(test_mode_switching);
    
    // Brightness Tests
    RUN_TEST(test_brightness_minimum);
    RUN_TEST(test_brightness_maximum);
    RUN_TEST(test_brightness_midrange);
    
    // Custom Color Tests
    RUN_TEST(test_custom_color_red);
    RUN_TEST(test_custom_color_green);
    RUN_TEST(test_custom_color_blue);
    RUN_TEST(test_custom_color_white);
    RUN_TEST(test_custom_color_black);
    
    // Effect Execution Tests
    RUN_TEST(test_sky_simulation_effect_runs);
    RUN_TEST(test_rain_effect_runs);
    RUN_TEST(test_meteor_effect_runs);
    RUN_TEST(test_apocalypse_effect_runs);
    RUN_TEST(test_basic_mode_runs);
    RUN_TEST(test_off_mode_stays_off);
    
    // Stress Tests
    RUN_TEST(test_rapid_mode_switching);
    RUN_TEST(test_rapid_brightness_changes);
    RUN_TEST(test_continuous_update);
    
    // Edge Case Tests
    RUN_TEST(test_off_from_active_effect);
    RUN_TEST(test_custom_color_overrides_mode);
    RUN_TEST(test_multiple_off_calls);
    
    UNITY_END();
}

/**
 * @brief Main loop - not used in testing
 */
void loop() {
    // Tests run once in setup()
    delay(1000);
}
