#include <Arduino.h>

// #include "i2cScan.h"
#include "LCD1602.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>

#include <vector>

#include "OTA32.h"
#include "Robot.h"
#include "MPU6050.h"

#include "wpilibws_processor.h"
#include "watchdog.h"

#include "RobotWebServer.h"
#include "RobotWebSocket.h"
// #include "RobotAsyncWebSocket.h"

xrp::Robot robot;

// Gyro
xrp::MPU6050 imu;

xrp::Watchdog dsWatchdog{"ds"};

// Chip Identifier
char chipID[20];

// ===================================================
// Handlers for INBOUND WS Messages
// ===================================================
void onDSGenericMessage() {
  // We use the DS messages to feed the watchdog
  // Serial.print("Feed robot watchdog");
  robot.watchdog.feed();
  // Serial.println("Feed ds watchdog");
  // dsWatchdog.feed();
}

void onDSEnabledMessage(bool enabled) {
  Serial.print("DS Enabled: ");
  Serial.println(enabled);
  drawDSState(enabled);     
  robot.setEnabled(enabled);
}

void onPWMMessage(int channel, double value) {
  robot.setPwmValue(channel, value);
}

void onEncoderInitMessage(int channel, bool init, int chA, int chB) {
  if (init) {
    robot.configureEncoder(channel, chA, chB);
  }
}

void onDIOMessage(int channel, bool value) {
  robot.setDioValue(channel, value);
}

// void onGyroInitMessage(std::string gyroName, bool enabled) {
//   imu.setEnabled(enabled);
// }

void hookupWSMessageHandlers() {
  // Hook up the event listeners to the message processor
  wsMsgProcessor.onDSGenericMessage(onDSGenericMessage);
  wsMsgProcessor.onDSEnabledMessage(onDSEnabledMessage);
  wsMsgProcessor.onPWMMessage(onPWMMessage);
  // wsMsgProcessor.onEncoderInitMessage(onEncoderInitMessage);
  wsMsgProcessor.onDIOMessage(onDIOMessage);
  // wsMsgProcessor.onGyroInitMessage(onGyroInitMessage);
}

// ===================================================
// Setup CPU (CORE1) Tasks
// This core broadcasts encoder and gyro updates
// ===================================================
unsigned long lastStatusMessageTime = 0;

// Task to run on core 1
void core1Task(void *pvParameters) {
  while (true) {
    // Send the WS messages. Can only send messages on intervals
    if (millis() - lastStatusMessageTime > 50) { 

      // Encoder messages  
      if (robot._leftMotor.encoder.updated()) {
        auto leftjsonMsg = wsMsgProcessor.makeEncoderMessage(0, robot._leftMotor.encoder.getTicks());
        broadcast(leftjsonMsg);
      }
      if (robot._rightMotor.encoder.updated()) {  
        auto rightjsonMsg = wsMsgProcessor.makeEncoderMessage(1, robot._rightMotor.encoder.getTicks() * -1);
        broadcast(rightjsonMsg);
      }

      // Gyro messages
      imu.update();
      auto gyroJsonMsg = wsMsgProcessor.makeGyroMessage(imu.getRates(), imu.getAngles());
      broadcast(gyroJsonMsg);

      lastStatusMessageTime = millis();
    } 
    
    // Yield to allow other tasks to run
    delay(10);
  }
}

// ===================================================
// Boot-Up and Main Control Flow
// ===================================================
void setup() {
  
  Serial.begin(115200);
  digitalWrite(BUILTIN_LED, LOW);

  imu.setup();

  // Setup the OLED.
  #if USE_OLED
    // setupOLED();
    setupLCD();
  #endif

  if (WiFi.status() != WL_CONNECTED) { // WiFi hasn't started
    connectWiFi(1); // STA mode 0, AP mode 1   
    // setupOTA(); // OTA WiFi connection.  
    delay(1000); 
  }

  // Set up WebSocket messages
  hookupWSMessageHandlers();

  // Set up the webServer on robot
  setupWebServer();

  robot.init();

  // Set up the webSocket on robot
  setupWebSocket();

  // Diagnostic test for I2C connectivity
  // i2cScan();

  // Create task for core 1
  xTaskCreatePinnedToCore(
    core1Task,     // Task function
    "Core1Task",   // Name of task
    10000,         // Stack size of task
    NULL,          // Parameter of the task
    1,             // Priority of the task
    NULL,          // Task handle
    1);            // Core where the task should run

}

// ===================================================
// Main (CORE0) Loop
// This core should process incoming WS messages 
// ===================================================
void loop() {
  // Robot Status
  robot.checkStatus();

  // Process websocket events
  loopWebSocket();
}
