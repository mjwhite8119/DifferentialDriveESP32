#include <Arduino.h>

// #include "i2cScan.h"
#include "LCD1602.h"
#include <WiFi.h>
#include <WiFiMulti.h>

#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#include <vector>

#include "OTA32.h"
#include "Robot.h"
#include "MPU6050.h"

#include "wpilibws_processor.h"
#include "watchdog.h"

#include "RobotWebServer.h"
// #include "RobotWebSocket.h"

wpilibws::WPILibWSProcessor wsMsgProcessor;

xrp::Robot robot;

// Gyro
xrp::MPU6050 imu;

xrp::Watchdog dsWatchdog{"ds"};

// Status Vars
// NetworkMode netConfigResult;

// Chip Identifier
char chipID[20];

// // Reconnection variables
// const int maxReconnectAttempts = 10;
// int reconnectAttempts = 0;
// const int reconnectInterval = 5000; // 5 seconds
// WebSocket server URL and port
const char* wsServer = "/wpilibws"; 
const int wsPort = 3300;
WebSocketsServer webSocket = WebSocketsServer(wsPort, wsServer);
// AsyncWebServer server(wsPort);
// AsyncWebSocket webSocket(wsServer);

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
// WebSocket management functions Core0
// ===================================================
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
// Setup CPU Core1 Tasks
// ===================================================

// Broadcast message back to all clients
void broadcast(std::string msg) {
  noInterrupts();
  webSocket.broadcastTXT(msg.c_str());
  interrupts();
}

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

  // // Set up WebSocket messages
  hookupWSMessageHandlers();

  webSocket.onEvent(onWebSocketEvent);
  webSocket.begin();

  Serial.println("WebSocket server started on /wpilibws on port 3300");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
  
  // Set up the webServer on robot
  setupWebServer();

  robot.init();

  // Set up the webSocket on robot
  // setupWebSocket();

  // Diagnostic test for I2C connectivity
  // i2cScan();

  // Create task for core 1
  // xTaskCreatePinnedToCore(
  //   core1Task,     // Task function
  //   "Core1Task",   // Name of task
  //   10000,         // Stack size of task
  //   NULL,          // Parameter of the task
  //   1,             // Priority of the task
  //   NULL,          // Task handle
  //   1);            // Core where the task should run

}

// Main (CORE0) Loop
// This core should process WS messages and update the robot accordingly
void loop() {
  // Robot Status
  robot.checkStatus();

  // Process WebSocket events
  // webSocket.cleanupClients();

  // webServer.handleClient();
  webSocket.loop();

  // Attempt to reconnect if disconnected
  // if (!webSocket.clientIsConnected() && reconnectAttempts < maxReconnectAttempts) {
  //   Serial.println("Reconnecting to WebSocket...");
  //   webSocket.begin();
  //   reconnectAttempts++;
  //   delay(reconnectInterval);
  // }

  // // Send the WS messages. Can only send messages on intervals
  // if (millis() - lastStatusMessageTime > 50) { 

  //   // Encoder messages  
  //   if (robot._leftMotor.encoder.updated()) {
  //     auto leftjsonMsg = wsMsgProcessor.makeEncoderMessage(0, robot._leftMotor.encoder.getTicks());
  //     broadcast(leftjsonMsg);
  //   }
  //   if (robot._rightMotor.encoder.updated()) {  
  //     auto rightjsonMsg = wsMsgProcessor.makeEncoderMessage(1, robot._rightMotor.encoder.getTicks() * -1);
  //     broadcast(rightjsonMsg);
  //   }

  //   // Gyro messages
  //   imu.update();
  //   auto gyroJsonMsg = wsMsgProcessor.makeGyroMessage(imu.getRates(), imu.getAngles());
  //   broadcast(gyroJsonMsg);

  //   lastStatusMessageTime = millis();
  // }
}
