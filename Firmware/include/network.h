/**
 * @file network.h
 * @brief Network configuration module using Preferences and Web Server
 * 
 * Luồng hoạt động:
 * 1. Kiểm tra SSID đã lưu trong Preferences chưa
 * 2. Nếu chưa -> Tạo AP và Web Server để người dùng nhập WiFi
 * 3. Nếu có -> Kết nối với WiFi đã lưu
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
    Adafruit_SSD1306* oledDisplay;  // Pointer to external OLED display
    
    String savedSSID;
    String savedPassword;
    bool isAPMode;
    
    // Web Server HTML Pages
    String getConfigPage();
    String getSuccessPage();
    
    // Web Server Handlers
    void handleRoot();
    void handleSave();
    void handleReset();
    void handleNotFound();
    
    // Helper functions
    void startAccessPoint();
    void startWebServer();
    void stopWebServer();
    void displayAPInfo();  // Hiển thị thông tin AP lên OLED
    
public:
    NetworkManager();
    ~NetworkManager();
    
    /**
     * @brief Thiết lập OLED display (gọi trước begin())
     * @param display Pointer đến Adafruit_SSD1306 đã khởi tạo
     */
    void setDisplay(Adafruit_SSD1306* display);
    
    /**
     * @brief Khởi tạo và kiểm tra cấu hình WiFi
     * @return true nếu đã có SSID lưu và kết nối thành công
     */
    bool begin();
    
    /**
     * @brief Xử lý Web Server (gọi trong loop nếu ở chế độ AP)
     */
    void handleClient();
    
    /**
     * @brief Kiểm tra xem có đang ở chế độ AP không
     */
    bool isAccessPointMode();
    
    /**
     * @brief Kiểm tra kết nối WiFi
     */
    bool isConnected();
    
    /**
     * @brief Lấy địa chỉ IP hiện tại
     */
    String getIPAddress();
    
    /**
     * @brief Xóa cấu hình WiFi đã lưu
     */
    void clearCredentials();
    
    /**
     * @brief Kết nối lại WiFi
     */
    bool reconnect();
};

#endif // NETWORK_H
