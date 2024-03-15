#pragma once
#include "Constants.h"
#include <unordered_map>
#include <vector>
#include <iostream>

#include "watchdog.h"
// #include "PWMChannel.h"
#include "PoluluMotor.h"

#define DEFAULT_WATCHDOG_TIMEOUT_MS 1000

const float wheelDiameter = 0.057; // wheel diameter in meters
const uint8_t wheelType = STANDARD_FIXED;

const float wheelSeparation = 0.179; // wheel separation in meters

namespace xrp {
  
  class Motor {
    public:
      Motor(const uint8_t pinGroup);

      void setValue(double value);
      // PMW for motors
      static const int freq = 30000;
      static const int resolution = 8;
    protected:
      int pinGroup_;
  };

  struct DIOChannel {
    bool isInput;
    int physicalPin;
    bool value;
  };

  class Robot {
    public:
      Robot();

      void configureEncoder(int deviceId, int chA, int chB);
      void configureDIO(int deviceId, bool isInput);

      void setEnabled(bool enabled);

      void setPwmValue(int channel, double value);
      void setDioValue(int channel, bool value);

      void setWheelSpeeds(const int channel, const float value);

      // std::vector<int> getActiveEncoderDeviceIds();
      // int getEncoderValueByDeviceId(int deviceId);
      // int getEncoderValue(int idx);

      void checkStatus();

      Watchdog watchdog{"robot"};

      PoluluMotor _leftMotor;

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
