#pragma once
#include <ESPAsyncWebServer.h>

#include <ArduinoJson.h>
#include "wpilibws_processor.h"

// Reconnection variables
const int maxReconnectAttempts = 10;
int reconnectAttempts = 0;
const int reconnectInterval = 5000; // 5 seconds

const char* wsServer = "/wpilibws"; 
const int webSocketPort = 3300;
AsyncWebServer server(webSocketPort);
AsyncWebSocket webSocket(wsServer);

wpilibws::WPILibWSProcessor wsMsgProcessor;

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("Client connected: %u\n", client->id());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("Client disconnected: %u\n", client->id());
      break;
    case WS_EVT_DATA: {
      JsonDocument jsonDoc;
      Serial.printf("Data received from client %u: %s\n", client->id(), data);    
      DeserializationError error = deserializeJson(jsonDoc, data);
      if (error) {
          Serial.println(error.f_str());
          break;
      }
      wsMsgProcessor.processMessage(jsonDoc);
      break;
    }
    case WS_EVT_PONG:
      Serial.printf("Pong received from client %u\n", client->id());
      break;
    case WS_EVT_ERROR:
      Serial.printf("Error on client %u: %s\n", client->id(), arg ? (char*)arg : "");
      break;
  }
}

// Broadcast message back to all clients
void broadcast(std::string msg) {
  noInterrupts();
  webSocket.textAll(msg.c_str());
  interrupts();
}

void setupWebSocket() {

    Serial.println("Starting WebSocket server");
    webSocket.onEvent(onWebSocketEvent);
    server.addHandler(&webSocket);

    Serial.println("WebSocket server started on /wpilibws on port 3300");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
}