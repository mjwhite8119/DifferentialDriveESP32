#pragma once
#include "Constants.h"
#include <unordered_map>
#include <vector>
#include <iostream>

#include "watchdog.h"
// #include "PWMChannel.h"
#include "PoluluMotor.h"

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
