#include "watchdog.h"

#include <Arduino.h>
// #include "OLED.h"
#include "LCD1602.h"

namespace xrp {
  void Watchdog::feed() {
    if (!_lastSatisfiedState) {
      // Transitioned to Simulator connected
      Serial.printf("[WD:%s - Feed] Simulator connected.\n", _name.c_str());
      drawDSConnect(true);
    } else {
      // Serial.print(_lastWatchdogFeedTime);
    }
    _lastSatisfiedState = true;
    _lastWatchdogFeedTime = millis();
  }

  bool Watchdog::satisfied() {

    if (_watchdogTimeout == 0) {
      if (!_lastSatisfiedState) {
        // Transitioned to Simulator connected
        Serial.printf("[WD:%s - Sat] Simulator connected.\n", _name.c_str());
        drawDSConnect(true);
      }
      _lastSatisfiedState = true;
      return true;
    }

    if (millis() - _lastWatchdogFeedTime < _watchdogTimeout) {
      if (!_lastSatisfiedState) {
        // Transitioned to Simulator connected. Initially after 1 second
        Serial.printf("[WD:%s - Sat] Simulator connected. After 1 second\n", _name.c_str());
        drawDSConnect(true);
      }
      _lastSatisfiedState = true;
      return true;
    }

    if (_lastSatisfiedState) {
      Serial.printf("[WD:%s - Sat] Simulator Disconnected!!!\n", _name.c_str());
      drawDSConnect(false);
    }
    _lastSatisfiedState = false;
    return false;
  }

  void Watchdog::setTimeout(unsigned long timeout) {
    _watchdogTimeout = timeout;
  }
}