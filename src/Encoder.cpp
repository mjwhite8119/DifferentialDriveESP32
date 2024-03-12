#include "Encoder.h"

// ------------------- Initialize static instances --------------

Encoder * Encoder::instances [2] = { NULL, NULL };
//const DRAM_ATTR int8_t Encoder::encoderStates[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

// ---------------------  Constructors -------------------------------

Encoder::Encoder(uint8_t pinGroup) 
  :pinGroup_(pinGroup)
{   
  // Connect encoder to GPIO pins
  
  pinMode(motorPinGroup[pinGroup].encoderA, INPUT); //  Left encoder, channel A
  digitalWrite(motorPinGroup[pinGroup].encoderA, HIGH); // turn on pullup resistors
  pinMode(motorPinGroup[pinGroup].encoderB, INPUT); //  Left encoder, channel B
  digitalWrite(motorPinGroup[pinGroup].encoderB, HIGH); // turn on pullup resistors

  // Attach interrupts
  switch (pinGroup)
  {
  case 0: 
    attachInterrupt (motorPinGroup[0].encoderB, encoderISR0, RISING);  // Left encoder
    instances [0] = this; 
    break;
    
  case 1: 
    Serial.print("Encoder ");Serial.println(motorPinGroup[1].encoderA);
    attachInterrupt (motorPinGroup[1].encoderA, encoderISR1, RISING); // Right encoder
    instances [1] = this;
    break;
    
  } // end of switch

  // Switch on interrupts
  sei();
  
  // Initialize pulses. 
  pulses = 0;

}

// ----------------  Private member methods ------------------------------
void IRAM_ATTR Encoder::encoderFired_() {
  // pulses is 4 bytes so make sure that the write is not interupted
  portENTER_CRITICAL_ISR(&timerMux);
  
  if (wheelDirection > 0) {
    pulses++;
  } else {
    pulses--;  
  } 
  portEXIT_CRITICAL_ISR(&timerMux);
}

// -------------------------------------------------------
// Get encoder pulses from left wheel. Called from ISR
// -------------------------------------------------------
void IRAM_ATTR Encoder::encoderLeftFired_()
{      
  // pulses is 4 bytes so make sure that the write is not interupted
  portENTER_CRITICAL_ISR(&timerMux);

  // uint8_t b_level = gpio_get_level(GPIO_NUM_37);
  // uint8_t a_level = gpio_get_level(GPIO_NUM_36);
  // uint8_t b_level_right = gpio_get_level(GPIO_NUM_39);
  // uint8_t a_level_right = gpio_get_level(GPIO_NUM_38);
//  pin_val = gpio_input_get(); 
//  a_val = pin_val & 1<< channelA;
  
//  if( digitalRead(motorPinGroup[0].encoderA) == 0 ) {
  if( gpio_get_level(GPIO_NUM_36) == 0 ) {  
    // B is low
    pulses--; // moving backward
  } else {
    // B is high
    pulses++; // moving forward
  }

//  enc_val <<= enc_val << 2;
//
//  enc_val |= ( ( (curval & 1<< channelA ) >> channelA  | (curval & 1<< channelB ) >> (channelB - 1) ) & 0x03 );  
//  testPulses = testPulses + Encoder::encoderStates[enc_val & 0x0f]; 
  
  portEXIT_CRITICAL_ISR(&timerMux);
}

// -------------------------------------------------------
// Get encoder pulses from right wheel. Called from ISR
// -------------------------------------------------------
void IRAM_ATTR Encoder::encoderRightFired_()
{     
  // pulses is 4 bytes so make sure that the write is not interupted
//  portENTER_CRITICAL_ISR(&timerMux);

//  uint8_t channelA = motorPinGroup[1].encoderA;
//  uint8_t channelB = motorPinGroup[1].encoderB; 
//  uint32_t curval = gpio_input_get(); 
   
//  if( digitalRead(motorPinGroup[1].encoderB) == 0 ) {
  if( gpio_get_level(GPIO_NUM_39) == 0 ) {  
    // B is low
    portENTER_CRITICAL_ISR(&timerMux);
    pulses--; // moving backward
  } else {
    // B is high
    portENTER_CRITICAL_ISR(&timerMux);
    pulses++; // moving forward
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}