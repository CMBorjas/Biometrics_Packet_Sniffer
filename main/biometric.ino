#include "packet_sniffer.h"

void listenForFingerprintCommand() {
  if (Serial2.available()) {
    String incoming = Serial2.readStringUntil('\n');
    incoming.trim(); // Remove any trailing \r or spaces

    if (incoming == "MATCH") {
      Serial.println("✅ Fingerprint MATCH received!");
      startSniffing();   // Start WiFi packet sniffing
    }
    else if (incoming == "TIMEOUT") {
      Serial.println("⚠️ Timeout received. No action taken.");
    }
  }
}
