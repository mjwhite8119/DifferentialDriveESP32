#include "Arduino.h"
#include "Wheel.h"

// ------------------ Constructors --------------------------
// This is for a powered wheel so we need a motor and a set of GPIO pins to connect 
// it to the microcontroller. 
Wheel::Wheel(const uint8_t pinGroup, const float wheelDiameter, const uint8_t wheelType) 
  :motor(pinGroup), diameter(wheelDiameter), type(wheelType)
{ 
  
}

// This is for an unpowered wheel so no motor gets attached. 
Wheel::Wheel(const float wheelDiameter, const uint8_t wheelType) 
  :diameter(wheelDiameter), type(wheelType)
{   
  
}

// ---------------- Public member methods -------------------------

// ------------------------------------------------------- 
// Set the origin of the wheel relative to the robot base
// -------------------------------------------------------
void Wheel::setOrigin(float * origin) {
  wheelOrigin = origin;
}

// ---------------------------------------------------
// Get wheel position in meters since encoder reset
// ---------------------------------------------------
float Wheel::currentPosition() const {
  return motor.encoder.pulses * distancePerPulse;
}

// ---------------------------------------------------
// Get the max speed in meters/sec
// ---------------------------------------------------
float Wheel::maxSpeed() const {
  return motor.maxPulsesPerSecond / pulsesPerMeter;
}

// ---------------------------------------------------
// Get current wheel pulses 
// ---------------------------------------------------
int32_t Wheel::encoderPulses() const {
  return motor.encoder.pulses;
}
