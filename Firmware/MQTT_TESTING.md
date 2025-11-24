# MQTT Testing Guide

Quick reference for testing MQTT communication with the ESP32 device.

## MQTT Broker Configuration

**Server**: 990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud  
**Port**: 8883 (TLS/SSL)  
**Username**: mqtt1  
**Password**: Mqtt123456  
**Client ID**: Device_01

## Testing with MQTT Client

### Option 1: HiveMQ Web Client
1. Visit: http://www.hivemq.com/demos/websocket-client/
2. Connect to the broker using credentials above
3. Subscribe to topics and send test messages

### Option 2: MQTT Explorer
1. Download: http://mqtt-explorer.com/
2. Create new connection with credentials above
3. Browse topics and send messages

### Option 3: mosquitto_pub/sub (Command Line)

**Subscribe to all device topics:**
```bash
mosquitto_sub -h 990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud \
  -p 8883 \
  -u mqtt1 \
  -P Mqtt123456 \
  -t "iot/device01/#" \
  --capath /etc/ssl/certs
```

**Publish LED control command:**
```bash
mosquitto_pub -h 990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud \
  -p 8883 \
  -u mqtt1 \
  -P Mqtt123456 \
  -t "iot/device01/led/control" \
  -m '{"mode":"basic","brightness":200,"color":{"r":255,"g":0,"b":0}}' \
  --capath /etc/ssl/certs
```

## Unified Control Payload

Device now accepts a single JSON structure on the control topic to manage LED mode, power state, auto presence mode (radar), brightness and color.

**Control Topic**: `iot/device01/led/control` (also used for radar auto-mode flag)

### Payload Schema
```jsonc
{
  "led_mode": "basic",            // one of: off, basic, sky_simulation, rain, meteor, apocalypse
  "brightness": 200,               // 0-255, ignored if led_is_on=false
  "led_is_on": true,               // explicit on/off override (false forces off regardless of mode)
  "presence_mode_enabled": true,   // when true and radar enabled, LED auto ON/OFF by presence (<20m)
  "color": "#FF00AA"               // hex RGB when led_mode=basic (ignored for effects)
}
```

### Field Details
- `led_mode`: Selects animation/effect; use `off` to turn off or rely on `led_is_on`.
- `brightness`: 0 (dark) .. 255 (max). If omitted keeps previous brightness.
- `led_is_on`: Master enable; if `false` forces LED off regardless of mode.
- `presence_mode_enabled`: Enables radar-driven automatic control (LED ON when human <20m, OFF otherwise). Requires radar to be enabled in firmware.
- `color`: Hex string `#RRGGBB` used only when `led_mode` = `basic`.

### Examples

Turn LED fully off (disables any effect display):
```json
{
  "led_mode": "off",
  "led_is_on": false
}
```

Basic solid color purple at half brightness:
```json
{
  "led_mode": "basic",
  "brightness": 128,
  "led_is_on": true,
  "presence_mode_enabled": false,
  "color": "#8000FF"
}
```

Enable sky simulation with presence auto mode (LED will still go off if no human detected):
```json
{
  "led_mode": "sky_simulation",
  "brightness": 180,
  "led_is_on": true,
  "presence_mode_enabled": true
}
```

Switch to meteor effect keeping previous brightness:
```json
{
  "led_mode": "meteor",
  "led_is_on": true
}
```

Just change color (keep current mode/basic assumed) and raise brightness:
```json
{
  "color": "#FF00AA",
  "brightness": 220
}
```

Disable presence mode but keep LEDs on in apocalypse effect:
```json
{
  "led_mode": "apocalypse",
  "led_is_on": true,
  "presence_mode_enabled": false,
  "brightness": 255
}
```

### mosquitto_pub Examples
```bash
mosquitto_pub -h 990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud -p 8883 -u mqtt1 -P Mqtt123456 \
  -t "iot/device01/led/control" \
  -m '{"led_mode":"basic","brightness":200,"led_is_on":true,"presence_mode_enabled":true,"color":"#FF00AA"}' \
  --capath /etc/ssl/certs

mosquitto_pub -h 990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud -p 8883 -u mqtt1 -P Mqtt123456 \
  -t "iot/device01/led/control" \
  -m '{"led_mode":"off","led_is_on":false}' \
  --capath /etc/ssl/certs
```

## Expected Device Responses

### LED Status Update
**Topic**: `iot/device01/led/status`  
**Message Example**:
```json
{
  "mode": "basic",
  "brightness": 150,
  "color": {
    "r": 255,
    "g": 0,
    "b": 0
  },
  "timestamp": 123456
}
```

### Sensor Data Update (Every 5 seconds)
**Topic**: `iot/device01/sensors`  
**Message Example**:
```json
{
  "temperature": 25.5,
  "turbidity": 10.5,
  "waterQuality": "Good",
  "timestamp": 123456
}
```

### Device Status (On Connect/Disconnect)
**Topic**: `iot/device01/status`  
**Message Example**:
```json
{
  "status": "online",
  "timestamp": 123456,
  "clientId": "Device_01"
}
```

## Monitoring Topics

Subscribe to these topics to monitor device activity:

1. **All device topics**: `iot/device01/#`
2. **LED status only**: `iot/device01/led/status`
3. **Sensor data only**: `iot/device01/sensors`
4. **Device status only**: `iot/device01/status`

## Troubleshooting

### No Response from Device
- Check ESP32 serial monitor for MQTT connection status
- Verify WiFi connection
- Check MQTT credentials
- Ensure device is powered on

### Commands Not Working
- Verify JSON format is valid
- Check topic name spelling
- Ensure brightness is 0-255
- Ensure RGB values are 0-255

### Connection Timeouts
- Check internet connection
- Verify TLS/SSL support in client
- Try increasing connection timeout
- Check firewall settings

## Python Example Script

```python
import paho.mqtt.client as mqtt
import json
import time

# MQTT Configuration
BROKER = "990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud"
PORT = 8883
USERNAME = "mqtt1"
PASSWORD = "Mqtt123456"

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("iot/device01/#")

def on_message(client, userdata, msg):
    print(f"Topic: {msg.topic}")
    print(f"Message: {msg.payload.decode()}")
    print("---")

# Create client
client = mqtt.Client()
client.username_pw_set(USERNAME, PASSWORD)
client.tls_set()
client.on_connect = on_connect
client.on_message = on_message

# Connect
client.connect(BROKER, PORT, 60)

# Start loop
client.loop_start()

# Send test commands
time.sleep(2)
print("Sending LED control command...")
command = {
    "mode": "basic",
    "brightness": 200,
    "color": {"r": 255, "g": 0, "b": 0}
}
client.publish("iot/device01/led/control", json.dumps(command))

# Keep running
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    client.loop_stop()
    client.disconnect()
```

## Node.js Example Script

```javascript
const mqtt = require('mqtt');

// MQTT Configuration
const options = {
  host: '990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud',
  port: 8883,
  protocol: 'mqtts',
  username: 'mqtt1',
  password: 'Mqtt123456'
};

// Connect
const client = mqtt.connect(options);

client.on('connect', () => {
  console.log('Connected to MQTT broker');
  
  // Subscribe to all device topics
  client.subscribe('iot/device01/#', (err) => {
    if (!err) {
      console.log('Subscribed to device topics');
      
      // Send test command
      const command = {
        mode: 'basic',
        brightness: 200,
        color: { r: 255, g: 0, b: 0 }
      };
      client.publish('iot/device01/led/control', JSON.stringify(command));
    }
  });
});

client.on('message', (topic, message) => {
  console.log(`Topic: ${topic}`);
  console.log(`Message: ${message.toString()}`);
  console.log('---');
});

client.on('error', (error) => {
  console.error('Connection error:', error);
});
```
