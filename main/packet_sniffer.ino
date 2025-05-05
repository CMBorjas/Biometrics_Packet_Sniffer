#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "packet_sniffer.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"

// ==========================
// Sniffer Callback
// ==========================
void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
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
// Initializer (called during setup only)
// ==========================
void initWiFiPromiscuous() {
  // REQUIRED by ESP32 Wi-Fi stack
  if (nvs_flash_init() != ESP_OK) {
    Serial.println("❌ NVS init failed");
    return;
  }

  if (esp_event_loop_create_default() != ESP_OK) {
    Serial.println("❌ Event loop creation failed");
    return;
  }

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_err_t ret = esp_wifi_init(&cfg);
  if (ret != ESP_OK) {
    Serial.println("❌ WiFi init failed");
    return;
  }

  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);  // Passive mode
  esp_wifi_start();

  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);  // Pick your sniffing channel

  wifi_promiscuous_filter_t filter = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_ALL
  };
  esp_wifi_set_promiscuous_filter(&filter);

  Serial.println("[Sniffer] Initialized — not sniffing yet");
}

// ==========================
// Runtime Sniff Control
// ==========================
void startSniffing() {
  WiFi.mode(WIFI_MODE_NULL);      // Ensure no active Wi-Fi connection
  WiFi.mode(WIFI_MODE_STA);       // Enable passive sniffing
  esp_wifi_set_promiscuous_rx_cb(&snifferCallback);  //  set callback
  esp_wifi_set_promiscuous(true);                    // Enable sniffing

  Serial.println("[Sniffer] Started Wi-Fi Sniffing");
}

void stopSniffing() {
  esp_wifi_set_promiscuous(false);
  Serial.println("[Sniffer] Stopped Wi-Fi Sniffing");
}

// ==========================
// Optional Compatibility Stub
// ==========================
void initWiFiSniffer() {
  WiFi.mode(WIFI_MODE_NULL);
  esp_wifi_set_promiscuous(false);
  Serial.println("[Sniffer] WiFi Sniffer (stub) ready");
}
