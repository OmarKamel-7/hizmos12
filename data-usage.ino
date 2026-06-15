 void datausage() {
static const unsigned char image_Hidden_window_bits[] U8X8_PROGMEM = {0xfe,0x01,0x7e,0x01,0xfe,0x01,0x02,0x01,0x7f,0x01,0x41,0x01,0xc1,0x01,0x7f,0x00};
static const unsigned char image_Lock_7x8_bits[] U8X8_PROGMEM = {0x1c,0x22,0x22,0x7f,0x7f,0x77,0x7f,0x3e};
static const unsigned char image_Temperature_16x16_bits[] U8X8_PROGMEM = {0x00,0x00,0x00,0x00,0x80,0x00,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x20,0x02,0xe0,0x03,0xe0,0x03,0xc0,0x01,0x00,0x00,0x00,0x00,0x00,0x00};




  // Get RAM info
  size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
  size_t totalHeap = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
  int ramUsage = 100 - ((freeHeap * 100) / totalHeap);

  // Get Flash info
  uint32_t flashSize = spi_flash_get_chip_size();
  uint32_t flashUsed = ESP.getSketchSize();
  int flashUsage = (flashUsed * 100) / flashSize;

  // Temperature
  uint8_t temperature = temperatureRead();

  char buf[16];

  u8g2.clearBuffer();

  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  // Title
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(37, 9, "ESP32-S3");

  // RAM Icon
  u8g2.drawXBMP(3, 14, 9, 8, image_Hidden_window_bits);
  u8g2.setFont(u8g2_font_haxrcorp4089_tr);
  u8g2.drawStr(16, 22, "RAM :");

  sprintf(buf, "%d%%", ramUsage);
  u8g2.drawStr(70, 22, buf);

  // Flash Icon
  u8g2.drawXBMP(4, 33, 7, 8, image_Lock_7x8_bits);
  u8g2.drawStr(16, 41, "FLASH :");

  sprintf(buf, "%d%%", flashUsage);
  u8g2.drawStr(70, 41, buf);

  // Temperature Icon
  u8g2.drawXBMP(0, 46, 16, 16, image_Temperature_16x16_bits);
  u8g2.drawStr(16, 59, "TEMPRETURE");

  sprintf(buf, "%dC", temperature);
  u8g2.drawStr(90, 59, buf);

  u8g2.sendBuffer();

  remote_sync();
  delay(500);
}