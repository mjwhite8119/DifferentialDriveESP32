#pragma once

#ifndef Arduino
  #include <Arduino.h>
#endif
#include <Constants.h>

#include "RobotTypes.h"
#include <SimpleFOC.h>

// Motor instance
BLDCDriver3PWM driver = BLDCDriver3PWM(32, 33, 25); // PWM pins for motor driver

// Magnetic Sensor instance
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C); // Use the AS5600 magnetic sensor on I2C

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(7); // Gimbal motor
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);

#define STOPPED 2
#define FORWARD 1
#define REVERSE 0

class GimbalMotor
{  
  public:

    // GimbalMotor() {} // Default constructor

    // Constructor to connect Motor GPIO pins to microcontroller
    GimbalMotor(uint8_t pinGroup);

    void init();
    void applyPower(DutyCycle speed);

  private:
    // Motor ports
    uint8_t pinGroup_;

    int count = 0;

    void printPort() {
      if (count > 1000) {
        Serial.print("Ports "); Serial.print(motorPinGroup[pinGroup_].motorIN1); Serial.print(","); Serial.print(motorPinGroup[pinGroup_].motorIN2);Serial.print(": ");
        count = 0;
      }  
      count += 1; 
    }

    void printSpeed(DutyCycle speed) {
      if (count > 1000) {
        Serial.print("Ports "); Serial.print(motorPinGroup[pinGroup_].motorIN1); Serial.print(", ");Serial.print("Speed "); Serial.println(speed);
        count = 0;
      }  
      count += 1; 
    }

};