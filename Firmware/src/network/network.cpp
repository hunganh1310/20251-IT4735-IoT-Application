/**
 * @file network.cpp
 * @brief Network connectivity implementation
 */

#include "network.h"
#include "config.h"
#include <time.h>

NetworkManager::NetworkManager() {
  wifiConnected = false;
  timeSynced = false;
  lastError = "";
}

bool NetworkManager::connectWiFi(const char* ssid, const char* password, uint32_t timeout) {
  if (DEBUG_MODE) {
    Serial.println("[Network] Connecting to WiFi...");
    Serial.print("[Network] SSID: ");
    Serial.println(ssid);
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  uint32_t startTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < timeout) {
    delay(500);
    if (DEBUG_MODE) Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    if (DEBUG_MODE) {
      Serial.println("\n[Network] WiFi connected!");
      Serial.print("[Network] IP Address: ");
      Serial.println(WiFi.localIP());
      Serial.print("[Network] Signal Strength: ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
    }
    return true;
  } else {
    wifiConnected = false;
    setError("WiFi connection timeout");
    if (DEBUG_MODE) Serial.println("\n[Network] WiFi connection failed!");
    return false;
  }
}

void NetworkManager::disconnectWiFi() {
  WiFi.disconnect();
  wifiConnected = false;
  if (DEBUG_MODE) Serial.println("[Network] WiFi disconnected");
}

bool NetworkManager::isConnected() {
  wifiConnected = (WiFi.status() == WL_CONNECTED);
  return wifiConnected;
}

String NetworkManager::getIPAddress() {
  if (isConnected()) {
    return WiFi.localIP().toString();
  }
  return "Not connected";
}

int NetworkManager::getSignalStrength() {
  if (isConnected()) {
    return WiFi.RSSI();
  }
  return 0;
}

bool NetworkManager::syncTime(const char* ntpServer, long gmtOffset, int daylightOffset) {
  if (!isConnected()) {
    setError("Cannot sync time: WiFi not connected");
    return false;
  }
  
  if (DEBUG_MODE) Serial.println("[Network] Synchronizing time with NTP...");
  
  configTime(gmtOffset, daylightOffset, ntpServer);
  
  // Wait for time to be set (max 10 seconds)
  int retry = 0;
  const int retry_count = 20;
  while (time(nullptr) < 100000 && ++retry < retry_count) {
    delay(500);
    if (DEBUG_MODE) Serial.print(".");
  }
  
  if (retry < retry_count) {
    timeSynced = true;
    if (DEBUG_MODE) {
      Serial.println("\n[Network] Time synchronized!");
      Serial.print("[Network] Current time: ");
      Serial.println(getFormattedTime());
    }
    return true;
  } else {
    setError("NTP sync timeout");
    if (DEBUG_MODE) Serial.println("\n[Network] Time sync failed!");
    return false;
  }
}

String NetworkManager::getFormattedTime() {
  if (!timeSynced) return "Time not synced";
  
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  char buffer[64];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  return String(buffer);
}

bool NetworkManager::sendData(const String& url, const String& jsonData) {
  if (!isConnected()) {
    setError("Cannot send data: WiFi not connected");
    return false;
  }
  
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(jsonData);
  
  if (httpResponseCode > 0) {
    if (DEBUG_MODE) {
      Serial.print("[Network] HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    return (httpResponseCode >= 200 && httpResponseCode < 300);
  } else {
    setError("HTTP POST failed: " + String(httpResponseCode));
    if (DEBUG_MODE) {
      Serial.print("[Network] HTTP POST failed, error: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    return false;
  }
}

String NetworkManager::getData(const String& url) {
  if (!isConnected()) {
    setError("Cannot get data: WiFi not connected");
    return "";
  }
  
  HTTPClient http;
  http.begin(url);
  
  int httpResponseCode = http.GET();
  String payload = "";
  
  if (httpResponseCode > 0) {
    payload = http.getString();
    if (DEBUG_MODE) {
      Serial.print("[Network] HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
  } else {
    setError("HTTP GET failed: " + String(httpResponseCode));
    if (DEBUG_MODE) {
      Serial.print("[Network] HTTP GET failed, error: ");
      Serial.println(httpResponseCode);
    }
  }
  
  http.end();
  return payload;
}

void NetworkManager::printStatus() {
  Serial.println("\n========== Network Status ==========");
  Serial.print("WiFi Connected: ");
  Serial.println(isConnected() ? "YES" : "NO");
  if (isConnected()) {
    Serial.print("IP Address: ");
    Serial.println(getIPAddress());
    Serial.print("Signal Strength: ");
    Serial.print(getSignalStrength());
    Serial.println(" dBm");
  }
  Serial.print("Time Synced: ");
  Serial.println(timeSynced ? "YES" : "NO");
  if (timeSynced) {
    Serial.print("Current Time: ");
    Serial.println(getFormattedTime());
  }
  if (lastError.length() > 0) {
    Serial.print("Last Error: ");
    Serial.println(lastError);
  }
  Serial.println("====================================\n");
}

void NetworkManager::setError(const String& error) {
  lastError = error;
}
