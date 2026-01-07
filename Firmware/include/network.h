/**
 * @file network.h
 * @brief WiFi configuration via web interface
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

class NetworkManager {
private:
    Preferences preferences;
    WebServer* server;
    Adafruit_SSD1306* oledDisplay;
    
    String savedSSID;
    String savedPassword;
    bool isAPMode;
    
    String getConfigPage();
    String getSuccessPage();
    
    void handleRoot();
    void handleSave();
    void handleReset();
    void handleNotFound();
    
    void startAccessPoint();
    void startWebServer();
    void stopWebServer();
    void displayAPInfo();
    
public:
    NetworkManager();
    ~NetworkManager();
    
    void setDisplay(Adafruit_SSD1306* display);
    
    bool begin();
    
    void handleClient();
    
    bool isAccessPointMode();
    
    bool isConnected();
    
    String getIPAddress();
    
    void clearCredentials();
    
    bool reconnect();
};

#endif // NETWORK_H
