/**
 * @file mqtt_handler.h
 * @brief MQTT communication handler for IoT Application
 */

#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "config.h"

// MQTT Topics
#define MQTT_TOPIC_LED_CONTROL "iot/device01/led/control"
#define MQTT_TOPIC_LED_STATUS "iot/device01/led/status"
#define MQTT_TOPIC_RADAR_CONTROL "iot/device01/radar/control"
#define MQTT_TOPIC_RADAR_STATUS "iot/device01/radar/status"
#define MQTT_TOPIC_SENSOR_DATA "iot/device01/sensors"
#define MQTT_TOPIC_TEMPERATURE "iot/device01/temperature"
#define MQTT_TOPIC_TURBIDITY "iot/device01/turbidity"
#define MQTT_TOPIC_STATUS "iot/device01/status"

class MQTTHandler {
public:
    // Constructor
    MQTTHandler();
    
    // Initialization
    bool init();
    
    // Connection management
    bool connect();
    void disconnect();
    bool isConnected();
    void loop();
    
    // Publishing
    bool publishSensorData(float temperature, float turbidity, String waterQuality);
    bool publishLEDStatus(String mode, uint8_t brightness, uint8_t r, uint8_t g, uint8_t b);
    bool publishStatus(String status);
    bool publishMessage(const char* topic, const char* payload);
    
    // Subscription callback
    void setCallback(void (*callback)(char*, uint8_t*, unsigned int));
    
    // Subscribe to topics
    bool subscribeToTopics();
    
private:
    WiFiClientSecure wifiClient;
    PubSubClient* mqttClient;
    unsigned long lastReconnectAttempt;
    static const unsigned long RECONNECT_INTERVAL = 5000;
    
    // SSL/TLS configuration
    void setupTLS();
};

#endif // MQTT_HANDLER_H
