#include <LiquidCrystal.h>

// Initialize LCD: (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(21, 22, 18, 19, 23, 5);

void setup() {
  lcd.begin(16, 2);  // 16 columns, 2 rows
  lcd.print("Hello, ESP32!");
}

void loop() {
  // Just show a count
  lcd.setCursor(0, 1);  // Column 0, Row 1
  lcd.print(millis() / 1000);
  lcd.print(" sec   ");
  delay(1000);
}
