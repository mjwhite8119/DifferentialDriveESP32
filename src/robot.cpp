#include "robot.h"
#include <Arduino.h>
// #include "quadrature.pio.h"

namespace xrp {
  Robot::Robot() :
      _enabled(false)
      // _leftMotor(LEFT_MOTOR_EN, LEFT_MOTOR_PH),
      // _rightMotor(RIGHT_MOTOR_EN, RIGHT_MOTOR_PH)
      // _motor3(MOTOR_3_EN, MOTOR_3_PH),
      // _motor4(MOTOR_4_EN, MOTOR_4_PH),
      // _servo1(SERVO_1_PIN),
      // _servo2(SERVO_2_PIN) 
      {

    // _pwmChannels.insert(std::make_pair(0, &_leftMotor));
    // _pwmChannels.insert(std::make_pair(1, &_rightMotor));
    // _pwmChannels.insert(std::make_pair(2, &_motor3));
    // _pwmChannels.insert(std::make_pair(3, &_motor4));
    // _pwmChannels.insert(std::make_pair(4, &_servo1));
    // _pwmChannels.insert(std::make_pair(5, &_servo2));

    // TODO Prep I2C connection to gyro

    // Set up encoders
    // uint offset0 = pio_add_program(pio0, &quadrature_program);
    // quadrature_program_init(pio0, ENCODER_CH_MOTOR_L, offset0, 4, 5);
    // quadrature_program_init(pio0, ENCODER_CH_MOTOR_R, offset0, 12, 13);
    // quadrature_program_init(pio0, ENCODER_CH_MOTOR_3, offset0, 0, 1);
    // quadrature_program_init(pio0, ENCODER_CH_MOTOR_4, offset0, 8, 9);

    pinMode(LED_BUILTIN, OUTPUT);

    // Set up the user button pin as input
    // pinMode(DIO_BUILTIN_PHYS_USER_BUTTON, INPUT_PULLUP);

    // Pre-register built in DIOs
    // _dioChannels.insert(std::make_pair(0, DIOChannel{true, DIO_BUILTIN_PHYS_USER_BUTTON, false}));
    // _dioChannels.insert(std::make_pair(1, DIOChannel{false, DIO_BUILTIN_PHYS_LED, false}));
  }

  // Hardware Configurations
  void Robot::configureEncoder(int deviceId, int chA, int chB) {
    if (chA == ENCODER_L_CH_A && chB == ENCODER_L_CH_B) {
      // Left Encoder
      _encoderChannels[deviceId] = ENCODER_CH_MOTOR_L;
    }
    else if (chA == ENCODER_R_CH_A && chB == ENCODER_R_CH_B) {
      // Right Encoder
      _encoderChannels[deviceId] = ENCODER_CH_MOTOR_R;
    }
    // else if (chA == ENCODER_3_CH_A && chB == ENCODER_3_CH_B) {
    //   // Motor 3 Encoder
    //   _encoderChannels[deviceId] = ENCODER_CH_MOTOR_3;
    // }
    // else if (chA == ENCODER_4_CH_A && chB == ENCODER_4_CH_B) {
    //   // Motor 4 Encoder
    //   _encoderChannels[deviceId] = ENCODER_CH_MOTOR_4;
    // }
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
      setPwmValue(0, 0, true);
      setPwmValue(1, 0, true);
    }

    bool prevEnabledValue = this->_enabled;
    this->_enabled = enabled;
    
    if (prevEnabledValue && !enabled) {
      // Switching to disabled mode. Stop all motors
      Serial.println("Disabling Robot");
      for (auto pwmChannel : _pwmChannels) {
        setPwmValue(pwmChannel.first, 0, true);
      }
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

    if (_pwmChannels.count(channel) > 0) {
      auto pwmChannel = _pwmChannels[channel];
      pwmChannel->setValue(value);
    }
  }

  void Robot::setDioValue(int channel, bool value) {
    // TEMP
    if (channel == 1) {
      digitalWrite(LED_BUILTIN, value ? HIGH : LOW);
    }
  }

  std::vector<int> Robot::getActiveEncoderDeviceIds() {
    std::vector<int> ret;
    for (auto& it: _encoderChannels) {
      ret.push_back(it.first);
    }

    return ret;
  }

  int Robot::getEncoderValueByDeviceId(int deviceId) {
    if (_encoderChannels.count(deviceId) > 0) {
      return getEncoderValue(_encoderChannels[deviceId]);
    }
    return 0;
  }

  int Robot::getEncoderValue(int idx) {
    return _encoderValues[idx];
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
  // Output is a wheel speed in pulses per/sec
  // -----------------------------------------------------------------
  void Robot::setWheelSpeeds(const WheelSpeedProportion leftWheelSpeed, 
                            const WheelSpeedProportion rightWheelSpeed) {
    
    // leftWheel.motor.setSpeed(leftWheelSpeed); // Wheel speed proportion
    // rightWheel.motor.setSpeed(rightWheelSpeed); // Wheel speed proportion
  }

  // ------------------------------------------------------
  // Returns the drive train velocity
  // ------------------------------------------------------
  // MetersPerSecond Robot::getSpeed() const {
  //   MetersPerSecond left_wheel = leftWheel.getSpeed();
  //   MetersPerSecond right_wheel = rightWheel.getSpeed();
  //   MetersPerSecond speed = (left_wheel + right_wheel) / 2;
  //   return speed;
  // }

  // PWMChannel
  PWMChannel::PWMChannel(int pin) : _pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void PWMChannel::setValue(double value) {
    // Convert (-1.0, 1.0) to (0, 255)
    int val = ((value + 1.0) / 2.0) * 255;
    // Serial.println("PWMChannel setValue");
    analogWrite(_pin, val);
  }

  // Motor
  Motor::Motor(int enPin, int phPin) :
          PWMChannel(-1),
          _enPin(enPin),
          _phPin(phPin) {
    pinMode(enPin, OUTPUT);
    pinMode(phPin, OUTPUT);
  }

  void Motor::setValue(double value) {
    // Serial.println("Motor setValue");
    // PinStatus phValue = (value < 0.0) ? LOW : HIGH;
    int enValue = (abs(value) * 255);

    // digitalWrite(_phPin, phValue);
    analogWrite(_enPin, enValue);
  }
}
