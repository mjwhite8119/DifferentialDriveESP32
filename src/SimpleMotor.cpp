#include "SimpleMotor.h"
#include <Arduino.h>
#include "Constants.h"
#include <iostream>

namespace xrp {
    SimpleMotor::SimpleMotor(const uint8_t pinGroup) :
            pinGroup_(pinGroup)
    {
        pinMode(motorPinGroup[pinGroup_].motorIN1, OUTPUT); 
        pinMode(motorPinGroup[pinGroup_].motorIN2, OUTPUT); //  channel B

        // create a PWM channels
        ledcSetup(channel_0, freq, resolution); 
        ledcSetup(channel_1, freq, resolution); 

        // attach channels to pins
        ledcAttachPin(motorPinGroup[pinGroup_].motorIN1, channel_0); 
        ledcAttachPin(motorPinGroup[pinGroup_].motorIN2, channel_1);

        // Make sure motor is off
        ledcWrite(channel_0, 0);
        ledcWrite(channel_1, 0);   
    }

    void SimpleMotor::applyPower(WheelSpeedProportion speed) {

        // Convert from proportional speed to PWM
        Serial.print(" Speed ");Serial.println(speed);
        DBSpeed_ = (abs(speed) * 255);

        // Don't try and move unless we have at least 100 PWM
        DBSpeed_ = applyDeadband(DBSpeed_, 50);
        if (DBSpeed_ > MAX_DUTY_CYCLE) {
            DBSpeed_ = MAX_DUTY_CYCLE;
        }
        Serial.print(" DBSpeed ");Serial.println(DBSpeed_);
        if (DBSpeed_ == 0) {
            ledcWrite(channel_0, 0); // Write a LOW
            ledcWrite(channel_1, 0); // Write a LOW
        } 
        else if (speed > 0) {
            ledcWrite(channel_0, abs(DBSpeed_)); // PWM speed
            ledcWrite(channel_1, 0);  // Write a LOW
        }
        else {
            ledcWrite(channel_1, abs(DBSpeed_)); // PWM speed
            ledcWrite(channel_0, 0);  // Write a LOW
        }
    }

}