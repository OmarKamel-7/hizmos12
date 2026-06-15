/*



   ██╗  ██╗██╗███████╗███╗   ███╗ ██████╗ ███████╗
   ██║  ██║██║     ██╝████╗ ████║██╔═══██╗██╔════╝
   ███████║██║  ███╗  ██╔████╔██║██║   ██║███████╗
   ██╔══██║██║██╔══╝  ██║╚██╔╝██║██║   ██║╚════██║
   ██║  ██║██║███████╗██║ ╚═╝ ██║╚██████╔╝███████║
   ╚═╝  ╚═╝╚═╝╚══════╝╚═╝     ╚═╝ ╚═════╝ ╚══════╝



this is the safe mode in hizmos 
it is only used in analysis an scanning
it doesnt transimit on any frequency 
fully safe to use

*/



void handlesafemenu() {
  const char* menuItems[] = {"RF(2.4 & SUBGHZ)", "BADUSB", "WIFI", "BLE", "APPS","Settings","INFRARED"};
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
          runLoop(handlesaferfmenu);
          break;
        case 1:
          break;
      
        case 2:
          runLoop(handlesafewifimenu);
          break;
        case 3:
          runLoop(handlesafeble);
          break;
        case 4:
          runLoop(handleappsmenu);
          break;

        case 5:
          runLoop(tobeimplemnented);
          break;
        case 6:
          runLoop(handleirmenu);
          break;  
      }

      lastInputTime = millis(); 
    }
  }

  // ===== عرض القائمة على الشاشة =====
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

  // ===== شريط التمرير الجانبي (نقط) =====
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


////////////////rf menu

////////////////// It is done
void handlesaferfmenu() {
  const char* menuItems[] = {"2.4ghz analyzer", "SubGhz Analyzer", "Freq Analyzer"};
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
          lazyInitCC1101(1); // شغل CC1101 الأول
          runLoop(drawHistogram); 
        break;
        case 2:
          lazyInitCC1101(1);
          runLoop(SPECTRUMANALYZER);  
          break;

      }

      lastInputTime = millis(); 
    }
  }

  // ===== عرض القائمة على الشاشة =====
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

  // ===== شريط التمرير الجانبي (نقط) =====
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






//////HANDLE WIFI MENU
/////////it is done
void handlesafewifimenu() {
  const char* menuItems[] = {"Scanner", "packet Analyzer"};
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
          static const unsigned char image_file_search_bits[] U8X8_PROGMEM = {
            0x80,0x0f,0x40,0x10,0x20,0x20,0x10,0x40,0x10,0x40,0x10,0x50,
            0x10,0x50,0x10,0x48,0x20,0x26,0x50,0x10,0xa8,0x0f,0x14,0x00,
            0x0a,0x00,0x05,0x00,0x03,0x00,0x00,0x00
          };
          static const unsigned char image_wifi_50_bits[] U8X8_PROGMEM = {
            0x80,0x0f,0x00,0x60,0x30,0x00,0x18,0xc0,0x00,0x84,0x0f,0x01,
            0x62,0x30,0x02,0x11,0x40,0x04,0x0a,0x87,0x02,0xc4,0x1f,0x01,
            0xe8,0xb8,0x00,0x70,0x77,0x00,0xa0,0x2f,0x00,0xc0,0x1d,0x00,
            0x80,0x0a,0x00,0x00,0x07,0x00,0x00,0x02,0x00,0x00,0x00,0x00
          };

          u8g2.clearBuffer();
          u8g2.setFontMode(1);
          u8g2.setBitmapMode(1);
          u8g2.drawXBMP(10, 21, 15, 16, image_file_search_bits);
          u8g2.drawXBMP(99, 20, 19, 16, image_wifi_50_bits);
          u8g2.setFont(u8g2_font_t0_13_tr);
          u8g2.drawStr(32, 15, "scanning");
          u8g2.drawStr(44, 32, "wifi ");
          u8g2.drawStr(33, 49, "networks");
          u8g2.sendBuffer();

          WiFi.mode(WIFI_STA);
          WiFi.disconnect();
          delay(100);
          wifi_networkCount = WiFi.scanNetworks();
          runLoop(scanningwifi);
          break;
        case 1:
          setupSnifferGraph();
          runLoop(updateSnifferGraph);
        break;

      }

      lastInputTime = millis(); 
    }
  }

  // ===== عرض القائمة على الشاشة =====
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

  // ===== شريط التمرير الجانبي (نقط) =====
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






//////handle safe ble
///it is done (ble keyboard not implemnted in the full system yet)

void handlesafeble() {
  const char* menuItems[] = {"Scanner", "BLE MOUSE","BLE KEYBOARD"};
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
            static const unsigned char image_bluetooth_bits[] U8X8_PROGMEM = {0x80,0x00,0x40,0x01,0x40,0x02,0x44,0x04,0x48,0x04,0x50,0x02,0x60,0x01,0xc0,0x00,0x60,0x01,0x50,0x02,0x48,0x04,0x44,0x04,0x40,0x02,0x40,0x01,0x80,0x00,0x00,0x00};
            static const unsigned char image_file_search_bits[] U8X8_PROGMEM = {0x80,0x0f,0x40,0x10,0x20,0x20,0x10,0x40,0x10,0x40,0x10,0x50,0x10,0x50,0x10,0x48,0x20,0x26,0x50,0x10,0xa8,0x0f,0x14,0x00,0x0a,0x00,0x05,0x00,0x03,0x00,0x00,0x00};


          u8g2.clearBuffer();
          u8g2.setFontMode(1);
          u8g2.setBitmapMode(1);
    // Layer 1
          u8g2.setFont(u8g2_font_t0_13_tr);
          u8g2.drawStr(34, 16, "scanning");

    // bluetooth
          u8g2.drawXBMP(100, 22, 14, 16, image_bluetooth_bits);

    // Layer 3
          u8g2.drawStr(51, 34, "ble ");

    // file_search
          u8g2.drawXBMP(11, 22, 15, 16, image_file_search_bits);

    // Layer 5
          u8g2.drawStr(40, 50, "devices");

          u8g2.sendBuffer();



          blescanner_scan();
          blescanner_selectedIndex = 0;
          blescanner_drawMenu();
          runLoop(blescanner);
          break;
        case 1:
          mouse_ble.begin();
          runLoop(blemouse);
          break;
        case 2:
          runLoop(loading);
          break;  

      }

      lastInputTime = millis(); 
    }
  }

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



///////Safe Ir Menu handling (done)

void handleirmenu() {
  const char* menuItems[] = {"READ SIGNAL", "SAVED SIGNAL"};
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
          runLoop(recvIR); 
          break;
        case 1: 
          runLoop(listIRFiles);
          break;
      }

      lastInputTime = millis(); 
    }
  }

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

  // ===== شريط التمرير الجانبي (نقط) =====
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



void tobeimplemnented() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14_tf);
  u8g2.drawStr(34, 16, "To Be Implemnted");
}


void showSafeModeWarning(void (*onYes)(), void (*onNo)()) {

  const char* menuItems[] = {"YES", "NO"};
  const int menuLength = 2;

  static int selected = 0;
  static unsigned long lastInputTime = 0;
  static bool waitingRelease = false;

  // =========================
  // INPUT (same logic style as your menu)
  // =========================
  if (millis() - lastInputTime > 150 && !waitingRelease) {

    if (digitalRead(BTN_LEFT) == LOW) {
      selected = 0;
      lastInputTime = millis();
    }

    if (digitalRead(BTN_RIGHT) == LOW) {
      selected = 1;
      lastInputTime = millis();
    }

    if (digitalRead(BTN_SELECT) == LOW) {
      waitingRelease = true;
      lastInputTime = millis();
    }

    if (digitalRead(BTN_BACK) == LOW) {
      waitingRelease = true;
      selected = 1; // NO
      lastInputTime = millis();
    }
  }

  // =========================
  // WAIT FOR RELEASE (FIX YOUR BUG)
  // =========================
  if (waitingRelease) {
    if (digitalRead(BTN_SELECT) == HIGH && digitalRead(BTN_BACK) == HIGH) {

      waitingRelease = false;

      if (selected == 0) {
        if (onYes) onYes();
      } else {
        if (onNo) onNo();
      }
    }
  }

  // =========================
  // UI RENDER
  // =========================
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);

  u8g2.drawStr(10, 10, "SAFE MODE WARNING");
  u8g2.drawStr(2, 25, "Testing & analysis only");
  u8g2.drawStr(2, 35, "NO attacks or RF TX");

  // buttons
  if (selected == 0) {
    u8g2.drawStr(20, 55, "> YES");
    u8g2.drawStr(80, 55, "NO");
  } else {
    u8g2.drawStr(20, 55, "YES");
    u8g2.drawStr(80, 55, "> NO");
  }

  u8g2.sendBuffer();
}


void handleshowsafemodewarningforsettings() {
  showSafeModeWarning(handlesafemenu, handlesettingsmenu);
}

void handleshowingsafemodelegalwarningforappsmenu() {
  showSafeModeWarning(handlesafemenu, handleappsmenu);
}