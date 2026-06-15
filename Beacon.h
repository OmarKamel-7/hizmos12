#ifndef BEACON_H
#define BEACON_H

#include <Arduino.h>
#include <WiFi.h>
extern "C" {
  #include "esp_wifi.h"
}

class BeaconManager {
public:
    BeaconManager();
    void begin(uint8_t channel = 6);
    void sendSingleBeacon(const char* ssid, const uint8_t* mac);
    void flood(int num_ssids, uint32_t delay_us = 100);
    void setChannel(uint8_t channel);
    
private:
    typedef struct {
        uint8_t frame_control[2];
        uint8_t duration[2];
        uint8_t destination[6];
        uint8_t source[6];
        uint8_t bssid[6];
        uint8_t seq_ctrl[2];
        uint8_t timestamp[8];
        uint8_t beacon_interval[2];
        uint8_t capability_info[2];
        uint8_t ssid_param[2];
        char ssid[32];
        uint8_t rates_tag[3];
    } __attribute__((packed)) wifi_beacon_packet_t;

    wifi_beacon_packet_t _packet;
    void _setupPacket(const char* ssid, const uint8_t* mac);
};

#endif