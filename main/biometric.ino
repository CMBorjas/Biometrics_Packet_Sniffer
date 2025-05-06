#include "lcd_ui.h"
#include "packet_sniffer.h"

extern bool scanningLocked;
extern void handleFallbackCommand(const String& cmd);

// Called during `setup()` to verify biometric access before allowing sniffer startup
bool checkBiometricAuthorization() {
  updateLCDStatus("Scan Finger (30s)...");

  Serial.println("🧬 Waiting for fingerprint match (Serial2)...");

  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {  // 10-second timeout
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
    else if (incoming == "FALLBACK") {
      Serial.println("🔓 FALLBACK override received");
      handleFallbackCommand("FALLBACK");
    }
  }
}

void logUnauthorizedAccess() {
  String timestamp = getTimeStamp();  // You can replace this with your time method
  String log = timestamp + " - Unauthorized fingerprint attempt\n";

  // Example for Serial log fallback
  Serial.print(log);

  // Optional: save to SD or EEPROM
  // File logFile = SD.open("access_log.txt", FILE_WRITE);
  // if (logFile) {
  //   logFile.print(log);
  //   logFile.close();
  // }
}

// Timestamp generator (fallback using millis)
String getTimeStamp() {
  unsigned long ms = millis();
  unsigned long seconds = ms / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  seconds %= 60;
  minutes %= 60;

  char buffer[20];
  snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return String(buffer);
}
