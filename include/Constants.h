#pragma once
#ifndef Arduino
  #include <Arduino.h>
#endif

//---------------------------------------
// Define the GPIO pins for the motors
//---------------------------------------
static struct DRAM_ATTR MotorPins {
  const byte motorDir1; // motor direction pin1
  const byte motorDir2; // motor direction pin2
  const byte enable; // Enable PMW 
  const byte encoderA; // encoder channel A
  const byte encoderB; // encoder channel B
} motorPinGroup[2] = {27, 26, 25, 36, 37, 
                      14, 12, 13, 38, 39};

// Define wheel types 
enum WheelTypes {
  STANDARD_FIXED = 1,
  STANDARD_FIXED_CONNECTED,
  STANDARD_STEERABLE,
  STANDARD_STEERABLE_CONNECTED,
  CASTER,
  SWEDISH,
  SPRERIC
};

#define HELTEC_WIFI_KIT_32 true
// -------------------------------------------------------//
// OLED configuration                                     //
// -------------------------------------------------------//
#define USE_OLED true

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