#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "wifi_config.h"
#include "wol_config.h"
#include "api.h"

extern WiFiUDP udp;

#define PING_TIMEOUT 2000
#define MAX_RETRY 5
#define RETRY_DELAY 3000

bool pingServer() {
  HTTPClient http;
  String url = String("http://") + SERVER_IP + ":5000/ping?token=" + TOKEN;

  http.begin(url);
  http.setTimeout(PING_TIMEOUT);
  int code = http.GET();
  http.end();

  return code > 0;
}

void sendWOL() {
  byte mac[6] = WOL_MAC;
  byte packet[102];

  for (int i = 0; i < 6; i++) packet[i] = 0xFF;
  for (int i = 1; i <= 16; i++) memcpy(&packet[i * 6], mac, 6);

  udp.beginPacket("255.255.255.255", 9);
  udp.write(packet, sizeof(packet));
  udp.endPacket();
}

bool waitForServer() {
  if (pingServer()) return true;

  sendWOL();

  for (int i = 0; i < MAX_RETRY; i++) {
    delay(RETRY_DELAY);
    if (pingServer()) return true;
  }
  return false;
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

State fetchMenu() {
  HTTPClient http;
  State state;

  String url = String("http://") + SERVER_IP + ":5000/menu?token=" + TOKEN;

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode != 200) {
    http.end();
    return state;
  }

  String payload = http.getString();
  http.end();

  StaticJsonDocument<2048> doc;
  if (deserializeJson(doc, payload)) return state;

  state.menu = doc["menu"].as<String>();
  state.cursor = doc["cursor"];

  JsonArray arr = doc["items"].as<JsonArray>();

  for (JsonObject obj : arr) {
    Item item;
    item.name = obj["name"].as<String>();
    state.items.push_back(item);
  }

  return state;
}

void sendAction(String type) {
  HTTPClient http;

  String url = String("http://") + SERVER_IP + ":5000/action?token=" + TOKEN;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"type\":\"" + type + "\"}";
  http.POST(body);
  http.end();
}