#pragma once
#ifndef Arduino
  #include <Arduino.h>
#endif

//---------------------------------------
// Define the GPIO pins for the motors
//---------------------------------------
const uint8_t leftWheelPinGroup = 0; // GPIO pin group config.h
const uint8_t rightWheelPinGroup = 1; // GPIO pin group config.h

// Pins for the DRV3388 motor driver
static struct DRAM_ATTR MotorPins {
  const byte motorIN1; // motor IN1 pin1
  const byte motorIN2; // motor IN2 pin2
} motorPinGroup[2] = {2, 3,
                      4, 5 };

// Pins for the Polulu motor encoders
static struct DRAM_ATTR EncoderPins {
  const byte encoderA; 
  const byte encoderB; 
} encoderPinGroup[1] = {8, 7};

// Pins for the L298N motor driver with optical encoders
static struct L298NMotorPins {
  const byte motorDir1; // motor direction pin1
  const byte motorDir2; // motor direction pin2
  const byte enable; // Enable PMW 
  const byte encoder; // Wheel encoder
} motorPinGroupL298N[2] = {18, 17, 16, 11,
                          15, 7, 5, 10};

// #define HELTEC_WIFI_KIT_32 false
// -------------------------------------------------------//
// OLED configuration                                     //
// -------------------------------------------------------//
#define USE_OLED true

//----------------------------------
// Include timer interrupt code
//----------------------------------
//  #include <esp_timer.h>
inline esp_timer_handle_t stateUpdateTimer;
inline esp_timer_handle_t motorTimer0, motorTimer1;

template<typename T>
static constexpr int sgn(const T val) {   
 if (val < 0) return -1;
 if (val == 0) return 0;
 return 1;
}