# 📡 MQTT API Documentation

Tài liệu mô tả đầy đủ các MQTT Topics và Commands cho team Software phát triển Backend/Frontend.

## 📌 Thông tin kết nối MQTT Broker

| Thông số | Giá trị |
|----------|---------|
| **Protocol** | MQTT over TLS/SSL |
| **Broker** | HiveMQ Cloud |
| **Port** | 8883 |
| **Client ID** | `Device_01` (mỗi thiết bị có ID riêng) |

> ⚠️ **Lưu ý**: Credentials được cấu hình trong firmware. Liên hệ team Firmware để lấy thông tin.

---

## 📊 Tổng quan Topics

### Wildcard Topic (Subscribe tất cả)

| Topic | Mô tả |
|-------|-------|
| `iot/device01/#` | Subscribe tất cả topics của device01 |

### Device → Server (Publish)

| Topic | Mô tả | Tần suất |
|-------|-------|----------|
| `iot/device01/sensors` | Dữ liệu cảm biến | Mỗi 5 giây |
| `iot/device01/led/status` | Trạng thái LED | Khi thay đổi |
| `iot/device01/radar/status` | Trạng thái radar | Khi thay đổi |
| `iot/device01/status` | Trạng thái thiết bị | Kết nối/ngắt kết nối |

### Server → Device (Subscribe)

| Topic | Mô tả |
|-------|-------|
| `iot/device01/led/control` | Điều khiển LED |
| `iot/device01/radar/control` | Điều khiển radar |

---

## 📥 TOPICS CHI TIẾT - Device Publish (Gửi đi)

### 1. Dữ liệu cảm biến - `iot/device01/sensors`

**Tần suất**: Gửi mỗi 5 giây

**Payload Schema:**
```json
{
  "temperature": 25.5,
  "turbidity": 10.5,
  "water_quality": "Good",
  "ph": 7.1,
  "timestamp": 123456789
}
```

**Mô tả các trường:**

| Field | Type | Range | Mô tả |
|-------|------|-------|-------|
| `temperature` | float | -55 đến 125 | Nhiệt độ nước (°C) |
| `turbidity` | float | 0 đến 3000+ | Độ đục (NTU) |
| `water_quality` | string | - | Đánh giá chất lượng nước |
| `ph` | float | 0 đến 14 | Độ pH (hiện tại mô phỏng) |
| `timestamp` | integer | - | Thời gian (millis từ lúc khởi động) |

**Bảng đánh giá Water Quality:**

| NTU Range | water_quality |
|-----------|---------------|
| < 5 | Excellent |
| 5 - 50 | Good |
| 50 - 100 | Fair |
| 100 - 500 | Poor |
| > 500 | Very Poor |

---

### 2. Trạng thái LED - `iot/device01/led/status`

**Tần suất**: Khi LED thay đổi trạng thái hoặc mỗi 10 giây

**Payload Schema:**
```json
{
  "mode": "basic",
  "brightness": 150,
  "color": {
    "r": 255,
    "g": 100,
    "b": 50
  },
  "timestamp": 123456789
}
```

**Mô tả các trường:**

| Field | Type | Range | Mô tả |
|-------|------|-------|-------|
| `mode` | string | - | Chế độ LED hiện tại |
| `brightness` | integer | 0-255 | Độ sáng |
| `color.r` | integer | 0-255 | Red component |
| `color.g` | integer | 0-255 | Green component |
| `color.b` | integer | 0-255 | Blue component |
| `timestamp` | integer | - | Thời gian |

**Các giá trị `mode`:**
- `off` - LED tắt
- `basic` - Màu đơn sắc
- `sky_simulation` - Mô phỏng bầu trời
- `rain` - Hiệu ứng mưa
- `meteor` - Hiệu ứng sao băng
- `apocalypse` - Hiệu ứng tận thế

---

### 3. Trạng thái Radar - `iot/device01/radar/status`

**Tần suất**: Khi có thay đổi phát hiện chuyển động

**Payload Schema:**
```json
{
  "enabled": true,
  "autoMode": true,
  "presenceDetected": true,
  "distance": 150,
  "timestamp": 123456789
}
```

**Mô tả các trường:**

| Field | Type | Mô tả |
|-------|------|-------|
| `enabled` | boolean | Radar đang bật/tắt |
| `autoMode` | boolean | Chế độ tự động (LED theo radar) |
| `presenceDetected` | boolean | Phát hiện có người |
| `distance` | integer | Khoảng cách đến đối tượng (cm) |
| `timestamp` | integer | Thời gian |

---

### 4. Trạng thái thiết bị - `iot/device01/status`

**Tần suất**: Khi kết nối/ngắt kết nối

**Payload Schema:**
```json
{
  "status": "online",
  "timestamp": 123456789,
  "clientId": "Device_01"
}
```

**Mô tả các trường:**

| Field | Type | Mô tả |
|-------|------|-------|
| `status` | string | `online` hoặc `offline` |
| `timestamp` | integer | Thời gian |
| `clientId` | string | ID thiết bị |

---

## 📤 TOPICS CHI TIẾT - Device Subscribe (Nhận vào)

### 1. Điều khiển LED - `iot/device01/led/control`

**Payload Schema đầy đủ:**
```json
{
  "led_mode": "basic",
  "brightness": 200,
  "led_is_on": true,
  "presence_mode_enabled": false,
  "color": "#FF00AA"
}
```

**Mô tả các trường:**

| Field | Type | Required | Range | Mô tả |
|-------|------|----------|-------|-------|
| `led_mode` | string | No | - | Chế độ LED |
| `brightness` | integer | No | 0-255 | Độ sáng |
| `led_is_on` | boolean | No | - | Bật/tắt LED |
| `presence_mode_enabled` | boolean | No | - | Bật chế độ tự động theo radar |
| `color` | string | No | Hex | Màu (chỉ dùng với mode `basic`) |

> 💡 Tất cả các trường đều **optional**. Chỉ gửi trường cần thay đổi.

**Các giá trị `led_mode`:**
- `off` - Tắt LED
- `basic` - Màu đơn sắc (sử dụng kèm `color`)
- `sky_simulation` - Mô phỏng bầu trời
- `rain` - Hiệu ứng mưa
- `meteor` - Hiệu ứng sao băng
- `apocalypse` - Hiệu ứng tận thế

---

### 📝 Ví dụ LED Control Commands

#### Tắt LED hoàn toàn
```json
{
  "led_mode": "off",
  "led_is_on": false
}
```

#### Bật LED màu đỏ, độ sáng 80%
```json
{
  "led_mode": "basic",
  "brightness": 200,
  "led_is_on": true,
  "color": "#FF0000"
}
```

#### Bật LED màu xanh dương
```json
{
  "led_mode": "basic",
  "led_is_on": true,
  "color": "#0000FF"
}
```

#### Chế độ mô phỏng bầu trời
```json
{
  "led_mode": "sky_simulation",
  "brightness": 180,
  "led_is_on": true
}
```

#### Hiệu ứng mưa
```json
{
  "led_mode": "rain",
  "brightness": 150,
  "led_is_on": true
}
```

#### Hiệu ứng sao băng
```json
{
  "led_mode": "meteor",
  "brightness": 200
}
```

#### Bật chế độ tự động (radar phát hiện người → LED bật)
```json
{
  "led_mode": "sky_simulation",
  "led_is_on": true,
  "presence_mode_enabled": true
}
```

#### Chỉ thay đổi độ sáng (giữ nguyên mode hiện tại)
```json
{
  "brightness": 100
}
```

#### Chỉ thay đổi màu (giữ nguyên mode basic)
```json
{
  "color": "#00FF00"
}
```

---

### 2. Điều khiển Radar - `iot/device01/radar/control`

**Payload Schema:**
```json
{
  "enabled": true
}
```

**Mô tả các trường:**

| Field | Type | Required | Mô tả |
|-------|------|----------|-------|
| `enabled` | boolean | Yes | Bật/tắt radar |

**Ví dụ:**

#### Bật radar (kích hoạt chế độ tự động)
```json
{
  "enabled": true
}
```

#### Tắt radar (LED sẽ tắt)
```json
{
  "enabled": false
}
```

---

## 🔄 Luồng hoạt động

### Kịch bản 1: Điều khiển LED thủ công

```
[Frontend/Backend]                    [ESP32 Device]
      |                                     |
      |---> iot/device01/led/control ------>|
      |     {"led_mode":"basic",            |
      |      "color":"#FF0000"}             |
      |                                     |
      |<--- iot/device01/led/status <-------|
      |     {"mode":"basic",                |
      |      "brightness":128,              |
      |      "color":{"r":255,"g":0,"b":0}} |
```

### Kịch bản 2: Chế độ tự động (Radar)

```
[Frontend/Backend]                    [ESP32 Device]
      |                                     |
      |---> iot/device01/led/control ------>|
      |     {"presence_mode_enabled":true}  |
      |                                     |
      |                              [Radar phát hiện người]
      |                                     |
      |<--- iot/device01/radar/status <-----|
      |     {"presenceDetected":true,       |
      |      "distance":150}                |
      |                                     |
      |<--- iot/device01/led/status <-------|
      |     {"mode":"basic"}  (LED tự bật)  |
```

### Kịch bản 3: Giám sát cảm biến

```
[Frontend/Backend]                    [ESP32 Device]
      |                                     |
      |<--- iot/device01/sensors <----------|  (Mỗi 5 giây)
      |     {"temperature":25.5,            |
      |      "turbidity":10.5,              |
      |      "water_quality":"Good",        |
      |      "ph":7.1}                      |
```

---

## 💻 Code Examples

### JavaScript/Node.js

```javascript
const mqtt = require('mqtt');

const options = {
  host: 'YOUR_BROKER.hivemq.cloud',
  port: 8883,
  protocol: 'mqtts',
  username: 'YOUR_USERNAME',
  password: 'YOUR_PASSWORD'
};

const client = mqtt.connect(options);

client.on('connect', () => {
  console.log('Connected to MQTT broker');
  
  // Subscribe tất cả topics của device
  client.subscribe('iot/device01/#');
  
  // Gửi lệnh điều khiển LED
  client.publish('iot/device01/led/control', JSON.stringify({
    led_mode: 'basic',
    brightness: 200,
    led_is_on: true,
    color: '#FF0000'
  }));
});

client.on('message', (topic, message) => {
  const data = JSON.parse(message.toString());
  console.log(`Topic: ${topic}`, data);
  
  // Xử lý dữ liệu cảm biến
  if (topic === 'iot/device01/sensors') {
    console.log(`Nhiệt độ: ${data.temperature}°C`);
    console.log(`Độ đục: ${data.turbidity} NTU`);
  }
});
```

### Python

```python
import paho.mqtt.client as mqtt
import json

BROKER = "YOUR_BROKER.hivemq.cloud"
PORT = 8883
USERNAME = "YOUR_USERNAME"
PASSWORD = "YOUR_PASSWORD"

def on_connect(client, userdata, flags, rc):
    print(f"Connected: {rc}")
    client.subscribe("iot/device01/#")
    
    # Gửi lệnh điều khiển LED
    command = {
        "led_mode": "basic",
        "brightness": 200,
        "led_is_on": True,
        "color": "#FF0000"
    }
    client.publish("iot/device01/led/control", json.dumps(command))

def on_message(client, userdata, msg):
    data = json.loads(msg.payload.decode())
    print(f"Topic: {msg.topic}")
    
    if msg.topic == "iot/device01/sensors":
        print(f"Nhiệt độ: {data['temperature']}°C")
        print(f"Độ đục: {data['turbidity']} NTU")

client = mqtt.Client()
client.username_pw_set(USERNAME, PASSWORD)
client.tls_set()
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT, 60)
client.loop_forever()
```

---

## 🧪 Testing

### Sử dụng MQTT Explorer
1. Download: http://mqtt-explorer.com/
2. Kết nối với broker credentials
3. Subscribe topic: `iot/device01/#`
4. Publish commands tới topic control

### Sử dụng HiveMQ Web Client
1. Truy cập: http://www.hivemq.com/demos/websocket-client/
2. Kết nối với credentials
3. Subscribe và publish messages

---

## ❓ FAQ

### Q: Tại sao không nhận được dữ liệu cảm biến?
A: Kiểm tra:
- Device đã online chưa (subscribe `iot/device01/status`)
- Kết nối WiFi của device
- Credentials MQTT đúng

### Q: LED không phản hồi command?
A: Kiểm tra:
- JSON format có đúng không
- Topic có chính xác không
- Xem Serial Monitor của ESP32

### Q: Làm sao biết device đang online?
A: Subscribe topic `iot/device01/status`, khi device kết nối sẽ publish `{"status":"online"}`

---

## 📞 Liên hệ

- **Firmware Issues**: Team Firmware
- **MQTT Broker Issues**: Team DevOps
- **API Questions**: Team Backend

---

**Version**: 1.0.0  
**Last Updated**: December 2025
1. cd vào fe
  npm install + npm run dev
2. cd vào be
  docker compose up -d
  npm install
  npx prisma migrate dev --name init
  npm run start:dev