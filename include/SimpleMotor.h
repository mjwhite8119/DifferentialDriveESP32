#pragma once
#include "Arduino.h"
#include "RobotTypes.h"

namespace xrp {

    class SimpleMotor {
        public:
        // SimpleMotor() {} // Default constructor
        SimpleMotor(const uint8_t pinGroup);
        void applyPower(DutyCycle speed);

        // PMW for motors
        const int channel_0 = 0;
        const int channel_1 = 1;
        static const int freq = 30000;
        static const int resolution = 8;
        int DBSpeed_ = 0;
        const int MAX_DUTY_CYCLE = (int)(pow(2, resolution) - 1);

        double applyDeadband(double input, double threshold) {
        if (input < -threshold || input > threshold) {
            return input;
        }
            return 0.0;
        }

        protected:
        uint8_t pinGroup_; // motor GPIO pins 
    };

}