/*
 *  BUTTON CALLBACKS — define any/all of these BEFORE the
 *  #include and they will be called on both physical press
 *  AND web button tap:
 *
 *      void onBtnUp()     { ... }
 *      void onBtnDown()   { ... }
 *      void onBtnLeft()   { ... }
 *      void onBtnRight()  { ... }
 *      void onBtnSelect() { ... }
 *      void onBtnBack()   { ... }
 *
 *  OLED — just use u8g2 as normal. After every sendBuffer()
 *  the framebuffer is automatically mirrored to the web.
 *  If you want manual sync:  remote_sync();
 *
 *  EXAMPLE USAGE:
 *
 *      void onBtnUp()   { Serial.println("UP pressed"); }
 *      void onBtnDown() { Serial.println("DOWN pressed"); }
 *      #include "FlipperESP_Remote.h"
 *
 *      void setup() {
 *        remote_begin();          // init WiFi + web server
 *        u8g2.clearBuffer();
 *        u8g2.setFont(u8g2_font_profont12_mr);
 *        u8g2.drawStr(0,30,"Hello world");
 *        u8g2.sendBuffer();
 *        remote_sync();           // push to web (optional)
 *      }
 *
 *      void loop() {
 *        remote_loop();           // handle web + buttons
 *      }
 *
 *  ARDUINO IDE SETTINGS:
 *    Board              : ESP32S3 Dev Module
 *    USB CDC On Boot    : Enabled
 *    Partition Scheme   : Huge APP (3MB No OTA)
 *    Flash Size         : 16MB
 *    CPU Frequency      : 240MHz
 * 
 * ============================================================
 */
 
#pragma once
 
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <U8g2lib.h>
#include <Wire.h>
 

#ifndef REMOTE_SSID
  #define REMOTE_SSID "HIZMOS"
#endif
#ifndef REMOTE_PASS
  #define REMOTE_PASS "12345678"
#endif
 

#ifndef REMOTE_OLED_TYPE
  #define REMOTE_OLED_TYPE U8G2_SH1106_128X64_NONAME_F_HW_I2C
#endif
 

REMOTE_OLED_TYPE u8g2(U8G2_R0, U8X8_PIN_NONE);
 

#ifndef PIN_BTN_UP
  #define PIN_BTN_UP     4
#endif
#ifndef PIN_BTN_DOWN
  #define PIN_BTN_DOWN   5
#endif
#ifndef PIN_BTN_SELECT
  #define PIN_BTN_SELECT 6
#endif
#ifndef PIN_BTN_BACK
  #define PIN_BTN_BACK   7
#endif
#ifndef PIN_BTN_LEFT
  #define PIN_BTN_LEFT   1
#endif
#ifndef PIN_BTN_RIGHT
  #define PIN_BTN_RIGHT  2
#endif
 

__attribute__((weak)) void onBtnUp()     {}
__attribute__((weak)) void onBtnDown()   {}
__attribute__((weak)) void onBtnLeft()   {}
__attribute__((weak)) void onBtnRight()  {}
__attribute__((weak)) void onBtnSelect() {}
__attribute__((weak)) void onBtnBack()   {}
 

static uint8_t _remoteBuf[1024];
 
inline void remote_sync() {
  memcpy(_remoteBuf, u8g2.getBufferPtr(), 1024);
}
 
// ── Web server ───────────────────────────────────────────────
static WebServer _srv(80);
 
 static const char _HTML[] PROGMEM = R"===(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
<title>HIZMOS Remote</title>

<style>
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent}

:root{
  --cy:#ff8c00;
  --cy2:#ff6a00;
  --bg:#0b0602;
  --surf:#140a03;
  --bord:#2a1405;
  --dim:#5a2d0a;
  --txt:#ffd9a0;
}

body{
  background:var(--bg);color:var(--txt);
  font-family:'Courier New',monospace;
  min-height:100vh;
  display:flex;flex-direction:column;align-items:center;
  padding:14px 10px 28px;

  background-image:
    repeating-linear-gradient(0deg,transparent,transparent 31px,rgba(255,140,0,.03) 32px),
    repeating-linear-gradient(90deg,transparent,transparent 31px,rgba(255,140,0,.03) 32px);
}

header{
  width:100%;max-width:440px;
  background:var(--surf);
  border:1px solid var(--bord);
  border-radius:8px;
  padding:9px 14px;
  display:flex;justify-content:space-between;align-items:center;
  margin-bottom:14px;
}

.logo{font-size:.9rem;letter-spacing:3px;color:var(--cy)}

.dot{
  width:8px;height:8px;border-radius:50%;
  background:var(--cy);
  box-shadow:0 0 7px var(--cy);
  animation:blink 2s infinite
}

@keyframes blink{50%{opacity:.2}}

/* OLED */
.oled-wrap{
  width:100%;max-width:440px;
  background:#050200;
  border:2px solid var(--bord);
  border-radius:12px;
  padding:14px;
  margin-bottom:16px;
  box-shadow:inset 0 0 40px rgba(0,0,0,.95),
             0 0 25px rgba(255,140,0,.15);
}

#oled{
  width:384px;height:192px;
  image-rendering:pixelated;
  border:1px solid var(--bord);
  background:#050200;
  box-shadow:
    0 0 25px rgba(255,140,0,.25),
    inset 0 0 12px rgba(0,0,0,.8);
}

/* buttons */
.db,.ab{
  background:var(--surf);
  border:1px solid var(--bord);
  color:var(--txt);
  cursor:pointer;
  transition:.1s;
}

.db:active,.ab:active{
  background:var(--cy);
  color:#000;
  transform:scale(.85);
  box-shadow:0 0 15px var(--cy);
}

.ctrl{
  width:100%;max-width:440px;
  display:grid;
  grid-template-columns:1fr auto 1fr;
  gap:10px;
  align-items:center;
}

.dpad{
  display:grid;
  grid-template-columns:repeat(3,50px);
  grid-template-rows:repeat(3,50px);
  gap:4px;
}

.empty{background:none;border:none}

.info{
  margin-top:10px;
  width:100%;max-width:440px;
  font-size:.7rem;
  color:var(--dim);
  display:flex;
  justify-content:space-between;
}

/* theme buttons */
.theme-bar{
  margin-top:10px;
  display:flex;
  gap:6px;
}

.theme-bar button{
  padding:6px 10px;
  border-radius:6px;
  border:1px solid var(--bord);
  background:var(--surf);
  color:var(--txt);
  cursor:pointer;
}
</style>
</head>

<body>

<header>
  <span class="logo">HIZMOS CONTROL</span>
  <div class="dot"></div>
</header>

<div class="oled-wrap">
  <canvas id="oled" width="128" height="64"></canvas>
</div>

<div class="ctrl">
  <div></div>

  <div class="dpad">
    <div class="empty"></div>
    <button class="db" onclick="send('UP')">▲</button>
    <div class="empty"></div>

    <button class="db" onclick="send('LEFT')">◀</button>
    <button class="db" onclick="send('SELECT')">OK</button>
    <button class="db" onclick="send('RIGHT')">▶</button>

    <div class="empty"></div>
    <button class="db" onclick="send('DOWN')">▼</button>
    <div class="empty"></div>
  </div>

  <div></div>
</div>

<div class="theme-bar">
  <button onclick="setTheme('cyan')">Cyan</button>
  <button onclick="setTheme('orange')">Orange</button>
  <button onclick="setTheme('green')">Green</button>
  <button onclick="setTheme('matrix')">Matrix</button>
</div>

<div class="info">
  <span>192.168.4.1</span>
  <span id="sta">STA:0</span>
</div>

<script>

/*  THEMES */
const THEMES = {
  cyan:   {accent:"#00e5ff", accent2:"#00b8d4", on:[0,229,255]},
  orange: {accent:"#ff8c00", accent2:"#ff6a00", on:[255,140,0]},
  green:  {accent:"#00ff99", accent2:"#00cc77", on:[0,255,153]},
  matrix: {accent:"#00ff00", accent2:"#00aa00", on:[0,255,0]}
};

let currentTheme = "orange";

/* set theme */
function setTheme(name){
  currentTheme = name;
  const t = THEMES[name];
  document.documentElement.style.setProperty('--cy', t.accent);
  document.documentElement.style.setProperty('--cy2', t.accent2);
}

/* OLED */
const ctx = document.getElementById('oled').getContext('2d');

function renderHex(hex){
  const img = ctx.createImageData(128,64);
  const d = img.data;

  for(let page=0;page<8;page++){
    for(let col=0;col<128;col++){
      const byte = parseInt(hex.substr((page*128+col)*2,2),16);

      for(let bit=0;bit<8;bit++){
        const px=((page*8+bit)*128+col)*4;

        if((byte>>bit)&1){
          const c = THEMES[currentTheme].on;
          d[px]=c[0];
          d[px+1]=c[1];
          d[px+2]=c[2];
        }else{
          d[px]=10;
          d[px+1]=5;
          d[px+2]=0;
        }
        d[px+3]=255;
      }
    }
  }

  ctx.putImageData(img,0,0);
}

/* polling */
setInterval(async()=>{
  try{
    const r=await fetch('/pixels');
    if(r.ok) renderHex(await r.text());
  }catch(e){}
},150);

/* buttons */
function send(btn){
  fetch('/btn?id='+btn);
}

/* status */
setInterval(async()=>{
  try{
    const d=await (await fetch('/status')).json();
    document.getElementById('sta').textContent='STA:'+d.clients;
  }catch(e){}
},3000);

</script>

</body>
</html>
)===";


static void _serveRoot()   { _srv.send_P(200,"text/html",_HTML); }
 
static void _servePixels() {
  char hex[2049];
  static const char h[]="0123456789abcdef";
  for(int i=0;i<1024;i++){
    hex[i*2]  =h[(_remoteBuf[i]>>4)&0xF];
    hex[i*2+1]=h[ _remoteBuf[i]    &0xF];
  }
  hex[2048]=0;
  _srv.send(200,"text/plain",hex);
}
 
static void _serveBtn() {
  if(!_srv.hasArg("id")){_srv.send(400,"text/plain","bad");return;}
  String btn = _srv.arg("id");
  if(btn=="UP")     onBtnUp();
  else if(btn=="DOWN")   onBtnDown();
  else if(btn=="LEFT")   onBtnLeft();
  else if(btn=="RIGHT")  onBtnRight();
  else if(btn=="SELECT") onBtnSelect();
  else if(btn=="BACK")   onBtnBack();
  remote_sync();
  _srv.send(200,"text/plain","ok");
}
 
static void _serveStatus(){
  String j="{\"clients\":"+String(WiFi.softAPgetStationNum())+"}";
  _srv.send(200,"application/json",j);
}
 
// ── Physical button state ────────────────────────────────
struct _Btn { uint8_t pin; void(*cb)(); bool last; };
static _Btn _btns[] = {
  {PIN_BTN_UP,    onBtnUp,    HIGH},
  {PIN_BTN_DOWN,  onBtnDown,  HIGH},
  {PIN_BTN_LEFT,  onBtnLeft,  HIGH},
  {PIN_BTN_RIGHT, onBtnRight, HIGH},
  {PIN_BTN_SELECT,onBtnSelect,HIGH},
  {PIN_BTN_BACK,  onBtnBack,  HIGH},
};
 
static void _pollButtons(){
  for(auto& b:_btns){
    bool cur=digitalRead(b.pin);
    if(cur==LOW && b.last==HIGH){ b.cb(); remote_sync(); }
    b.last=cur;
  }
}
 

static unsigned long _lastSync=0;
 

inline void remote_begin() {
  // Serial
  if(!Serial) { Serial.begin(115200); delay(1500); }
  Serial.println("\nhizmos webpage] starting...");
 
  // Button pins
  pinMode(PIN_BTN_UP,    INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN,  INPUT_PULLUP);
  pinMode(PIN_BTN_LEFT,  INPUT_PULLUP);
  pinMode(PIN_BTN_RIGHT, INPUT_PULLUP);
  pinMode(PIN_BTN_SELECT,INPUT_PULLUP);
  pinMode(PIN_BTN_BACK,  INPUT_PULLUP);
 

  u8g2.begin();
  memset(_remoteBuf,0,sizeof(_remoteBuf));
 

  WiFi.persistent(false);
  WiFi.disconnect(true,true);
  delay(500);
  WiFi.mode(WIFI_OFF);
  delay(500);
  WiFi.mode(WIFI_AP);
  delay(200);
  WiFi.softAPConfig(
    IPAddress(192,168,4,1),
    IPAddress(192,168,4,1),
    IPAddress(255,255,255,0)
  );
  delay(200);
  bool ok=WiFi.softAP(REMOTE_SSID,REMOTE_PASS,6,0,4);
  if(!ok) WiFi.softAP(REMOTE_SSID);
  delay(500);
  Serial.printf("[Remote] AP IP: %s  SSID: %s\n",
    WiFi.softAPIP().toString().c_str(), REMOTE_SSID);
 
  // Routes
  _srv.on("/",       HTTP_GET, _serveRoot);
  _srv.on("/pixels", HTTP_GET, _servePixels);
  _srv.on("/btn",    HTTP_GET, _serveBtn);
  _srv.on("/status", HTTP_GET, _serveStatus);
  _srv.begin();
  Serial.println("[Remote] HTTP server started → http://192.168.4.1");
}
 
inline void remote_loop() {
  _srv.handleClient();
  _pollButtons();

  unsigned long now=millis();
  if(now-_lastSync>=100){ remote_sync(); _lastSync=now; }
}
 