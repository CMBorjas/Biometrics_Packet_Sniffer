#include "biometric.h"
#include "packet_sniffer.h"
#include "lcd_ui.h"
#include "crypto_utils.h"
#include "isr_handlers.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("ESP32 Setup Starting...");

  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Serial2 UART Initialized (listening to Arduino)");

  initLCD();
  initWiFiSniffer();

  Serial.println("WiFi Initialization Complete");
  Serial.println("ESP32 Ready!");
}

void loop() {
  listenForFingerprintCommand();

  delay(100);
}
