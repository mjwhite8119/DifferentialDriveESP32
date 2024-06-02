#pragma once
#include "Constants.h"
#include <unordered_map>
#include <vector>
#include <iostream>

// #include "OLED.h"
#include "LCD1602.h"
#include "watchdog.h"
// #include "PoluluMotor.h"
// #include "SimpleMotor.h"
#include "MotorL298N.h"

#define DEFAULT_WATCHDOG_TIMEOUT_MS 1000

namespace xrp {

  struct DIOChannel {
    bool isInput;
    int physicalPin;
    bool value;
  };

  class Robot {
    public:
      Robot();

      void init();
      
      void configureEncoder(int deviceId, int chA, int chB);
      void configureDIO(int deviceId, bool isInput);

      void setEnabled(bool enabled);

      void setPwmValue(int channel, double value);
      void setDioValue(int channel, bool value);

      void setWheelSpeeds(const float leftWheelSpeed, const float rightWheelSpeed);
      void setWheelSpeed(const int channel, const float value);

      void checkStatus();
      double applyDeadband(double input, double threshold);

      Watchdog watchdog{"robot"};

      // Motors 
      // PoluluMotor _leftMotor;
      // SimpleMotor _rightMotor;
      MotorL298N _leftMotor;
      MotorL298N _rightMotor;

    private:
      bool _enabled;

      void setPwmValue(int channel, double value, bool override);
      
      // Channel Maps
      // std::unordered_map<int, PWMChannel*> _pwmChannels;
      std::unordered_map<int, int> _encoderChannels; // Map from encoder device # to actual

      // Map from WPILib DIO -> Onboard DIO
      std::unordered_map<int, DIOChannel> _dioChannels;

      // Encoder Values
      int _encoderValues[4] = {0, 0, 0, 0};
  };


}
