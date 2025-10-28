#include <Arduino.h>
#include <LD2410.h>

// Chọn chân UART cho ESP32 (RX, TX)
#define RADAR_RX_PIN 16  // nối tới module TX
#define RADAR_TX_PIN 17  // nối tới module RX
#define OUT_PIN 4        // nối tới LD2410 OUT

HardwareSerial radarSerial(2); // Serial2 (UART2)
LD2410 radar;

volatile bool out_changed = false;
volatile int out_state = 0;

void IRAM_ATTR outISR() {
    out_state = digitalRead(OUT_PIN);
    out_changed = true;
}

void setup() {
    Serial.begin(115200);
    delay(100);

    pinMode(OUT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(OUT_PIN), outISR, CHANGE);

    // Khởi tạo Serial2 tốc độ 256000, mapping RX/TX pins
    radarSerial.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
    Serial.println("Starting radar...");

    // Khởi tạo library với Stream là radarSerial
    if (!radar.begin(radarSerial, true)) {
        Serial.println("LD2410 not detected or failed to init.");
    } else {
        Serial.println("LD2410 initialised!");
    }

  // Optional: bật debug output từ library
  // radar.debug(Serial);
}

void loop() {
  // Gọi read thường xuyên để library xử lý frames
    radar.read();

  // Dữ liệu đơn giản
    if (radar.presenceDetected()) {
        Serial.print("[Serial] Presence detected! ");
        if (radar.movingTargetDetected()) {
            Serial.print("Moving @");
            Serial.print(radar.movingTargetDistance());
            Serial.print("cm energy=");
            Serial.print(radar.movingTargetEnergy());
        }
        if (radar.stationaryTargetDetected()) {
            Serial.print(" | Stationary @");
            Serial.print(radar.stationaryTargetDistance());
            Serial.print("cm energy=");
            Serial.print(radar.stationaryTargetEnergy());
        }
        Serial.println();
    }

  // Kiểm tra OUT pin (interrupt)
    if (out_changed) {
        noInterrupts();
        bool s = out_state;
        out_changed = false;
        interrupts();
        Serial.print("[OUT pin] state=");
        Serial.println(s ? "HIGH (presence)" : "LOW (no-presence)");
    }

  // Nếu muốn in raw distances continuously (ví dụ demo)
    static unsigned long last = 0;
    if (millis() - last > 1000) {
        last = millis();
        Serial.print("Moving distance: ");
        Serial.print(radar.movingTargetDistance());
        Serial.print(" cm, energy ");
        Serial.println(radar.movingTargetEnergy());
    }

    delay(50);
}
