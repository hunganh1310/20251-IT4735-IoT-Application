/**
 * @file test_led_visual.cpp
 * @brief Visual LED Controller Test
 * 
 * This is a manual test program that cycles through all LED modes
 * for visual verification. Upload this to see each effect in action.
 * 
 * To use this test:
 * 1. Comment out main.cpp in platformio.ini
 * 2. Upload this file
 * 3. Watch the LED effects cycle through
 */

#include <Arduino.h>
#include "led_controller.h"
#include "config.h"

LEDController ledController;

// Test configuration
const unsigned long MODE_DURATION = 10000; // 10 seconds per mode
const unsigned long TEST_CYCLE_DELAY = 2000; // 2 seconds between modes

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n====================================");
    Serial.println("LED Controller Visual Test");
    Serial.println("====================================\n");
    
    // Initialize LED controller
    if (!ledController.init()) {
        Serial.println("[ERROR] LED Controller initialization failed!");
        while(1) { delay(1000); }
    }
    
    Serial.println("[OK] LED Controller initialized successfully\n");
    Serial.println("This test will cycle through all LED modes.");
    Serial.println("Each mode will display for 10 seconds.\n");
    Serial.println("Press RESET to restart the test.\n");
}

void loop() {
    static int testPhase = 0;
    static unsigned long lastChange = 0;
    unsigned long now = millis();
    
    // Check if it's time to change mode
    if (now - lastChange >= MODE_DURATION) {
        lastChange = now;
        
        switch(testPhase) {
            case 0:
                Serial.println("\n[TEST 1/8] MODE_OFF - All LEDs should be off");
                ledController.setMode(MODE_OFF);
                break;
                
            case 1:
                Serial.println("\n[TEST 2/8] MODE_SKY_SIMULATION - Sky color simulation");
                Serial.println("  - Should show time-based sky colors");
                Serial.println("  - Colors change based on time of day");
                ledController.setMode(MODE_SKY_SIMULATION);
                break;
                
            case 2:
                Serial.println("\n[TEST 3/8] MODE_RAIN - Stormy weather effect");
                Serial.println("  - Dark blue-gray base color");
                Serial.println("  - Random raindrops");
                Serial.println("  - Lightning flashes");
                ledController.setMode(MODE_RAIN);
                break;
                
            case 3:
                Serial.println("\n[TEST 4/8] MODE_METEOR - Meteor shower effect");
                Serial.println("  - Orange/yellow meteors");
                Serial.println("  - Trailing tails");
                Serial.println("  - Multiple meteors moving");
                ledController.setMode(MODE_METEOR);
                break;
                
            case 4:
                Serial.println("\n[TEST 5/8] MODE_APOCALYPSE - Fire/smoke effect");
                Serial.println("  - Flickering orange/red flames");
                Serial.println("  - Dark smoke patches");
                ledController.setMode(MODE_APOCALYPSE);
                break;
                
            case 5:
                Serial.println("\n[TEST 6/8] MODE_BASIC - Solid Red");
                ledController.setCustomColor(255, 0, 0);
                break;
                
            case 6:
                Serial.println("\n[TEST 7/8] MODE_BASIC - Solid Green");
                ledController.setCustomColor(0, 255, 0);
                break;
                
            case 7:
                Serial.println("\n[TEST 8/8] MODE_BASIC - Solid Blue");
                ledController.setCustomColor(0, 0, 255);
                break;
                
            case 8:
                Serial.println("\n[TEST COMPLETE] Brightness test");
                Serial.println("  - Cycling through brightness levels");
                ledController.setCustomColor(255, 255, 255);
                
                // Fade brightness up and down
                for(int brightness = 0; brightness <= 255; brightness += 5) {
                    ledController.setBrightness(brightness);
                    ledController.update();
                    delay(20);
                }
                for(int brightness = 255; brightness >= 0; brightness -= 5) {
                    ledController.setBrightness(brightness);
                    ledController.update();
                    delay(20);
                }
                
                // Reset to default
                ledController.setBrightness(DEFAULT_BRIGHTNESS);
                
                Serial.println("\n====================================");
                Serial.println("All tests complete!");
                Serial.println("Restarting test cycle in 5 seconds...");
                Serial.println("====================================\n");
                
                delay(5000);
                testPhase = -1; // Will become 0 after increment
                break;
        }
        
        testPhase++;
    }
    
    // Update LED display
    ledController.update();
    delay(10); // Small delay to prevent overwhelming the system
}

/**
 * Visual Test Checklist:
 * 
 * [ ] MODE_OFF - All LEDs completely off
 * [ ] SKY_SIMULATION - Smooth color transitions based on time
 * [ ] RAIN - Dark stormy background with lightning flashes
 * [ ] METEOR - Multiple moving streaks with trails
 * [ ] APOCALYPSE - Flickering fire effect with smoke
 * [ ] BASIC Red - Solid red color across all LEDs
 * [ ] BASIC Green - Solid green color across all LEDs
 * [ ] BASIC Blue - Solid blue color across all LEDs
 * [ ] Brightness - Smooth fade from dark to bright and back
 * 
 * If all items check out, the LED controller is working correctly!
 */
