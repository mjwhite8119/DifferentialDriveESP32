#ifndef _ENCODER_H_
#define _ENCODER_H_
#include <freertos/portmacro.h>
#include <FunctionalInterrupt.h>
#include <esp32-hal-gpio.h>
#include "Constants.h"

// Temporary variable to simulate encoder interrupt
const int interruptFreq = 10; // Every 10 milliseconds 
const int interruptsPerSecond = 1000 / interruptFreq;

// Mutex for protecting critical code in interrupts
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

class Encoder
{  
   public:

    // Class variables
    static constexpr int PPR = 341;  // Encoder Count per Revolutions 

    // Mutex for protecting critical code in interrupts
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
    
    // Default constructor
    Encoder() {}

    // Constructor to connect encoder GPIO pins to microcontroller
    explicit Encoder(uint8_t pinGroup);

    volatile int32_t pulses;
    int8_t wheelDirection = 0;

    void resetEncoder() {pulses = 0;}

    static Encoder * instances [2];
   
  private:

    uint8_t pinGroup_;

    // Encoder interrupt routines
    static void encoderISR0 ()
    {
      if (Encoder::instances [0] != NULL)
        Encoder::instances [0]->encoderFired_();
    } 
    
    static void encoderISR1 ()
    {
      if (Encoder::instances [1] != NULL)
        Encoder::instances [1]->encoderFired_();
    }
      
    // Instance members to get encoder pulses. Called from ISR

    // Checks encoder A
    void IRAM_ATTR encoderFired_();
    
    // Checks encoder A
    void IRAM_ATTR encoderLeftFired_();

    // Checks encoder B
    void IRAM_ATTR encoderRightFired_();
    
};
#endif // _ENCODER_H_
