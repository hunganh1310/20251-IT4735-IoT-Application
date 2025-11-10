/**
 * @file config.h
 * @brief Global configuration for IoT Application
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== WiFi Configuration ====================
// WiFi credentials are loaded from .env file via build flags
// Create .env file from .env.example and set your credentials there
// WIFI_SSID and WIFI_PASSWORD are defined at compile time
#ifndef WIFI_SSID
#define WIFI_SSID "default_ssid"  // Fallback if .env not found
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "default_password"  // Fallback if .env not found
#endif
#define WIFI_TIMEOUT 10000  // Connection timeout in ms

// ==================== LED Configuration ====================
#define LED_PIN 18
#define NUM_LEDS 60
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define DEFAULT_BRIGHTNESS 128

// ==================== LD2410B Pins ====================
#define LD_RX_PIN 16    // connect to LD2410B TX
#define LD_TX_PIN 17    // connect to LD2410B RX
#define LD_OUT_PIN 4    // connect to LD2410B OUT

// ==================== Timing Configuration ====================
#define SENSOR_READ_INTERVAL 1000    // Read sensors every 1 second
#define DATA_SEND_INTERVAL 5000      // Send data to server every 5 seconds
#define NTP_UPDATE_INTERVAL 3600000  // Update time every hour

// ==================== NTP Configuration ====================
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 25200  // Vietnam UTC+7 (7 * 3600)
#define DAYLIGHT_OFFSET_SEC 0

#endif // CONFIG_H
