#include <LiquidCrystal.h>

// Create LCD object with safe pins
LiquidCrystal lcd(4, 5, 18, 19, 21, 22);

void initLCD() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("ESP32 + LCD OK!");
}

void updateLCDStatus(const String& message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}
