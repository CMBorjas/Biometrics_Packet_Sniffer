#include <LiquidCrystal.h>
#include "lcd_ui.h"
#include "packet_sniffer.h"

LiquidCrystal lcd(4, 5, 18, 19, 21, 22);  // Adjust as needed

void initLCD() {
  lcd.begin(16, 2);
  lcd.clear();
  //lcd.print("ESP32 Ready");
}

void updateLCDStatus(const String& message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}

void lcdClear() {
  lcd.clear();
}

void showSnifferStats() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Packets:");
  lcd.print(getPacketCount());

  lcd.setCursor(0, 1);
  lcd.print("Last:");
  lcd.print((millis() - getLastPacketTime()) / 1000);
  lcd.print("s ago");
}