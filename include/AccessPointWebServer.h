/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "vikings!";

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputLedState = "off";
String deviceType = "";

void setupAccessPoint() {
  
  digitalWrite(BUILTIN_LED, LOW);
 
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  // Set static IP
  IPAddress AP_LOCAL_IP(10, 0, 0, 2);
  IPAddress AP_GATEWAY_IP(0, 0, 0, 0);
  IPAddress AP_NETWORK_MASK(255, 255, 255, 0);
  if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY_IP, AP_NETWORK_MASK)) {
    Serial.println("AP Config Failed");
    return;
  }

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

// Set web server port number to 80 to display a webpage on the access point
WiFiServer server(80);
void setupWebServer() {
  server.begin();
}

void printPageHeader(WiFiClient client) {
  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");
}

void printScript(WiFiClient client) {
  String script = "<script>var Socket;function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':3300/wpilibws'); Socket.onmessage = function(event) { processCommand(event); };}function processCommand(event) { var obj = JSON.parse(event.data); document.getElementById('deviceType').innerHTML = obj.type; console.log(obj.type);}window.onload = function(event) { init();}</script>";
  client.println(script);
}

void loopWiFiServer(){
  WiFiClient client = server.available();   // Listen for incoming request from the browser

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // server.println("Hello");
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("Builtin LED on");
              outputLedState = "on";
              digitalWrite(BUILTIN_LED, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("Builtin LED off");
              outputLedState = "off";
              digitalWrite(BUILTIN_LED, LOW);
            }
            
            printPageHeader(client);
            printScript(client);

            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");

            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>Builtin LED - State " + outputLedState + "</p>");
            // If the outputLedState is off, it displays the ON button       
            if (outputLedState=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            client.println("<p>Device <span id='deviceType'></span></p>");
            client.println("<p><span id='axes0'></span></p>");
            client.println("<p><span id='axes1'></span></p>");
            client.println("<p><span id='axes2'></span></p>");
            client.println("<p><span id='axes3'></span></p>");
      
            // The HTTP response ends with another blank line
            client.println();

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}