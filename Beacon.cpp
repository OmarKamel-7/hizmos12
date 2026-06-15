#include "Beacon.h"

BeaconManager::BeaconManager() {}

void BeaconManager::begin(uint8_t channel) {
    WiFi.mode(WIFI_MODE_AP);
    setChannel(channel);
}

void BeaconManager::setChannel(uint8_t channel) {
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

void BeaconManager::_setupPacket(const char* ssid, const uint8_t* mac) {
    memset(&_packet, 0, sizeof(_packet));
    _packet.frame_control[0] = 0x80;
    _packet.frame_control[1] = 0x00;
    memset(_packet.destination, 0xFF, 6);
    memcpy(_packet.source, mac, 6);
    memcpy(_packet.bssid, mac, 6);
    _packet.ssid_param[0] = 0x00;
    _packet.ssid_param[1] = strlen(ssid);
    memcpy(_packet.ssid, ssid, strlen(ssid));
    _packet.rates_tag[0] = 0x01;
    _packet.rates_tag[1] = 0x01;
    _packet.rates_tag[2] = 0x82;
}

void BeaconManager::sendSingleBeacon(const char* ssid, const uint8_t* mac) {
    _setupPacket(ssid, mac);
    esp_wifi_80211_tx(WIFI_IF_AP, &_packet, sizeof(_packet), false);
}

void BeaconManager::flood(int num_ssids, uint32_t delay_us) {
    for (int i = 0; i < num_ssids; i++) {
        char ssid[32];
        snprintf(ssid, sizeof(ssid), "freewifi_%03d_%04X", i, random(0xFFFF));
        uint8_t mac[6];
        for (int j = 0; j < 6; j++) mac[j] = random(256);
        
        sendSingleBeacon(ssid, mac);
        delayMicroseconds(delay_us);
    }
}