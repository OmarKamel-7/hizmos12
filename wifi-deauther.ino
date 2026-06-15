// ─────────────────────────────────────────────────────────────────────────────
// scanningwifi.ino
// Requires: deauth.h / deauth.cpp, u8g2 library, buttons defined elsewhere
// foundNetworks[] and foundCount come from deauth.cpp via deauth.h
// ─────────────────────────────────────────────────────────────────────────────

bool is_attacking = false;

// Returns human-readable encryption type string
String wifi_encryptionType(wifi_auth_mode_t enc) {
  switch (enc) {
    case WIFI_AUTH_OPEN:         return "Open";
    case WIFI_AUTH_WEP:          return "WEP";
    case WIFI_AUTH_WPA_PSK:      return "WPA";
    case WIFI_AUTH_WPA2_PSK:     return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA3_PSK:     return "WPA3";
    default:                     return "Unknown";
  }
}

void scanningwifi() {

  // ── Nothing scanned yet ────────────────────────────────────────────────────
  if (foundCount <= 0) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x13B_tr);
    u8g2.drawStr(15, 32, "No Networks Found");
    u8g2.sendBuffer();
    return;
  }

  // ── Non-blocking button debounce ───────────────────────────────────────────
  static unsigned long lastDebounceTime = 0;
  unsigned long now = millis();
  const unsigned long DEBOUNCE      = 150;
  const unsigned long BACK_DEBOUNCE = 250;

  if (now - lastDebounceTime > DEBOUNCE) {

    // Scroll down
    if (digitalRead(BTN_DOWN) == LOW) {
      wifi_selectedIndex = (wifi_selectedIndex + 1) % foundCount;
      lastDebounceTime = now;
    }

    // Scroll up
    if (digitalRead(BTN_UP) == LOW) {
      wifi_selectedIndex = (wifi_selectedIndex - 1 + foundCount) % foundCount;
      lastDebounceTime = now;
    }

    // Show detail info page
    if (digitalRead(BTN_RIGHT) == LOW) {
      wifi_showInfo    = true;
      lastDebounceTime = now;
    }

    // Toggle attack on selected network
    if (digitalRead(BTN_SELECT) == LOW) {
      is_attacking = !is_attacking;
      if (is_attacking)
        start_deauth(wifi_selectedIndex, DEAUTH_TYPE_SINGLE, 1);
      else
        stop_deauth();
      lastDebounceTime = now;
    }

    // Back — longer debounce
    if (digitalRead(BTN_BACK) == LOW && (now - lastDebounceTime > BACK_DEBOUNCE)) {
      if (wifi_showInfo) {
        wifi_showInfo = false;
      } else if (is_attacking) {
        stop_deauth();
        is_attacking = false;
      }
      lastDebounceTime = now;
    }
  }

  // ── Draw ───────────────────────────────────────────────────────────────────
  u8g2.clearBuffer();

  if (!wifi_showInfo) {

    // ── Network list ─────────────────────────────────────────────────────────
    u8g2.setFont(u8g2_font_6x13B_tr);

    int start = max(0, wifi_selectedIndex - 2);
    int end   = min(start + 4, foundCount);

    for (int i = start; i < end; i++) {

      int    y    = (i - start) * 16 + 2;
      String ssid = foundNetworks[i].ssid;     // ← cached, always valid
      if (ssid.length() > 18)
        ssid = ssid.substring(0, 18) + "...";

      if (i == wifi_selectedIndex) {
        if (is_attacking) {
          // Hollow box while attacking (inverted look)
          u8g2.setDrawColor(0);
          u8g2.drawRBox(0, y, 128, 16, 4);
          u8g2.setDrawColor(1);
        } else {
          // Filled box for selected
          u8g2.drawRBox(0, y, 128, 16, 4);
          u8g2.setDrawColor(0);
        }
        u8g2.drawStr(4, y + 12, ssid.c_str());
        u8g2.setDrawColor(1);
      } else {
        u8g2.drawStr(4, y + 12, ssid.c_str());
      }
    }

    // Status bar at bottom
    u8g2.drawFrame(0, 54, 128, 10);
    u8g2.drawStr(2, 62, is_attacking ? "ATTACK RUNNING" : "SELECT=START");

  } else {

    // ── Detail info page ─────────────────────────────────────────────────────
    u8g2.setFont(u8g2_font_5x8_tr);

    NetworkInfo &net = foundNetworks[wifi_selectedIndex];   // ← cached struct

    // SSID
    u8g2.drawFrame(0, 0, 128, 13);
    u8g2.drawStr(2, 9, ("SSID: " + net.ssid).c_str());

    // RSSI
    u8g2.drawFrame(0, 14, 128, 13);
    u8g2.drawStr(2, 23, ("RSSI: " + String(net.rssi) + " dBm").c_str());

    // MAC / BSSID
    u8g2.drawFrame(0, 28, 128, 13);
    u8g2.drawStr(2, 37, ("MAC: " + net.bssid).c_str());

    // Encryption — still read from scan buffer (index still valid after scan)
    u8g2.drawFrame(0, 42, 128, 13);
    u8g2.drawStr(2, 51,
      ("ENC: " + wifi_encryptionType(
        WiFi.encryptionType(wifi_selectedIndex))).c_str());

    // Kick count while attacking
    u8g2.drawFrame(0, 56, 128, 8);
    if (is_attacking)
      u8g2.drawStr(2, 63, ("KICKS: " + String(get_kick_count())).c_str());
    else
      u8g2.drawStr(2, 63, "BACK=RETURN");
  }

  u8g2.sendBuffer();
}
