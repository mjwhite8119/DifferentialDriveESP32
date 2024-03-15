#include "PoluluMotor.h"
#include <iostream>

// -------------------Constructors -----------------------------------

// Constructor to connect Motor GPIO pins to microcontroller
PoluluMotor::PoluluMotor(uint8_t pinGroup)
  :encoder(pinGroup), pinGroup_(pinGroup)
  {  
    std::cout << "PoluluMotor Constructor" << pinGroup_ << std::endl;
    std::cout << "Motor IN1" << poluluPinGroup[pinGroup_].motorIN1 << std::endl;
    std::cout << "Motor IN2" << poluluPinGroup[pinGroup_].motorIN2 << std::endl;
    pinMode(poluluPinGroup[pinGroup_].motorIN1, OUTPUT); 
    pinMode(poluluPinGroup[pinGroup_].motorIN2, OUTPUT); //  channel B

    // create a PWM channels
    ledcSetup(channel_0, freq, resolution); 
    ledcSetup(channel_1, freq, resolution); 

    // attach channels to pins
    ledcAttachPin(poluluPinGroup[pinGroup_].motorIN1, channel_0); 
    ledcAttachPin(poluluPinGroup[pinGroup_].motorIN2, channel_1);

    // Make sure motor is off
    ledcWrite(channel_0, 0);
    ledcWrite(channel_1, 0);
  }  


void PoluluMotor::init() {
  std::cout << "Polulu Encoder Init A" << poluluPinGroup[pinGroup_].encoderA << std::endl;
  std::cout << "Polulu Encoder Init B" << poluluPinGroup[pinGroup_].encoderB << std::endl;
  encoder.init();

  encoder.resetEncoder();
  Serial.print("Encoder "); encoder.printPort(); Serial.println("");
}

void PoluluMotor::applyPower(WheelSpeedProportion speed) {

  // Convert from proportional speed to PWM
  Serial.print(" Speed ");Serial.println(speed);
  DBSpeed_ = (abs(speed) * 255);

  // Don't try and move unless we have at least 100 PWM
  DBSpeed_ = applyDeadband(DBSpeed_, 100);
  if (DBSpeed_ > MAX_DUTY_CYCLE) {
    DBSpeed_ = MAX_DUTY_CYCLE;
  }
  Serial.print(" DBSpeed ");Serial.println(DBSpeed_);
  if (DBSpeed_ == 0) {
    ledcWrite(channel_0, 0); // Write a LOW
    ledcWrite(channel_1, 0); // Write a LOW
    encoder.direction = STOPPED;
  } 
  else if (speed > 0) {
    ledcWrite(channel_0, abs(DBSpeed_)); // PWM speed
    ledcWrite(channel_1, 0);  // Write a LOW
    encoder.direction = FORWARD;

    printSpeed();
    encoder.printInfo();
    // Serial.print("Finger flexed ");encoder.printInfo();
  }
  else {
    ledcWrite(channel_1, abs(DBSpeed_)); // PWM speed
    ledcWrite(channel_0, 0);  // Write a LOW
    encoder.direction = REVERSE;

    // printPort(); 
    printSpeed();
    encoder.printInfo();
    // Serial.print("Finger extended "); encoder.printInfo();
  }
}