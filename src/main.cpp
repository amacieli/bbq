// BBQ controller - scaffold: Wi-Fi + e-paper + SSE push + setpoint POST.
// Temperature is SIMULATED until the probe hardware is chosen.
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <ESPAsyncWebServer.h>
#include "pins.h"
#include "version.h"
#include "web_index.h"
#if __has_include("secrets.h")
#include "secrets.h"
#else
#error "Copy src/secrets.h.example to src/secrets.h and fill in Wi-Fi credentials"
#endif

// Waveshare 4.2" V2 (SSD1683, 400x300) == GDEY042T81 in GxEPD2
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
    GxEPD2_420_GDEY042T81(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

U8G2_FOR_ADAFRUIT_GFX u8g2;   // smooth big fonts (Adafruit GFX built-ins are tiny/blocky)
AsyncWebServer server(80);
AsyncEventSource events("/events");

struct State { float temp = 20; int setpoint = 110; bool fan = false; } st;

static void centerText(const char *t, int y) {
  u8g2.setCursor((display.width() - u8g2.getUTF8Width(t)) / 2, y);
  u8g2.print(t);
}

// Landscape 400x300. Big temperature (logisoso92 digits), status line, version footer.
static void drawScreen(bool full) {
  char temp[16], line[48];
  snprintf(temp, sizeof temp, "%.1f", st.temp);
  snprintf(line, sizeof line, "Set %d    Fan %s", st.setpoint, st.fan ? "ON" : "off");
  display.setRotation(0);
  if (full) display.setFullWindow(); else display.setPartialWindow(0, 0, display.width(), display.height());
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    u8g2.setFontMode(1);
    u8g2.setForegroundColor(GxEPD_BLACK);
    u8g2.setBackgroundColor(GxEPD_WHITE);
    u8g2.setFont(u8g2_font_helvB14_tf);
    u8g2.setCursor(10, 24);  u8g2.print("BBQ");
    u8g2.setFont(u8g2_font_helvR10_tf);
    u8g2.setCursor(display.width() - u8g2.getUTF8Width("\xC2\xB0" "C") - 10, 24); u8g2.print("\xC2\xB0" "C");
    u8g2.setFont(u8g2_font_logisoso92_tn);
    centerText(temp, 160);
    display.drawFastHLine(10, 185, display.width() - 20, GxEPD_BLACK);
    u8g2.setFont(u8g2_font_helvB18_tf);
    centerText(line, 225);
    u8g2.setFont(u8g2_font_helvR10_tf);
    u8g2.setCursor(10, 290);  u8g2.print(WiFi.localIP().toString());
    char ver[32]; snprintf(ver, sizeof ver, "v" BUILD_VERSION);
    u8g2.setCursor(display.width() - u8g2.getUTF8Width(ver) - 10, 290); u8g2.print(ver);
  } while (display.nextPage());
}

static void pushState() {
  char buf[96];
  snprintf(buf, sizeof buf, "{\"temp\":%.1f,\"setpoint\":%d,\"fan\":%s}",
           st.temp, st.setpoint, st.fan ? "true" : "false");
  events.send(buf, "state", millis());
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nBBQ scaffold booting, version " BUILD_VERSION);

  SPI.begin(EPD_SCK, -1, EPD_MOSI, EPD_CS);
  Serial.println("display init...");
  display.init(115200, true, 2, false);
  Serial.println("display ok");
  display.setRotation(0);   // landscape, 400x300
  u8g2.begin(display);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);  // modem sleep makes the unit miss ARP/broadcast -> unreachable
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Wi-Fi");
  for (int i = 0; i < 60 && WiFi.status() != WL_CONNECTED; i++) { delay(500); Serial.print('.'); }
  Serial.printf("\nstatus=%d ip=%s\n", WiFi.status(), WiFi.localIP().toString().c_str());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *r) { r->send(200, "text/html", INDEX_HTML); });
  server.on("/api/setpoint", HTTP_POST, [](AsyncWebServerRequest *r) {
    if (r->hasParam("delta")) st.setpoint = constrain(st.setpoint + r->getParam("delta")->value().toInt(), 50, 400);
    pushState();
    r->send(204);
  });
  server.addHandler(&events);
  server.begin();
}

void loop() {
  static uint32_t lastSample = 0, lastEpd = 0;
  uint32_t now = millis();
  static uint32_t lastBeat = 0;
  if (now - lastBeat >= 5000) { lastBeat = now; Serial.printf("v" BUILD_VERSION " [%lus] wifi=%d ip=%s heap=%u psram=%u temp=%.1f\n", now/1000, WiFi.status(), WiFi.localIP().toString().c_str(), ESP.getFreeHeap(), ESP.getFreePsram(), st.temp); }
  if (now - lastSample >= 1000) {          // simulated temperature drifting toward setpoint
    lastSample = now;
    st.temp += (st.setpoint - st.temp) * 0.02f;
    st.fan = st.temp < st.setpoint;
    pushState();
  }
  if (now - lastEpd >= 5000) {              // e-paper refresh every 5 s (partial); full refresh every ~10 min to clear ghosting
    lastEpd = now;
    static uint32_t n = 0;
    drawScreen(n++ % 120 == 0);
  }
}
