///////////packets counter

#define BINS 13

uint32_t binHits[BINS];
int scanScroll = 0;

void scanSpectrumBins() {

  memset(binHits, 0, sizeof(binHits));

  for (uint8_t ch = 0; ch < 126; ch++) {

    uint16_t hits = 0;

    for (uint8_t i = 0; i < 20; i++) {

      radio1.setChannel(ch);
      delayMicroseconds(130);

      if (radio1.testRPD()) {
        hits++;
      }
    }

    uint8_t bin = ch / 10;

    if (bin >= BINS)
      bin = BINS - 1;

    binHits[bin] += hits;
  }
}

void drawSpectrumList() {

  static unsigned long lastInput = 0;

  if (millis() - lastInput > 150) {

    if (digitalRead(BTN_UP) == LOW) {
      if (scanScroll > 0)
        scanScroll--;
      lastInput = millis();
    }

    if (digitalRead(BTN_DOWN) == LOW) {
      if (scanScroll < (BINS - 4))
        scanScroll++;
      lastInput = millis();
    }
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);

  u8g2.drawStr(0, 8, "2.4GHz Analyzer");

  // ===== LIST =====
  for (int row = 0; row < 4; row++) {

    int idx = scanScroll + row;

    if (idx >= BINS)
      break;

    int startFreq = 2400 + (idx * 10);
    int endFreq   = startFreq + 9;

    if (idx == BINS - 1)
      endFreq = 2525;

    char line[28];

    sprintf(
      line,
      "%d-%d(%lu)",
      startFreq,
      endFreq,
      (unsigned long)binHits[idx]
    );

    u8g2.drawStr(0, 20 + row * 11, line);
  }

  // ===== SCROLL BAR =====

  const int barX = 123;
  const int barY = 12;
  const int barH = 50;

  // Track
  u8g2.drawFrame(barX, barY, 4, barH);

  int visibleItems = 4;

  int knobH = (barH * visibleItems) / BINS;

  if (knobH < 8)
    knobH = 8;

  int maxScroll = BINS - visibleItems;

  int knobY;

  if (maxScroll == 0) {
    knobY = barY;
  } else {
    knobY = barY +
            ((barH - knobH) * scanScroll) /
            maxScroll;
  }

  u8g2.drawBox(barX + 1,
               knobY + 1,
               2,
               knobH - 2);

  u8g2.sendBuffer();
}

void nrfSpectrumAnalyzer() {

  scanSpectrumBins();

  drawSpectrumList();
}




void handlenrftoolsmenu() {
  const char* menuItems[] = { "analyzer", "hijack nrf device" ,"2.4 ghz jammer","jamm spes channel","bluejammer","packets counter"};
  const int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);
  const int visibleItems = 3;

  static int selectedItem = 0;
  static int scrollOffset = 0;

 
  static unsigned long lastInputTime = 0;

  if (millis() - lastInputTime > 150) {
    

  if (digitalRead(BTN_UP) == LOW) {
    selectedItem--;
    if (selectedItem < 0) selectedItem = menuLength - 1;
    scrollOffset = constrain(selectedItem - visibleItems + 1, 0, menuLength - visibleItems);
     lastInputTime = millis(); 
  }

  if (digitalRead(BTN_DOWN) == LOW) {
    selectedItem++;
    if (selectedItem >= menuLength) selectedItem = 0;
    scrollOffset = constrain(selectedItem - visibleItems + 1, 0, menuLength - visibleItems);
     lastInputTime = millis(); 
  }

  if (digitalRead(BTN_SELECT) == LOW) {
    switch (selectedItem) {
      case 0:

    
      u8g2.setFont(u8g2_font_5x8_tr);


      radio1.setPALevel(RF24_PA_MAX);
  radio1.setDataRate(RF24_2MBPS);
  radio1.setAutoAck(false);

  radio2.setPALevel(RF24_PA_MAX);
  radio2.setDataRate(RF24_2MBPS);
  radio2.setAutoAck(false);

  radio1.startListening();
  radio2.startListening();
     runLoop(nrfscanner);
        break;
      case 1:
       
       break;
      case 2:
       radio1.setPALevel(RF24_PA_MAX);
  radio1.setDataRate(RF24_2MBPS);
  radio1.setAutoAck(false);

  radio2.setPALevel(RF24_PA_MAX);
  radio2.setDataRate(RF24_2MBPS);
  radio2.setAutoAck(false);

  drawMenuPage(nrf::currentPage);

       runLoop(nrfjammer);
        break;

        case 3:
        updateRadios();
        drawjammspeschannelMenu();
        runLoop(jammspecchannel);



        break;

        case 4:
          classicBtJammer();
          break;
          
        case 5:
          runLoop(nrfSpectrumAnalyzer);
          break;  // ← CALL THIS

     
    }
     lastInputTime = millis(); 
  }
  }

  // ===== عرض الشاشة =====
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14_tf); 

  for (int i = 0; i < visibleItems; i++) {
    int menuIndex = i + scrollOffset;
    if (menuIndex >= menuLength) break;

    int y = i * 20 + 16;

    if (menuIndex == selectedItem) {
      u8g2.drawRBox(4, y - 12, 120, 16, 4);
      u8g2.setDrawColor(0);
      u8g2.drawStr(10, y, menuItems[menuIndex]);
      u8g2.setDrawColor(1);
    } else {
      u8g2.drawStr(10, y, menuItems[menuIndex]);
    }
  }

  
  int barX = 124;
  int spacing = 64 / menuLength;

  for (int i = 0; i < menuLength; i++) {
    int dotY = i * spacing + spacing / 2;
    if (i == selectedItem) {
      u8g2.drawBox(barX, dotY - 3, 3, 6);
    } else {
      u8g2.drawPixel(barX + 1, dotY);
    }
  }

  u8g2.sendBuffer();
  
}

















