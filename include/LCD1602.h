#pragma once
#include "Constants.h"
// #include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define I2C_SDA 8
#define I2C_SCL 9

#define SCREEN_WIDTH 16 // OLED display width, in pixels
#define SCREEN_HEIGHT 2 // OLED display height, in pixels

inline LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

inline void setupLCD(){
  Serial.print("LCD1602 setup...");
	lcd.init(I2C_SDA, I2C_SCL); // initialize the lcd to use user defined I2C pins
	lcd.backlight();
  lcd.clear();
	// lcd.setCursor(3,0);
	// lcd.print("Hello, world!");
	// lcd.setCursor(2,1);
	// lcd.print("Time is now");
  Serial.println("Done");
}

// // Clear a rectangled area of the screen
// inline void clearRect(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height)
// {
//     lcd.setCursor(x,y);
//     lcd.print("                ");
// }

// inline void clearLinePart(const uint16_t lineNumber, const uint16_t startPosition, const uint16_t width)
// {
// #if USE_OLED  
//     uint16_t yPos=0;
//     const uint16_t lineWidth = 11;
//     yPos = (lineNumber * lineWidth) - 1;
//     if (lineNumber > 3) {yPos--;}
//     clearRect(startPosition, yPos, width, lineWidth);
// #endif  
// }

inline void clearLine(int line)
{      
#if USE_OLED           
  lcd.setCursor(0,line);
  for(int n = 0; n < 16; n++) 
  {
    lcd.print(" ");
  }
#endif
}

inline void clearDisplay() {
#if USE_OLED  
    lcd.clear();
#endif  
}

inline void clearDisplayBelowHeader() {
}

inline void drawText(const uint16_t col, const uint16_t row, const String text) {
#if USE_OLED  
    lcd.setCursor(col, row);
    lcd.print(text);
    lcd.display();
#endif   
}

inline void drawDSConnect(bool connected, int row=0) {
//   clearLinePart(line, 20, 22);
  if (connected) {
    drawText(14, row, "DS");
  } else {
    drawText(14, row, "  ");
  }
}

inline void drawDSState(bool enabled, int row=1) {
  // clearLine(row);
  if (enabled) {
    drawText(0, row, "ENABLED         ");
  } else {
    drawText(0, row, "DISABLED        ");
  }
}

inline void drawWiFi(const uint16_t x=0, const uint16_t y=0) {
#if USE_OLED  
//   clearLinePart(0, 0, 20);
//   drawBitmap(x, y, WIFI_width, WIFI_height, WIFI_bits);
#endif  
}

inline void drawAP(const uint16_t x=90, const uint16_t y=0) {
#if USE_OLED  
//   drawBitmap(x, y, AP_width, AP_height, AP_bits);
#endif  
}

inline void drawConnecting(String id) {
#if USE_OLED  
  drawText(0, 0, "Connecting...");
  drawText(0, 1, id);
#endif
}

inline void drawConnected(IPAddress IP) {
#if USE_OLED 
  clearDisplay();
  drawText(0, 0, "IP ");drawText(3, 0, IP.toString());
#endif
}