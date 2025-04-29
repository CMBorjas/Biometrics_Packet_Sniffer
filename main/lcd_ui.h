// lcd_ui.h
#ifndef LCD_UI_H
#define LCD_UI_H

#include <LiquidCrystal.h>

// LCD Pin Definitions (adjust if needed)
#define LCD_RS 4
#define LCD_E  5
#define LCD_D4 18
#define LCD_D5 19
#define LCD_D6 21
#define LCD_D7 22

// LCD Object
extern LiquidCrystal lcd;

// Functions to Initialize and Update LCD
void initLCD();
void lcdDisplayMessage(const String& line1, const String& line2);
void lcdClear();
void lcdPrintLine1(const String& text);
void lcdPrintLine2(const String& text);
void lcdSetCursorPosition(uint8_t col, uint8_t row);
void lcdPrintAtPosition(uint8_t col, uint8_t row, const String& text);
void lcdDisplayFingerprintSuccess();
void lcdDisplayTimeout();
void lcdDisplayUnknownInput();
void lcdDisplaySystemStatus(const String& status);
void lcdDisplaySniffingStatus(const String& status);

#endif // LCD_UI_H
