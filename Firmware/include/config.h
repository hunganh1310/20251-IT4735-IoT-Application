/**
 * @file config.h
 * @brief Global configuration for IoT Application
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== WiFi Configuration ====================
#define WIFI_SSID "BXT235"  // Fallback if .env not found
#define WIFI_PASSWORD "321Thai@"  // Fallback if .env not found
#define WIFI_TIMEOUT 10000  // Connection timeout in ms

// ==================== MQTT Server Configuration ====================
#define MQTT_SERVER "990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "mqtt1"
#define MQTT_PASSWORD "Mqtt123456"
#define MQTT_CLIENT_ID "Device_01"

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

// ==================== Turbidity Sensor Configuration ====================
#define TURBIDITY_SENSOR_PIN 34

// ==================== DS18B20 Sensor Configuration ====================
#define DS18B20_PIN 21

// ==================== NTP Configuration ====================
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 25200  // Vietnam UTC+7 (7 * 3600)
#define DAYLIGHT_OFFSET_SEC 0

#endif // CONFIG_H
