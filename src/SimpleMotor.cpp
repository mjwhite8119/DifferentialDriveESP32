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

    void SimpleMotor::applyPower(DutyCycle speed) {

        // Convert from dutyCycle speed to PWM
        // Don't try and move unless we have at least 50 PWM
        const int PWM = map(abs(speed), 0, 1, 50, MAX_DUTY_CYCLE);
        Serial.print(" Speed=");Serial.print(speed);Serial.print(" PWM=");Serial.println(PWM);

        if (speed == 0) {
            ledcWrite(channel_0, 0); // Write a LOW
            ledcWrite(channel_1, 0); // Write a LOW
        } 
        else if (speed > 0) {
            ledcWrite(channel_0, abs(PWM)); // PWM speed
            ledcWrite(channel_1, 0);  // Write a LOW
        }
        else {
            ledcWrite(channel_1, abs(PWM)); // PWM speed
            ledcWrite(channel_0, 0);  // Write a LOW
        }
    }

}