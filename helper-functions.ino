

void showMessageBox(const char* msg, uint16_t duration = 2000) {
  u8g2.clearBuffer();

  // Box dimensions
  int boxX = 14;
  int boxY = 20;
  int boxW = 100;
  int boxH = 24;

  // Draw box
  u8g2.drawFrame(boxX, boxY, boxW, boxH);

  // Center text
  u8g2.setFont(u8g2_font_6x12_tr);
  int textW = u8g2.getStrWidth(msg);
  int textX = boxX + (boxW - textW) / 2;
  int textY = boxY + 16;

  u8g2.drawStr(textX, textY, msg);

  u8g2.sendBuffer();

  delay(duration);
}

