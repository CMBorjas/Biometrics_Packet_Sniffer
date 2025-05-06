#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "packet_sniffer.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "crypto_utils.h"


unsigned long packetCount = 0;
unsigned long lastPacketTime = 0;

// ==========================
// Helper: Filter by protocol ports
// ==========================
bool isInterestingPacket(const uint8_t *data, int len) {
  // Skip 802.11 header (typically 24 or 30 bytes)
  const int IP_HEADER_OFFSET = 30;
  if (len < IP_HEADER_OFFSET + 20) return false;

  uint8_t ipProto = data[IP_HEADER_OFFSET + 9];
  uint16_t srcPort = (data[IP_HEADER_OFFSET + 20] << 8) | data[IP_HEADER_OFFSET + 21];
  uint16_t dstPort = (data[IP_HEADER_OFFSET + 22] << 8) | data[IP_HEADER_OFFSET + 23];

  // Match common protocols
  return (
    (ipProto == 6 && (srcPort == 80 || dstPort == 80)) ||   // HTTP
    (ipProto == 6 && (srcPort == 21 || dstPort == 21)) ||   // FTP
    (ipProto == 6 && (srcPort == 20 || dstPort == 20)) ||   // FTP Data
    (ipProto == 17 && (srcPort == 53 || dstPort == 53))     // DNS
  );
}

// ==========================
// Sniffer Callback
// ==========================
void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (type == WIFI_PKT_MGMT || type == WIFI_PKT_DATA) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    uint8_t *data = pkt->payload;
    int len = pkt->rx_ctrl.sig_len;

    packetCount++;
    lastPacketTime = millis();

    Serial.print("📡 Packet captured: ");
    Serial.print(len);
    Serial.println(" bytes");

    Serial.print("🔍 Source MAC: ");
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
      data[10], data[11], data[12], data[13], data[14], data[15]);
    Serial.println(macStr);

    // Build log entry
    char logMsg[64];
    snprintf(logMsg, sizeof(logMsg), "[%lu] MAC: %s\n", millis(), macStr);
    Serial.print("🔐 Encrypting log: "); Serial.print(logMsg);

    // Optional: DEBUGGING to test encryption works
    char encrypted[64];
    encryptToHex(logMsg, encrypted, sizeof(encrypted));
    Serial.print("🧾 Encrypted (hex): "); Serial.println(encrypted);
  }
}

// ==========================
// Initializer (called during setup only)
// ==========================
void initWiFiPromiscuous() {
  if (nvs_flash_init() != ESP_OK) {
    Serial.println("❌ NVS init failed");
    return;
  }

  if (esp_event_loop_create_default() != ESP_OK) {
    Serial.println("❌ Event loop creation failed");
    return;
  }

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  if (esp_wifi_init(&cfg) != ESP_OK) {
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

  Serial.println("[Sniffer] Initialized — not sniffing yet");
}

// ==========================
// Runtime Sniff Control
// ==========================
void startSniffing() {
  if (sniffingActive) return;
  WiFi.mode(WIFI_MODE_NULL);
  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_promiscuous_rx_cb(&snifferCallback);
  esp_wifi_set_promiscuous(true);
  sniffingActive = true;
  lastActivityTime = millis();
  Serial.println("[Sniffer] Started Wi-Fi Sniffing");
}

void stopSniffing() {
  esp_wifi_set_promiscuous(false);
  Serial.println("[Sniffer] Stopped Wi-Fi Sniffing");
}

void initWiFiSniffer() {
  WiFi.mode(WIFI_MODE_NULL);
  esp_wifi_set_promiscuous(false);
  Serial.println("[Sniffer] WiFi Sniffer (stub) ready");
}

unsigned long getPacketCount() {
  return packetCount;
}

unsigned long getLastPacketTime() {
  return lastPacketTime;
}
