#pragma once

#ifndef Arduino
  #include <Arduino.h>
#endif
#include <Constants.h>
#include "PoluluEncoder.h"
#include "RobotTypes.h"

#define STOPPED 2
#define FORWARD 1
#define REVERSE 0

class PoluluMotor
{  
  public:

    // PoluluMotor() {} // Default constructor

    // Constructor to connect Motor GPIO pins to microcontroller
    PoluluMotor(uint8_t pinGroup);

    // Encoder attached to the motor
    PoluluEncoder encoder;

    // PMW for motors
    const int channel_0 = 0;
    const int channel_1 = 1;
    const int freq = 30000;
    const int resolution = 8;
    const int MAX_DUTY_CYCLE = (int)(pow(2, resolution) - 1);

    void init();
    void applyPower(WheelSpeedProportion speed);

  private:
    // Motor ports
    uint8_t pinGroup_;

     int DBSpeed_ = 0;
     int count = 0;

    double applyDeadband(double input, double threshold) {
      if (input < -threshold || input > threshold) {
        return input;
      }
      return 0.0;
    }

    void printPort() {
      if (count > 1000) {
        Serial.print("Ports "); Serial.print(motorPinGroup[pinGroup_].motorIN1); Serial.print(","); Serial.print(motorPinGroup[pinGroup_].motorIN2);Serial.print(": ");
        count = 0;
      }  
      count += 1; 
    }

    void printSpeed() {
      if (count > 1000) {
        Serial.print("Ports "); Serial.print(poluluPinGroup[pinGroup_].motorIN1); Serial.print(", ");Serial.print("Speed "); Serial.println(DBSpeed_);
        count = 0;
      }  
      count += 1; 
    }

};