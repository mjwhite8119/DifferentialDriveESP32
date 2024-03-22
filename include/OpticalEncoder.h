#pragma once

#include "Arduino.h"
#include "Constants.h"

#define STOPPED 2
#define FORWARD 0
#define REVERSE 1

// Temporary variable to simulate encoder interrupt
const int interruptFreq = 10; // Every 10 milliseconds 
const int interruptsPerSecond = 1000 / interruptFreq;

class OpticalEncoder
{  
   public:

    // Default constructor
    OpticalEncoder() {}

    /**
     * Constructor. Connects encoder GPIO pin to microcontroller
     * 
     * @param pinGroup - GPIO pin group for this encoder.
     */
    explicit OpticalEncoder(uint8_t pinGroup);

    // Class variables
    static constexpr int PPR = 20;  // Encoder Count per Revolutions 

    // Mutex for protecting critical code in interrupts
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

    volatile int32_t ticks = 0;
    // int8_t wheelDirection = 0;
    uint8_t direction = STOPPED;
    int32_t lastTicks = ticks;

    /**
     * Reset ticks.
     */
    void resetEncoder() {ticks = 0;}

    bool updated();

    int32_t IRAM_ATTR getTicks();

    // void setWheelDirection(const int dir) { wheelDirection = dir; }

    static OpticalEncoder * instances [2];
   
  private:

    uint8_t pinGroup_;
 
    /**
     * Encoder interrupt routines. These fire from an external interupt
     * GPIO pin that connects to the encoder.
     */
    static void encoderISR0 ()
    {
      if (OpticalEncoder::instances [0] != NULL)
        OpticalEncoder::instances [0]->encoderFired_();
    } 
    
    static void encoderISR1 ()
    {
      if (OpticalEncoder::instances [1] != NULL)
        OpticalEncoder::instances [1]->encoderFired_();
    }

    /**
     * This routine runs each time an interrupt fires.  It increases
     * or decreases the number of ticks.
     */
    void IRAM_ATTR encoderFired_();
    
};