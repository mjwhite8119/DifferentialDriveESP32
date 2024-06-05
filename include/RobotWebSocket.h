#pragma once

#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "wpilibws_processor.h"

// WebSocket server URL and port
const char* wsServer = "/wpilibws"; 
const int wsPort = 3300;
WebSocketsServer webSocket = WebSocketsServer(wsPort, wsServer);

wpilibws::WPILibWSProcessor wsMsgProcessor;

// ===================================================
// WebSocket management functions Core0
// ===================================================
// Broadcast message back to all clients
void broadcast(std::string msg) {
  webSocket.broadcastTXT(msg.c_str());
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  
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

void setupWebSocket() {

    Serial.println("Starting WebSocket server");
    webSocket.onEvent(onWebSocketEvent);
    webSocket.begin();

    Serial.printf("WebSocket server started on %s on port %i\n", wsServer, wsPort);
    Serial.printf("IP Address: %s\n", WiFi.softAPIP().toString().c_str());
}

void loopWebSocket() {

    webSocket.loop();
    // Attempt to reconnect if disconnected
    // if (!webSocket.clientIsConnected() && reconnectAttempts < maxReconnectAttempts) {
    //   Serial.println("Reconnecting to WebSocket...");
    //   webSocket.begin();
    //   reconnectAttempts++;
    //   delay(reconnectInterval);
    // }
}