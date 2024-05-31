#include <Arduino.h>

// #include "i2cScan.h"
#include "LCD1602.h"
#include <WiFi.h>
#include <WiFiMulti.h>

#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#include "RobotWebServer.h"

#include <vector>

#include "OTA32.h"
#include "Robot.h"
#include "MPU6050.h"

#include "wpilibws_processor.h"
#include "watchdog.h"


// Resource strings
extern "C" {
const unsigned char* GetResource_index_html(size_t* len);
const unsigned char* GetResource_normalize_css(size_t* len);
const unsigned char* GetResource_skeleton_css(size_t* len);
const unsigned char* GetResource_xrp_js(size_t* len);
}

// XRPConfiguration config;

wpilibws::WPILibWSProcessor wsMsgProcessor;

xrp::Robot robot;

// Gyro
xrp::MPU6050 imu;

xrp::Watchdog dsWatchdog{"ds"};

// Status Vars
// NetworkMode netConfigResult;

// Chip Identifier
char chipID[20];

WebSocketsServer webSocket = WebSocketsServer(3300, "/wpilibws");

// ===================================================
// Handlers for INBOUND WS Messages
// ===================================================
void onDSGenericMessage() {
  // We use the DS messages to feed the watchdog
  robot.watchdog.feed();
  dsWatchdog.feed();
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
// WebSocket management functions
// ===================================================

void broadcast(std::string msg) {
  webSocket.broadcastTXT(msg.c_str());
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  // Serial.println("got request");
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT: {
        JsonDocument jsonDoc;
        DeserializationError error = deserializeJson(jsonDoc, payload);
        if (error) {
          Serial.println(error.f_str());
          break;
        }
        wsMsgProcessor.processMessage(jsonDoc);
      }
      break;
  }
}

// ===================================================
// Setup CPU Core Tasks
// ===================================================

// Function to run on core 0
void Task0(void *pvParameters) {
  while (true) {
    Serial.println("Hello from core 0");
    delay(1000); // Delay for 1 second
  }
}

// Function to run on core 1
void Task1(void *pvParameters) {
  while (true) {
    Serial.println("Hello from core 1");
    delay(2000); // Delay for 2 seconds
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

  webSocket.onEvent(onWebSocketEvent);
  webSocket.begin();

  Serial.println("WebSocket server started on /wpilibws on port 3300");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
  
  // Set up the webserver on robot
  setupRobotWebServer();

  Serial.println("HTTP Server started on port 3300");

  // Diagnostic test for I2C connectivity
  // i2cScan();

  // Create a task to run on core 0
  xTaskCreatePinnedToCore(
    Task0,          // Task function
    "Task0",        // Name of task
    10000,          // Stack size of task
    NULL,           // Parameter of the task
    1,              // Priority of the task
    NULL,           // Task handle
    0);             // Core where the task should run

  // Create a task to run on core 1
  xTaskCreatePinnedToCore(
    Task1,          // Task function
    "Task1",        // Name of task
    10000,          // Stack size of task
    NULL,           // Parameter of the task
    1,              // Priority of the task
    NULL,           // Task handle
    1);   

}

unsigned long lastStatusMessageTime = 0;

// Main (CORE0) Loop
// This core should process WS messages and update the robot accordingly
void loop() {
  // Robot Status
  robot.checkStatus();

  // webServer.handleClient();
  webSocket.loop();

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
}
