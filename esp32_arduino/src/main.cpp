#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "wifi_config.h"
#include "wol_config.h"

// ===== ピン =====
#define SW 14
#define DT 27
#define CLK 26
#define BACK 21
#define I2C_SCL 22
#define I2C_SDA 23

// ===== KY040 =====
bool clkStateChanged = false;
bool rotatedRight = false;
bool rotatedLeft = false;
int lastCLK = HIGH;
int lastDT = HIGH;

unsigned long lastFetch = 0;
int lastEncoded = 0;
int encoderValue = 0;
int stepCount = 0;

unsigned long lastRotateTime = 0;
const int ROTATE_DELAY = 120; // ms

// ===== BackButton&KY040Push =====
bool pressed = false;
bool backPressed = false;

int lastSW = HIGH;
int lastBack = HIGH;

String pendingAction = "";
String lastLine1 = "";
String lastLine2 = "";
struct Item
{
  String name;
};

struct State
{
  String menu;
  int cursor;
  std::vector<Item> items;
};

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP udp;

// =========================
// 設定
// =========================
#define PING_TIMEOUT 2000
#define MAX_RETRY 5
#define RETRY_DELAY 3000

// =========================
// 表示
// =========================
void show(const char *l1, const char *l2 = "")
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2);
}

// =========================
// WiFi接続
// =========================
void connectWiFi()
{
  show("WiFi...", "Connecting");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    lcd.print(".");
  }

  show("WiFi OK", WiFi.localIP().toString().c_str());
  delay(1000);
}

// =========================
// ping
// =========================
bool pingServer()
{
  HTTPClient http;

  String url = String("http://") + SERVER_IP + ":5000/ping?token=" + TOKEN;

  http.begin(url);
  http.setTimeout(PING_TIMEOUT);

  int code = http.GET();
  http.end();

  return code > 0;
}

// =========================
// WOL送信
// =========================
void sendWOL()
{
  byte mac[6] = WOL_MAC;
  byte packet[102];

  for (int i = 0; i < 6; i++)
    packet[i] = 0xFF;

  for (int i = 1; i <= 16; i++)
    memcpy(&packet[i * 6], mac, 6);

  udp.beginPacket("255.255.255.255", 9);
  udp.write(packet, sizeof(packet));
  udp.endPacket();
}

// =========================
// サーバ起動待ち
// =========================
bool waitForServer()
{
  show("Checking...", "Server");

  if (pingServer())
    return true;

  show("Waking PC...", "");
  sendWOL();

  for (int i = 0; i < MAX_RETRY; i++)
  {
    show("Retry...", String(i + 1).c_str());

    delay(RETRY_DELAY);

    if (pingServer())
      return true;
  }

  return false;
}

// =========================
// setup
// =========================
void setup()
{
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(BACK, INPUT_PULLUP);
  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  show("Booting...", "");
  delay(1000);

  // WiFi
  connectWiFi();

  // サーバ確認
  bool ok = waitForServer();

  if (ok)
  {
    show("Server OK", "");
  }
  else
  {
    show("ERROR", "Server Down");
  }
}

State fetchMenu()
{
  HTTPClient http;
  State state;

  String url = String("http://") + SERVER_IP + ":5000/menu?token=" + TOKEN;

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode != 200)
  {
    Serial.println("HTTP ERROR");
    http.end();
    return state;
  }

  String payload = http.getString();
  http.end();

  Serial.println(payload);

  // JSONパース
  StaticJsonDocument<2048> doc;
  DeserializationError err = deserializeJson(doc, payload);

  if (err)
  {
    Serial.println("JSON parse error");
    return state;
  }

  state.menu = doc["menu"].as<String>();
  state.cursor = doc["cursor"];

  JsonArray arr = doc["items"].as<JsonArray>();

  for (JsonObject obj : arr)
  {
    Item item;
    item.name = obj["name"].as<String>();
    state.items.push_back(item);
  }

  return state;
}
void render(State &state)
{
  int total = state.items.size();
  if (total == 0)
    return;

  int top = (state.cursor == 0) ? 0 : state.cursor - 1;

  String line1 = "";
  String line2 = "";

  // --- 1行目 ---
  if (top < total)
  {
    line1 += (top == state.cursor) ? ">" : " ";
    line1 += state.items[top].name;
  }

  // --- 2行目 ---
  if (top + 1 < total)
  {
    line2 += (top + 1 == state.cursor) ? ">" : " ";
    line2 += state.items[top + 1].name;
  }

  // --- 差分描画 ---
  if (line1 != lastLine1)
  {
    lcd.setCursor(0, 0);
    lcd.print("                "); // 消す
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lastLine1 = line1;
  }

  if (line2 != lastLine2)
  {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lastLine2 = line2;
  }
}
void sendAction(String type)
{
  HTTPClient http;

  String url = String("http://") + SERVER_IP + ":5000/action?token=" + TOKEN;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"type\":\"" + type + "\"}";

  int code = http.POST(body);

  Serial.println(code);

  http.end();
}
void readEncoder()
{
  rotatedRight = false;
  rotatedLeft = false;

  int clk = digitalRead(CLK);

  if (lastCLK == HIGH && clk == LOW)
  {

    if (millis() - lastRotateTime > ROTATE_DELAY)
    {

      int dt = digitalRead(DT);

      if (dt == LOW)
      {
        rotatedRight = true;
      }
      else
      {
        rotatedLeft = true;
      }

      lastRotateTime = millis();
    }
  }

  lastCLK = clk;
}
void readButtons()
{
  pressed = false;
  backPressed = false;

  int sw = digitalRead(SW);
  int back = digitalRead(BACK);

  // 押した瞬間だけ拾う
  if (lastSW == HIGH && sw == LOW)
  {
    pressed = true;
  }

  if (lastBack == HIGH && back == LOW)
  {
    backPressed = true;
  }

  lastSW = sw;
  lastBack = back;
}
// =========================
// loop
// =========================
void loop()
{

  readEncoder();
  readButtons();

  // 入力は即記録
  if (rotatedRight)
    pendingAction = "down";
  if (rotatedLeft)
    pendingAction = "up";
  if (pressed)
    pendingAction = "select";
  if (backPressed)
    pendingAction = "back";

  // 通信は1回だけ
  if (pendingAction != "")
  {
    sendAction(pendingAction);
    pendingAction = "";
  }

  if (millis() - lastFetch > 50)
  {
    State state = fetchMenu();
    render(state);
    lastFetch = millis();
  }
}