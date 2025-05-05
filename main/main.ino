#include "biometric.h"
#include "packet_sniffer.h"
#include "lcd_ui.h"
#include "crypto_utils.h"
#include "isr_handlers.h"

#include <WiFi.h>
#include "esp_wifi.h"

#define SIGNAL_INPUT_PIN 15
#define DEBUG_SERIAL false
#define TIMEOUT_MS 300000  // 5 minutes

bool sniffingActive = false;
bool scanningLocked = false;
bool biometricTimedOut = false;

unsigned long lastSignalTime = 0;
unsigned long lastAuthAttempt = 0;

// 🔐 Forward declaration to prevent compilation error
void handleFallbackCommand(const String& cmd);

extern bool checkBiometricAuthorization();
extern void initWiFiPromiscuous();
extern void startSniffing();
extern void stopSniffing();

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("===== ESP32 BOOT =====");
  Serial2.begin(9600, SERIAL_8N1, 16, 17);  // RX, TX from Arduino
  pinMode(SIGNAL_INPUT_PIN, INPUT);

  initLCD();
  updateLCDStatus("ESP32 Booting...");

  initWiFiPromiscuous();  // ✅ Initialize Wi-Fi, but DO NOT start promiscuous mode yet
  Serial.println("✅ WiFi Sniffer Ready");

  updateLCDStatus("ESP32 Ready!");
  lcd.clear();
  lcd.print("Waiting for scanner");

  lastAuthAttempt = millis();  // Start timeout timer
}

void loop() {
  // 🔄 Check for fallback reset over Serial2
  if (Serial2.available()) {
  String cmd = Serial2.readStringUntil('\n');
  cmd.trim();  // Remove newline/space

    if (cmd.equalsIgnoreCase("MATCH")) {
      if (!sniffingActive && !biometricTimedOut) {
        Serial.println("🔓 MATCH received from Arduino");
        lcd.print("Sniffing: ON");
        startSniffing();
        sniffingActive = true;
        scanningLocked = true;
      }
    } else if (cmd.equalsIgnoreCase("STOP")) {
      if (sniffingActive) {
        Serial.println("🛑 STOP received from Arduino");
        updateLCDStatus("Sniffing: OFF");
        stopSniffing();
        sniffingActive = false;
        scanningLocked = false;
        lastAuthAttempt = millis();  // Reset timeout
      }
    } else {
      handleFallbackCommand(cmd);
    }
  }


  if (!scanningLocked && !biometricTimedOut) {
    listenForFingerprintCommand();  // Start on fingerprint match
  }

  handleSignalInput();      // Check GPIO trigger (from Arduino)
  checkBiometricTimeout();  // Timeout lock if inactive too long

  delay(100);  // Prevent watchdog reset
}

// 🕒 Lock out if inactive for too long
void checkBiometricTimeout() {
  if (!scanningLocked && !biometricTimedOut) {
    if (millis() - lastAuthAttempt > TIMEOUT_MS) {
      biometricTimedOut = true;
      updateLCDStatus("⚠️ Timeout Reached");
      Serial.println("❌ TIMEOUT: No auth in 5 min");
      Serial2.println("TIMEOUT");
    }
  }
}

// 🧠 Handle START/STOP signals from Arduino
void handleSignalInput() {
  static bool prevSignalState = LOW;
  int signal = digitalRead(SIGNAL_INPUT_PIN);

  if (signal == HIGH && prevSignalState == LOW && millis() - lastSignalTime > 1000) {
    lastSignalTime = millis();
    prevSignalState = HIGH;

    if (!sniffingActive && !biometricTimedOut) {
      Serial.println("🔓 START signal received (GPIO)");
      lcd.print("Sniffing: ON");
      startSniffing();  // ✅ Enable promiscuous mode here
      sniffingActive = true;
      scanningLocked = true;
    } else if (sniffingActive) {
      Serial.println("🛑 STOP signal received (GPIO)");
      updateLCDStatus("Sniffing: OFF");
      stopSniffing();  // ✅ Disable promiscuous mode
      sniffingActive = false;
      scanningLocked = false;
      lastAuthAttempt = millis();  // Reset timeout timer
    }
  }

  if (signal == LOW && prevSignalState == HIGH) {
    prevSignalState = LOW;
  }
}

// 🔁 External unlock command (e.g., "FALLBACK")
void handleFallbackCommand(const String& cmd) {
  if (cmd.equalsIgnoreCase("FALLBACK")) {
    biometricTimedOut = false;
    scanningLocked = false;
    updateLCDStatus("Fallback Unlocked");
    Serial.println("🔓 FALLBACK command accepted");
    lastAuthAttempt = millis();
  }
}
