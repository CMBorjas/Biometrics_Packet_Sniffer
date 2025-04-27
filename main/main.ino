#include "biometric.h"         // Handles fingerprint UART reception
#include "packet_sniffer.h"     // Handles Wi-Fi packet sniffing
#include "lcd_ui.h"             // LCD Display handling (optional if ready)
#include "crypto_utils.h"       // AES encryption utilities (future phase)
#include "isr_handlers.h"       // ISR routines if needed (future)

void setup() {
  // Serial for debugging
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial to be ready
  }
  Serial.println("ESP32 Setup Starting...");

  // Serial2 for UART communication from Arduino
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (adjust GPIOs if needed)
  Serial.println("Serial2 UART Initialized (listening to Arduino)");

  // Initialize LCD Display (optional for later)
  initLCD(); 

  // Initialize WiFi (and/or set WiFi sniffer mode)
  initWiFiSniffer(); 

  Serial.println("WiFi Initialization Complete");
  Serial.println("ESP32 Ready!");
}

void loop() {
  listenForFingerprintCommand(); // From biometric.ino

  // Future hooks:
  // handleSnifferEvents();      // From packet_sniffer.ino, if needed
  // updateLCDStatus();          // From lcd_ui.ino if live updates are added

  delay(100); // Small delay to avoid CPU overrun
}
