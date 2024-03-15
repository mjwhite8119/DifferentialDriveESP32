#include "PoluluEncoder.h"
#include <iostream>

PoluluEncoder * PoluluEncoder::instances [2] = { NULL, NULL };

// Constructor to connect encoder GPIO pins to microcontroller
PoluluEncoder::PoluluEncoder(uint8_t pinGroup)
  :pinGroup_(pinGroup)
{
  // Switch on interrupts
  sei();

  // Initialize pulses. 
  ticks = 0;
  std::cout << "PoluluEncoder Constructor" << pinGroup_ << std::endl;
}  

/*
  Interrupts cannot be attached in the constructor since the interrupt handler 
  may not have been started. With one pin attached you get 6 ticks per/revolution.
  This will be doubled to 12 if both pins are attached.
*/
void PoluluEncoder::init() {
  pinMode(poluluPinGroup[pinGroup_].encoderA, INPUT); //  channel A
  pinMode(poluluPinGroup[pinGroup_].encoderB, INPUT); //  channel B
  
  attachInterrupt (poluluPinGroup[pinGroup_].encoderA, encoderISRA, CHANGE);  // Left encoder
  // attachInterrupt (6, encoderISRA, CHANGE);  // Left encoder
  instances [0] = this; 

  attachInterrupt (poluluPinGroup[pinGroup_].encoderB, encoderISRB, CHANGE); // Right encoder
  // attachInterrupt (7, encoderISRB, CHANGE); // Right encoder
  instances [1] = this;
}

void IRAM_ATTR PoluluEncoder::encoderAFired_() {
  // ticks is 4 bytes so make sure that the write is not interupted
  portENTER_CRITICAL_ISR(&timerMux);
  if (direction == FORWARD) {
    ticks++;
  } else {
    ticks--;
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR PoluluEncoder::encoderBFired_() {
  // ticks is 4 bytes so make sure that the write is not interupted
  portENTER_CRITICAL_ISR(&timerMux);
  if (direction == FORWARD) {
    ticks++;
  } else {
    ticks--;
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

bool PoluluEncoder::updated() {
  int32_t currentTicks = getTicks();
  if (currentTicks != lastTicks) {
    lastTicks = currentTicks;
    return true;
  } else {
    return false;  
  }
}

int32_t IRAM_ATTR PoluluEncoder::getTicks() {
  portENTER_CRITICAL_ISR(&timerMux);
  int32_t tmp = ticks;
  portEXIT_CRITICAL_ISR(&timerMux);
  return tmp;
}

