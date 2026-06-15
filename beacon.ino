void beaconflood() {
   u8g2.clearBuffer();
    // string 2
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(36, 14, "beacon flood");
    
    // string 2
    u8g2.drawStr(42, 33, "started");
    u8g2.sendBuffer();
     beacon.flood(50, 200); 
}