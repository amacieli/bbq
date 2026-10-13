// BBQ controller - scaffold: Wi-Fi + e-paper + SSE push + setpoint POST.
// Temperature is SIMULATED until the probe hardware is chosen.
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
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

AsyncWebServer server(80);
AsyncEventSource events("/events");

struct State { float temp = 20; int setpoint = 110; bool fan = false; } st;
volatile bool epdDirty = true;

static void drawScreen() {
  display.setRotation(0);  // landscape 400x300
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(6);
    display.setCursor(20, 90);
    display.printf("%.1f", st.temp);
    display.setTextSize(2);
    display.setCursor(20, 150);
    display.printf("Set %d  Fan %s", st.setpoint, st.fan ? "ON" : "off");
    display.setCursor(20, 270);
    display.print(WiFi.localIP());
    display.setTextSize(1);
    display.setCursor(20, 288);
    display.print("v" BUILD_VERSION);
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
  display.setFullWindow();

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);  // modem sleep makes the unit miss ARP/broadcast -> unreachable
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Wi-Fi");
  for (int i = 0; i < 60 && WiFi.status() != WL_CONNECTED; i++) { delay(500); Serial.print('.'); }
  Serial.printf("\nstatus=%d ip=%s\n", WiFi.status(), WiFi.localIP().toString().c_str());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *r) { r->send(200, "text/html", INDEX_HTML); });
  server.on("/api/setpoint", HTTP_POST, [](AsyncWebServerRequest *r) {
    if (r->hasParam("delta")) st.setpoint = constrain(st.setpoint + r->getParam("delta")->value().toInt(), 50, 400);
    pushState(); epdDirty = true;
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
  if (epdDirty && now - lastEpd >= 15000) { // e-paper: at most every 15 s
    lastEpd = now; epdDirty = false; drawScreen();
  }
  if (now - lastEpd >= 30000) epdDirty = true;
}
