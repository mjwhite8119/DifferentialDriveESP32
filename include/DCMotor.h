#ifndef _DC_MOTOR_H_
#define _DC_MOTOR_H_

#ifndef Encoder
  #include "Encoder.h"
#endif
#include "Constants.h"
#include <RobotTypes.h>
    
class DCMotor
{
  public:
        
    DCMotor() {} // Default constructor

    // Constructor to connect motor GPIO pins to microcontroller
    explicit DCMotor(const uint8_t pinGroup);

    // PMW for motors
    const int freq = 30000;
    const int resolution = 8;

    // Encoder attached to the motor
    Encoder encoder;
    const bool usesEncoder = true;

    const int maxPulsesPerSecond = 700; 

    // Include feed forward class
    // const float kS = 155.0;
    const float kS = 200.0;
    const float kV = 50.0;

    // Mutex for protecting critical code in interrupts
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
    
    // Set the wheel speed   
    void setSpeed(const EncoderPulsesPerSecond pulseSetpoint);

    void setSpeed(const WheelSpeedProportion wheelSpeed);

    int getPWM() const {return PWM_;}

    int32_t getPulsesPerSecond() const {return pulsesPerSec_;}

    float getProportional() const {return pPart_;}

    float getFeedforward() const {return (kS + (kV * wheelSpeedProportion_));}

    int getPulseSetpoint() const {return pulseSetpoint_;}

  private:
    
    uint8_t pinGroup_; // motor GPIO pins 

    static DCMotor * instances [2];

    // Encoder interrupt routines
    static void motorISR0 (void *pArg)
    {
      if (DCMotor::instances [0] != NULL)
        DCMotor::instances [0]->setPower_();
    } 
    
    static void motorISR1 (void *pArg)
    {
      if (DCMotor::instances [1] != NULL)
        DCMotor::instances [1]->setPower_();
    }
    
    // Motor speed variables
    int32_t pulsesLast_ = 0; 
    int32_t pulsesPerSec_ = 0;
    int error_ = 0;      
    float pPart_ = 0.0, iPart_ = 0.0; // PI control
    int PWM_ = 0; // Current PWM
    int pulseSetpoint_ = 0; // Current pulse setpoint
    double lastMessageTime_;
    const int timeOut_ = 1000;
    WheelSpeedProportion wheelSpeedProportion_ = 0.0;
    
    // PI control. Adjust gain constants as necessary.  Place in RAM
    const float Kp = 0.17, Ki = 0.1; // gain constants

    // Set motor power
    void IRAM_ATTR setPower_();

    // Apply power to the motor
    void IRAM_ATTR applyPower_(const int dir, const int PWM);
};
#endif // _DC_MOTOR_H_