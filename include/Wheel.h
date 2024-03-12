#ifndef _WHEEL_H_
#define _WHEEL_H_

#ifndef Arduino
  #include <Arduino.h>
#endif

#include "DCMotor.h"
#include "RobotTypes.h"

class Wheel
{ 
  public:   

    // Default constructor
    Wheel() {}
    
    // Constructor for powered wheels   
    Wheel(const uint8_t pinGroup, const float wheelDiameter, const uint8_t wheelType);

    // Constructor for unpowered wheels   
    Wheel(const float wheelDiameter, const uint8_t wheelType);

    // -------------------- Member variables -----------------  
    
    // --- Configuration (physical) properties ---
    DCMotor motor;
    float diameter; // wheel diameter in meters
    uint8_t type; 
    float * origin;  

    // ---  Kinematic (motion) properties ---- 
    // static constexpr float wDiameter = 0.063; 
    static constexpr float wDiameter = 0.057;  
    static constexpr float pulsesPerCM = 17.3; // Number of pulses to move 1 cm    
    static constexpr float distancePerPulse = (PI * wDiameter) / Encoder::PPR;   // PI*D/PPR (0.00058)
    static constexpr float pulsesPerMeter = 1 / distancePerPulse; // Number of pulses to move 1 meter (1723) 

    // Current direction of the wheel
    int currentDirection = 0;
    
    // Get the current linear_x position of the wheel
    float currentPosition() const;

    float maxSpeed() const;

    MetersPerSecond getSpeed() const
      {return motor.getPulsesPerSecond() * distancePerPulse;}

    // ------------------- Convenience methods ------------------
          
    // Set wheel speed in meters/sec and power the motor
    void setSpeed(EncoderPulsesPerSecond pulseSetpoint);

    // Encoder pulses from attached motor
    int32_t encoderPulses() const;

    // Set the origin of the wheel relative to the robot base
    void setOrigin(float * origin); 
    
  private:

    float * wheelOrigin;
};
#endif // _WHEEL_H_
