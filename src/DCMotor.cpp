#include "DCMotor.h"

// ------------------- Initialize static instances --------------

DCMotor * DCMotor::instances[2] = { NULL, NULL };

// Frequency at which motor power is monitored
const int speedCtrlPeriodMillis = 100;             // speed control period in milli seconds
const int speedCtrlPeriodMicros = speedCtrlPeriodMillis*1000; // speed control period in micro seconds
const int DRAM_ATTR periodsPerSec = 1000/speedCtrlPeriodMillis;    // number of speed control periods/sec

// -------------------Constructors -----------------------------------

DCMotor::DCMotor(const uint8_t pinGroup) 
  :encoder(pinGroup), pinGroup_(pinGroup), PWMChannel(pinGroup)
{   
  // Connect motor to GPIO pins
  // pinMode(motorPinGroup[pinGroup].motorDir1, OUTPUT); // motor direction
  // pinMode(motorPinGroup[pinGroup].motorDir2, OUTPUT); // motor direction
  // pinMode(motorPinGroup[pinGroup].enable, OUTPUT);

  // // Setup PWM signal
  // ledcSetup(pinGroup_, freq, resolution); // create a PWM channel 
  // ledcAttachPin(motorPinGroup[pinGroup_].enable, pinGroup_); // attach channel to pin

  // Start motor power timers 
  switch (pinGroup)
  {
  case 0: 
    {
      const esp_timer_create_args_t periodic_timer_args = {.callback = &motorISR0};
      esp_timer_create(&periodic_timer_args, &motorTimer0);
      esp_timer_start_periodic(motorTimer0, speedCtrlPeriodMicros); // Time in milliseconds (50)
      instances [0] = this; 
    }
    break;
    
  case 1: 
    {
      const esp_timer_create_args_t periodic_timer_args = {.callback = &motorISR1};
      esp_timer_create(&periodic_timer_args, &motorTimer1);
      esp_timer_start_periodic(motorTimer1, speedCtrlPeriodMicros); // Time in milliseconds (50)
      instances [1] = this;
    }
    break;
    
  } // end of switch

}

// ---------------- Public member methods -------------------------

// -------------------------------------------------------- 
// Sets the local pulseSetpoint which is picked up in the
// setPower_() periodic timer. This will give pulses between
// 70 and 700.  700 pulses per/sec is about 40 cm/sec
// --------------------------------------------------------
// void DCMotor::setSpeed(const EncoderPulsesPerSecond pulseSetpoint) {

//   // PulseSetpoint drives the action of the setPower_() routine
//   pulseSetpoint_ = pulseSetpoint;
//   encoder.wheelDirection = sgn(pulseSetpoint);

//   // Log the time that the last setpoint request was received
//   lastMessageTime_ = millis();
// }

// -------------------------------------------------------- 
// Set the left and right wheel speeds 
// Input is a speed ratio between -1.0 (backwards) and 1.0 (forward) 
// Output is a wheel speed in pulses per/sec
// --------------------------------------------------------
void DCMotor::setSpeed(const WheelSpeedProportion wheelSpeed) {

  // Log the wheel speed proportion to calculate feedforward
  wheelSpeedProportion_ = wheelSpeed;

  // Translate the speed ratio into pulses per/sec.
  // Where speed ratio of 1.0 equals 700 pulses per/sec
  pulseSetpoint_ = int(maxPulsesPerSecond * wheelSpeed);

  // Don't go over max pulses/sec
  if (abs(pulseSetpoint_) > maxPulsesPerSecond) {
    pulseSetpoint_ = maxPulsesPerSecond;
  }

  encoder.wheelDirection = sgn(pulseSetpoint_);

  // Log the time that the last setpoint request was received
  lastMessageTime_ = millis();
}  

// ----------------------------------------------------------------
// Set motor power.  This runs in a periodic timer.
// It compares the difference between the current pulses per/sec
// and the required pulses per/sec
// ---------------------------------------------------------------- 
void IRAM_ATTR DCMotor::setPower_() {

  portENTER_CRITICAL_ISR(&timerMux);

  // Stop the motor if you don't receive a message within 100 milliseconds
  if ( (millis() - lastMessageTime_) > timeOut_ ) {
    PWM_ = 0;
  }  
  else 
  {
    
    // Get the number of pulses since the last period
    const int32_t pulses = encoder.pulses;  
    const int32_t pulsesThisPeriod = abs(pulses - pulsesLast_);
       
    // Save the last pulses
    pulsesLast_ = pulses;
  
    // Compute the error between requested pulses/sec and actual pulses/sec
    pulsesPerSec_ = pulsesThisPeriod * periodsPerSec;
    error_ = abs(pulseSetpoint_) - pulsesPerSec_; 
    
    // PI control
    pPart_ = Kp * error_; // Proportional

    // Compute feed forward
    const float feedforward = (int)(kS + (kV * wheelSpeedProportion_));
  
    // Compute the PWM
    PWM_ = int(pPart_ + feedforward);
    if (PWM_ > 255) { PWM_ = 255; }
  
    // Motors are shutting down
    if (pulseSetpoint_ == 0) { PWM_ = 0;} 
    
  } // End else
 
  // log_d("PWM", PWM_);

  // Apply the power with the direction and PWM signal
  applyPower_(sgn(pulseSetpoint_), PWM_);
  
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

// ------------------------------------------------
// Apply power to motor 
// ------------------------------------------------
void IRAM_ATTR DCMotor::applyPower_(const int dir, const int PWM) {

  int level;
  if(dir >= 0) {
    level = HIGH; 
  } else {
    level = LOW; 
  }

  // digitalWrite(motorPinGroup[pinGroup_].motorDir2, level); // Direction HIGH forward, LOW backward
  // digitalWrite(motorPinGroup[pinGroup_].motorDir1, (!level)); // Write the opposite value
  
  // // See setupPWM(pinGroup) in Config.h
  // ledcWrite(pinGroup_, abs(PWM));

}

// Simple function to run the motor
void DCMotor::runMotor(const WheelSpeedProportion wheelSpeed) {
  const int direction = sgn(wheelSpeed);
  // Convert (-1.0, 1.0) to (0, 255)
  int val = ((wheelSpeed + 1.0) / 2.0) * 255;
  applyPower_(direction, val);
}