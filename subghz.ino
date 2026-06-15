
void drawHistogram() {
  remote_loop();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tr);

  char hdr[30];
  sprintf(hdr, "HIST %.2fMHz", currentFreq);
  u8g2.drawStr(0, 7, hdr);

  float span = 1.0;
  float startF = currentFreq - span / 2.0;
  float step = span / 64.0;

  int x = 0;
  for (int i = 0; i < 64; i++) {
    float f = startF + i * step;
    ELECHOUSE_cc1101.setMHZ(f);
    ELECHOUSE_cc1101.SetRx();
    safeDelay(5);

    int rssi = ELECHOUSE_cc1101.getRssi();
    int barH = map(rssi, -110, -30, 0, 54);
    if (barH < 0) barH = 0;
    if (barH > 54) barH = 54;

    u8g2.drawBox(x, 63 - barH, 2, barH);
    x += 2;
  }

  ELECHOUSE_cc1101.setMHZ(currentFreq);
  ELECHOUSE_cc1101.SetRx();
  u8g2.sendBuffer();
  remote_sync();
  if (isButtonPressed(BTN_BACK)) {
    mode = -1;
  }
}








//////////////////////////////////////
//////////////////////////////////
void SPECTRUMANALYZER() {
    static const int NUM_RECENT_FREQS      = 10;
    static const int SIGNAL_HOLD_TIME      = 3000;
    static const int STRONG_SIGNAL_THRESHOLD = -60;
    static const int WEAK_SIGNAL_THRESHOLD   = -85;
    static const int HOP_SPEED             = 200;
    static const int MONITOR_INTERVAL      = 100;  // ms between locked RSSI polls
    static const int LOCK_CONFIRM_SAMPLES  = 3;    // consecutive hits before locking

    static float freq_history[10]    = {0};
    static int   rssi_history[10]    = {-120,-120,-120,-120,-120,-120,-120,-120,-120,-120};
    static unsigned long signalFoundTime = 0;
    static bool  signalLocked        = false;
    static float freq_active         = 0.0;
    static int   rssi_active         = -120;
    static float locked_freq         = 0.0;
    static int   locked_rssi         = -120;
    static int   confirmCount        = 0;   // FIX: debounce lock acquisition

    static unsigned long lastHopTime     = 0;
    static unsigned long lastMonitorTime = 0;  // FIX: proper locked-monitoring timer
    static int currentHopIndex           = 0;

    static const float POPULAR_FREQS[] = {
        433.92, 315.0, 868.0, 433.0, 434.0,
        915.0,  390.0, 303.875, 418.0, 310.0, 330.0, 345.0
    };
    static const int NUM_POPULAR_FREQS = 12;

    // --- icons (unchanged) ---
    static const unsigned char image_arrow_right_bits[]   U8X8_PROGMEM = {0x10,0x20,0x7f,0x20,0x10};
    static const unsigned char image_network_1_bar_bits[] U8X8_PROGMEM = {0x00,0x70,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x57,0x00,0x55,0x00,0x55,0x00,0x55,0x70,0x55,0x50,0x55,0x50,0x55,0x50,0x55,0x57,0x55,0x57,0x55,0x77,0x77,0x00,0x00};
    static const unsigned char image_network_1_bits[]     U8X8_PROGMEM = {0x00,0x70,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x57,0x00,0x55,0x00,0x55,0x00,0x55,0x70,0x55,0x50,0x55,0x50,0x55,0x50,0x55,0x57,0x55,0x55,0x55,0x77,0x77,0x00,0x00};
    static const unsigned char image_network_2_bars_bits[]U8X8_PROGMEM = {0x00,0x70,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x57,0x00,0x55,0x00,0x55,0x00,0x55,0x70,0x55,0x70,0x55,0x70,0x55,0x70,0x55,0x77,0x55,0x77,0x55,0x77,0x77,0x00,0x00};
    static const unsigned char image_network_3_bars_bits[]U8X8_PROGMEM = {0x00,0x70,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x57,0x00,0x57,0x00,0x57,0x00,0x57,0x70,0x57,0x70,0x57,0x70,0x57,0x70,0x57,0x77,0x57,0x77,0x57,0x77,0x77,0x00,0x00};
    static const unsigned char image_network_4_bars_bits[]U8X8_PROGMEM = {0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x77,0x00,0x77,0x00,0x77,0x00,0x77,0x70,0x77,0x70,0x77,0x70,0x77,0x70,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x00,0x00};

    // =========================================================
    // HOPPING MODE
    // =========================================================
    if (!signalLocked && millis() - lastHopTime >= HOP_SPEED) {
        lastHopTime = millis();

        float hopFreq = POPULAR_FREQS[currentHopIndex];
        ELECHOUSE_cc1101.setMHZ(hopFreq);
        ELECHOUSE_cc1101.SetRx();
        // FIX: removed blocking delay(5) — the 200ms hop interval already
        //      gives the radio ample settle time across calls.

        int currentRssi = ELECHOUSE_cc1101.getRssi();

        Serial.print("[HOP] Freq: "); Serial.print(hopFreq, 2);
        Serial.print(" MHz | RSSI: "); Serial.print(currentRssi); Serial.print(" dBm");

        if (currentRssi > STRONG_SIGNAL_THRESHOLD) {
            Serial.println(" << STRONG SIGNAL");

            // Update or insert into history
            bool updated = false;
            for (int i = 0; i < NUM_RECENT_FREQS; i++) {
                if (freq_history[i] > 0 && fabsf(freq_history[i] - hopFreq) < 1.0f) {
                    if (currentRssi > rssi_history[i]) {
                        freq_history[i] = hopFreq;
                        rssi_history[i] = currentRssi;
                    }
                    updated = true;
                    break;
                }
            }
            if (!updated) {
                int weakestIdx = 0;
                for (int i = 1; i < NUM_RECENT_FREQS; i++) {
                    if (rssi_history[i] < rssi_history[weakestIdx]) weakestIdx = i;
                }
                if (currentRssi > rssi_history[weakestIdx]) {
                    freq_history[weakestIdx] = hopFreq;
                    rssi_history[weakestIdx] = currentRssi;
                }
            }
        } else {
            Serial.println();
            confirmCount = 0;  // reset debounce on any weak hop
        }

        currentHopIndex++;
        if (currentHopIndex >= NUM_POPULAR_FREQS) {
            currentHopIndex = 0;
            Serial.println("========== HOP CYCLE COMPLETE ==========");
        }
    }

    // =========================================================
    // FIND BEST CANDIDATE (used in both modes)
    // =========================================================
    int maxRssiIdx = -1;
    int maxRssi    = STRONG_SIGNAL_THRESHOLD;  // FIX: use threshold, not -120
    for (int i = 0; i < NUM_RECENT_FREQS; i++) {
        if (freq_history[i] > 0 && rssi_history[i] > maxRssi) {
            maxRssi    = rssi_history[i];
            maxRssiIdx = i;
        }
    }

    // =========================================================
    // STATE TRANSITIONS
    // =========================================================
    if (!signalLocked) {
        if (maxRssiIdx >= 0) {
            // FIX: require LOCK_CONFIRM_SAMPLES consecutive strong readings
            confirmCount++;
            if (confirmCount >= LOCK_CONFIRM_SAMPLES) {
                signalLocked    = true;
                signalFoundTime = millis();
                locked_freq     = freq_history[maxRssiIdx];
                locked_rssi     = maxRssi;
                confirmCount    = 0;

                Serial.println();
                Serial.println("╔════════════════════════════════════╗");
                Serial.print("║ SIGNAL LOCKED: "); Serial.print(locked_freq, 2); Serial.println(" MHz      ║");
                Serial.print("║ RSSI: "); Serial.print(locked_rssi); Serial.println(" dBm                    ║");
                Serial.println("╚════════════════════════════════════╝");

                ELECHOUSE_cc1101.setMHZ(locked_freq);
                ELECHOUSE_cc1101.SetRx();
            }
        } else {
            confirmCount = 0;
        }
        freq_active  = (maxRssiIdx >= 0) ? freq_history[maxRssiIdx] : 0.0f;
        rssi_active  = (maxRssiIdx >= 0) ? rssi_history[maxRssiIdx] : -120;

    } else {
        // FIX: non-blocking monitor using a proper timestamp
        if (millis() - lastMonitorTime >= MONITOR_INTERVAL) {
            lastMonitorTime = millis();
            int currentRssi = ELECHOUSE_cc1101.getRssi();

            Serial.print("[LOCKED] "); Serial.print(locked_freq, 2);
            Serial.print(" MHz | RSSI: "); Serial.print(currentRssi); Serial.println(" dBm");

            if (currentRssi > WEAK_SIGNAL_THRESHOLD) {
                locked_rssi     = currentRssi;
                signalFoundTime = millis();
            }
        }

        bool timedOut   = (millis() - signalFoundTime > SIGNAL_HOLD_TIME);
        bool signalGone = (locked_rssi < WEAK_SIGNAL_THRESHOLD);

        if (timedOut || signalGone) {
            signalLocked = false;
            // FIX: do NOT wipe freq_history — keep good candidates for fast re-lock
            freq_active  = 0.0f;
            rssi_active  = -120;
            locked_freq  = 0.0f;
            locked_rssi  = -120;
            confirmCount = 0;

            Serial.println();
            Serial.println("▼▼▼ SIGNAL LOST - UNLOCKING ▼▼▼");
        } else {
            freq_active = locked_freq;
            rssi_active = locked_rssi;
        }
    }

    // =========================================================
    // DISPLAY
    // =========================================================
    int network_level = 0;
    if      (rssi_active > -40) network_level = 4;
    else if (rssi_active > -55) network_level = 3;
    else if (rssi_active > -70) network_level = 2;
    else if (rssi_active > -85) network_level = 1;

    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    u8g2.setFont(u8g2_font_4x6_tr);
    if (signalLocked) {
        u8g2.drawStr(20, 8, "frequency analyzer [LOCK]");
    } else {
        char hopMsg[30];
        sprintf(hopMsg, "freq analyzer [HOP:%d/%d]", currentHopIndex + 1, NUM_POPULAR_FREQS);
        u8g2.drawStr(10, 8, hopMsg);
    }

    u8g2.setFont(u8g2_font_profont17_tr);
    char freqStr[16];
    if (freq_active > 0.1f) {
        int freq_int = (int)freq_active;
        int freq_dec = (int)((freq_active - freq_int) * 1000);
        sprintf(freqStr, "%03d.%03d", freq_int, freq_dec);
    } else {
        strcpy(freqStr, "000.000");
    }
    u8g2.drawStr(19, 25, freqStr);

    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.drawStr(90, 24, "MHZ");

    u8g2.setFont(u8g2_font_profont10_tr);
    char rssiStr[10];
    if (freq_active > 0.1f) sprintf(rssiStr, "RS:%d", rssi_active);
    else                     strcpy(rssiStr, "RS:---");
    u8g2.drawStr(4, 58, rssiStr);

    // Signal strength icon
    if      (network_level == 4) u8g2.drawXBMP(25, 42, 15, 16, image_network_4_bars_bits);
    else if (network_level == 3) u8g2.drawXBMP(25, 41, 15, 16, image_network_3_bars_bits);
    else if (network_level == 2) u8g2.drawXBMP(25, 40, 15, 16, image_network_2_bars_bits);
    else if (network_level == 1) u8g2.drawXBMP(25, 42, 15, 16, image_network_1_bar_bits);
    else                         u8g2.drawXBMP(25, 42, 15, 16, image_network_1_bits);

    u8g2.drawXBMP(49, 34, 7, 5, image_arrow_right_bits);
    u8g2.drawXBMP(49, 45, 7, 5, image_arrow_right_bits);
    u8g2.drawXBMP(49, 56, 7, 5, image_arrow_right_bits);

    // FIX: show recent freqs in both modes (exclude the currently active one)
    u8g2.setFont(u8g2_font_profont10_tr);
    int displayCount = 0;
    for (int i = 0; i < NUM_RECENT_FREQS && displayCount < 3; i++) {
        if (freq_history[i] > 0 && rssi_history[i] > STRONG_SIGNAL_THRESHOLD) {
            // Skip whichever slot is the active/locked frequency
            bool isActive = signalLocked
                ? (fabsf(freq_history[i] - locked_freq) < 1.0f)
                : (i == maxRssiIdx);
            if (isActive) continue;

            char lastFreq[20];
            int f_int = (int)freq_history[i];
            int f_dec = (int)((freq_history[i] - f_int) * 100);
            sprintf(lastFreq, "%d.%02d/%d", f_int, f_dec, rssi_history[i]);
            u8g2.drawStr(60, 39 + (displayCount * 11), lastFreq);
            displayCount++;
        }
    }

    u8g2.sendBuffer();
}


////////////////////////////////////////////


// [BEGIN lopaka generated assets]
static const unsigned char image_ButtonLeft_bits[] U8X8_PROGMEM = {0x08,0x0c,0x0e,0x0f,0x0e,0x0c,0x08};
static const unsigned char image_ButtonRight_bits[] U8X8_PROGMEM = {0x01,0x03,0x07,0x0f,0x07,0x03,0x01};
// [END lopaka generated assets]

void drawSettingsMenu() {
  const char* modNames[] = {"OOK", "2-FSK", "GFSK", "MSK"};
  
  const float presetFreqs[] = {
    300.00, 303.87, 304.25, 310.00, 315.00, 318.00,
    390.00, 418.00, 433.05, 433.42, 433.92, 434.42, 434.77,
    868.35, 868.95, 915.00, 925.00
  };
  const int numPresets = sizeof(presetFreqs) / sizeof(presetFreqs[0]);
  
  const float bwTable[] = {58.0, 68.0, 81.0, 102.0, 135.0, 203.0, 325.0, 406.0, 541.0, 812.0};
  const int numBWs = sizeof(bwTable) / sizeof(bwTable[0]);

  static int freqPresetIndex = -1;
  static bool hoppingEnabled = false;
  static unsigned long lastHopTime = 0;
  static int currentHopIndex = 0;
  const unsigned long HOP_INTERVAL = 100; 
  
  // Non-blocking button input clock
  static unsigned long lastInputTime = 0;
  const unsigned long INPUT_DEBOUNCE = 130; // ms between continuous registration
  
  // --- BACKGROUND PROCESSES ---
  if (hoppingEnabled && millis() - lastHopTime > HOP_INTERVAL) {
    currentHopIndex = (currentHopIndex + 1) % numPresets;
    currentFreq = presetFreqs[currentHopIndex];
    ELECHOUSE_cc1101.setMHZ(currentFreq);
    lastHopTime = millis();
    Serial.printf("HOP [%d/%d]: %.2f MHz\r", currentHopIndex + 1, numPresets, currentFreq);
  }

  // --- RENDERING INTEGRATION ---
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_haxrcorp4089_tr);

  // Static Layout Labels (Lopaka Coordinates)
  u8g2.drawStr(5, 9, "frequency");
  u8g2.drawStr(5, 22, "Hopping");
  u8g2.drawStr(5, 36, "Modulation");
  u8g2.drawStr(4, 48, "RX Band width");
  u8g2.drawStr(4, 60, "TX power");

  // Render Arrow Bitmaps (Lopaka Positions)
  u8g2.drawXBMP(77, 3,  4, 7, image_ButtonLeft_bits);  u8g2.drawXBMP(122, 2,  4, 7, image_ButtonRight_bits);
  u8g2.drawXBMP(77, 16, 4, 7, image_ButtonLeft_bits);  u8g2.drawXBMP(122, 16, 4, 7, image_ButtonRight_bits);
  u8g2.drawXBMP(77, 28, 4, 7, image_ButtonLeft_bits);  u8g2.drawXBMP(122, 28, 4, 7, image_ButtonRight_bits);
  u8g2.drawXBMP(77, 41, 4, 7, image_ButtonLeft_bits);  u8g2.drawXBMP(122, 41, 4, 7, image_ButtonRight_bits);
  u8g2.drawXBMP(77, 53, 4, 7, image_ButtonLeft_bits);  u8g2.drawXBMP(122, 53, 4, 7, image_ButtonRight_bits);

  // --- DYNAMIC VALUES CALCULATION ---
  char valBuf[20];
  int targetCenterY[] = {9, 22, 36, 48, 60};

  for (int i = 0; i < 5; i++) {
    valBuf[0] = '\0';
    switch (i) {
      case 0: // Frequency
        if (freqPresetIndex >= 0) sprintf(valBuf, "%.2f", presetFreqs[freqPresetIndex]);
        else sprintf(valBuf, "%.2f*", currentFreq);
        break;
      case 1: // Hopping
        sprintf(valBuf, "%s", hoppingEnabled ? "ON" : "OFF");
        break;
      case 2: // Modulation
        sprintf(valBuf, "%s", modNames[currentMod]);
        break;
      case 3: // RX BW
        sprintf(valBuf, "%.0fk", rxBW);
        break;
      case 4: // TX Power
        sprintf(valBuf, "%ddB", powerLevel);
        break;
    }
    // Dynamic horizontal centering inside the arrow columns (X: 81 to 122)
    int strW = u8g2.getStrWidth(valBuf);
    int printX = 81 + ((122 - 81) - strW) / 2;
    u8g2.drawStr(printX, targetCenterY[i], valBuf);
  }

  // Modern UI Polish: Draw a smooth, inverted selection box behind active choice
  int boxY[] = {0, 13, 26, 39, 51};
  int boxH[] = {12, 12, 12, 11, 13};
  u8g2.setDrawColor(2); // XOR mode creates perfect text inversion
  u8g2.drawBox(0, boxY[settingsIndex], 128, boxH[settingsIndex]);
  u8g2.setDrawColor(1); // Revert to normal drawing color

  u8g2.sendBuffer();

  // --- SMOOTH NON-BLOCKING CONTROLS ---
  if (millis() - lastInputTime > INPUT_DEBOUNCE) {
    bool inputRegistered = false;

    // Row Selection Navigation
    if (isButtonPressed(BTN_DOWN)) {
      settingsIndex = (settingsIndex + 1) % 5;
      inputRegistered = true;
    }
    if (isButtonPressed(BTN_UP)) {
      settingsIndex = (settingsIndex - 1 + 5) % 5;
      inputRegistered = true;
    }

    // Direct Parameter Manipulation via Left/Right Arrows
    if (isButtonPressed(BTN_RIGHT) || isButtonPressed(BTN_SELECT)) {
      inputRegistered = true;
      switch (settingsIndex) {
        case 0: // Freq Up
          if (!hoppingEnabled) {
            freqPresetIndex++;
            if (freqPresetIndex >= numPresets) freqPresetIndex = -1; // Switch back to custom loop tracking
            if (freqPresetIndex >= 0) currentFreq = presetFreqs[freqPresetIndex];
            else currentFreq = 300.0;
            ELECHOUSE_cc1101.setMHZ(currentFreq);
          }
          break;
        case 1: // Hopping Change
          hoppingEnabled = !hoppingEnabled;
          if (hoppingEnabled) { currentHopIndex = 0; lastHopTime = millis(); }
          break;
        case 2: // Modulation Change
          currentMod = (ModType)((currentMod + 1) % 4);
          if (currentMod == MOD_ASK_OOK) setupOOKMode(); else setupFSKMode();
          break;
        case 3: // RX BW Up
          {
            int bwIdx = 0;
            for (int b = 0; b < numBWs; b++) { if (rxBW == bwTable[b]) { bwIdx = b; break; } }
            bwIdx = (bwIdx + 1) % numBWs;
            rxBW = bwTable[bwIdx];
            ELECHOUSE_cc1101.setRxBW(rxBW);
          }
          break;
        case 4: // TX Power Up
          powerLevel++;
          if (powerLevel > 10) powerLevel = -30;
          ELECHOUSE_cc1101.setPA(powerLevel);
          break;
      }
    }

    if (isButtonPressed(BTN_LEFT)) {
      inputRegistered = true;
      switch (settingsIndex) {
        case 0: // Freq Down
          if (!hoppingEnabled) {
            freqPresetIndex--;
            if (freqPresetIndex < -1) freqPresetIndex = numPresets - 1;
            if (freqPresetIndex >= 0) currentFreq = presetFreqs[freqPresetIndex];
            else currentFreq = 928.0;
            ELECHOUSE_cc1101.setMHZ(currentFreq);
          }
          break;
        case 1: // Hopping Change
          hoppingEnabled = !hoppingEnabled;
          break;
        case 2: // Modulation Change
          currentMod = (ModType)((currentMod - 1 + 4) % 4);
          if (currentMod == MOD_ASK_OOK) setupOOKMode(); else setupFSKMode();
          break;
        case 3: // RX BW Down
          {
            int bwIdx = 0;
            for (int b = 0; b < numBWs; b++) { if (rxBW == bwTable[b]) { bwIdx = b; break; } }
            bwIdx = (bwIdx - 1 + numBWs) % numBWs;
            rxBW = bwTable[bwIdx];
            ELECHOUSE_cc1101.setRxBW(rxBW);
          }
          break;
        case 4: // TX Power Down
          powerLevel--;
          if (powerLevel < -30) powerLevel = 10;
          ELECHOUSE_cc1101.setPA(powerLevel);
          break;
      }
    }

    if (isButtonPressed(BTN_BACK)) {
      if (hoppingEnabled) hoppingEnabled = false;
      mode = -1;
      inputRegistered = true;
    }

    if (inputRegistered) {
      lastInputTime = millis();
      setStatusMsg("Updated!");
    }
  }
}

//////////////////////////////////////////////////////////////////////

void runRadioTestMode() {

  // =========================
  // STATIC VARIABLES
  // =========================
  static bool isTransmitting = false;
  static unsigned long lastTransmissionTime = 0;
  static int selectedFreq = 0;
  static bool inBurst = false;

  // =========================
  // ICONS
  // =========================
  static const unsigned char image_ButtonCenter_bits[] U8X8_PROGMEM = {
    0x1c,0x22,0x5d,0x5d,0x5d,0x22,0x1c
  };

  static const unsigned char image_ButtonUp_bits[] U8X8_PROGMEM = {
    0x08,0x1c,0x3e,0x7f
  };

  static const unsigned char image_SmallArrowDown_bits[] U8X8_PROGMEM = {
    0x7f,0x3e,0x1c,0x08
  };

  static const unsigned char image_Warning_bits[] U8X8_PROGMEM = {
    0x80,0x00,0xc0,0x01,0xe0,0x01,0x60,0x03,
    0x70,0x07,0x78,0x07,0x78,0x0f,0x7c,0x1f,
    0x7e,0x1f,0xfe,0x3f,0x7f,0x7f,0xff,0x3f
  };

  // =========================
  // RF SETTINGS
  // =========================
 const float testFrequencies[] = {
  300.00,
  303.87,
  304.25,
  307.00,
  310.00,
  312.00,
  315.00,
  318.00,
  330.00,
  350.00,
  387.00,
  390.00,
  418.00,
  430.00,
  433.05,
  433.42,
  433.92,
  434.42,
  434.79,
  435.00,
  440.00,
  450.00,
  455.00,
  470.00,
  480.00,
  779.00,
  800.00,
  810.00,
  820.00,
  868.00,
  868.30,
  868.35,
  868.95,
  902.00,
  905.00,
  908.42,
  910.00,
  915.00,
  918.00,
  920.00,
  925.00,
  928.00
};

  const int numFreqs =
    sizeof(testFrequencies) / sizeof(testFrequencies[0]);

  const unsigned long txInterval = 20;
  const unsigned long burstDuration = 5;

  // =========================
  // DRAW UI
  // =========================
  u8g2.clearBuffer();

  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  // Warning icon
  u8g2.drawXBMP(2, 1, 15, 12, image_Warning_bits);

  // Title
  u8g2.setFont(u8g2_font_haxrcorp4089_tr);
  u8g2.drawStr(28, 11, "RF JAMMER");

  // Frequency text
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(8, 30, "Frequency");

  // Frequency value
  char freqStr[20];
  sprintf(freqStr, "%.2f", testFrequencies[selectedFreq]);

  u8g2.setFont(u8g2_font_profont17_tr);
  u8g2.drawStr(18, 46, freqStr);

  // MHz
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(92, 45, "MHz");

  // Up icon
  u8g2.drawXBMP(108, 20, 7, 4, image_ButtonUp_bits);

  // Down icon
  u8g2.drawXBMP(108, 34, 7, 4, image_SmallArrowDown_bits);

  // Center button icon
  u8g2.drawXBMP(7, 52, 7, 7, image_ButtonCenter_bits);

  // =========================
  // IDLE MODE
  // =========================
  if (!isTransmitting) {

    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.drawStr(20, 58, "PRESS TO START");

    u8g2.sendBuffer();

    // =========================
    // CHANGE FREQ
    // =========================
    if (isButtonPressed(BTN_UP)) {

      selectedFreq++;

      if (selectedFreq >= numFreqs)
        selectedFreq = 0;

      delay(150);
    }

    if (isButtonPressed(BTN_DOWN)) {

      selectedFreq--;

      if (selectedFreq < 0)
        selectedFreq = numFreqs - 1;

      delay(150);
    }

    // =========================
    // START TX
    // =========================
    if (isButtonPressed(BTN_SELECT)) {

      isTransmitting = true;

      lastTransmissionTime = millis();

      inBurst = false;

      ELECHOUSE_cc1101.Init();

      ELECHOUSE_cc1101.setMHZ(
        testFrequencies[selectedFreq]
      );

      ELECHOUSE_cc1101.setModulation(0);

      ELECHOUSE_cc1101.setPA(12);

      ELECHOUSE_cc1101.SetTx();

      pinMode(CC1101_GDO0, OUTPUT);

      digitalWrite(CC1101_GDO0, LOW);

      delay(250);
    }

  }

  // =========================
  // TRANSMIT MODE
  // =========================
  else {

    u8g2.setFont(u8g2_font_5x8_tr);

    // TX text
    static bool blink = false;
    blink = !blink;

    if (blink) {
      u8g2.drawStr(20, 58, "TRANSMITTING...");
      setColor(255,0,0);
    } else {
      u8g2.drawStr(20, 58, "SIGNAL ACTIVE");
    }

    // RF animation
    int bars = (millis() / 120) % 4;

    for (int i = 0; i < bars; i++) {

      u8g2.drawBox(
        102 + (i * 5),
        58 - (i * 3),
        3,
        3 + (i * 3)
      );
    }

    u8g2.sendBuffer();

    unsigned long currentMillis = millis();

    // =========================
    // RF BURST
    // =========================
    if (!inBurst &&
        (currentMillis - lastTransmissionTime >= txInterval)) {

      inBurst = true;

      lastTransmissionTime = currentMillis;

      digitalWrite(CC1101_GDO0, HIGH);
    }

    else if (inBurst &&
             (currentMillis - lastTransmissionTime >= burstDuration)) {

      inBurst = false;

      lastTransmissionTime = currentMillis;

      digitalWrite(CC1101_GDO0, LOW);
    }

    // =========================
    // STOP TX
    // =========================
    if (isButtonPressed(BTN_SELECT)) {

      isTransmitting = false;

      digitalWrite(CC1101_GDO0, LOW);

      pinMode(CC1101_GDO0, INPUT);

      ELECHOUSE_cc1101.SetRx();

      setStatusMsg("Transmission Stopped");
      setColor(0,0,0);

      delay(150);
    }
  }
}

//////////////////////////////////////////////////////////////////



void drawBruteForce() {

  static bool isRunning = false;
  static int bruteMode = 0; // 0=8bit,1=12bit,2=16bit
  static uint32_t currentCode = 0;
  static uint32_t maxCode = 0xFF;
  static unsigned long lastSend = 0;
  static int protoSelect = 0;
  static unsigned long startTime = 0;
  static int lastReportedProgress = -1;

  // =========================
  // PROTOCOL NAMES
  // =========================
  const char* modeNames[] = {
    "8-bit (256)",
    "12-bit (4K)",
    "16-bit (65K)"
  };

  const char* protoNames[] = {
    "PT2262",
    "EV1527",
    "HT12E",
    "CAME"
  };

  // =========================
  // DISPLAY UI
  // =========================
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(5, 10, "BRUTE FORCE");

  u8g2.setFont(u8g2_font_5x7_tr);

  if (!isRunning) {

    char line[40];

    u8g2.drawStr(0, 24, ">");
    sprintf(line, "Bits : %s", modeNames[bruteMode]);
    u8g2.drawStr(8, 24, line);

    sprintf(line, "Proto: %s", protoNames[protoSelect]);
    u8g2.drawStr(8, 36, line);

    sprintf(line, "Freq : %.2fMHz", currentFreq);
    u8g2.drawStr(8, 48, line);

    uint32_t totalCodes =
      (bruteMode == 0) ? 256 :
      (bruteMode == 1) ? 4096 :
      65536;

    sprintf(line, "Codes: %lu", (unsigned long)totalCodes);
    u8g2.drawStr(0, 60, line);

    drawStatusBar();
    u8g2.sendBuffer();

    // =========================
    // CHANGE BIT MODE
    // =========================
    if (isButtonPressed(BTN_UP)) {

      bruteMode++;
      if (bruteMode > 2) bruteMode = 0;

      Serial.println();
      Serial.println("========== BRUTE CONFIG ==========");
      Serial.printf("Mode      : %s\n", modeNames[bruteMode]);
      Serial.printf("Protocol  : %s\n", protoNames[protoSelect]);
      Serial.printf("Frequency : %.2f MHz\n", currentFreq);
      Serial.println("==================================");

      safeDelay(200);
    }

    // =========================
    // CHANGE PROTOCOL
    // =========================
    if (isButtonPressed(BTN_DOWN)) {

      protoSelect++;
      if (protoSelect > 3) protoSelect = 0;

      Serial.printf("Protocol changed -> %s\n",
                    protoNames[protoSelect]);

      safeDelay(200);
    }

    // =========================
    // START ATTACK
    // =========================
    if (isButtonPressed(BTN_SELECT)) {

      isRunning = true;

      currentCode = 0;
      startTime = millis();
      lastReportedProgress = -1;

      if (bruteMode == 0)
        maxCode = 0xFF;
      else if (bruteMode == 1)
        maxCode = 0xFFF;
      else
        maxCode = 0xFFFF;

      Serial.println();
      Serial.println("==================================");
      Serial.println(" BRUTE FORCE STARTED");
      Serial.println("==================================");
      Serial.printf("Protocol : %s\n",
                    protoNames[protoSelect]);
      Serial.printf("Frequency: %.2f MHz\n",
                    currentFreq);
      Serial.printf("Codes    : %lu\n",
                    (unsigned long)(maxCode + 1));
      Serial.println("==================================");

      safeDelay(200);
    }

  } else {

    // =========================
    // RUNNING SCREEN
    // =========================
    unsigned long elapsed =
      (millis() - startTime) / 1000;

    float progress =
      ((float)currentCode / (float)maxCode) * 100.0;

    char line[40];

    sprintf(line, "Progress %.1f%%", progress);
    u8g2.drawStr(0, 22, line);

    sprintf(line, "0x%04X", currentCode);
    u8g2.drawStr(0, 34, line);

    sprintf(line, "%lus", elapsed);
    u8g2.drawStr(0, 46, line);

    // progress bar
    u8g2.drawFrame(0, 56, 128, 8);

    int barW = (progress / 100.0) * 126;

    if (barW > 0)
      u8g2.drawBox(1, 57, barW, 6);

    drawStatusBar();
    u8g2.sendBuffer();

    // =========================
    // SEND RF CODE
    // =========================
    if (millis() - lastSend > 50) {

      ELECHOUSE_cc1101.setMHZ(currentFreq);
      ELECHOUSE_cc1101.SetTx();

      // ===================================
      // RC SWITCH SETTINGS
      // ===================================

      switch (protoSelect) {

        case 0: // PT2262
          mySwitch.setProtocol(1);
          mySwitch.setPulseLength(350);
          break;

        case 1: // EV1527
          mySwitch.setProtocol(1);
          mySwitch.setPulseLength(321);
          break;

        case 2: // HT12E
          mySwitch.setProtocol(1);
          mySwitch.setPulseLength(400);
          break;

        case 3: // CAME
          mySwitch.setProtocol(6);
          mySwitch.setPulseLength(450);
          break;
      }

      // repeat for reliability
      mySwitch.setRepeatTransmit(6);

      // number of bits
      int bits =
        (bruteMode == 0) ? 8 :
        (bruteMode == 1) ? 12 :
        16;

      // send code
      mySwitch.send(currentCode, bits);

      ELECHOUSE_cc1101.SetRx();

      // serial logging
      Serial.printf(
        "TX -> Code: 0x%04X | Bits: %d | Proto: %s\n",
        currentCode,
        bits,
        protoNames[protoSelect]
      );

      currentCode++;
      lastSend = millis();

      // =========================
      // PROGRESS REPORT
      // =========================
      int progressPercent = (int)progress;

      if (progressPercent % 10 == 0 &&
          progressPercent != lastReportedProgress) {

        lastReportedProgress = progressPercent;

        Serial.println();
        Serial.println("========== PROGRESS ==========");
        Serial.printf("Progress : %d%%\n",
                      progressPercent);
        Serial.printf("Current  : 0x%04X\n",
                      currentCode);
        Serial.printf("Elapsed  : %lus\n",
                      elapsed);
        Serial.println("==============================");
      }

      // =========================
      // FINISHED
      // =========================
      if (currentCode > maxCode) {

        isRunning = false;

        unsigned long totalTime =
          (millis() - startTime) / 1000;

        Serial.println();
        Serial.println("==================================");
        Serial.println(" BRUTE FORCE COMPLETE");
        Serial.println("==================================");
        Serial.printf("Codes Sent : %lu\n",
                      (unsigned long)currentCode);
        Serial.printf("Time       : %lus\n",
                      totalTime);
        Serial.println("==================================");

        setStatusMsg("Complete!");
      }
    }

    // =========================
    // STOP BUTTON
    // =========================
    if (isButtonPressed(BTN_BACK)) {

      isRunning = false;

      ELECHOUSE_cc1101.SetRx();

      Serial.println();
      Serial.println("==================================");
      Serial.println(" BRUTE FORCE STOPPED");
      Serial.println("==================================");
      Serial.printf("Stopped at : 0x%04X\n",
                    currentCode);
      Serial.println("==================================");

      safeDelay(200);
    }
  }

  // =========================
  // EXIT
  // =========================
  if (isButtonPressed(BTN_BACK) && !isRunning) {

    Serial.println("Exit Brute Force");

    mode = -1;

    safeDelay(200);
  }
}



/*
static const unsigned char image_ButtonCenter_bits[]    U8X8_PROGMEM = {0x1c,0x22,0x5d,0x5d,0x5d,0x22,0x1c};
static const unsigned char image_ButtonLeftSmall_bits[] U8X8_PROGMEM = {0x04,0x06,0x07,0x06,0x04};
static const unsigned char image_paint_14_bits[]        U8X8_PROGMEM = {0x01};
*/
// [BEGIN lopaka generated]
static const unsigned char image_BLE_beacon_bits[] U8X8_PROGMEM = {0x22,0x49,0x55,0x49,0x2a,0x08,0x08,0x3e};
static const unsigned char image_ButtonCenter_bits[] U8X8_PROGMEM = {0x1c,0x22,0x5d,0x5d,0x5d,0x22,0x1c};
//static const unsigned char image_ButtonLeft_bits[] U8X8_PROGMEM = {0x08,0x0c,0x0e,0x0f,0x0e,0x0c,0x08};
static const unsigned char image_paint_7_bits[] U8X8_PROGMEM = {0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x15,0x10,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x15,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static const unsigned char image_SmallArrowDown_bits[] U8X8_PROGMEM = {0x7f,0x3e,0x1c,0x08};

void read_raw() {

    enum RawState { IDLE, SCAN, CAPTURED };
    static RawState state = IDLE;

    static const float    FREQ        = 433.92f;
    static const int      RSSI_THRESH = -78;
    static const int      END_THRESH  = -92;

    static const uint32_t MAX_GAP_US  = 45000;
    static const int      MIN_PULSES  = 10;

    static int32_t        pulseBuffer[512];
    static int            pulseCount = 0;

    static unsigned long  lastEdge   = 0;
    static bool           lastPin    = false;
    static bool           capturing  = false;

    static unsigned long  lastRssiMs = 0;
    static int            burstRssi  = 0;
    static int            liveRssi   = -120;

    static unsigned long  lastDisplay = 0;

    // result
    static bool           decoded      = false;
    static uint32_t       decodedValue = 0;
    static uint32_t       decodedBits  = 0;
    static uint32_t       decodedProto = 0;
    static uint32_t       decodedDelay = 0; // Added: Needs to be saved for exact TX matching
    static int            capRssi      = -120;

    // animation
    static int8_t         rssiCol[100];
    static int            sweepX       = 0;
    static unsigned long  lastSweepTick = 0;

    // radio init
    static bool radioInit = false;
    if (!radioInit) {
        radioInit = true;
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setMHZ(FREQ);
        ELECHOUSE_cc1101.setModulation(2); // ASK/OOK
        ELECHOUSE_cc1101.setDRate(3.79);
        ELECHOUSE_cc1101.setRxBW(58);
        ELECHOUSE_cc1101.SpiWriteReg(0x02, 0x0D);
        ELECHOUSE_cc1101.SetRx();
        pinMode(CC1101_GDO0, INPUT);
        mySwitch.enableReceive(digitalPinToInterrupt(CC1101_GDO0));
        mySwitch.setReceiveTolerance(60);
        sweepX = 0;
        memset(rssiCol, 0, sizeof(rssiCol));
    }

    // ══════════════════════════════════════════════════════
    //  BACKGROUND RSSI & SIGNAL CAPTURE
    // ══════════════════════════════════════════════════════
    liveRssi = ELECHOUSE_cc1101.getRssi();
    unsigned long nowMs = millis();
    if (nowMs - lastRssiMs >= 8) {
        burstRssi  = liveRssi;
        lastRssiMs = nowMs;
    }

    if (state == SCAN) {
        // --- 1. Check for Decoded rc-switch Signals ---
        if (mySwitch.available()) {
            uint32_t val   = mySwitch.getReceivedValue();
            uint32_t bits  = mySwitch.getReceivedBitlength();
            uint32_t proto = mySwitch.getReceivedProtocol();
            uint32_t dlay  = mySwitch.getReceivedDelay(); // Grab pulse length
            mySwitch.resetAvailable();
            
            if (val != 0) {
                decodedValue = val;
                decodedBits  = bits;
                decodedProto = proto;
                decodedDelay = dlay;
                capRssi      = ELECHOUSE_cc1101.getRssi();
                decoded      = true;
                pulseCount   = 0; // Wipe raw pulses since we have a clean decode
                state        = CAPTURED;
            }
        }

        // --- 2. Record Raw Pulses concurrently ---
        if (liveRssi > RSSI_THRESH) {
            bool pin = digitalRead(CC1101_GDO0);
            if (pin != lastPin) {
                unsigned long now = micros();
                uint32_t dur = (uint32_t)(now - lastEdge);
                lastEdge = now;
                
                if (!capturing) {
                    capturing = true;
                } else if (dur >= 50 && dur < MAX_GAP_US && pulseCount < 512) {
                    pulseBuffer[pulseCount++] = lastPin ? (int32_t)dur : -(int32_t)dur;
                }
                lastPin = pin;
            }
        }

        uint32_t silence = (uint32_t)(micros() - lastEdge);
        bool signalGone  = capturing && (burstRssi < END_THRESH);
        bool longGap     = capturing && (silence > MAX_GAP_US);

        if (capturing && (signalGone || longGap)) {
            ELECHOUSE_cc1101.SetRx();
            if (pulseCount >= MIN_PULSES) {
                decoded  = false;
                capRssi  = burstRssi;
                state    = CAPTURED;
            } else {
                capturing  = false;
                pulseCount = 0;
            }
        }
    }

    // ══════════════════════════════════════════════════════
    //  SWEEP TICK — ONLY moves when active recording (SCAN)
    // ══════════════════════════════════════════════════════
    if (state == SCAN && millis() - lastSweepTick >= 25) {
        lastSweepTick = millis();

        int fill = constrain((liveRssi + 90) * 33 / 30, 0, 33);
        rssiCol[sweepX] = (int8_t)fill;

        for (int i = 1; i <= 3; i++) {
            rssiCol[(sweepX + i) % 100] = 0;
        }
        sweepX = (sweepX + 1) % 100;
    }

    // ══════════════════════════════════════════════════════
    //  BUTTONS & LOGIC FLOW
    // ══════════════════════════════════════════════════════
    if (isButtonPressed(BTN_BACK)) {
        mySwitch.disableReceive();
        pinMode(CC1101_GDO0, INPUT);
        ELECHOUSE_cc1101.SetRx();
        radioInit  = false;
        decoded    = false;
        capturing  = false;
        pulseCount = 0;
        state      = IDLE;
        // mode       = -1; // Uncomment if 'mode' is defined globally in your sketch
        safeDelay(200);
        return;
    }

    if (state == IDLE) {
        if (isButtonPressed(BTN_SELECT)) {
            pulseCount = 0;
            capturing  = false;
            decoded    = false;
            sweepX     = 0;
            memset(rssiCol, 0, sizeof(rssiCol));
            mySwitch.resetAvailable();
            state = SCAN;
            safeDelay(150);
        }
    } 
    else if (state == SCAN) {
        if (isButtonPressed(BTN_SELECT)) {
            capturing = false;
            state = IDLE;
            safeDelay(150);
        }
    } 
    else if (state == CAPTURED) {
        
        // --- FIXED: PROPER TRANSMISSION BRANCHING ---
        if (isButtonPressed(BTN_UP)) {
            // Drop receiver logic out of the way
            mySwitch.disableReceive();
            ELECHOUSE_cc1101.SetTx();

            if (decoded) {
                // 1. SEND DECODED SIGNAL
                mySwitch.enableTransmit(CC1101_GDO0);
                mySwitch.setProtocol(decodedProto);
                if (decodedDelay > 0) {
                    mySwitch.setPulseLength(decodedDelay);
                }
                // rc-switch handles repetitive bursts naturally
                mySwitch.send(decodedValue, decodedBits);
                mySwitch.disableTransmit();
                
            } else if (pulseCount > 0) {
                // 2. REPLAY RAW SIGNAL
                pinMode(CC1101_GDO0, OUTPUT);
                for (int r = 0; r < 5; r++) {
                    for (int i = 0; i < pulseCount; i++) {
                        int32_t dur = pulseBuffer[i];
                        digitalWrite(CC1101_GDO0, dur > 0 ? HIGH : LOW);
                        
                        // Avoid abs() macros and delayMicroseconds limits safely
                        uint32_t waitUs = dur > 0 ? dur : -dur;
                        if (waitUs > 16000) {
                            delay(waitUs / 1000);
                            delayMicroseconds(waitUs % 1000);
                        } else {
                            delayMicroseconds(waitUs);
                        }
                    }
                    digitalWrite(CC1101_GDO0, LOW);
                    delay(15); // Frame gap
                }
            }

            // Put radio back safely into listening mode
            ELECHOUSE_cc1101.SetRx();
            pinMode(CC1101_GDO0, INPUT);
            mySwitch.enableReceive(digitalPinToInterrupt(CC1101_GDO0));
            safeDelay(200);
        } 
        else if (isButtonPressed(BTN_SELECT) || isButtonPressed(BTN_DOWN) || 
                 isButtonPressed(BTN_LEFT)   || isButtonPressed(BTN_RIGHT)) {
            decoded    = false;
            pulseCount = 0;
            capturing  = false;
            mySwitch.resetAvailable();
            ELECHOUSE_cc1101.SetRx();
            memset(rssiCol, 0, sizeof(rssiCol));
            sweepX = 0;
            state  = IDLE;
            safeDelay(150);
        }
    }

    // ══════════════════════════════════════════════════════
    //  DISPLAY — 25 ms gate
    // ══════════════════════════════════════════════════════
    if (millis() - lastDisplay < 25) return;
    lastDisplay = millis();

    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    // ══════════════════════════════════════════════════════
    //  CAPTURED SCREEN
    // ══════════════════════════════════════════════════════
    if (state == CAPTURED) {
        u8g2.drawLine(0,   14, 102, 14);
        u8g2.drawLine(1,   48, 102, 48);
        u8g2.drawLine(102, 15, 102, 47);
        u8g2.drawLine(11,  13, 11,  47);

        u8g2.setFont(u8g2_font_haxrcorp4089_tr);
        u8g2.drawStr(1, 8, "433.92  AM");
        u8g2.drawXBMP(120, 1, 7, 8, image_BLE_beacon_bits);
        u8g2.drawStr(103, 9, "spl");

        u8g2.setFont(u8g2_font_4x6_tr);
        char line[22];

        if (decoded) {
            snprintf(line, sizeof(line), "DEC:%lu", decodedValue);
            u8g2.drawStr(14, 23, line);
            snprintf(line, sizeof(line), "HEX:0x%lX", decodedValue);
            u8g2.drawStr(14, 30, line);
            snprintf(line, sizeof(line), "BIT:%lu  PT:%lu", decodedBits, decodedProto);
            u8g2.drawStr(14, 37, line);
            snprintf(line, sizeof(line), "RS:%d dBm", capRssi);
            u8g2.drawStr(14, 44, line);
        } else {
            snprintf(line, sizeof(line), "RAW %d pulses", pulseCount);
            u8g2.drawStr(14, 25, line);
            snprintf(line, sizeof(line), "RS:%d dBm", capRssi);
            u8g2.drawStr(14, 33, line);
            u8g2.drawStr(14, 41, "PROTO:UNKNOWN");
        }

        // Action indicator on the side of the screen
        u8g2.setFont(u8g2_font_4x6_tr);
        u8g2.drawStr(106, 25, "[UP]");
        u8g2.drawStr(106, 35, "to");
        u8g2.drawStr(106, 45, "TX");

        u8g2.setFont(u8g2_font_haxrcorp4089_tr);
        u8g2.setDrawColor(2);
        u8g2.drawBox(0,  53, 35, 11);
        u8g2.drawBox(51, 53, 25, 11);
        u8g2.setDrawColor(1);
        u8g2.drawXBMP(1,  55, 4, 7, image_ButtonLeft_bits);
        u8g2.drawStr(7,  61, "back");
        u8g2.drawXBMP(52, 55, 7, 7, image_ButtonCenter_bits);
        u8g2.drawStr(60, 61, "clr");

        u8g2.sendBuffer();
        return;
    }

    // ══════════════════════════════════════════════════════
    //  LISTENING / IDLE SCREEN
    // ══════════════════════════════════════════════════════
    u8g2.drawLine(0,   14, 102, 14);
    u8g2.drawLine(0,   48, 102, 48);
    u8g2.drawLine(102, 15, 102, 47);

    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(2, 8, "433.92");
    
    if (state == SCAN) {
        u8g2.drawStr(43, 8, "REC");
    } else {
        u8g2.drawStr(43, 8, "IDLE");
    }

    u8g2.drawStr(104, 8, "spl");
    u8g2.drawStr(104, 48, "rssi");

    u8g2.drawXBMP(120, 1, 7, 8, image_BLE_beacon_bits);
    u8g2.drawXBMP(1, 55, 4, 7, image_ButtonLeft_bits);
    u8g2.drawStr(8, 61, "config");
    u8g2.drawXBMP(52, 55, 7, 7, image_ButtonCenter_bits);
    
    if (state == IDLE) {
        u8g2.drawStr(61, 61, "rec");
    } else {
        u8g2.drawStr(61, 61, "stop");
    }

    u8g2.setDrawColor(2);
    u8g2.drawBox(0, 53, 42, 11);
    u8g2.drawBox(50, 53, 30, 11);
    u8g2.setDrawColor(1);

    {
        char spl[8];
        snprintf(spl, sizeof(spl), "%d", pulseCount);
        u8g2.setFont(u8g2_font_5x8_tr);
        u8g2.drawStr(106, 22, spl);
    }

    // ─────────────────────────────────────────────
    // CURSOR & RSSI GRAPH
    // ─────────────────────────────────────────────
    int cursorX = sweepX + 1; 
    int arrowX = constrain(cursorX - 3, 0, 121);

    u8g2.drawXBMP(arrowX, 10, 7, 4, image_SmallArrowDown_bits);

    {
        const int BBOT = 47;
        u8g2.setDrawColor(2);

        for (int col = 0; col < 100; col++) {
            int fill = rssiCol[col];
            if (fill <= 1) continue;

            int screenX = col + 1; 
            int yTop = BBOT - fill + 1;

            u8g2.drawVLine(screenX, yTop, fill);

            if (fill > 8 && screenX > 1 && screenX < 100) {
                u8g2.drawPixel(screenX - 1, yTop);
                u8g2.drawPixel(screenX + 1, yTop);
            }
        }
        u8g2.setDrawColor(1);
    }

    u8g2.drawVLine(cursorX, 15, 33);
    u8g2.sendBuffer();
}








void read() {
 
    static unsigned long receivedValue    = 0;
    static int           receivedBitLength = 0;
    static int           receivedProtocol  = 0;
 
    static const uint32_t subghz_frequency_list[] = {
        300000000, 303875000, 304250000, 310000000, 315000000, 318000000,
        390000000, 418000000,
        433075000, 433420000, 433920000, 434420000, 434775000, 438900000,
        868350000, 868000000,
        915000000, 925000000
    };
 
    static const int numFrequencies =
        sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
    static int currentFrequencyIndex = 0;
 
    const int rssi_threshold = -100;
 
    static bool initialized = false;
 
    // =====================================================
    // INIT
    // =====================================================
 
    if (!initialized) {
        initialized = true;
 
        ELECHOUSE_cc1101.Init();
 
        // Full config applied in one place — see applyRxConfig() pattern below
        ELECHOUSE_cc1101.setSidle();
        delay(10);
        ELECHOUSE_cc1101.setMHZ(subghz_frequency_list[currentFrequencyIndex] / 1000000.0);
        ELECHOUSE_cc1101.setModulation(2);  // OOK/ASK
        ELECHOUSE_cc1101.setDRate(3.79);
        ELECHOUSE_cc1101.setRxBW(812.50);
        ELECHOUSE_cc1101.SetRx();
        delay(50);
 
        // FIX 4: Explicitly disable transmit on init to guarantee clean state.
        // Without this, if the module was previously used for TX, the transmit
        // pin remains set and corrupts the first receive.
        mySwitch.disableTransmit();
 
        mySwitch.enableReceive(CC1101_2_GDO0);
        mySwitch.setReceiveTolerance(80);
 
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_6x12_tf);
        u8g2.drawStr(12, 28, "Initializing...");
        u8g2.sendBuffer();
 
        Serial.println("[READ] Initialized. Waiting for signal...");
        delay(1000);
    }
 
    // =====================================================
    // BACK BUTTON
    // =====================================================
 
    if (isButtonPressed(BTN_BACK)) {
        mySwitch.disableReceive();
        mySwitch.disableTransmit();
        ELECHOUSE_cc1101.setSidle();
 
        initialized    = false;
        receivedValue     = 0;
        receivedBitLength = 0;
        receivedProtocol  = 0;
 
        mode = -1;
        safeDelay(200);
        return;
    }
 
    // =====================================================
    // FREQUENCY ADJUSTMENT
    // =====================================================
 
    bool frequencyChanged = false;
 
    if (isButtonPressed(BTN_UP)) {
        currentFrequencyIndex = (currentFrequencyIndex + 1) % numFrequencies;
        frequencyChanged = true;
    } else if (isButtonPressed(BTN_DOWN)) {
        currentFrequencyIndex = (currentFrequencyIndex - 1 + numFrequencies) % numFrequencies;
        frequencyChanged = true;
    }
 
    if (frequencyChanged) {
        receivedValue     = 0;
        receivedBitLength = 0;
        receivedProtocol  = 0;
 
        mySwitch.disableReceive();
 
        ELECHOUSE_cc1101.setSidle();
        delay(10);
        ELECHOUSE_cc1101.setMHZ(subghz_frequency_list[currentFrequencyIndex] / 1000000.0);
        ELECHOUSE_cc1101.setModulation(2);
 
        // FIX 3: Re-apply data rate and bandwidth after every frequency change.
        // Some library versions reset these registers when setMHZ() is called,
        // silently degrading receive sensitivity until the device is rebooted.
        ELECHOUSE_cc1101.setDRate(3.79);
        ELECHOUSE_cc1101.setRxBW(812.50);
 
        ELECHOUSE_cc1101.SetRx();
        delay(20);
 
        mySwitch.enableReceive(CC1101_2_GDO0);
 
        Serial.print("[READ] Frequency changed to: ");
        Serial.print(subghz_frequency_list[currentFrequencyIndex] / 1000000.0);
        Serial.println(" MHz");
 
        safeDelay(200);
    }
 
    // =====================================================
    // RECEIVE
    // =====================================================
 
    if (mySwitch.available()) {
        unsigned long value = mySwitch.getReceivedValue();
 
        Serial.print("[READ] mySwitch triggered. Raw value: ");
        Serial.println(value);
 
        if (value != 0) {
            int rssi = ELECHOUSE_cc1101.getRssi();
 
            Serial.print("[READ] RSSI: ");
            Serial.println(rssi);
 
            if (rssi > rssi_threshold) {
                receivedValue     = value;
                receivedBitLength = mySwitch.getReceivedBitlength();
                receivedProtocol  = mySwitch.getReceivedProtocol();
 
                Serial.print("[READ] Captured! Val=");
                Serial.print(receivedValue);
                Serial.print(" Bits=");
                Serial.print(receivedBitLength);
                Serial.print(" Proto=");
                Serial.println(receivedProtocol);
            } else {
                Serial.print("[READ] Signal too weak, ignoring. RSSI=");
                Serial.println(rssi);
            }
        } else {
            Serial.println("[READ] Undecoded signal (value=0). Remote may use unsupported protocol.");
        }
 
        mySwitch.resetAvailable();
    }
 
    // =====================================================
    // SEND
    // =====================================================
 
    if (isButtonPressed(BTN_SELECT) && receivedValue != 0) {
 
        // Step 1: Stop receiving
        mySwitch.disableReceive();
        delay(50);
 
        // Step 2: Switch CC1101 to TX mode
        ELECHOUSE_cc1101.setSidle();
        delay(10);
        ELECHOUSE_cc1101.SetTx();
 
        // FIX 5: Delay after SetTx() so the CC1101 PLL locks before we start
        // clocking data. Without this, the first few bits are garbled.
        delay(15);
 
        // FIX 1: CRITICAL — enableTransmit() tells RC-Switch which pin to
        // drive. Without this call, mySwitch.send() does absolutely nothing;
        // nTransmitterPin defaults to -1 and the send loop is skipped entirely.
        mySwitch.enableTransmit(CC1101_2_GDO0);
        mySwitch.setProtocol(receivedProtocol);
 
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_6x12_tf);
        u8g2.drawStr(20, 20, "Sending...");
        char txt[32];
        sprintf(txt, "%lu", receivedValue);
        u8g2.drawStr(10, 40, txt);
        u8g2.sendBuffer();
 
        Serial.print("[READ] Sending Val=");
        Serial.print(receivedValue);
        Serial.print(" Bits=");
        Serial.print(receivedBitLength);
        Serial.print(" Proto=");
        Serial.println(receivedProtocol);
 
        for (int i = 0; i < 5; i++) {
            mySwitch.send(receivedValue, receivedBitLength);
            delay(10);
        }
 
        delay(300);
 
        // FIX 2: Disable the transmit pin before restoring RX.
        // Without this, nTransmitterPin stays set and RC-Switch randomly
        // drives the GDO0 line during receive, causing self-interference.
        mySwitch.disableTransmit();
 
        // Step 5: Restore full RX config (frequency stays the same)
        ELECHOUSE_cc1101.setSidle();
        delay(10);
        ELECHOUSE_cc1101.setModulation(2);
        ELECHOUSE_cc1101.setDRate(3.79);
        ELECHOUSE_cc1101.setRxBW(812.50);
        ELECHOUSE_cc1101.SetRx();
 
        mySwitch.enableReceive(CC1101_2_GDO0);
 
        Serial.println("[READ] TX done. Back to RX.");
        safeDelay(200);
    }
 
    // =====================================================
    // DISPLAY (capped at 10 Hz)
    // =====================================================
 
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay < 100) return;
    lastDisplay = millis();
 
    u8g2.clearBuffer();
 
    // HEADER
    u8g2.drawBox(0, 0, 128, 12);
    u8g2.setDrawColor(0);
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(2, 9, "CC1101 SUBGHZ");
    u8g2.setDrawColor(1);
 
    // FREQUENCY
    char freqTxt[24];
    sprintf(freqTxt, "%.3f MHz",
            subghz_frequency_list[currentFrequencyIndex] / 1000000.0);
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.drawStr(0, 24, "FREQ:");
    u8g2.drawStr(42, 24, freqTxt);
 
    // VALUE
    char valTxt[32];
    sprintf(valTxt, "%lu", receivedValue);
    u8g2.drawStr(0, 40, "VAL:");
    u8g2.drawStr(30, 40, valTxt);
 
    // BIT + PROTOCOL
    char infoTxt[32];
    sprintf(infoTxt, "BIT:%d P:%d", receivedBitLength, receivedProtocol);
    u8g2.drawStr(0, 54, infoTxt);
 
    // RSSI (live readout)
    int rssi = ELECHOUSE_cc1101.getRssi();
    char rssiTxt[20];
    sprintf(rssiTxt, "%d dBm", rssi);
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr(78, 62, rssiTxt);
 
    // FOOTER
    u8g2.drawLine(0, 56, 128, 56);
    u8g2.drawStr(0, 62, "SEL=SEND");
 
    u8g2.sendBuffer();
}





