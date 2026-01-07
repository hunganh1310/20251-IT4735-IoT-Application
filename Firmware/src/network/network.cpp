/**
 * @file network.cpp
 * @brief Network configuration module implementation
 */

#include "network.h"

#define PREF_NAMESPACE "wifi-config"
#define PREF_SSID "ssid"
#define PREF_PASSWORD "password"

NetworkManager::NetworkManager() {
    server = nullptr;
    oledDisplay = nullptr;
    isAPMode = false;
    savedSSID = "";
    savedPassword = "";
}

NetworkManager::~NetworkManager() {
    if (server != nullptr) {
        server->stop();
        delete server;
    }
    preferences.end();
}

void NetworkManager::setDisplay(Adafruit_SSD1306* display) {
    oledDisplay = display;
}

bool NetworkManager::begin() {
    Serial.println("\n========================================");
    Serial.println("Network Manager - Initializing...");
    Serial.println("========================================");
    
    preferences.begin(PREF_NAMESPACE, false);
    
    savedSSID = preferences.getString(PREF_SSID, "");
    savedPassword = preferences.getString(PREF_PASSWORD, "");
    
    if (savedSSID.length() == 0) {
        Serial.println("[INFO] No saved WiFi credentials found");
        Serial.println("[INFO] Starting Access Point mode...");
        
        startAccessPoint();
        startWebServer();
        
        isAPMode = true;
        return false;
    } else {
        Serial.println("[INFO] Found saved WiFi configuration:");
        Serial.print("  - SSID: ");
        Serial.println(savedSSID);
        Serial.println("  - Password: ********");
        
        Serial.println("\n[INFO] Connecting to WiFi...");
        
        WiFi.mode(WIFI_STA);
        WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
        
        int timeout = 0;
        while (WiFi.status() != WL_CONNECTED && timeout < WIFI_CONNECT_TIMEOUT) {
            delay(500);
            Serial.print(".");
            timeout += 500;
        }
        Serial.println();
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("[SUCCESS] WiFi connected successfully!");
            Serial.print("[INFO] IP Address: ");
            Serial.println(WiFi.localIP());
            Serial.print("[INFO] Signal Strength: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
            
            isAPMode = false;
            return true;
        } else {
            Serial.println("[ERROR] Could not connect to WiFi!");
            Serial.println("[INFO] SSID/Password may be incorrect or network unavailable.");
            Serial.println("[INFO] Starting Access Point for reconfiguration...");
            
            startAccessPoint();
            startWebServer();
            
            isAPMode = true;
            return false;
        }
    }
}

void NetworkManager::startAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    
    delay(100);
    
    Serial.println("\n[INFO] Access Point started:");
    Serial.print("  - SSID: ");
    Serial.println(AP_SSID);
    if (strlen(AP_PASSWORD) > 0) {
        Serial.println("  - Password: (configured)");
    } else {
        Serial.println("  - Password: (none - Open)");
    }
    Serial.print("  - IP Address: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("\n[INFO] Access http://192.168.4.1 to configure WiFi");
    
    displayAPInfo();
}

void NetworkManager::displayAPInfo() {
    if (oledDisplay == nullptr) {
        return;
    }
    
    oledDisplay->clearDisplay();
    oledDisplay->setTextSize(1);
    oledDisplay->setTextColor(SSD1306_WHITE);
    
    oledDisplay->setCursor(0, 0);
    oledDisplay->println("== WiFi Config ==");
    
    oledDisplay->setCursor(0, 12);
    oledDisplay->print("SSID: ");
    oledDisplay->println(AP_SSID);
    
    oledDisplay->setCursor(0, 22);
    if (strlen(AP_PASSWORD) > 0) {
        oledDisplay->println("Pass: (set)");
    } else {
        oledDisplay->println("Pass: (open)");
    }
    
    oledDisplay->setCursor(0, 32);
    oledDisplay->print("IP: ");
    oledDisplay->println(WiFi.softAPIP());
    
    oledDisplay->setCursor(0, 46);
    oledDisplay->println("Connect to WiFi");
    oledDisplay->setCursor(0, 56);
    oledDisplay->println("Access: 192.168.4.1");
    
    oledDisplay->display();
}

void NetworkManager::startWebServer() {
    if (server != nullptr) {
        delete server;
    }
    
    server = new WebServer(80);
    
    server->on("/", HTTP_GET, [this]() { handleRoot(); });
    server->on("/save", HTTP_POST, [this]() { handleSave(); });
    server->on("/reset", HTTP_GET, [this]() { handleReset(); });
    server->onNotFound([this]() { handleNotFound(); });
    
    server->begin();
    Serial.println("[INFO] Web Server started on port 80");
}

void NetworkManager::stopWebServer() {
    if (server != nullptr) {
        server->stop();
        delete server;
        server = nullptr;
    }
}

void NetworkManager::handleClient() {
    if (server != nullptr && isAPMode) {
        server->handleClient();
    }
}

String NetworkManager::getConfigPage() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 WiFi Configuration</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
        body {
            font-family: 'Segoe UI', Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            padding: 40px;
            width: 100%;
            max-width: 400px;
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 10px;
            font-size: 24px;
        }
        .subtitle {
            color: #666;
            text-align: center;
            margin-bottom: 30px;
            font-size: 14px;
        }
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            color: #555;
            margin-bottom: 8px;
            font-weight: 600;
        }
        input[type="text"],
        input[type="password"] {
            width: 100%;
            padding: 15px;
            border: 2px solid #e0e0e0;
            border-radius: 10px;
            font-size: 16px;
            transition: border-color 0.3s;
        }
        input:focus {
            outline: none;
            border-color: #667eea;
        }
        .btn {
            width: 100%;
            padding: 15px;
            border: none;
            border-radius: 10px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            transition: transform 0.2s, box-shadow 0.2s;
        }
        .btn-primary {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            margin-bottom: 10px;
        }
        .btn-primary:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 20px rgba(102, 126, 234, 0.4);
        }
        .btn-secondary {
            background: #f0f0f0;
            color: #666;
        }
        .btn-secondary:hover {
            background: #e0e0e0;
        }
        .icon {
            text-align: center;
            font-size: 60px;
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>My Aquarium - Wifi Configuration</h1>
        <p class="subtitle">Enter Wifi Information:</p>
        
        <form action="/save" method="POST">
            <div class="form-group">
                <label for="ssid">Wifi name (SSID)</label>
                <input type="text" id="ssid" name="ssid" placeholder="Enter Wifi Name" required>
            </div>
            
            <div class="form-group">
                <label for="password">Password</label>
                <input type="password" id="password" name="password" placeholder="Enter Password">
            </div>
            
            <button type="submit" class="btn btn-primary">Save and Connect</button>
        </form>
        
        <a href="/reset" onclick="return confirm('Confirm delete?')">
            <button type="button" class="btn btn-secondary">Delete historical configuration</button>
        </a>
    </div>
</body>
</html>
)rawliteral";
    
    return html;
}

String NetworkManager::getSuccessPage() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Save successfully!</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
        body {
            font-family: 'Segoe UI', Arial, sans-serif;
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            padding: 40px;
            width: 100%;
            max-width: 400px;
            text-align: center;
        }
        .icon {
            font-size: 80px;
            margin-bottom: 20px;
        }
        h1 {
            color: #11998e;
            margin-bottom: 15px;
        }
        p {
            color: #666;
            margin-bottom: 10px;
            line-height: 1.6;
        }
        .countdown {
            font-size: 48px;
            color: #11998e;
            font-weight: bold;
            margin: 20px 0;
        }
    </style>
    <script>
        var count = 5;
        function countdown() {
            document.getElementById('count').textContent = count;
            if (count <= 0) {
                document.getElementById('count').textContent = '...';
            } else {
                count--;
                setTimeout(countdown, 1000);
            }
        }
        window.onload = countdown;
    </script>
</head>
<body>
    <div class="container">
        <div class="icon"></div>
        <h1>Save successfully</h1>
        <p>Wifi Configuration saved</p>
        <p>The device will restart in</p>
        <div class="countdown" id="count">5</div>
    </div>
</body>
</html>
)rawliteral";
    
    return html;
}

void NetworkManager::handleRoot() {
    Serial.println("[WEB] Configuration page accessed");
    server->send(200, "text/html", getConfigPage());
}

void NetworkManager::handleSave() {
    String newSSID = server->arg("ssid");
    String newPassword = server->arg("password");
    
    Serial.println("\n[WEB] Save configuration request:");
    Serial.print("  - SSID: ");
    Serial.println(newSSID);
    Serial.println("  - Password: ********");
    
    if (newSSID.length() == 0) {
        server->send(400, "text/html", "<h1>Error: SSID cannot be empty!</h1>");
        return;
    }
    
    preferences.putString(PREF_SSID, newSSID);
    preferences.putString(PREF_PASSWORD, newPassword);
    
    Serial.println("[INFO] Configuration saved to Preferences");
    
    server->send(200, "text/html", getSuccessPage());
    
    Serial.println("[INFO] Restarting ESP32 in 5 seconds...");
    delay(5000);
    ESP.restart();
}

void NetworkManager::handleReset() {
    Serial.println("[WEB] Clear configuration request");
    
    clearCredentials();
    
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="2;url=/">
    <title>Configuration Cleared</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background: #f0f0f0;
        }
        .msg {
            background: white;
            padding: 40px;
            border-radius: 10px;
            text-align: center;
            box-shadow: 0 5px 20px rgba(0,0,0,0.1);
        }
    </style>
</head>
<body>
    <div class="msg">
        <h2>🗑️ Configuration Cleared!</h2>
        <p>Redirecting...</p>
    </div>
</body>
</html>
)rawliteral";
    
    server->send(200, "text/html", html);
}

void NetworkManager::handleNotFound() {
    server->send(404, "text/plain", "404 - Page not found");
}

bool NetworkManager::isAccessPointMode() {
    return isAPMode;
}

bool NetworkManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String NetworkManager::getIPAddress() {
    if (isAPMode) {
        return WiFi.softAPIP().toString();
    } else {
        return WiFi.localIP().toString();
    }
}

void NetworkManager::clearCredentials() {
    preferences.remove(PREF_SSID);
    preferences.remove(PREF_PASSWORD);
    Serial.println("[INFO] WiFi configuration cleared");
}

bool NetworkManager::reconnect() {
    if (savedSSID.length() == 0) {
        return false;
    }
    
    Serial.println("[INFO] Reconnecting to WiFi...");
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
    
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < WIFI_CONNECT_TIMEOUT) {
        delay(500);
        Serial.print(".");
        timeout += 500;
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[SUCCESS] WiFi reconnected successfully!");
        return true;
    }
    
    Serial.println("[ERROR] Could not reconnect to WiFi!");
    return false;
}
