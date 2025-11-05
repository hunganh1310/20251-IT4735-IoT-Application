# Firmware - ESP32 IoT Application

## 📌 Overview

Professional modular firmware for ESP32-based IoT aquarium monitoring system with integrated sensors, LED effects, and cloud connectivity. Built with PlatformIO for better dependency management and build system.

## 📂 Project Structure

```
Firmware/
├── platformio.ini          # PlatformIO configuration
├── README.md              # This file
├── include/               # Header files
│   ├── config.h          # Global configuration
│   ├── sensors.h         # Sensor interfaces
│   ├── network.h         # Network interfaces
│   └── led_controller.h  # LED controller
├── src/                   # Source files
│   ├── main.ino          # Main program
│   ├── sensors/
│   │   └── sensors.cpp   # Sensor implementations
│   ├── network/
│   │   └── network.cpp   # Network implementations
│   └── led_controller.cpp # LED effects
└── test/                  # Unit tests (optional)
```

## 🔌 Hardware Requirements

- **ESP32 Development Board** (ESP32-WROOM-32 or similar)
- **DS18B20** - Waterproof temperature sensor
- **LD2410** - 24GHz human presence radar sensor
- **Turbidity Sensor** - Water quality monitoring
- **WS2812/WS2813** - RGB LED strip (60 LEDs recommended)
- **Power Supply** - 5V, adequate current for LED strip

## 📐 Wiring Diagram

```
ESP32 Pin Connections:
├── GPIO 5  → WS2812 LED Data Pin
├── GPIO 4  → DS18B20 Data Pin (with 4.7kΩ pull-up resistor)
├── GPIO 16 → LD2410 RX (connect to LD2410 TX)
├── GPIO 17 → LD2410 TX (connect to LD2410 RX)
├── GPIO 4  → LD2410 OUT pin
└── GPIO 34 → Turbidity Sensor Analog Output (ADC1)
```

**Important Notes:**
- DS18B20 requires a 4.7kΩ pull-up resistor between data pin and VCC
- WS2812 requires adequate power supply (up to 60mA per LED at full brightness)
- LD2410 operates at 3.3V logic level
- Turbidity sensor may require voltage divider if it outputs 5V

## 📚 Required Libraries

Install these libraries via Arduino Library Manager:

```
- FastLED (v3.6.0+)
- LD2410 (by ncmreynolds)
- OneWire (v2.3.7+)
- DallasTemperature (v3.9.0+)
```

## 🔧 Configuration

Edit `main/config.h` for your setup:

```cpp
// WiFi Settings
#define WIFI_SSID "YourNetworkName"
#define WIFI_PASSWORD "YourPassword"

// Server Settings
#define SERVER_HOST "192.168.1.100"  // Your server IP
#define SERVER_PORT 8080

// Pin Configuration (if different from defaults)
#define LED_PIN 5
#define TEMP_SENSOR_PIN 4
#define RADAR_RX_PIN 16
#define RADAR_TX_PIN 17
#define TURBIDITY_PIN 34

// LED Configuration
#define NUM_LEDS 60
#define DEFAULT_BRIGHTNESS 128
```

## 📂 Project Structure

```
main/
├── main.ino           # Main program loop and setup
├── config.h           # Configuration constants
├── sensors.h          # Sensor interface definitions
├── sensors.cpp        # Sensor implementation
├── network.h          # Network interface
├── network.cpp        # WiFi and HTTP implementation
└── led_controller.h   # LED effects header
```

## 🚀 Upload Instructions

### Using PlatformIO (Recommended):

1. **Install PlatformIO**:
   - Install VS Code
   - Add PlatformIO IDE extension from marketplace
   
2. **Open Project**:
   - Open the `Firmware` folder in VS Code
   - PlatformIO will automatically detect `platformio.ini`
   
3. **Configure Settings**:
   - Edit `include/config.h` with your WiFi and server details
   
4. **Build and Upload**:
   
   **Windows (using Python module):**
   ```cmd
   # Build project
   python -m platformio run
   
   # Upload to ESP32
   python -m platformio run --target upload
   
   # Monitor serial output
   python -m platformio device monitor
   ```
   
   **Linux/Mac (if pio is in PATH):**
   ```bash
   # Build project
   pio run
   
   # Upload to ESP32
   pio run --target upload
   
   # Monitor serial output
   pio device monitor
   ```
   
   **Or use PlatformIO toolbar buttons in VS Code** (recommended)

### Using Arduino IDE:

1. Install ESP32 board support:
   - File → Preferences → Additional Board Manager URLs:
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   
2. Install required libraries (see above)

3. **Important**: Copy all files to Arduino sketch folder structure:
   ```
   Firmware/
   ├── Firmware.ino (rename main.ino)
   ├── config.h
   ├── sensors.h
   ├── sensors.cpp
   ├── network.h
   ├── network.cpp
   ├── led_controller.h
   └── led_controller.cpp
   ```

4. Select board:
   - Tools → Board → ESP32 Dev Module
   - Tools → Flash Size → 4MB
   - Tools → Upload Speed → 921600
   - Tools → Port → (your ESP32 port)

5. Click Upload button

## 🎮 Serial Commands

Connect via Serial Monitor at 115200 baud:

| Command | Description |
|---------|-------------|
| `status` | Display complete system status |
| `sensor` | Show current sensor readings |
| `network` | Display network information |
| `led off` | Turn off all LEDs |
| `led sunrise` | Activate sunrise/sunset effect |
| `led rain` | Activate rain effect |
| `led meteor` | Activate meteor effect |
| `led apocalypse` | Activate apocalypse effect |
| `brightness 0-255` | Set LED brightness |
| `help` | Show all available commands |

## 📡 Data Transmission

The device sends sensor data to the server every 5 seconds (configurable) in JSON format:

```json
{
  "radar": {
    "presence": true,
    "moving": {
      "detected": true,
      "distance": 150,
      "energy": 75
    },
    "stationary": {
      "detected": false,
      "distance": 0,
      "energy": 0
    },
    "timestamp": 123456789
  },
  "temperature": {
    "celsius": 25.5,
    "fahrenheit": 77.9,
    "valid": true,
    "timestamp": 123456789
  },
  "turbidity": {
    "voltage": 2.1,
    "ntu": 15.3,
    "rawADC": 2580,
    "timestamp": 123456789
  }
}
```

## 🎨 LED Effects Explained

### 1. Sunrise/Sunset Effect
- Uses Planck's law to simulate natural sunlight color temperature
- Color temperature varies from 2000K (warm sunrise) to 5500K (midday) to 2000K (sunset)
- Applies convolution smoothing for realistic transitions
- Based on real-world physics of black body radiation

### 2. Rain Effect
- Dark stormy sky background (blue-gray)
- Random lightning flashes
- Rain drop animations

### 3. Meteor Effect
- Shooting stars across the LED strip
- Multiple meteors with trails
- Random colors and speeds

### 4. Apocalypse Effect
- Fire simulation with flickering
- Smoke effects
- Dramatic red/orange/yellow colors

## 🔍 Troubleshooting

### WiFi Won't Connect
- Verify SSID and password in `config.h`
- Check signal strength (ESP32 should be within range)
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

### Sensors Not Reading
- **DS18B20**: Check 4.7kΩ pull-up resistor, verify wiring
- **LD2410**: Ensure RX/TX are not swapped, check 3.3V power
- **Turbidity**: Verify ADC pin (use GPIO 32-39 for ESP32)

### LED Issues
- **Not lighting**: Check data pin connection, verify power supply
- **Wrong colors**: Adjust `COLOR_ORDER` in `config.h` (try GRB, RGB, etc.)
- **Flickering**: Add 100-500µF capacitor across LED power supply

### Data Not Sending to Server
- Verify server IP and port in `config.h`
- Check if server is running (`curl http://SERVER_IP:8080/api/health`)
- Monitor serial output for HTTP response codes

## ⚡ Power Considerations

- ESP32: ~240mA (peak with WiFi)
- Each LED: up to 60mA at full white brightness
- Example: 60 LEDs at 50% brightness ≈ 1.8A
- **Recommended**: 5V 3-5A power supply for stable operation
- Use separate power supply for LED strip (not USB)

## 🔒 Security Notes

- Change default WiFi credentials before deployment
- Consider implementing WPA2-Enterprise for production
- Avoid hardcoding sensitive credentials in production code
- Use HTTPS for server communication (requires additional setup)

## 📝 Calibration

### Turbidity Sensor
1. Place sensor in clean water
2. Note voltage reading (V_clean)
3. Place sensor in known turbid sample
4. Note voltage reading (V_turbid) and NTU value
5. Update calibration values in `sensors.cpp`:
```cpp
turbidity_a_coeff = (NTU_turbid - 0) / (V_turbid - V_clean);
turbidity_b_coeff = -turbidity_a_coeff * V_clean;
```

## 📞 Support

For issues or questions:
1. Check serial monitor output for error messages
2. Verify all wiring connections
3. Ensure all libraries are properly installed
4. Check server connectivity
5. Consult the main README.md for system-wide issues
