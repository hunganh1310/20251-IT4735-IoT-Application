/**
 * @file config.example.h
 * @brief Example configuration file for IoT Application
 * 
 * INSTRUCTIONS:
 * 1. Copy this file to config.h
 * 2. Replace placeholder values with your actual credentials
 * 3. DO NOT commit config.h to version control
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== WiFi Configuration ====================
#define WIFI_SSID "YOUR_WIFI_SSID"          // Your WiFi network name
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"  // Your WiFi password
#define WIFI_TIMEOUT 10000                  // Connection timeout in ms

// ==================== Access Point Configuration ====================
#define AP_SSID "ESP32-IoT-Config"          // AP name for WiFi configuration mode
#define AP_PASSWORD ""                       // AP password (empty = open)
#define WIFI_CONNECT_TIMEOUT 15000          // WiFi connection timeout

// ==================== MQTT Server Configuration ====================
#define MQTT_SERVER "YOUR_MQTT_BROKER.hivemq.cloud"  // MQTT broker address
#define MQTT_PORT 8883                               // MQTT port (8883 for TLS)
#define MQTT_USER "YOUR_MQTT_USERNAME"               // MQTT username
#define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"           // MQTT password
#define MQTT_CLIENT_ID "Device_01"                   // Unique device identifier

// ==================== LED Configuration ====================
#define LED_PIN 18                  // WS2812 LED data pin
#define NUM_LEDS 60                 // Number of LEDs in strip
#define LED_TYPE WS2812             // LED type
#define COLOR_ORDER GRB             // Color order
#define DEFAULT_BRIGHTNESS 128      // Default brightness (0-255)

// ==================== SSD1306 OLED Configuration ====================
#define OLED_SDA 22                 // I2C SDA pin
#define OLED_SCL 23                 // I2C SCL pin
#define SCREEN_WIDTH 128            // OLED width in pixels
#define SCREEN_HEIGHT 64            // OLED height in pixels
#define OLED_RESET -1               // Reset pin (-1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C         // I2C address

// ==================== LD2410B Radar Sensor Pins ====================
#define LD_RX_PIN 16                // Connect to LD2410B TX
#define LD_TX_PIN 17                // Connect to LD2410B RX
#define LD_OUT_PIN 4                // Connect to LD2410B OUT

// ==================== Timing Configuration ====================
#define SENSOR_READ_INTERVAL 1000       // Read sensors every 1 second
#define DATA_SEND_INTERVAL 5000         // Send data to server every 5 seconds
#define NTP_UPDATE_INTERVAL 3600000     // Update time every hour
#define DISPLAY_UPDATE_INTERVAL 500     // Update OLED display every 500ms

// ==================== pH Sensor Configuration (Simulated) ====================
#define PH_MIN 6.9                  // Minimum pH value for simulation
#define PH_MAX 7.2                  // Maximum pH value for simulation

// ==================== Turbidity Sensor Configuration ====================
#define TURBIDITY_SENSOR_PIN 34     // Analog input pin for turbidity sensor

// ==================== DS18B20 Temperature Sensor Configuration ====================
#define DS18B20_PIN 21              // OneWire data pin

// ==================== NTP Configuration ====================
#define NTP_SERVER "pool.ntp.org"   // NTP server address
#define GMT_OFFSET_SEC 25200        // Vietnam UTC+7 (7 * 3600 seconds)
#define DAYLIGHT_OFFSET_SEC 0       // Daylight saving offset

#endif // CONFIG_H
