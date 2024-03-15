#pragma once
#include "Constants.h"

// ---- Use the Heltec library ----------- //
#if HELTEC
  #include "SSD1306.h" 

  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels

  #define OLED_SDA    4
  #define OLED_SCL   15
  #define OLED_RESET   16 //RST must be set by software

  inline SSD1306  display(0x3c, OLED_SDA, OLED_SCL);
#endif

#if USE_OLED
  #include "images.h"
#endif

/* ------------------- Drawing functions -----------------------------*/
// Clear a rectangled area of the screen
inline void clearRect(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height)
{

#if HELTEC
  display.setColor(BLACK);
  display.fillRect(x, y, width, height);
  display.display();
  display.setColor(WHITE);
#endif
delay(1);
}

inline void clearLinePart(const uint16_t lineNumber, const uint16_t startPosition, const uint16_t width)
{
#if USE_OLED  
  uint16_t yPos=0;
  const uint16_t lineWidth = 11;
  #if HELTEC
    yPos = (lineNumber * lineWidth) - 1;
    if (lineNumber > 3) {yPos--;}
  #else
    lineWidth = 11;
    yPos = (lineNumber * lineWidth);
  #endif  
  clearRect(startPosition, yPos, width, lineWidth);
#endif  
}

inline void clearLine(const uint16_t lineNumber)
{
#if USE_OLED  
  clearLinePart(lineNumber, 0, SCREEN_WIDTH);
#endif  
}

inline void drawText(const uint16_t lineNumber, const uint16_t startPosition, const String text) {
#if USE_OLED  
  const uint16_t lineWidth = 10;
  const uint16_t yPos=lineNumber * lineWidth;
  #if USE_U8G2
    // U8G2 uses c-string so convert.
    const char * char_text = text.c_str();
    // write something to the internal memory
    lineWidth = 11;
    yPos = (lineNumber * lineWidth) + 9;
    u8g2.drawStr(startPosition, yPos, char_text);  
    // transfer internal memory to the display
    u8g2.sendBuffer();             
  #endif
  
  #if HELTEC
    display.drawString(startPosition, yPos, text);
    display.display();
  #endif 
#endif   
}

inline void drawBitmap(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, const unsigned char* bm_bits) 
{
#if USE_OLED  
  #if USE_U8G2
    u8g2.drawXBMP(x, y, width, height, bm_bits);
    u8g2.sendBuffer();  
  #endif
  #if HELTEC
    display.drawXbm(x, y, width, height, bm_bits);
    display.display();
  #endif  
#endif  
}

// Clear the entire screen
inline void clearDisplay() {
#if USE_OLED  
  #if USE_U8G2
    u8g2.clear();
  #endif
  #if HELTEC
    display.clear();
  #endif
#endif  
}

inline void clearDisplayBelowHeader() {
#if USE_OLED  
  clearRect(0, 10, SCREEN_WIDTH, SCREEN_HEIGHT-10);
#endif  
}

inline void drawDSConnect(bool connected, int line=0) {
  clearLinePart(line, 20, 22);
  if (connected) {
    drawText(line, 20, "DS");
  } else {
    clearLine(3);clearLine(4);clearLine(5);
  }
}

inline void drawDSState(bool enabled, int line=3) {
  clearLine(line);
  if (enabled) {
    drawText(line, 0, "ENABLED");
  } else {
    drawText(line, 0, "DISABLED");
  }
}

inline void drawBattery(const uint16_t x=108, const uint16_t y=0, const uint8_t capacity=3) {
#if USE_OLED 
  clearLinePart(0, x, y);
//  LOGLN2("Capacity",capacity);
  if (capacity == 1) {
    drawBitmap(x, y, BAT_width, BAT_height, BATLow_bits); 
//    LOGLN2("Full",capacity);
  } else if (capacity == 2) {
//    LOGLN2("Medium",capacity);
    drawBitmap(x, y, BAT_width, BAT_height, BATMedium_bits);
  } else {
    drawBitmap(x, y, BAT_width, BAT_height, BAT_bits);
  }  
#endif  
}

inline void drawAP(const uint16_t x=90, const uint16_t y=0) {
#if USE_OLED  
  drawBitmap(x, y, AP_width, AP_height, AP_bits);
#endif  
}

inline void drawSerial(const uint16_t x=45, const uint16_t y=0) {
#if USE_OLED  
  drawBitmap(x, y, SERIAL_width, SERIAL_height, SERIAL_bits);
#endif  
}

inline void drawMQTT(const uint16_t x=48, const uint16_t y=0) {
#if USE_OLED  
  drawBitmap(x, y, MQTT_width, MQTT_height, MQTT_bits);
#endif  
}

inline void drawLORA(const uint16_t x=35, const uint16_t y=0) {
#if USE_OLED  
  drawBitmap(x, y, LORA_width, LORA_height, LORA_bits);
#endif  
}

inline void drawWiFi(const uint16_t x=0, const uint16_t y=0) {
#if USE_OLED  
  clearLinePart(0, 0, 20);
  drawBitmap(x, y, WIFI_width, WIFI_height, WIFI_bits);
#endif  
}

// Remove the WiFi icon and display the disconnect message
inline void clearWiFi(const uint16_t x=0, const uint16_t y=0) {
#if USE_OLED
  clearLinePart(0, 0, 15);
  clearLine(2);
  drawText(2, 0, "WiFi disconnected!");
#endif  
}

// Clear the node connecting message and draw the ROS icon
inline void drawROS(const uint16_t x=18, const uint16_t y=0) {
#if USE_OLED  
  drawText(0, x, "ROS");
//  clearLine(1);
#endif  
}

// Remove the ROS icon and display the disconnect message
inline void clearROS(const uint16_t x=16, const uint16_t y=0) {
#if USE_OLED  
  clearLinePart(0, x, 24);
//  drawText(1, 0, "Node disconnected!");
#endif  
}

inline void drawIPAddress(const uint16_t lineNumber, const uint16_t startPosition, const IPAddress ip) {
#if USE_OLED  
  drawText(lineNumber, startPosition, String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]));
#endif  
}

inline void drawProgressBar(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, const uint16_t progress) {
#if USE_OLED  
  #if HELTEC
    display.drawProgressBar(x, y, width, height, progress); 
  #endif
#endif  
}

//******************************************************//
// setup oled
// *****************************************************//
inline void setupOLED()
{
#if USE_U8G2
  LOGLN("U8G2 OLED connect");

  #define OLED_SDA    4
  #define OLED_SCL   15
  #define OLED_RESET   16 //RST must be set by software
  U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
  u8g2.begin();
  // clear the internal memory
  u8g2.clearBuffer();   
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_t0_11_mr);         
#endif  
  
#if HELTEC

  pinMode(OLED_RESET,OUTPUT);
  digitalWrite(OLED_RESET, LOW);    
  delay(50); 
  digitalWrite(OLED_RESET, HIGH);

  display.init();

  display.setContrast(255);
  
//  display.flipScreenVertically(); 
  display.setFont(ArialMT_Plain_10);        
  display.setTextAlignment(TEXT_ALIGN_LEFT);
#endif  
}
