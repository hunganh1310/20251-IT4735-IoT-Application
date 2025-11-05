/**
 * @file main.ino
 * @brief Main IoT Application Controller
 * @details Integrates sensors, network, and LED control
 */

#include "config.h"
#include "version.h"
#include "sensors.h"
#include "network.h"
#include "led_controller.h"
#include "utils.h"

// Sensor objects (needed by sensors.cpp)
HardwareSerial radarSerial(2);
LD2410 radar;
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature tempSensors(&oneWire);

// Manager objects
SensorManager sensorManager;
NetworkManager networkManager;
LEDController ledController;

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastDataSend = 0;
unsigned long lastTimeUpdate = 0;

// System state
bool systemReady = false;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("========================================");
  Serial.print("    ");
  Serial.print(FIRMWARE_NAME);
  Serial.println("    ");
  Serial.print("    Version: ");
  Serial.println(FIRMWARE_VERSION);
  Serial.print("    Build: ");
  Serial.print(BUILD_DATE);
  Serial.print(" ");
  Serial.println(BUILD_TIME);
  Serial.println("========================================");
  
  // Print system information
  if (DEBUG_MODE) {
    Utils::printSystemInfo();
    Utils::printMemoryInfo();
  }
  
  // Initialize sensors
  Serial.println("\n[Init] Initializing sensors...");
  if (!sensorManager.initAll()) {
    Serial.println("[Init] Warning: Some sensors failed to initialize");
  }
  
  // Initialize LED controller
  Serial.println("[Init] Initializing LED controller...");
  if (ledController.init()) {
    ledController.setMode(MODE_SUNRISE_SUNSET);
    Serial.println("[Init] LED controller ready");
  } else {
    Serial.println("[Init] Warning: LED controller failed");
  }
  
  // Connect to WiFi
  Serial.println("[Init] Connecting to WiFi...");
  if (networkManager.connectWiFi(WIFI_SSID, WIFI_PASSWORD, WIFI_TIMEOUT)) {
    // Sync time with NTP
    Serial.println("[Init] Synchronizing time...");
    networkManager.syncTime(NTP_SERVER, GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC);
    
    // Print WiFi info
    if (DEBUG_MODE) {
      Utils::printWiFiInfo();
    }
  } else {
    Serial.println("[Init] Warning: WiFi connection failed");
    Serial.println("[Init] System will run in offline mode");
  }
  
  Serial.println("\n========================================");
  Serial.println("      System Initialization Complete    ");
  Serial.println("========================================\n");
  
  systemReady = true;
  
  // Print initial status
  networkManager.printStatus();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Update LED effects
  ledController.update();
  
  // Read sensors at regular intervals
  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = currentTime;
    
    SensorData data = sensorManager.readAll();
    
    if (DEBUG_MODE && (currentTime % 10000 < SENSOR_READ_INTERVAL)) {
      // Print sensor data every 10 seconds
      sensorManager.printSensorData(data);
    }
    
    // Send data to server at regular intervals
    if (networkManager.isConnected() && 
        (currentTime - lastDataSend >= DATA_SEND_INTERVAL)) {
      lastDataSend = currentTime;
      
      String jsonData = sensorManager.toJSON(data);
      String serverUrl = String("http://") + SERVER_HOST + ":" + 
                        String(SERVER_PORT) + API_ENDPOINT;
      
      if (DEBUG_MODE) {
        Serial.println("[Main] Sending data to server...");
      }
      
      if (networkManager.sendData(serverUrl, jsonData)) {
        if (DEBUG_MODE) Serial.println("[Main] Data sent successfully");
      } else {
        if (DEBUG_MODE) Serial.println("[Main] Failed to send data");
      }
    }
  }
  
  // Update time periodically
  if (networkManager.isConnected() && 
      (currentTime - lastTimeUpdate >= NTP_UPDATE_INTERVAL)) {
    lastTimeUpdate = currentTime;
    networkManager.syncTime(NTP_SERVER, GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC);
  }
  
  // Handle serial commands (optional - for debugging/control)
  if (Serial.available()) {
    handleSerialCommand();
  }
  
  // Small delay to prevent watchdog issues
  delay(10);
}

void handleSerialCommand() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  if (command == "status") {
    Serial.println("\n========== System Status ==========");
    Serial.print("Firmware: ");
    Serial.print(FIRMWARE_NAME);
    Serial.print(" v");
    Serial.println(FIRMWARE_VERSION);
    Serial.print("Uptime: ");
    Serial.println(Utils::formatUptime(millis()));
    Serial.println("===================================\n");
    
    networkManager.printStatus();
    SensorData data = sensorManager.readAll();
    sensorManager.printSensorData(data);
    
    if (DEBUG_MODE) {
      Utils::printMemoryInfo();
    }
  }
  else if (command == "sensor") {
    SensorData data = sensorManager.readAll();
    sensorManager.printSensorData(data);
  }
  else if (command == "network") {
    networkManager.printStatus();
    if (DEBUG_MODE) {
      Utils::printWiFiInfo();
    }
  }
  else if (command == "memory") {
    Utils::printMemoryInfo();
  }
  else if (command == "sysinfo") {
    Utils::printSystemInfo();
  }
  else if (command.startsWith("led ")) {
    String mode = command.substring(4);
    if (mode == "off") ledController.setMode(MODE_OFF);
    else if (mode == "sunrise") ledController.setMode(MODE_SUNRISE_SUNSET);
    else if (mode == "rain") ledController.setMode(MODE_RAIN);
    else if (mode == "meteor") ledController.setMode(MODE_METEOR);
    else if (mode == "apocalypse") ledController.setMode(MODE_APOCALYPSE);
    Serial.print("[Command] LED mode set to: ");
    Serial.println(mode);
  }
  else if (command.startsWith("brightness ")) {
    int brightness = command.substring(11).toInt();
    if (brightness >= 0 && brightness <= 255) {
      ledController.setBrightness(brightness);
      Serial.print("[Command] Brightness set to: ");
      Serial.println(brightness);
    }
  }
  else if (command == "help") {
    Serial.println("\n========== Available Commands ==========");
    Serial.println("status       - Show complete system status");
    Serial.println("sensor       - Read and display sensor data");
    Serial.println("network      - Show network status");
    Serial.println("memory       - Show memory information");
    Serial.println("sysinfo      - Show system information");
    Serial.println("led <mode>   - Change LED mode (off/sunrise/rain/meteor/apocalypse)");
    Serial.println("brightness <0-255> - Set LED brightness");
    Serial.println("help         - Show this help message");
    Serial.println("========================================\n");
  }
  else {
    Serial.println("[Command] Unknown command. Type 'help' for available commands.");
  }
}
