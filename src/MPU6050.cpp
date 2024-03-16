#include "MPU6050.h"

namespace xrp {

    MPU6050::MPU6050() {
        
    }

    void MPU6050::setup() {
        // Try to initialize!
        if (!mpu.begin()) {
            Serial.println("Failed to find MPU6050 chip");
            while (1) {
            delay(10);
            }
        }
        Serial.println("MPU6050 Found!");

        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        Serial.print("Gyro range set to: ");
        switch (mpu.getGyroRange()) {
        case MPU6050_RANGE_250_DEG:
            Serial.println("+- 250 deg/s");
            break;
        case MPU6050_RANGE_500_DEG:
            Serial.println("+- 500 deg/s");
            break;
        case MPU6050_RANGE_1000_DEG:
            Serial.println("+- 1000 deg/s");
            break;
        case MPU6050_RANGE_2000_DEG:
            Serial.println("+- 2000 deg/s");
            break;
        }

        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        Serial.print("Filter bandwidth set to: ");
        switch (mpu.getFilterBandwidth()) {
        case MPU6050_BAND_260_HZ:
            Serial.println("260 Hz");
            break;
        case MPU6050_BAND_184_HZ:
            Serial.println("184 Hz");
            break;
        case MPU6050_BAND_94_HZ:
            Serial.println("94 Hz");
            break;
        case MPU6050_BAND_44_HZ:
            Serial.println("44 Hz");
            break;
        case MPU6050_BAND_21_HZ:
            Serial.println("21 Hz");
            break;
        case MPU6050_BAND_10_HZ:
            Serial.println("10 Hz");
            break;
        case MPU6050_BAND_5_HZ:
            Serial.println("5 Hz");
            break;
        }
    }

    void MPU6050::update() {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        rates[0] = g.gyro.x; rates[1] = g.gyro.y; rates[2] = g.gyro.z; 
        angles[0] = g.gyro.roll; angles[1] = g.gyro.pitch; angles[2] = g.gyro.heading;
    }

    float* MPU6050::getRates() {return rates;}
    float* MPU6050::getAngles() {return angles;}

}