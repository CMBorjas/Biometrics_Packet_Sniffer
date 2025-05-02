#include "lcd_ui.h"
#include "packet_sniffer.h"

extern bool scanningLocked;

// Called during `setup()` to verify biometric access before allowing sniffer startup
bool checkBiometricAuthorization() {
  updateLCDStatus("Scan Finger...");
  Serial.println("🧬 Waiting for fingerprint match (Serial2)...");

  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {  // 10-second timeout
    if (Serial2.available()) {
      String incoming = Serial2.readStringUntil('\n');
      incoming.trim();

      if (incoming == "MATCH") {
        Serial.println("✅ Fingerprint matched (setup)");
        updateLCDStatus("Fingerprint OK!");
        return true;
      }
    }
    delay(100);
  }

  Serial.println("❌ Fingerprint timeout (setup)");
  updateLCDStatus("Fingerprint Timeout");
  return false;
}

// Called during loop() to watch for fingerprint-triggered events
void listenForFingerprintCommand() {
  if (scanningLocked) return;

  if (Serial2.available()) {
    String incoming = Serial2.readStringUntil('\n');
    incoming.trim();

    if (incoming == "MATCH") {
      Serial.println("✅ MATCH received from Arduino");
      updateLCDStatus("Fingerprint OK!");
      startSniffing();
      scanningLocked = true;
    }
    else if (incoming == "STOP") {
      Serial.println("🛑 STOP received from Arduino");
      updateLCDStatus("Stopping Scan");
      stopSniffing();
      scanningLocked = false;
    }
  }
}
