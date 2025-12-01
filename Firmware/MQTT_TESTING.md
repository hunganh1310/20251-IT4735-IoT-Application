# 🧪 MQTT Testing Guide

Hướng dẫn test giao tiếp MQTT với thiết bị ESP32.

---

## 📌 Thông tin kết nối MQTT Broker

| Thông số | Giá trị |
|----------|---------|
| **Protocol** | MQTT over TLS/SSL |
| **Broker** | `<YOUR_BROKER>.hivemq.cloud` |
| **Port** | `8883` |
| **Username** | `<YOUR_USERNAME>` |
| **Password** | `<YOUR_PASSWORD>` |
| **Client ID** | `Device_01` |

> ⚠️ **Lưu ý**: Liên hệ team Firmware để lấy credentials thực tế.

---

## 🛠️ Công cụ Test MQTT

### Option 1: HiveMQ Web Client (Đơn giản nhất)
1. Truy cập: http://www.hivemq.com/demos/websocket-client/
2. Điền thông tin kết nối
3. Subscribe topics và gửi test messages

### Option 2: MQTT Explorer (Recommended)
1. Download: http://mqtt-explorer.com/
2. Tạo connection mới với credentials
3. Browse và gửi messages trực quan

### Option 3: mosquitto CLI (Command Line)

**Subscribe tất cả topics của device:**
```bash
mosquitto_sub -h <YOUR_BROKER>.hivemq.cloud \
  -p 8883 \
  -u <YOUR_USERNAME> \
  -P <YOUR_PASSWORD> \
  -t "iot/device01/#" \
  --capath /etc/ssl/certs
```

**Publish LED control command:**
```bash
mosquitto_pub -h <YOUR_BROKER>.hivemq.cloud \
  -p 8883 \
  -u <YOUR_USERNAME> \
  -P <YOUR_PASSWORD> \
  -t "iot/device01/led/control" \
  -m '{"led_mode":"basic","brightness":200,"color":"#FF0000"}' \
  --capath /etc/ssl/certs
```

---

## 📤 LED Control Payload

Device nhận JSON structure trên topic `iot/device01/led/control` để điều khiển LED.

### Payload Schema

```jsonc
{
  "led_mode": "basic",            // off, basic, sky_simulation, rain, meteor, apocalypse
  "brightness": 200,               // 0-255
  "led_is_on": true,               // true/false - bật/tắt LED
  "presence_mode_enabled": true,   // true/false - chế độ tự động theo radar
  "color": "#FF00AA"               // Hex RGB (chỉ dùng với mode "basic")
}
```

### Mô tả các trường

| Field | Type | Mô tả |
|-------|------|-------|
| `led_mode` | string | Chế độ LED: `off`, `basic`, `sky_simulation`, `rain`, `meteor`, `apocalypse` |
| `brightness` | int | Độ sáng 0-255. Bỏ qua nếu giữ nguyên |
| `led_is_on` | bool | Master switch - `false` = tắt LED bất kể mode |
| `presence_mode_enabled` | bool | Bật chế độ tự động theo radar (LED ON khi có người < 20m) |
| `color` | string | Màu hex `#RRGGBB` (chỉ dùng với `led_mode: basic`) |

> 💡 Tất cả các trường đều **optional**. Chỉ gửi trường cần thay đổi.

---

## 📝 Ví dụ LED Control

### Tắt LED hoàn toàn
```json
{
  "led_mode": "off",
  "led_is_on": false
}
```

### Bật LED màu tím, độ sáng 50%
```json
{
  "led_mode": "basic",
  "brightness": 128,
  "led_is_on": true,
  "color": "#8000FF"
}
```

### Chế độ mô phỏng bầu trời + tự động theo radar
```json
{
  "led_mode": "sky_simulation",
  "brightness": 180,
  "led_is_on": true,
  "presence_mode_enabled": true
}
```

### Hiệu ứng sao băng (giữ nguyên brightness)
```json
{
  "led_mode": "meteor",
  "led_is_on": true
}
```

### Chỉ đổi màu và tăng độ sáng
```json
{
  "color": "#FF00AA",
  "brightness": 220
}
```

### Hiệu ứng apocalypse, tắt chế độ tự động
```json
{
  "led_mode": "apocalypse",
  "led_is_on": true,
  "presence_mode_enabled": false,
  "brightness": 255
}
```

### mosquitto_pub Examples

**Bật LED màu hồng với presence mode:**
```bash
mosquitto_pub -h <YOUR_BROKER>.hivemq.cloud -p 8883 \
  -u <YOUR_USERNAME> -P <YOUR_PASSWORD> \
  -t "iot/device01/led/control" \
  -m '{"led_mode":"basic","brightness":200,"led_is_on":true,"presence_mode_enabled":true,"color":"#FF00AA"}' \
  --capath /etc/ssl/certs
```

**Tắt LED:**
```bash
mosquitto_pub -h <YOUR_BROKER>.hivemq.cloud -p 8883 \
  -u <YOUR_USERNAME> -P <YOUR_PASSWORD> \
  -t "iot/device01/led/control" \
  -m '{"led_mode":"off","led_is_on":false}' \
  --capath /etc/ssl/certs
```

---

## 📥 Device Responses

### 💡 LED Status - `iot/device01/led/status`

```json
{
  "mode": "basic",
  "brightness": 150,
  "color": { "r": 255, "g": 0, "b": 0 },
  "timestamp": 123456
}
```

### 📊 Sensor Data - `iot/device01/sensors` (mỗi 5 giây)

```json
{
  "temperature": 25.5,
  "turbidity": 10.5,
  "water_quality": "Good",
  "ph": 7.1,
  "timestamp": 123456
}
```

### 🔌 Device Status - `iot/device01/status`

```json
{
  "status": "online",
  "timestamp": 123456,
  "clientId": "Device_01"
}
```

---

## 📡 Monitoring Topics

| Topic | Mô tả |
|-------|-------|
| `iot/device01/#` | Tất cả topics của device |
| `iot/device01/led/status` | Chỉ trạng thái LED |
| `iot/device01/sensors` | Chỉ dữ liệu cảm biến |
| `iot/device01/radar/status` | Chỉ trạng thái radar |
| `iot/device01/status` | Chỉ trạng thái device |

---

## ❌ Troubleshooting

### Device không phản hồi
- Kiểm tra Serial Monitor của ESP32
- Xác nhận kết nối WiFi
- Kiểm tra MQTT credentials
- Đảm bảo device đang bật nguồn

### Commands không hoạt động
- Kiểm tra JSON format hợp lệ
- Kiểm tra chính tả topic name
- Đảm bảo brightness trong range 0-255
- Đảm bảo màu sắc đúng format hex `#RRGGBB`

### Connection Timeout
- Kiểm tra kết nối internet
- Xác nhận TLS/SSL được hỗ trợ
- Tăng connection timeout
- Kiểm tra firewall settings

---

## 🐍 Python Example Script

```python
import paho.mqtt.client as mqtt
import json
import time

# MQTT Configuration - Thay bằng credentials thực tế
BROKER = "<YOUR_BROKER>.hivemq.cloud"
PORT = 8883
USERNAME = "<YOUR_USERNAME>"
PASSWORD = "<YOUR_PASSWORD>"

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # Subscribe tất cả topics của device
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
client.loop_start()

# Send test command
time.sleep(2)
print("Sending LED control command...")
command = {
    "led_mode": "basic",
    "brightness": 200,
    "led_is_on": True,
    "color": "#FF0000"
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

---

## 📦 Node.js Example Script

```javascript
const mqtt = require('mqtt');

// MQTT Configuration - Thay bằng credentials thực tế
const options = {
  host: '<YOUR_BROKER>.hivemq.cloud',
  port: 8883,
  protocol: 'mqtts',
  username: '<YOUR_USERNAME>',
  password: '<YOUR_PASSWORD>'
};

const client = mqtt.connect(options);

client.on('connect', () => {
  console.log('Connected to MQTT broker');
  
  // Subscribe tất cả topics của device
  client.subscribe('iot/device01/#', (err) => {
    if (!err) {
      console.log('Subscribed to device topics');
      
      // Send test command
      const command = {
        led_mode: 'basic',
        brightness: 200,
        led_is_on: true,
        color: '#FF0000'
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

---

**Xem thêm**: [MQTT_API.md](MQTT_API.md) - Tài liệu API chi tiết cho team Software
