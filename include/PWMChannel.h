#pragma once
#include "RobotTypes.h"

namespace xrp {
    class PWMChannel {
        public:
        PWMChannel() : _pin(-1) {}
        PWMChannel(int pin);
        virtual void setValue(double value);

        protected:
        int _pin;
    };
}