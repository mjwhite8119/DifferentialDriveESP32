#include "PWMChannel.h"
#include <Arduino.h>
#include "OLED.h"

namespace xrp {

    PWMChannel::PWMChannel(int pin) : _pin(pin) {
        pinMode(pin, OUTPUT);
    }

    void PWMChannel::setValue(double value) {
        // Convert (-1.0, 1.0) to (0, 255)
        int val = ((value + 1.0) / 2.0) * 255;
        clearLine(4 + _pin);
        drawText(4 + _pin, 0, String(value));
        // analogWrite(_pin, val);
    }

}