#pragma once
#include "Arduino.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "RobotTypes.h"

namespace xrp {

    class MPU6050 {
        public:
        MPU6050();

        Adafruit_MPU6050 mpu;
        float rates[3];
        float angles[3];
        void setup();
        void update();
        float* getRates();
        float* getAngles();
    };
}        