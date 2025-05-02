#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"

// ==========================
// Pin and State Definitions
// ==========================
#define LED_PIN 2  // Built-in LED on most ESP32 boards

bool biometricAuthorized = false;
bool snifferActive = false;

// ==========================
// Sniffer Callback Function
// ==========================
void sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (type == WIFI_PKT_MGMT || type == WIFI_PKT_DATA) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    uint8_t *data = pkt->payload;
    int len = pkt->rx_ctrl.sig_len;

    Serial.print("📡 Packet captured: ");
    Serial.print(len);
    Serial.println(" bytes");

    Serial.print("🔍 Source MAC: ");
    for (int i = 10; i < 16; i++) {
      Serial.printf("%02x", data[i]);
      if (i < 15) Serial.print(":");
    }
    Serial.println("\n");
  }
}

// ==========================
// WiFi Sniffer Initialization
// ==========================
void initWiFiPromiscuous() {
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_err_t ret = esp_wifi_init(&cfg);
  if (ret != ESP_OK) {
    Serial.println("❌ WiFi init failed");
    return;
  }

  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);

  wifi_promiscuous_filter_t filter = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_ALL
  };

  esp_wifi_set_promiscuous_filter(&filter);

  esp_wifi_set_promiscuous_rx_cb(&sniffer_callback);
  esp_wifi_set_promiscuous(true);

  Serial.println("✅ ESP32 is now in promiscuous mode");
}

// ==========================
// Simulated Biometric Auth
// ==========================
bool checkBiometricAuthorization() {
  Serial.println("🧬 Waiting for biometric authentication...");
  delay(2000);  // Simulated fingerprint scan delay
  return true;  // Set to false to simulate denial
}

// ==========================
// Arduino Setup Function
// ==========================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize NVS — required for WiFi
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }

  Serial.println("🚀 System Initialization...");
  biometricAuthorized = checkBiometricAuthorization();

  if (biometricAuthorized) {
    snifferActive = true;
    Serial.println("🔓 Biometric access granted. Sniffer enabled.");
    digitalWrite(LED_PIN, HIGH);
    initWiFiPromiscuous();
  } else {
    snifferActive = false;
    Serial.println("🔒 Biometric access denied. Sniffer disabled.");
    digitalWrite(LED_PIN, LOW);
  }
}

// ==========================
// Arduino Loop Function
// ==========================
void loop() {
  delay(1000);  // Passive loop — sniffer is interrupt-driven
}
