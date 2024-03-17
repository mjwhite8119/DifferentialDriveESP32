#pragma once
#ifndef Arduino
  #include <Arduino.h>
#endif

//---------------------------------------
// Define the GPIO pins for the motors
//---------------------------------------
const uint8_t leftWheelPinGroup = 0; // GPIO pin group config.h
const uint8_t rightWheelPinGroup = 1; // GPIO pin group config.h

static struct DRAM_ATTR MotorPins {
  const byte motorIN1; // motor IN1 pin1
  const byte motorIN2; // motor IN2 pin2
} motorPinGroup[2] = {2, 3,
                      4, 5 };

static struct DRAM_ATTR EncoderPins {
  const byte encoderA; 
  const byte encoderB; 
} encoderPinGroup[1] = {6, 7};

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