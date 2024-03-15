#include "SimpleMotor.h"
#include <Arduino.h>
#include "Constants.h"
#include <iostream>

namespace xrp {
    SimpleMotor::SimpleMotor(const uint8_t pinGroup) :
            pinGroup_(pinGroup)
    {
        // // Connect motor to GPIO pins
        // pinMode(motorPinGroup[pinGroup].motorDir1, OUTPUT); // motor direction
        // pinMode(motorPinGroup[pinGroup].motorDir2, OUTPUT); // motor direction
        // pinMode(motorPinGroup[pinGroup].enable, OUTPUT);

        // // Setup PWM signal
        // int myVariable = 42;
        // std::cout << "The value of myVariable is: " << myVariable << std::endl;
        // std::cout << "Motor Constructor called. " << pinGroup_ << std::endl;
        // ledcSetup(pinGroup_, freq, resolution); // create a PWM channel 
        // ledcAttachPin(motorPinGroup[pinGroup_].enable, pinGroup_); // attach channel to pin       
    }

    void SimpleMotor::setValue(double value) {      
        // const int direction = sgn(value);
        // int PWM = (abs(value) * 255);
        // int level;
        // if(direction >= 0) {
        //     level = HIGH; 
        // } else {
        //     level = LOW; 
        // }
        // Serial.print(pinGroup_);Serial.print(" Motor setValue ");Serial.println(PWM);

        // digitalWrite(motorPinGroup[pinGroup_].motorDir2, level); // Direction HIGH forward, LOW backward
        // digitalWrite(motorPinGroup[pinGroup_].motorDir1, (!level)); // Write the opposite value
        
        // ledcWrite(motorPinGroup[pinGroup_].enable, abs(PWM));
    }

}