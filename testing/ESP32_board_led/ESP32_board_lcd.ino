#include <LiquidCrystal.h>

// (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(21, 22, 18, 19, 23, 5);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Hello, ESP32!");
}

void loop() {
  // do nothing
}
