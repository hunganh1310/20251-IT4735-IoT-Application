/**
 * @file config.h
 * @brief Global configuration for IoT Application
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== WiFi Configuration ====================
#define WIFI_SSID "BXT235"
#define WIFI_PASSWORD "321Thai@"
#define WIFI_TIMEOUT 10000  // Connection timeout in ms

// ==================== Server Configuration ====================
#define SERVER_HOST "192.168.1.100"  // Replace with your server IP
#define SERVER_PORT 8080
#define API_ENDPOINT "/api/data"

// ==================== LED Configuration ====================
#define LED_PIN 5
#define NUM_LEDS 60
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define DEFAULT_BRIGHTNESS 128

// ==================== Sensor Pins ====================
#define RADAR_RX_PIN 16
#define RADAR_TX_PIN 17
#define RADAR_OUT_PIN 4
#define TEMP_SENSOR_PIN 4
#define TURBIDITY_PIN 34  // ADC1 channel (GPIO34)

// ==================== Timing Configuration ====================
#define SENSOR_READ_INTERVAL 1000    // Read sensors every 1 second
#define DATA_SEND_INTERVAL 5000      // Send data to server every 5 seconds
#define NTP_UPDATE_INTERVAL 3600000  // Update time every hour

// ==================== NTP Configuration ====================
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 25200  // Vietnam UTC+7 (7 * 3600)
#define DAYLIGHT_OFFSET_SEC 0

// ==================== Debug Configuration ====================
#define DEBUG_MODE true
#define SERIAL_BAUD_RATE 115200

#endif // CONFIG_H
