#pragma once
#ifndef Arduino
  #include <Arduino.h>
#endif

//---------------------------------------
// Define the GPIO pins for the motors
//---------------------------------------
const uint8_t leftWheelPinGroup = 0; // GPIO pin group config.h
const uint8_t rightWheelPinGroup = 1; // GPIO pin group config.h

// static struct DRAM_ATTR MotorPins {
//   const byte motorDir1; // motor direction pin1
//   const byte motorDir2; // motor direction pin2
//   const byte enable; // Enable PMW 
//   const byte encoderA; // encoder channel A
//   const byte encoderB; // encoder channel B
// } motorPinGroup[2] = {27, 26, 25, 36, 37, 
//                       14, 12, 13, 38, 39};

// --- Define ENCODERS ---
// These refer to the GPIO numbers
#define ENCODER1_A 6 
#define ENCODER1_B 7 

// #define ENCODER2_A 38 
// #define ENCODER2_B 39 

// #define ENCODER3_A 32
// #define ENCODER3_B 33

// #define ENCODER4_A 25
// #define ENCODER4_B 26

// --- Define MOTORS ---
// These refer to the GPIO numbers
#define MOTOR1_IN1 2 // Left Motor
#define MOTOR1_IN2 3 // Left Motor

#define MOTOR2_IN1 4 // Right Motor
#define MOTOR2_IN2 5 // Right Motot

// #define MOTOR3_IN1 19 // MIDDLE_IN1
// #define MOTOR3_IN2 18 // MIDDLE_IN2 

// #define MOTOR4_IN1 5 // INDEX_IN3
// #define MOTOR4_IN2 17 // INDEX_IN4

// #define MOTOR5_IN1 16
// #define MOTOR5_IN2 4

// #define MOTOR6_IN1 6
// #define MOTOR6_IN2 7 

//---------------------------------------
// Define the GPIO pins for the motors
//---------------------------------------
static struct DRAM_ATTR PoluluMotorPins {
  const byte motorIN1; // motor IN1 pin1
  const byte motorIN2; // motor IN2 pin2
  const byte encoderA; // encoder channel A
  const byte encoderB; // encoder channel B
} poluluPinGroup[1] = {MOTOR1_IN1, MOTOR1_IN2, ENCODER1_A, ENCODER1_B
                      // MOTOR2_IN1, MOTOR2_IN2, ENCODER2_A, ENCODER2_B
                      // MOTOR3_IN1, MOTOR3_IN2, ENCODER3_A, ENCODER3_B,
                      // MOTOR4_IN1, MOTOR4_IN2, ENCODER4_A, ENCODER4_B
                      };

static struct DRAM_ATTR MotorPins {
  const byte motorIN1; // motor IN1 pin1
  const byte motorIN2; // motor IN2 pin2
} motorPinGroup[1] = {MOTOR2_IN1, MOTOR2_IN2
                      // MOTOR3_IN1, MOTOR3_IN2, ENCODER3_A, ENCODER3_B,
                      // MOTOR4_IN1, MOTOR4_IN2, ENCODER4_A, ENCODER4_B
                      };

#define HELTEC_WIFI_KIT_32 true
// -------------------------------------------------------//
// OLED configuration                                     //
// -------------------------------------------------------//
#define USE_OLED false

#if USE_OLED
  // Library used for OLED
  #if (HELTEC_WIFI_KIT_32 || TTGO_LORA32_OLED)
    #define HELTEC true
    #define USE_U8G2 false
  #else  
    #define USE_U8G2 true
    #define HELTEC false
  #endif   
#endif

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