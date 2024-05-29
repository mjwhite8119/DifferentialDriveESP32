#include "watchdog.h"

#include <Arduino.h>
// #include "OLED.h"
#include "LCD1602.h"

namespace xrp {
  void Watchdog::feed() {
    if (!_lastSatisfiedState) {
      // Transitioned from false -> true
      Serial.printf("[WD:%s - Sat] F -> T\n", _name.c_str());
      drawDSConnect(true);
    }
    _lastSatisfiedState = true;
    _lastWatchdogFeedTime = millis();
  }

  bool Watchdog::satisfied() {

    if (_watchdogTimeout == 0) {
      if (!_lastSatisfiedState) {
        // Transitioned from false -> true
        Serial.printf("[WD:%s - Sat] F -> T\n", _name.c_str());
        drawDSConnect(true);
      }
      _lastSatisfiedState = true;
      return true;
    }

    if (millis() - _lastWatchdogFeedTime < _watchdogTimeout) {
      if (!_lastSatisfiedState) {
        // Transitioned from false -> true. Initially after 1 second
        Serial.printf("[WD:%s - Sat] F -> T\n", _name.c_str());
        drawDSConnect(true);
      }
      _lastSatisfiedState = true;
      return true;
    }

    if (_lastSatisfiedState) {
      Serial.printf("[WD:%s - Sat] T -> F\n", _name.c_str());
      drawDSConnect(false);
    }
    _lastSatisfiedState = false;
    return false;
  }

  void Watchdog::setTimeout(unsigned long timeout) {
    _watchdogTimeout = timeout;
  }
}