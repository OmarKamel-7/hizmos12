void handlewifimenu() {
  const char* menuItems[] = {"DEAUTHER", "PACKET ANALYZER", "BEACON", "CAPTIVE PORTAL", "DEAUTH DETECTOR"};
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

       // ── AFTER (correct — fills foundCount + foundNetworks[] that scanningwifi reads) ──
         loading();
         WiFi.disconnect();
         delay(100);
         wifi_networkCount = scan_networks();   // does mode+scan+cache all in one
         runLoop(scanningwifi);
       
        break;
      case 1:
       remote_loop();
       setupSnifferGraph(); 
       runLoop(updateSnifferGraph);
       
       break;
      case 2:
        beacon.begin(8); // Start on channel 6
        runLoop(beaconflood);
        break;
      case 3: 
      runLoop(loading);
       break;
      case 4:
      runLoop(loading);
        break;
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
