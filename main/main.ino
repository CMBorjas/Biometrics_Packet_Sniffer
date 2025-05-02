#include "biometric.h"
#include "packet_sniffer.h"
#include "lcd_ui.h"
#include "crypto_utils.h"
#include "isr_handlers.h"
#include "nvs_flash.h"


#define SIGNAL_INPUT_PIN 15
#define DEBUG_SERIAL false

bool sniffingActive = false;
bool scanningLocked = false;
unsigned long lastSignalTime = 0;

// External from biometric.ino and packet_sniffer.ino
extern bool checkBiometricAuthorization();
extern void initWiFiPromiscuous();  // NEW: actual sniffer initializer

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);  // Wait for Serial to be ready

  Serial.println("===== ESP32 BOOT =====");
  Serial.println("Initializing peripherals...");

  // UART for fingerprint sensor
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("✅ Serial2 UART Initialized");

  // GPIO from Arduino
  pinMode(SIGNAL_INPUT_PIN, INPUT);

  // LCD setup
  initLCD();
  updateLCDStatus("ESP32 Booting...");

  // NVS init (required for esp_wifi)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }

  updateLCDStatus("Auth Check...");
  if (checkBiometricAuthorization()) {
    updateLCDStatus("Auth: OK, Sniffing Ready");
    Serial.println("🔓 Biometric access granted.");
    initWiFiPromiscuous();  // ✅ ACTUAL PROMISCUOUS MODE INIT
  } else {
    updateLCDStatus("Auth Failed");
    Serial.println("🔒 Biometric access denied.");
  }

  Serial.println("===== SETUP COMPLETE =====");
  lcd.clear();
  lcd.print("Waiting for scanner");
}

void loop() {
  // Allow fingerprint command only if unlocked
  if (!scanningLocked) {
    listenForFingerprintCommand();
  }

  // Check for GPIO pin toggle from Arduino
  handleSignalInput();

  delay(100);
}

void handleSignalInput() {
  int signal = digitalRead(SIGNAL_INPUT_PIN);

  if (signal == HIGH && millis() - lastSignalTime > 1000) {
    lastSignalTime = millis();  // Debounce

    if (!sniffingActive) {
      Serial.println("🔓 START signal received (GPIO)");
      updateLCDStatus("Sniffing: ON");
      sniffingActive = true;
      scanningLocked = true;
      // Promiscuous sniffing is already started in setup
    } else {
      Serial.println("🛑 STOP signal received (GPIO)");
      updateLCDStatus("Sniffing: OFF");
      sniffingActive = false;
      scanningLocked = false;
      // You could disable sniffer here if desired
    }
  }
}
