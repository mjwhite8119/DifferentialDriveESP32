#pragma once

#ifndef Arduino
  #include <Arduino.h>
#endif
#include <Constants.h>

#define STOPPED 2
#define FORWARD 1
#define REVERSE 0

class PoluluEncoder
{  
   public:

    PoluluEncoder() {} // Default constructor  

    // Constructor to connect encoder GPIO pins to microcontroller
    PoluluEncoder(uint8_t pinGroup);

    // Ticks of the right and left encoder. This is volatile so 
    // that it doesn't interfere with the rest of the code.
    volatile int32_t ticks = 0;
    uint8_t direction = STOPPED;
    bool encoderFired = false;
    int32_t lastTicks = ticks;

    // Encoder interrupt routines
    static void encoderISRA ()
    {
      if (PoluluEncoder::instances [0] != NULL)
        PoluluEncoder::instances [0]->encoderAFired_();
    } 
    
    static void encoderISRB ()
    {
      if (PoluluEncoder::instances [1] != NULL)
        PoluluEncoder::instances [1]->encoderBFired_();
    }
      
    void init();

    void resetEncoder() {
      ticks = 0;
      printPort();Serial.println(" Encoder reset");
    }

    bool encoderUpdated();

    int32_t IRAM_ATTR getTicks();

    int count = 0;
    void printInfo() {
      if (count > 1000) {
        Serial.print("Ticks:");Serial.println(getTicks());   
        count = 0;
      }  
      count += 1;      
    }

    void printPort() {
      Serial.print("Port ");Serial.print(poluluPinGroup[pinGroup_].encoderA);
    }

    static PoluluEncoder * instances [2];

  private:

    uint8_t pinGroup_;
    // Mutex for protecting critical code in interrupts
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

    // Instance members to get encoder ticks. Called from ISR

    // Checks encoder A
    void IRAM_ATTR encoderAFired_();

    // // Checks encoder B
    void IRAM_ATTR encoderBFired_();
};    
