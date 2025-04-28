#include "lcd_ui.h"
#include "packet_sniffer.h" // We will trigger sniffer later

void listenForFingerprintCommand() {
  if (Serial2.available()) {
    String incoming = Serial2.readStringUntil('\n');
    incoming.trim();

    Serial.print("📨 UART Received: ");
    Serial.println(incoming);

    if (incoming == "🎉 Match found! ID #1") {
      updateLCDStatus("Fingerprint OK!");
      Serial.println("✅ Match: Starting next phase...");
      startSniffing(); // We can call this to start sniffing
    }
    else if (incoming == "TIMEOUT") {
      updateLCDStatus("Timeout Occurred");
      Serial.println("⚠️ Timeout received");
    }
    else {
      updateLCDStatus("Unknown Input");
      Serial.println("❓ Unknown UART input");
    }
  }
}
