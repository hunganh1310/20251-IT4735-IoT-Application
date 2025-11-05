/**
 * @file utils.cpp
 * @brief Utility functions implementation
 */

#include "utils.h"
#include "config.h"
#include <WiFi.h>
#include <time.h>

namespace Utils {

String formatUptime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  
  String uptime = "";
  if (days > 0) uptime += String(days) + "d ";
  if (hours > 0) uptime += String(hours) + "h ";
  if (minutes > 0) uptime += String(minutes) + "m ";
  uptime += String(seconds) + "s";
  
  return uptime;
}

String formatBytes(size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0, 2) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0, 2) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0, 2) + " GB";
}

void printMemoryInfo() {
  Serial.println("\n========== Memory Info ==========");
  Serial.print("Free Heap: ");
  Serial.println(formatBytes(ESP.getFreeHeap()));
  Serial.print("Heap Size: ");
  Serial.println(formatBytes(ESP.getHeapSize()));
  Serial.print("Free PSRAM: ");
  Serial.println(formatBytes(ESP.getFreePsram()));
  Serial.print("PSRAM Size: ");
  Serial.println(formatBytes(ESP.getPsramSize()));
  Serial.println("=================================\n");
}

size_t getFreeHeap() {
  return ESP.getFreeHeap();
}

String getTimestamp() {
  time_t now = time(nullptr);
  return String(now);
}

String formatDateTime(time_t timestamp) {
  struct tm* timeinfo = localtime(&timestamp);
  char buffer[64];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  return String(buffer);
}

void printSystemInfo() {
  Serial.println("\n========== System Info ==========");
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Chip Revision: ");
  Serial.println(ESP.getChipRevision());
  Serial.print("CPU Frequency: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("Flash Size: ");
  Serial.println(formatBytes(ESP.getFlashChipSize()));
  Serial.print("SDK Version: ");
  Serial.println(ESP.getSdkVersion());
  Serial.print("Uptime: ");
  Serial.println(formatUptime(millis()));
  Serial.println("=================================\n");
}

void printWiFiInfo() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Not connected");
    return;
  }
  
  Serial.println("\n========== WiFi Info ==========");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Signal Strength: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("Channel: ");
  Serial.println(WiFi.channel());
  Serial.println("===============================\n");
}

} // namespace Utils
