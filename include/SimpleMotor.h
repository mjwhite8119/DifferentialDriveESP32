#pragma once
#include "PWMChannel.h"

namespace xrp {

    class SimpleMotor {
        public:
        // SimpleMotor() {} // Default constructor
        SimpleMotor(const uint8_t pinGroup);
        void setValue(double value);

        // PMW for motors
        static const int freq = 30000;
        static const int resolution = 8;

        protected:
        uint8_t pinGroup_; // motor GPIO pins 
    };

}