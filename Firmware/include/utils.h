/**
 * @file utils.h
 * @brief Utility functions and helpers
 */

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

namespace Utils {
  // String utilities
  String formatUptime(unsigned long milliseconds);
  String formatBytes(size_t bytes);
  
  // Memory utilities
  void printMemoryInfo();
  size_t getFreeHeap();
  
  // Time utilities
  String getTimestamp();
  String formatDateTime(time_t timestamp);
  
  // Debug utilities
  void printSystemInfo();
  void printWiFiInfo();
}

#endif // UTILS_H
