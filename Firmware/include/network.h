/**
 * @file network.h
 * @brief Network connectivity and communication
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

class NetworkManager {
public:
  NetworkManager();
  
  // WiFi management
  bool connectWiFi(const char* ssid, const char* password, uint32_t timeout = 10000);
  void disconnectWiFi();
  bool isConnected();
  String getIPAddress();
  int getSignalStrength();
  
  // Time synchronization
  bool syncTime(const char* ntpServer, long gmtOffset, int daylightOffset);
  String getFormattedTime();
  
  // HTTP communication
  bool sendData(const String& url, const String& jsonData);
  String getData(const String& url);
  
  // Status
  void printStatus();
  
private:
  bool wifiConnected;
  bool timesynced;
  
  String lastError;
  void setError(const String& error);
};

#endif // NETWORK_H
