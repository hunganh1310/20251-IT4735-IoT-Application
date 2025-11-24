# ESP32 IoT Firmware

Complete IoT firmware for ESP32 with LED control, sensor monitoring, and MQTT communication.

## 🚀 Features

- **LED Control**: WS2812 LED strip with multiple effects
  - Sky Simulation (dynamic sun movement)
  - Rain Effect
  - Meteor Effect
  - Apocalypse Effect
  - Basic (solid color)
- **Sensors**:
  - 🌡️ DS18B20 temperature sensor (water temperature)
  - 💧 Turbidity sensor (water quality monitoring)
  - 📡 LD2410B radar sensor (presence detection)
- **Communication**:
  - 📡 MQTT over TLS/SSL (HiveMQ Cloud)
  - 🌐 WiFi connectivity
  - ⏰ NTP time synchronization

## 📋 Quick Start

1. **Install PlatformIO** (VS Code extension or CLI)
2. **Configure WiFi & MQTT** in `include/config.h`
3. **Build**: `pio run`
4. **Upload**: `pio run --target upload`
5. **Monitor**: `pio device monitor`

## 📖 Documentation

- **[README.md](README.md)** - This file (overview)
- **[WIRING.md](WIRING.md)** - Complete hardware wiring guide
- **[MQTT_TESTING.md](MQTT_TESTING.md)** - MQTT testing and examples

## 🔌 Hardware Connections

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| WS2812 LED | GPIO18 | Requires 5V power supply |
| DS18B20 | GPIO21 | Needs 4.7kΩ pull-up resistor |
| Turbidity | GPIO19 | Analog input (0-3.3V) |
| LD2410B RX | GPIO16 | Radar sensor |
| LD2410B TX | GPIO17 | Radar sensor |
| LD2410B OUT | GPIO4 | Presence detection |

See **[WIRING.md](WIRING.md)** for detailed wiring diagrams.

## 📡 MQTT Topics

### Subscribe (Device receives)
- `iot/device01/led/control` - LED commands

### Publish (Device sends)
- `iot/device01/led/status` - LED status
- `iot/device01/sensors` - Sensor data (temp + turbidity)
- `iot/device01/status` - Device status

See **[MQTT_TESTING.md](MQTT_TESTING.md)** for message formats and examples.

## 🎮 LED Control Examples

### Turn Off
```json
{"mode": "off"}
```

### Basic Color (Red)
```json
{
  "mode": "basic",
  "brightness": 150,
  "color": {"r": 255, "g": 0, "b": 0}
}
```

### Sky Simulation
```json
{"mode": "sky_simulation", "brightness": 180}
```

## 📊 Sensor Data Format

```json
{
  "temperature": 25.5,
  "turbidity": 10.5,
  "waterQuality": "Good",
  "timestamp": 123456
}
```

Water quality levels:
- **Excellent**: < 5 NTU
- **Good**: 5-50 NTU
- **Fair**: 50-100 NTU
- **Poor**: 100-500 NTU
- **Very Poor**: > 500 NTU

## 🛠️ Configuration

Edit `include/config.h`:

```cpp
// WiFi
#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"

// MQTT
#define MQTT_SERVER "your-broker.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "your-username"
#define MQTT_PASSWORD "your-password"

// Pins (already configured)
#define LED_PIN 18
#define DS18B20_PIN 21
#define TURBIDITY_SENSOR_PIN 19
```

## 📁 Project Structure

```
Firmware/
├── include/               # Header files
│   ├── config.h          # Global configuration
│   ├── led_controller.h
│   ├── ds18b20_sensor.h
│   ├── turbidity_sensor.h
│   └── mqtt_handler.h
├── src/                  # Source files
│   ├── main.cpp          # Main application
│   ├── led/
│   │   └── led_controller.cpp
│   ├── sensors/
│   │   ├── ds18b20_sensor.cpp
│   │   └── turbidity_sensor.cpp
│   └── mqtt/
│       └── mqtt_handler.cpp
├── platformio.ini        # PlatformIO config
├── WIRING.md            # Wiring guide
└── MQTT_TESTING.md      # MQTT testing guide
```

## 📦 Dependencies

Automatically managed by PlatformIO:

- `fastled/FastLED` - LED control
- `ncmreynolds/ld2410` - Radar sensor
- `knolleary/PubSubClient` - MQTT client
- `bblanchon/ArduinoJson` - JSON parsing
- `paulstoffregen/OneWire` - OneWire protocol
- `milesburton/DallasTemperature` - DS18B20 sensor

## 🔧 Troubleshooting

### DS18B20 Not Found
- Check 4.7kΩ pull-up resistor between Data and VCC
- Verify wiring connections
- Try another sensor

### MQTT Connection Failed
- Verify WiFi connection (check serial monitor)
- Check MQTT credentials in config.h
- Ensure broker address and port are correct
- Verify TLS/SSL certificate

### LED Not Working
- Check 5V power supply (sufficient current)
- Verify data pin connection (GPIO18)
- Test with lower brightness first

### Turbidity Readings Unstable
- Calibrate sensor: `turbiditySensor.calibrate(clearVoltage, dirtyVoltage)`
- Ensure sensor is fully submerged
- Keep sensor clean

## 🧪 Testing

```bash
# Run all tests
pio test

# Monitor serial output
pio device monitor
```

## 🐛 Serial Monitor Output

Expected output after successful initialization:

```
=================================
ESP32 IoT Application Starting...
=================================

[WiFi] Connecting to: YourSSID
[WiFi] Connected!
[WiFi] IP Address: 192.168.1.100

[NTP] Time synchronized!
[LED] Controller initialized
[Radar] LD2410B initialized successfully
[DS18B20] Sensor initialized on pin 21
[Turbidity] Sensor initialized on pin 19
[MQTT] Handler initialized
[MQTT] Connecting to broker... Connected!

=================================
System Initialization Complete!
=================================

[Sensor] Temperature: 25.5 °C
[Sensor] Turbidity: 10.5 NTU (Good)
[MQTT] Published sensor data
```

## 🔒 Security Notes

- MQTT uses TLS/SSL encryption (port 8883)
- Change default WiFi credentials before deployment
- Use strong MQTT passwords
- Consider implementing OTA updates for production

## 📝 License

See LICENSE file in the root directory.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📞 Support

For issues and questions:
- Check documentation in this folder
- Review serial monitor output for errors
- Verify hardware connections
- Test individual components separately

---

**Last Updated**: November 2025  
**Version**: 1.0.0  
**Platform**: ESP32 (Arduino Framework)
