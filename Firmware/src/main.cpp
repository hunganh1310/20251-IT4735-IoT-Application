/**
 * @file main.cpp
 * @brief Main application for ESP32 IoT system
 * Features: LED control, DS18B20 temperature sensor, Turbidity sensor, MQTT
 * communication
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ld2410.h>
#include <time.h>

// Project headers
#include "config.h"
#include "ds18b20_sensor.h"
#include "led_controller.h"
#include "mqtt_handler.h"
#include "turbidity_sensor.h"

// ==================== Global Objects ====================
LEDController ledController;
ld2410 radar;
TurbiditySensor turbiditySensor(TURBIDITY_SENSOR_PIN);
DS18B20Sensor temperatureSensor(DS18B20_PIN);
MQTTHandler mqttHandler;

// ==================== Timing Variables ====================
unsigned long lastSensorRead = 0;
unsigned long lastDataSend = 0;
unsigned long lastLEDStatusPublish = 0;

// ==================== Sensor Data ====================
float currentTemperature = 0.0;
float currentTurbidity = 0.0;
String currentWaterQuality = "Unknown";

// ==================== LED State ====================
LEDMode lastLEDMode = MODE_OFF;
uint8_t lastBrightness = DEFAULT_BRIGHTNESS;

// ==================== Radar State ====================
bool radarAutoMode =
    false; // Auto mode: ON when human detected, OFF when no human
bool radarEnabled = false; // Manual control: enable/disable radar
unsigned long lastRadarCheck = 0;
const unsigned long RADAR_CHECK_INTERVAL = 500; // Check radar every 500ms

// ==================== Function Prototypes ====================
void setupWiFi();
void setupNTP();
void setupRadar();
void readSensors();
void publishSensorData();
void publishLEDStatus();
void publishRadarStatus();
void mqttCallback(char *topic, uint8_t *payload, unsigned int length);
void handleLEDControl(JsonDocument &doc);
void handleRadarControl(JsonDocument &doc);
void checkRadarAndControlLED();
void parseHexColor(const char *hexColor, uint8_t &r, uint8_t &g, uint8_t &b);

// ==================== Setup ====================
void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=================================");
  Serial.println("ESP32 IoT Application Starting...");
  Serial.println("=================================\n");

  // Setup WiFi
  setupWiFi();

  // Setup NTP for time synchronization
  setupNTP();

  // Configure ADC for turbidity sensor
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  // Also set per-pin attenuation to ensure correct range on selected pin
  analogSetPinAttenuation(TURBIDITY_SENSOR_PIN, ADC_11db);

  // Initialize LED Controller
  if (!ledController.init()) {
    Serial.println("[ERROR] LED Controller initialization failed!");
  }

  // Initialize Radar Sensor
  setupRadar();
  ledController.setRadarSensor(&radar);
  radarEnabled = false; // Start with radar disabled
  radarAutoMode = false;

  // Initialize Temperature Sensor
  if (!temperatureSensor.init()) {
    Serial.println("[ERROR] DS18B20 initialization failed!");
  }

  // Initialize Turbidity Sensor
  if (!turbiditySensor.init()) {
    Serial.println("[ERROR] Turbidity Sensor initialization failed!");
  }

  // Initialize MQTT
  if (!mqttHandler.init()) {
    Serial.println("[ERROR] MQTT initialization failed!");
  }
  mqttHandler.setCallback(mqttCallback);
  mqttHandler.connect();

  // Set default LED mode
  ledController.setMode(MODE_OFF); // Start with LED off

  Serial.println("\n=================================");
  Serial.println("System Initialization Complete!");
  Serial.println("=================================\n");
}

// ==================== Main Loop ====================
void loop() {
  unsigned long currentMillis = millis();

  // Update radar sensor
  radar.read();

  // Check radar and control LED if auto mode is enabled
  if (currentMillis - lastRadarCheck >= RADAR_CHECK_INTERVAL) {
    lastRadarCheck = currentMillis;
    checkRadarAndControlLED();
  }

  // Update LED effects
  ledController.update();

  // Handle MQTT
  mqttHandler.loop();

  // Read sensors periodically
  if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = currentMillis;
    readSensors();
  }

  // Send data to MQTT server periodically
  if (currentMillis - lastDataSend >= DATA_SEND_INTERVAL) {
    lastDataSend = currentMillis;
    publishSensorData();
  }

  // Publish LED status if mode or brightness changed
  LEDMode currentMode = ledController.getMode();
  if (currentMode != lastLEDMode ||
      currentMillis - lastLEDStatusPublish >= 10000) {
    lastLEDMode = currentMode;
    lastLEDStatusPublish = currentMillis;
    publishLEDStatus();
  }

  // Small delay to prevent watchdog issues
  delay(10);
}

// ==================== WiFi Setup ====================
void setupWiFi() {
  Serial.println("[WiFi] Connecting to: " + String(WIFI_SSID));

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttempt < WIFI_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Connected!");
    Serial.print("[WiFi] IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\n[WiFi] Connection failed!");
    Serial.println("[WiFi] Continuing without WiFi...");
  }
}

// ==================== NTP Setup ====================
void setupNTP() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[NTP] Skipping - WiFi not connected");
    return;
  }

  Serial.println("[NTP] Configuring time...");
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  // Wait for time sync (max 5 seconds)
  int retries = 0;
  time_t now = time(nullptr);
  while (now < 24 * 3600 && retries < 10) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    retries++;
  }

  if (now >= 24 * 3600) {
    Serial.println("\n[NTP] Time synchronized!");

    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    Serial.print("[NTP] Current time: ");
    Serial.println(asctime(&timeinfo));
  } else {
    Serial.println("\n[NTP] Time sync failed!");
  }
}

// ==================== Radar Setup ====================
void setupRadar() {
  Serial.println("[Radar] Initializing LD2410B...");

  Serial1.begin(256000, SERIAL_8N1, LD_RX_PIN, LD_TX_PIN);
  delay(500);

  if (radar.begin(Serial1)) {
    Serial.println("[Radar] LD2410B initialized successfully");
    Serial.print("[Radar] Firmware: v");
    Serial.print(radar.firmware_major_version);
    Serial.print(".");
    Serial.print(radar.firmware_minor_version);
    Serial.print(".");
    Serial.println(radar.firmware_bugfix_version);
  } else {
    Serial.println("[Radar] LD2410B initialization failed!");
  }
}

// ==================== Read Sensors ====================
void readSensors() {
  // Read temperature
  currentTemperature = temperatureSensor.readTemperature();
  if (currentTemperature != DEVICE_DISCONNECTED_C) {
    Serial.print("[Sensor] Temperature: ");
    Serial.print(currentTemperature);
    Serial.println(" °C");
  } else {
    Serial.println("[Sensor] Temperature: ERROR - Sensor disconnected");
  }

  // Read turbidity
  currentTurbidity = turbiditySensor.readNTU();
  currentWaterQuality = turbiditySensor.getWaterQuality();

  Serial.print("[Sensor] Turbidity: ");
  Serial.print(currentTurbidity);
  Serial.print(" NTU (");
  Serial.print(currentWaterQuality);
  Serial.println(")");

  float voltage = turbiditySensor.readVoltage();
  int rawValue = turbiditySensor.readRawValue();
  Serial.print("[Sensor] Turbidity Voltage: ");
  Serial.print(voltage);
  Serial.print(" V (Raw: ");
  Serial.print(rawValue);
  Serial.println(")");
}

// ==================== Publish Sensor Data ====================
void publishSensorData() {
  if (!mqttHandler.isConnected()) {
    Serial.println("[MQTT] Not connected - skipping sensor data publish");
    return;
  }

  // Publish combined sensor data
  bool success = mqttHandler.publishSensorData(
      currentTemperature, currentTurbidity, currentWaterQuality);

  if (success) {
    Serial.println("[Data] Sensor data published successfully");
  } else {
    Serial.println("[Data] Failed to publish sensor data");
  }
}

// ==================== Publish LED Status ====================
void publishLEDStatus() {
  if (!mqttHandler.isConnected()) {
    return;
  }

  LEDMode mode = ledController.getMode();
  String modeStr;

  switch (mode) {
  case MODE_OFF:
    modeStr = "off";
    break;
  case MODE_SKY_SIMULATION:
    modeStr = "sky_simulation";
    break;
  case MODE_RAIN:
    modeStr = "rain";
    break;
  case MODE_METEOR:
    modeStr = "meteor";
    break;
  case MODE_APOCALYPSE:
    modeStr = "apocalypse";
    break;
  case MODE_BASIC:
    modeStr = "basic";
    break;
  default:
    modeStr = "unknown";
  }

  mqttHandler.publishLEDStatus(modeStr, lastBrightness, 255, 255, 255);
}

// ==================== MQTT Callback ====================
void mqttCallback(char *topic, uint8_t *payload, unsigned int length) {
  Serial.print("[MQTT] Message received on topic: ");
  Serial.println(topic);

  // Convert payload to string
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("[MQTT] Payload: ");
  Serial.println(message);

  // Parse JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("[MQTT] JSON parsing failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Handle LED control messages
  if (strcmp(topic, MQTT_TOPIC_LED_CONTROL) == 0) {
    handleLEDControl(doc);
  }
  // Handle radar control messages
  else if (strcmp(topic, MQTT_TOPIC_RADAR_CONTROL) == 0) {
    handleRadarControl(doc);
  }
}

// ==================== Handle LED Control ====================
void handleLEDControl(JsonDocument &doc) {
  // Handle presence_mode_enabled (radar auto mode)
  if (doc.containsKey("presence_mode_enabled")) {
    bool presenceModeEnabled = doc["presence_mode_enabled"].as<bool>();
    radarEnabled = presenceModeEnabled;
    radarAutoMode = presenceModeEnabled;

    if (presenceModeEnabled) {
      Serial.println(
          "[Control] Presence mode ENABLED - Radar will control LED");
    } else {
      Serial.println("[Control] Presence mode DISABLED - Manual LED control");
    }
  }

  // Handle led_is_on (manual LED on/off)
  if (doc.containsKey("led_is_on")) {
    bool ledIsOn = doc["led_is_on"].as<bool>();

    if (!radarAutoMode) {
      // Only apply manual control if not in presence mode
      if (ledIsOn) {
        if (ledController.getMode() == MODE_OFF) {
          ledController.setMode(MODE_BASIC);
        }
      } else {
        ledController.setMode(MODE_OFF);
      }
      Serial.println("[Control] LED manually turned " +
                     String(ledIsOn ? "ON" : "OFF"));
    }
  }

  // Handle led_mode change
  if (doc.containsKey("led_mode")) {
    String mode = doc["led_mode"].as<String>();

    if (mode == "off") {
      ledController.setMode(MODE_OFF);
    } else if (mode == "sky_simulation") {
      ledController.setMode(MODE_SKY_SIMULATION);
    } else if (mode == "rain") {
      ledController.setMode(MODE_RAIN);
    } else if (mode == "meteor") {
      ledController.setMode(MODE_METEOR);
    } else if (mode == "apocalypse") {
      ledController.setMode(MODE_APOCALYPSE);
    } else if (mode == "basic") {
      ledController.setMode(MODE_BASIC);
    }

    Serial.println("[Control] LED mode changed to: " + mode);
  }

  // Handle brightness change
  if (doc.containsKey("brightness")) {
    uint8_t brightness = doc["brightness"];
    ledController.setBrightness(brightness);
    lastBrightness = brightness;
    Serial.println("[Control] Brightness changed to: " + String(brightness));
  }

  // Handle color change (hex format like #FF00AA)
  if (doc.containsKey("color")) {
    const char *hexColor = doc["color"];
    uint8_t r, g, b;
    parseHexColor(hexColor, r, g, b);

    ledController.setCustomColor(r, g, b);
    Serial.print("[Control] Color changed to ");
    Serial.print(hexColor);
    Serial.print(" -> RGB(");
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.print(b);
    Serial.println(")");
  }

  // Publish updated status
  publishLEDStatus();
  publishRadarStatus();
}

// ==================== Handle Radar Control ====================
void handleRadarControl(JsonDocument &doc) {
  // Handle radar enable/disable
  if (doc.containsKey("enabled")) {
    radarEnabled = doc["enabled"].as<bool>();

    if (!radarEnabled) {
      // When radar is disabled, turn off LED
      ledController.setMode(MODE_OFF);
      radarAutoMode = false;
      Serial.println("[Radar] Radar disabled - LED turned OFF");
    } else {
      // When radar is enabled, activate auto mode
      radarAutoMode = true;
      Serial.println("[Radar] Radar enabled - Auto mode activated");
    }
  }

  // Publish updated radar status
  publishRadarStatus();
}

// ==================== Check Radar and Control LED ====================
void checkRadarAndControlLED() {
  if (!radarEnabled || !radarAutoMode) {
    return; // Do nothing if radar is disabled or not in auto mode
  }

  // Check if human is detected
  bool presenceDetected = radar.presenceDetected();

  // Get detection distance
  uint16_t distance = radar.stationaryTargetDistance();
  if (distance == 0) {
    distance = radar.movingTargetDistance();
  }

  // Control LED based on presence detection within 20m (2000cm)
  if (presenceDetected && distance > 0 && distance <= 2000) {
    // Human detected within 20m - turn LED ON
    if (ledController.getMode() == MODE_OFF) {
      ledController.setMode(MODE_BASIC);
      ledController.setCustomColor(255, 255, 255); // White
      ledController.setBrightness(200);
      Serial.print("[Radar] Human detected at ");
      Serial.print(distance);
      Serial.println("cm - LED ON");
      publishRadarStatus();
    }
  } else {
    // No human detected or beyond 20m - turn LED OFF
    if (ledController.getMode() != MODE_OFF) {
      ledController.setMode(MODE_OFF);
      Serial.println("[Radar] No human detected - LED OFF");
      publishRadarStatus();
    }
  }
}

// ==================== Publish Radar Status ====================
void publishRadarStatus() {
  if (!mqttHandler.isConnected()) {
    return;
  }

  // Create JSON document
  StaticJsonDocument<256> doc;
  doc["enabled"] = radarEnabled;
  doc["autoMode"] = radarAutoMode;
  doc["presenceDetected"] = radar.presenceDetected();

  uint16_t distance = radar.stationaryTargetDistance();
  if (distance == 0) {
    distance = radar.movingTargetDistance();
  }
  doc["distance"] = distance;
  doc["timestamp"] = millis();

  // Serialize JSON to string
  char buffer[256];
  serializeJson(doc, buffer);

  // Publish to radar status topic
  mqttHandler.publishMessage(MQTT_TOPIC_RADAR_STATUS, buffer);

  Serial.println("[MQTT] Published radar status: " + String(buffer));
}

// ==================== Parse Hex Color ====================
void parseHexColor(const char *hexColor, uint8_t &r, uint8_t &g, uint8_t &b) {
  // Skip '#' if present
  const char *colorStr = hexColor;
  if (colorStr[0] == '#') {
    colorStr++;
  }

  // Parse hex string
  long colorValue = strtol(colorStr, NULL, 16);

  // Extract RGB components
  r = (colorValue >> 16) & 0xFF;
  g = (colorValue >> 8) & 0xFF;
  b = colorValue & 0xFF;
}
