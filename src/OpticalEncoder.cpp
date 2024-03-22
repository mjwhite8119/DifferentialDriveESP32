#include "OpticalEncoder.h"

// ------------------- Initialize static instances --------------

OpticalEncoder * OpticalEncoder::instances [2] = { NULL, NULL };

// ---------------------  Constructors -------------------------------

OpticalEncoder::OpticalEncoder(uint8_t pinGroup) 
  :pinGroup_(pinGroup)
{   
  // Connect encoder to GPIO pins
  pinMode(motorPinGroupL298N[pinGroup].encoder, INPUT); //  Left encoder, channel A
  digitalWrite(motorPinGroupL298N[pinGroup].encoder, HIGH); // turn on pullup resistors

  // Attach interrupts
  switch (pinGroup)
  {
  case 0: 
    attachInterrupt (motorPinGroupL298N[0].encoder, encoderISR0, RISING);  // Left encoder
    instances [0] = this; 
    break;
    
  case 1: 
    attachInterrupt (motorPinGroupL298N[1].encoder, encoderISR1, RISING); // Right encoder
    instances [1] = this;
    break;
    
  } // end of switch

  // Switch on interrupts
  sei();
  
  // Initialize ticks. 
  ticks = 0;

}

// ----------------  Private member methods ------------------------------
void IRAM_ATTR OpticalEncoder::encoderFired_() {
  // ticks is 4 bytes so make sure that the write is not interupted
  portENTER_CRITICAL_ISR(&timerMux);
  
  if (direction == FORWARD) {
    ticks++;
  } else {
    ticks--;  
  } 
  portEXIT_CRITICAL_ISR(&timerMux);
}

bool OpticalEncoder::updated() {
  int32_t currentTicks = getTicks();
  if (currentTicks != lastTicks) {
    lastTicks = currentTicks;
    return true;
  } else {
    return false;  
  }
}

/** The optical encoder is only 20CPR, we multiple to
 *  simulate the Polulu encoder, which is 360CPR  
*/ 
int32_t IRAM_ATTR OpticalEncoder::getTicks() {
  portENTER_CRITICAL_ISR(&timerMux);
  int32_t tmp = ticks;
  portEXIT_CRITICAL_ISR(&timerMux);
  return tmp * 18;
}