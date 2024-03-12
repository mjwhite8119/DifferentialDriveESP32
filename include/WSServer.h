#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// Globals
WebSocketsServer webSocketFromSimulator = WebSocketsServer(3300);
WebSocketsServer webSocketToWebpage = WebSocketsServer(81);

String deviceTypeStr = "";

void processPayload(uint8_t * payload) {
  JsonDocument doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }

  // Extract values from JSON
  int device = doc["device"];
  const char* deviceType = doc["type"];

  // Print the parsed values
  // Serial.print("Device: ");
  // Serial.println(device);
  // Serial.print("Type: ");
  // Serial.println(deviceType);

  // Extract and print data object
  JsonObject data = doc["data"];
  // Serial.println("Data:");
  // serializeJson(data, Serial);

  // Extract values from JSON
  int outputs = data["<outputs"];
  int rumble_left = data["<rumble_left"];
  int rumble_right = data["<rumble_right"];

  // Print the parsed values
  // Serial.print("<outputs: ");
  // Serial.println(outputs);
  // Serial.print("<rumble_left: ");
  // Serial.println(rumble_left);
  // Serial.print("<rumble_right: ");
  // Serial.println(rumble_right);

  // Extract and print arrays
  JsonArray axes = data[">axes"];
  // Serial.println("Axes:");
  // serializeJson(axes, Serial);
  // Serial.println();
  // serializeJson(axes[0], Serial);
  // Serial.println();
  // Serial.println(axes);


  JsonArray buttons = data[">buttons"];
  // Serial.println("Buttons:");
  // serializeJson(buttons, Serial);
  // Serial.println();

  JsonArray povs = data[">povs"];
  // Serial.println("Povs:");
  // serializeJson(povs, Serial);

  if (strcmp(deviceType, "Joystick") == 0 && device == 0) {
    // Create a JsonDocument to send to website
    JsonDocument newdoc;

    // Add values to the nested object
    newdoc["device"] = device;
    newdoc["deviceType"] = deviceType;
    newdoc["axes"] = axes;

    // Serialize the document to a string
    String jsonString;
    serializeJson(newdoc, jsonString);
    // Serial.println("new string");
    Serial.println(jsonString);

    webSocketToWebpage.broadcastTXT(jsonString); 
  }
}

int lineCount = 0;
// Called when receiving any WebSocket message
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  
  
  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocketFromSimulator.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
      
      processPayload(payload);

      // if (lineCount < 20) {       
        // Serial.printf("[%u] Text: %s\n", num, payload);
        // Serial.printf("[%u] Line count\n", lineCount);
      // }  
      lineCount ++;
      webSocketFromSimulator.sendTXT(num, payload);
      // webSocket.broadcastTXT(jsonString); 
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void joystickUpdateToWpilib(int joystickNumber, String joystickPayload) {
    // const msg: IWpilibWsMsg = {
    //     type: JoystickDeviceType,
    //     device: joystickNum.toString(),
    //     data: payload
    // };

    // this._sendWpilibUpdateMessage(msg);
}

void setupSimulatorWSServer() {
  // Start WebSocket from the Simulator and assign callback
  webSocketFromSimulator.begin();
  webSocketFromSimulator.onEvent(onWebSocketEvent);
  const char* jsonString = "{\"data\":{\"<outputs\":0,\"<rumble_left\":0,\"<rumble_right\":0,\">axes\":[0.003921627998352051,-0.003921568393707275,0.003921627998352051,0.003921627998352051],\">buttons\":[],\">povs\":[]},\"device\":\"3\",\"type\":\"Joystick\"}";

  // Parse JSON object
  JsonDocument doc; // Adjust the size as necessary
  DeserializationError error = deserializeJson(doc, jsonString);

  // Check for parsing errors
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract values from JSON
  const char* device = doc["device"];
  const char* deviceType = doc["type"];

  // Print the parsed values
  Serial.print("Device: ");
  Serial.println(device);
  Serial.print("Type: ");
  Serial.println(deviceType);

  // Extract and print data object
  JsonObject data = doc["data"];
  Serial.println("Data:");
  serializeJson(data, Serial);


  // Extract values from JSON
  int outputs = data["<outputs"];
  int rumble_left = data["<rumble_left"];
  int rumble_right = data["<rumble_right"];

  // Print the parsed values
  Serial.print("<outputs: ");
  Serial.println(outputs);
  Serial.print("<rumble_left: ");
  Serial.println(rumble_left);
  Serial.print("<rumble_right: ");
  Serial.println(rumble_right);

  // Extract and print arrays
  JsonArray axes = data[">axes"];
  Serial.println("Axes:");
  serializeJson(axes, Serial);
  Serial.println();
  serializeJson(axes[0], Serial);
  Serial.println();
  Serial.println(axes);

  JsonArray buttons = data[">buttons"];
  Serial.println("Buttons:");
  serializeJson(buttons, Serial);
  Serial.println();

  JsonArray povs = data[">povs"];
  Serial.println("Povs:");
  serializeJson(povs, Serial);


  
  // Create a StaticJsonDocument to hold the objects
  JsonDocument newdoc;

  // Add values to the nested object
  newdoc["device"] = device;
  newdoc["deviceType"] = deviceType;
  newdoc["axes"] = axes;

  // Serialize the document to a string
  String jsonStringTxt;
  serializeJson(newdoc, jsonStringTxt);
  Serial.println("new string");
  Serial.println(jsonStringTxt);

}

void loopWSServerFromSimulator() {

  // Look for and handle WebSocket data
  webSocketFromSimulator.loop();

  // webSocketToWebpage.loop();
}