#include "Robot.h"

namespace xrp {

  Robot::Robot() :
      _enabled(false),
      _leftMotor(0), // Pin group of Motor
      _rightMotor(1) // Pin group of Motor
      {
      std::cout << "Robot Constructor called." << std::endl;
      _leftMotor.init();
      _rightMotor.init();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
  }

  void Robot::configureDIO(int deviceId, bool isInput) {
    // We should ignore all DIO channels 11 and below (since these are spoken for in WPILib)
    if (deviceId <= 11) {
      return;
    }
    // TODO We need a list of available IO pins and assignments
  }

  void Robot::setEnabled(bool enabled) {
    // TEMP: This prevents the motors from starting off with some arbitrary speed
    if (!this->_enabled && enabled) {
      setWheelSpeeds(0, 0);
    }

    bool prevEnabledValue = this->_enabled;
    this->_enabled = enabled;
    
    if (prevEnabledValue && !enabled) {
      // Switching to disabled mode. Stop all motors
      Serial.println("Disabling Robot");
      drawDSState(false);
      setWheelSpeeds(0, 0);
    }
    else if (!prevEnabledValue && enabled) {
      Serial.println("Enabling Robot");
    }

  }

  void Robot::setPwmValue(int channel, double value) {   
    setPwmValue(channel, value, false);
  }

  void Robot::setPwmValue(int channel, double value, bool override) {
    if (!this->_enabled && !override) {
      return;
    }

    // don't set PWM values if the watchdog is not fed
    if (!watchdog.satisfied() && !override) {
      return;
    }

    setWheelSpeed(channel, value);
  }

  void Robot::setDioValue(int channel, bool value) {
    // TEMP
    if (channel == 1) {
      digitalWrite(LED_BUILTIN, value ? HIGH : LOW);
    }
  }

  void Robot::checkStatus() {
    if (!watchdog.satisfied()) {
      // Disable the robot
      setEnabled(false);
    }
  }

  // -----------------------------------------------------------------
  // Set the left and right wheel speeds 
  // Input is a speed ratio between -1.0 (backwards) and 1.0 (forward) 
  // -----------------------------------------------------------------
  void Robot::setWheelSpeeds(const float leftWheelSpeed, 
                            const float rightWheelSpeed) {
    _leftMotor.applyPower(leftWheelSpeed);
    _rightMotor.applyPower(rightWheelSpeed);
  }

  // -----------------------------------------------------------------
  // Set the wheel speed of the specifed channel 
  // Input is a speed ratio between -1.0 (backwards) and 1.0 (forward) 
  // -----------------------------------------------------------------
  void Robot::setWheelSpeed(const int channel, const float value) { 
    // clearLine(4 + channel);
    drawText(8 + channel * 5, 1, String(int(value*100)));
    //Serial.print("Channel ");Serial.print(channel);Serial.print(" Speed ");Serial.println(value);
    const DutyCycle speed = applyDeadband(value, 0.05);
    if (channel == 0) {
      drawText(8, 1, "    ");
      drawText(8, 1, String(int(value*100)));     
      _leftMotor.applyPower(speed); // Wheel speed proportion 
    } else if (channel == 1) {
      drawText(13, 1, "    ");
      drawText(13, 1, String(int(value*100)));
      _rightMotor.applyPower(speed); // Wheel speed proportion
    }
  }

  double Robot::applyDeadband(double input, double threshold) {
    if (input < -threshold || input > threshold) {
      return input;
    }
    return 0.0;
  }

}
