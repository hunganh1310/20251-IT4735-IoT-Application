# 🌊 IoT Aquarium Monitoring System

A comprehensive IoT solution for monitoring and controlling aquarium conditions with real-time data visualization and LED ambient lighting effects.

## 📋 Project Structure

```
20251-IT4735-IoT-Application/
├── Firmware/                 # ESP32 embedded firmware (PlatformIO)
│   ├── platformio.ini       # PlatformIO configuration
│   ├── include/             # Header files
│   │   ├── config.h        # Configuration settings
│   │   ├── sensors.h       # Sensor interfaces
│   │   ├── network.h       # Network interfaces
│   │   └── led_controller.h # LED controller
│   ├── src/                 # Source files
│   │   ├── main.ino        # Main program
│   │   ├── sensors/        # Sensor implementations
│   │   ├── network/        # Network implementations
│   │   └── led_controller.cpp # LED effects
│   └── README.md           # Firmware documentation
│
├── server/                  # Node.js Express backend
│   ├── server.js           # Main server entry point
│   ├── config/             # Database configuration
│   ├── models/             # MongoDB schemas
│   ├── routes/             # API endpoints
│   └── README.md           # Backend documentation
│
├── client/                  # React frontend dashboard
│   ├── public/             # Static files
│   ├── src/                # React components
│   │   ├── App.js         # Main dashboard
│   │   ├── App.css        # Styling
│   │   └── config.js      # API configuration
│   └── README.md           # Frontend documentation
│
└── README.md               # This file
```

## 🎯 Features

### Hardware (ESP32)
- **Multi-sensor integration:**
  - DS18B20 Temperature sensor
  - LD2410 Human presence radar
  - Turbidity sensor (water quality)
- **WS2812 LED Effects:**
  - Sunrise/sunset simulation
  - Rain effect
  - Meteor shower
  - Apocalypse effect
- **WiFi connectivity** with NTP time sync
- **Real-time data transmission** to server
- **Serial command interface** for debugging

### Back-end (Node.js)
- **RESTful API** for sensor data and configuration
- **WebSocket support** for real-time updates
- **MongoDB database** for data persistence
- **Automatic data cleanup** (TTL indexes)
- **Statistics and historical data** endpoints
- **CORS enabled** for cross-origin requests

### Front-end (React)
- **Real-time dashboard** with WebSocket updates
- **Interactive charts** for historical data
- **LED control panel** with live configuration
- **Responsive design** for mobile and desktop
- **Connection status** indicator
- **Beautiful gradient UI** with card-based layout

## 🚀 Quick Start

### Prerequisites
- **ESP32** development board
- **Sensors**: DS18B20, LD2410, Turbidity sensor, WS2812 LED strip
- **Development Tools**: PlatformIO (recommended) or Arduino IDE
- **Backend**: Node.js 16+ and npm
- **Database**: MongoDB (local or Atlas)

### 1. Firmware Setup (PlatformIO Recommended)

**Option A: Using PlatformIO (Recommended)**
```bash
# Install PlatformIO Core
python -m pip install -U platformio

# Navigate to firmware directory
cd Firmware

# Update configuration
# Edit include/config.h with your WiFi and server details

# Build and upload (Windows)
python -m platformio run --target upload

# Or on Linux/Mac (if pio is in PATH)
pio run --target upload

# Monitor serial output
python -m platformio device monitor
```

**Option B: Using Arduino IDE**
1. Wire sensors according to pin configurations in `Firmware/include/config.h`
2. Install required libraries: FastLED, LD2410, OneWire, DallasTemperature
3. Copy all files from `Firmware/` to Arduino sketch folder
4. Update WiFi credentials and server IP in `config.h`
5. Upload to ESP32

### 2. Backend Server Setup

```bash
cd server
npm install
cp .env.example .env
# Edit .env with your MongoDB connection string
npm start
```

Server runs on:
- HTTP: `http://localhost:8080`
- WebSocket: `ws://localhost:8081`

### 3. Frontend Dashboard Setup

```bash
cd client
npm install
cp .env.example .env
# Edit .env with your backend server URL
npm start
```

Dashboard opens at: `http://localhost:3000`

## 📡 API Endpoints

### Sensor Data
- `POST /api/data` - Receive sensor data from ESP32
- `GET /api/data/latest` - Get latest sensor reading
- `GET /api/data/history` - Get historical data (with pagination)
- `GET /api/data/stats` - Get statistics for specified period
- `DELETE /api/data/cleanup` - Clean up old data

### Configuration
- `GET /api/config/:deviceId` - Get device configuration
- `PUT /api/config/:deviceId` - Update full configuration
- `PATCH /api/config/:deviceId` - Update partial configuration
- `DELETE /api/config/:deviceId` - Reset to defaults

### Health Check
- `GET /api/health` - Server health status

## 🔧 Configuration

### Firmware Configuration (`Firmware/include/config.h`)
```cpp
#define WIFI_SSID "YourWiFi"
#define WIFI_PASSWORD "YourPassword"
#define SERVER_HOST "192.168.1.100"
#define SERVER_PORT 8080
#define LED_PIN 5
#define NUM_LEDS 60
```

### Backend Configuration (`server/.env`)
```
PORT=8080
MONGODB_URI=mongodb://localhost:27017/iot_application
WS_PORT=8081
CORS_ORIGIN=http://localhost:3000
```

### Frontend Configuration (`client/.env`)
```
REACT_APP_API_URL=http://localhost:8080
REACT_APP_WS_URL=ws://localhost:8081
```

## 📊 Data Flow

```
ESP32 Sensors → WiFi → Backend API (Express) → MongoDB
                              ↓
                        WebSocket Server
                              ↓
                    React Dashboard (Client)
```

## 🛠️ Development Tools

### Firmware Development
- **PlatformIO** (Recommended): Professional embedded development
  - Automatic library management
  - Better debugging support
  - Multi-platform builds
- **Arduino IDE**: Alternative for beginners

### Backend Development
- **Node.js + Express**: RESTful API server
- **MongoDB**: NoSQL database for sensor data
- **WebSocket**: Real-time bidirectional communication

### Frontend Development
- **React**: Modern UI framework
- **Recharts**: Data visualization library
- **Axios**: HTTP client for API calls

## 🎨 LED Effects

1. **Sunrise/Sunset** - Simulates natural daylight using Planck's law for color temperature
2. **Rain** - Dark stormy sky with lightning flashes
3. **Meteor** - Meteor shower effects across the LED strip
4. **Apocalypse** - Dramatic fire and smoke effects
5. **Off** - All LEDs off

## 🛠️ Serial Commands

Connect to ESP32 via Serial Monitor (115200 baud):
- `status` - Show complete system status
- `sensor` - Display current sensor readings
- `network` - Show network status
- `led <mode>` - Change LED mode (off/sunrise/rain/meteor/apocalypse)
- `brightness <0-255>` - Set LED brightness
- `help` - Show available commands

## 📈 Future Enhancements

- [ ] MQTT support for better IoT integration
- [ ] Mobile app (React Native)
- [ ] Alert system for abnormal conditions
- [ ] Machine learning for anomaly detection
- [ ] Multiple device support
- [ ] User authentication
- [ ] Data export (CSV, JSON)
- [ ] Automated LED schedules

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 👥 Authors

- Hung Anh - IoT Application Development

## 🙏 Acknowledgments

- FastLED library for LED control
- LD2410 library for radar sensor
- DallasTemperature for DS18B20
- Express.js and React communities
