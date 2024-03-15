#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#include <ESPAsyncWebServer.h>

#include <vector>

#include "OTA32.h"
#include "robot.h"
// #include "imu.h"
#include "wpilibws_processor.h"
// #include "config.h"
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
// xrp::LSM6IMU imu;

xrp::Watchdog dsWatchdog{"ds"};

// Status Vars
// NetworkMode netConfigResult;

// Chip Identifier
char chipID[20];

AsyncWebServer webServer(3300);
WebSocketsServer webSocket = WebSocketsServer(3300, "/wpilibws");

// ===================================================
// Handlers for INBOUND WS Messages
// ===================================================
void broadcast(std::string msg) {
  webSocket.broadcastTXT(msg.c_str());
}

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
// Web Server management functions
// ===================================================
void setupWebServerRoutes() {
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    Serial.println("GET /");        // for debugging
    request->send(200, "text/html", "<!DOCTYPE html><html lang='en'><head> <meta charset='utf-8'> <title>XRP Configuration</title> <link rel='stylesheet' href='normalize.css'> <link rel='stylesheet' href='skeleton.css'> <script type='text/javascript' src='xrp.js'></script></head><body> <div class='container'> <div class='row'> <div class='column' style='margin-top: 5%'> <h4>XRP Configuration</h4> <p>Edit the JSON below as necessary</p> </div> </div> <form> <label for='configJson'>Configuration JSON</label> <textarea class='u-full-width' style='white-space: pre; height: auto' rows='20' id='configJson'></textarea> <input class='button' id='resetButton' type='submit' value='Reset to Default'> <input class='button-primary' id='saveButton' type='submit' value='Save'> </form> </div></body></html>");
  });

  webServer.on("/normalize.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/css", "/*! normalize.css v3.0.2 | MIT License | git.io/normalize */img,legend{border:0}legend,td,th{padding:0}html{font-family:sans-serif;-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%}body{margin:0}article,aside,details,figcaption,figure,footer,header,hgroup,main,menu,nav,section,summary{display:block}audio,canvas,progress,video{display:inline-block;vertical-align:baseline}audio:not([controls]){display:none;height:0}[hidden],template{display:none}a{background-color:transparent}a:active,a:hover{outline:0}abbr[title]{border-bottom:1px dotted}b,optgroup,strong{font-weight:700}dfn{font-style:italic}h1{font-size:2em;margin:.67em 0}mark{background:#ff0;color:#000}small{font-size:80%}sub,sup{font-size:75%;line-height:0;position:relative;vertical-align:baseline}sup{top:-.5em}sub{bottom:-.25em}svg:not(:root){overflow:hidden}figure{margin:1em 40px}hr{-moz-box-sizing:content-box;box-sizing:content-box;height:0}pre,textarea{overflow:auto}code,kbd,pre,samp{font-family:monospace,monospace;font-size:1em}button,input,optgroup,select,textarea{color:inherit;font:inherit;margin:0}button{overflow:visible}button,select{text-transform:none}button,html input[type=button],input[type=reset],input[type=submit]{-webkit-appearance:button;cursor:pointer}button[disabled],html input[disabled]{cursor:default}button::-moz-focus-inner,input::-moz-focus-inner{border:0;padding:0}input{line-height:normal}input[type=checkbox],input[type=radio]{box-sizing:border-box;padding:0}input[type=number]::-webkit-inner-spin-button,input[type=number]::-webkit-outer-spin-button{height:auto}input[type=search]{-webkit-appearance:textfield;-moz-box-sizing:content-box;-webkit-box-sizing:content-box;box-sizing:content-box}input[type=search]::-webkit-search-cancel-button,input[type=search]::-webkit-search-decoration{-webkit-appearance:none}fieldset{border:1px solid silver;margin:0 2px;padding:.35em .625em .75em}table{border-collapse:collapse;border-spacing:0}");
  });

  webServer.on("/skeleton.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/css", ".column,.columns,.container{width:100%;box-sizing:border-box}h1,h2,h3{letter-spacing:-.1rem}body,h6{line-height:1.6}ol,p,ul{margin-top:0}.container{position:relative;max-width:960px;margin:0 auto;padding:0 20px}.column,.columns{float:left}@media (min-width:400px){.container{width:85%;padding:0}}html{font-size:62.5%}body{font-size:1.5em;font-weight:400;font-family:Raleway,HelveticaNeue,'Helvetica Neue',Helvetica,Arial,sans-serif;color:#222}h1,h2,h3,h4,h5,h6{margin-top:0;margin-bottom:2rem;font-weight:300}h1{font-size:4rem;line-height:1.2}h2{font-size:3.6rem;line-height:1.25}h3{font-size:3rem;line-height:1.3}h4{font-size:2.4rem;line-height:1.35;letter-spacing:-.08rem}h5{font-size:1.8rem;line-height:1.5;letter-spacing:-.05rem}h6{font-size:1.5rem;letter-spacing:0}@media (min-width:550px){.container{width:80%}.column,.columns{margin-left:4%}.column:first-child,.columns:first-child{margin-left:0}.one.column,.one.columns{width:4.66666666667%}.two.columns{width:13.3333333333%}.three.columns{width:22%}.four.columns,.one-third.column{width:30.6666666667%}.five.columns{width:39.3333333333%}.one-half.column,.six.columns{width:48%}.seven.columns{width:56.6666666667%}.eight.columns,.two-thirds.column{width:65.3333333333%}.nine.columns{width:74%}.ten.columns{width:82.6666666667%}.eleven.columns{width:91.3333333333%}.twelve.columns{width:100%;margin-left:0}.offset-by-one.column,.offset-by-one.columns{margin-left:8.66666666667%}.offset-by-two.column,.offset-by-two.columns{margin-left:17.3333333333%}.offset-by-three.column,.offset-by-three.columns{margin-left:26%}.offset-by-four.column,.offset-by-four.columns,.offset-by-one-third.column,.offset-by-one-third.columns{margin-left:34.6666666667%}.offset-by-five.column,.offset-by-five.columns{margin-left:43.3333333333%}.offset-by-one-half.column,.offset-by-one-half.columns,.offset-by-six.column,.offset-by-six.columns{margin-left:52%}.offset-by-seven.column,.offset-by-seven.columns{margin-left:60.6666666667%}.offset-by-eight.column,.offset-by-eight.columns,.offset-by-two-thirds.column,.offset-by-two-thirds.columns{margin-left:69.3333333333%}.offset-by-nine.column,.offset-by-nine.columns{margin-left:78%}.offset-by-ten.column,.offset-by-ten.columns{margin-left:86.6666666667%}.offset-by-eleven.column,.offset-by-eleven.columns{margin-left:95.3333333333%}h1{font-size:5rem}h2{font-size:4.2rem}h3{font-size:3.6rem}h4{font-size:3rem}h5{font-size:2.4rem}h6{font-size:1.5rem}}a{color:#1eaedb}a:hover{color:#0fa0ce}.button,button,input[type=button],input[type=reset],input[type=submit]{display:inline-block;height:38px;padding:0 30px;color:#555;text-align:center;font-size:11px;font-weight:600;line-height:38px;letter-spacing:.1rem;text-transform:uppercase;text-decoration:none;white-space:nowrap;background-color:transparent;border-radius:4px;border:1px solid #bbb;cursor:pointer;box-sizing:border-box}ol,td:first-child,th:first-child,ul{padding-left:0}.button:focus,.button:hover,button:focus,button:hover,input[type=button]:focus,input[type=button]:hover,input[type=reset]:focus,input[type=reset]:hover,input[type=submit]:focus,input[type=submit]:hover{color:#333;border-color:#888;outline:0}.button.button-primary,button.button-primary,input[type=button].button-primary,input[type=reset].button-primary,input[type=submit].button-primary{color:#fff;background-color:#33c3f0;border-color:#33c3f0}.button.button-primary:focus,.button.button-primary:hover,button.button-primary:focus,button.button-primary:hover,input[type=button].button-primary:focus,input[type=button].button-primary:hover,input[type=reset].button-primary:focus,input[type=reset].button-primary:hover,input[type=submit].button-primary:focus,input[type=submit].button-primary:hover{color:#fff;background-color:#1eaedb;border-color:#1eaedb}input[type=email],input[type=number],input[type=password],input[type=search],input[type=tel],input[type=text],input[type=url],select,textarea{height:38px;padding:6px 10px;background-color:#fff;border:1px solid #d1d1d1;border-radius:4px;box-shadow:none;box-sizing:border-box}input[type=email],input[type=number],input[type=password],input[type=search],input[type=tel],input[type=text],input[type=url],textarea{-webkit-appearance:none;-moz-appearance:none;appearance:none}textarea{min-height:65px;padding-top:6px;padding-bottom:6px}input[type=email]:focus,input[type=number]:focus,input[type=password]:focus,input[type=search]:focus,input[type=tel]:focus,input[type=text]:focus,input[type=url]:focus,select:focus,textarea:focus{border:1px solid #33c3f0;outline:0}label,legend{display:block;margin-bottom:.5rem;font-weight:600}fieldset{padding:0;border-width:0}input[type=checkbox],input[type=radio]{display:inline}label>.label-body{display:inline-block;margin-left:.5rem;font-weight:400}ul{list-style:circle inside}ol{list-style:decimal inside}ol ol,ol ul,ul ol,ul ul{margin:1.5rem 0 1.5rem 3rem;font-size:90%}.button,button,li{margin-bottom:1rem}code{padding:.2rem .5rem;margin:0 .2rem;font-size:90%;white-space:nowrap;background:#f1f1f1;border:1px solid #e1e1e1;border-radius:4px}pre>code{display:block;padding:1rem 1.5rem;white-space:pre}td,th{padding:12px 15px;text-align:left;border-bottom:1px solid #e1e1e1}td:last-child,th:last-child{padding-right:0}fieldset,input,select,textarea{margin-bottom:1.5rem}blockquote,dl,figure,form,ol,p,pre,table,ul{margin-bottom:2.5rem}.u-full-width{width:100%;box-sizing:border-box}.u-max-full-width{max-width:100%;box-sizing:border-box}.u-pull-right{float:right}.u-pull-left{float:left}hr{margin-top:3rem;margin-bottom:3.5rem;border-width:0;border-top:1px solid #e1e1e1}.container:after,.row:after,.u-cf{content:"";display:table;clear:both}");
  });

  webServer.on("/xrp.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/javascript", "window.onload = () => {console.log('hello');const configJsonEntry = document.getElementById('configJson');const resetButton = document.getElementById('resetButton');const saveButton = document.getElementById('saveButton');// Hook up button eventsresetButton.onclick = (e) => { e.preventDefault(); console.log('Reset Button Clicked'); fetch('/resetconfig', { method: 'post' }) .then(() => { loadConfig(); });};saveButton.onclick = (e) => { e.preventDefault(); console.log('Save Button Clicked'); try { JSON.parse(configJsonEntry.value); fetch('/saveconfig', { body: configJsonEntry.value, headers: { 'Content-Type': 'text/json' }, method: 'post' }) .then(() => { alert('Configuration Updated. Please reset the XRP'); }) } catch (e) { alert('Invalid JSON. Please check and try again'); }}// load data and then enable the buttonsloadConfig();function loadConfig() { fetch('/getconfig') .then((response) => response.json()) .then((xrpConfigJson) => { configJsonEntry.value = JSON.stringify(xrpConfigJson, null, 4); }) .catch((err) => { console.log('ERROR'); console.log(err); });}};");
  });
}

// ===================================================
// WebSocket management functions
// ===================================================

// void broadcast(std::string msg) {
//   webSocket.broadcastTXT(msg.c_str());
// }

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

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

// ===================================================
// Boot-Up and Main Control Flow
// ===================================================

void setup() {
  
  Serial.begin(115200);
  digitalWrite(BUILTIN_LED, LOW);

  // Setup the OLED.
  #if USE_OLED
    setupOLED();
  #endif

  if (WiFi.status() != WL_CONNECTED) { // WiFi hasn't started
    connectWiFi(1); // AP mode 1   
    // setupOTA(); // OTA WiFi connection.  
    delay(1000); 
  }

  // Set up WebSocket messages
  hookupWSMessageHandlers();

  // Set up the web server and websocket server hooks
  setupWebServerRoutes();

  // webServer.addHook(wsServer.hookForWebserver("/wpilibws", onWebSocketEvent));
  webSocket.onEvent(onWebSocketEvent);
  webSocket.begin();
  
  webServer.onNotFound(notFound);
  webServer.begin();

  Serial.println("HTTP Server started on port 3300");
  Serial.println("WebSocket server started on /wpilibws on port 3300");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());

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
    if (robot._leftMotor.encoder.updated()) {
      auto leftjsonMsg = wsMsgProcessor.makeEncoderMessage(0, robot._leftMotor.encoder.getTicks());
      broadcast(leftjsonMsg);
      // Simulate this since there's no encoder on the right motor
      auto rightjsonMsg = wsMsgProcessor.makeEncoderMessage(1, robot._leftMotor.encoder.getTicks());
      broadcast(rightjsonMsg);
    }
    lastStatusMessageTime = millis();
  }
}
