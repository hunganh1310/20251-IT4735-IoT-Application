#include <OneWire.h>
#include <DallasTemperature.h>

// Chọn pin DATA
#define ONE_WIRE_BUS 4   // đổi sang pin bạn dùng

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  delay(100);
  sensors.begin();

  Serial.println("Scanning DS18B20 devices...");
  DeviceAddress deviceAddress;
  int count = 0;
  oneWire.reset_search();
  while (oneWire.search(deviceAddress)) {
    Serial.print("Found device #"); Serial.print(count++);
    Serial.print(" : ");
    for (uint8_t i = 0; i < 8; i++) {
      if (deviceAddress[i] < 16) Serial.print('0');
      Serial.print(deviceAddress[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
  if (count == 0) Serial.println("No DS18B20 found!");
}

void loop() {
  sensors.requestTemperatures(); // gửi lệnh đọc cho tất cả cảm biến
  float tempC = sensors.getTempCByIndex(0); // lấy cảm biến đầu tiên
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
  } else {
    Serial.print("Temp C: ");
    Serial.print(tempC, 2);
    Serial.print("  | F: ");
    Serial.println( (tempC * 9.0/5.0) + 32.0, 2 );
  }
  delay(2000);
}
