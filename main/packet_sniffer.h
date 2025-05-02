#ifndef PACKET_SNIFFER_H
#define PACKET_SNIFFER_H

#include <esp_wifi.h>
#include <esp_wifi_types.h>

void initWiFiSniffer();
void startSniffing();
void stopSniffing();
void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);

#endif
