#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include "wifi_config.h"
#include "display.h"
#include "menu.h"
#include "actions.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== ピン =====
#define SW 14
#define DT 27
#define CLK 26
#define BACK 21
#define I2C_SCL 22
#define I2C_SDA 23

WiFiUDP udp;

// ===== 状態 =====
enum State
{
  MENU,
  EXECUTING,
  CONFIRM
};

enum MenuType
{
  MENU_MAIN,
  MENU_POWER,
  MENU_MONITOR
};

State state = MENU;

const char *confirmTitle = "";
const char *confirmPath = nullptr;

int confirmCursor = 0;  // 0:NO 1:YES
int confirmTarget = -1; // どのアクションか識別

int lastCLK;
int cursor = 0;
int menu = MENU_MAIN;

unsigned long lastMove = 0;
unsigned long actionStart = 0;
unsigned long bootTime;
bool bootDone = false;

// ===== 関数 =====
void handleClick();
void handleWiFi();
void handleEncoder();
void handleButtons();

// =========================
// 初期化
// =========================
void setup()
{
  bootTime = millis();
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(BACK, INPUT_PULLUP);
  Wire.begin(I2C_SDA, I2C_SCL);
  initDisplay();

  showMessage("Hello", "Connecting...");
  delay(2000);

  lastCLK = digitalRead(CLK);

  drawMenu(menu, cursor);

  // WiFi（非ブロッキング）
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

// =========================
// メインループ
// =========================
void loop()
{
  handleWiFi();

  if (!bootDone)
  {
    if (millis() - bootTime > 2000)
    {
      bootDone = true;
      drawMenu(menu, cursor);
    }
    return;
  }

  handleEncoder();
  handleButtons();

  if (state == EXECUTING)
  {
    if (millis() - actionStart > 1000)
    {
      state = MENU;
      drawMenu(menu, cursor);
    }
  }
}

// =========================
// Wifi処理
// =========================
void handleWiFi()
{
  static bool connected = false;
  static unsigned long wifiMsgTime = 0;

  // 接続された瞬間
  if (!connected && WiFi.status() == WL_CONNECTED)
  {
    connected = true;

    showMessage("WiFi Connected", "");

    wifiMsgTime = millis();
  }

  // 一定時間後にメニューへ戻る
  if (connected && wifiMsgTime != 0 && millis() - wifiMsgTime > 1000)
  {
    wifiMsgTime = 0;
    drawMenu(menu, cursor);
  }
}
// =========================
// エンコーダ処理
// =========================
void handleEncoder()
{
  int currentCLK = digitalRead(CLK);

  if (currentCLK != lastCLK && currentCLK == LOW)
  {
    if (millis() - lastMove > 35)
    {
      // ===== CONFIRM =====
      if (state == CONFIRM)
      {
        confirmCursor = (confirmCursor + 1) % 2;
        drawConfirm(confirmTitle, confirmCursor);
      }
      // ===== MENU =====
      else if (state == MENU)
      {
        bool clockwise = (digitalRead(DT) == currentCLK);

        if (clockwise)
          cursor++;
        else
          cursor--;

        int maxItems = getMenuSize(menu);
        cursor = (cursor + maxItems) % maxItems;

        drawMenu(menu, cursor);
      }

      lastMove = millis();
    }
  }

  lastCLK = currentCLK;
}

// =========================
// ボタン処理
// =========================
void handleButtons()
{
  static unsigned long lastClick = 0;

  // 決定
  if (digitalRead(SW) == LOW && millis() - lastClick > 200)
  {
    lastClick = millis();

    if (state == CONFIRM)
    {
      if (confirmCursor == 1)
      {
        executeApi("Shutdown", confirmPath, false);
      }

      state = MENU;
      drawMenu(menu, cursor);
      return;
    }

    handleClick();
  }

  // 戻る（いつでも効く）
  if (digitalRead(BACK) == LOW && millis() - lastClick > 200)
  {
    lastClick = millis();

    state = MENU;
    menu = MENU_MAIN;
    cursor = 0;

    drawMenu(menu, cursor);

    return;
  }
}

// =========================
// 決定処理
// =========================
void handleClick()
{
  if (menu == MENU_MAIN)
  {
    menu = cursor + 1;
    cursor = 0;
    drawMenu(menu, cursor);
    return;
  }

  MenuItem *items = getCurrentMenu(menu);

  if (menu == MENU_POWER && cursor == 1)
  {
    confirmTitle = "Shutdown?";
    confirmPath = "shutdown";

    state = CONFIRM;
    confirmCursor = 0;

    drawConfirm(confirmTitle, confirmCursor);
    return;
  }

  MenuItem item = items[cursor];

  // メニュー遷移
  if (menu == MENU_MAIN)
  {
    menu = cursor + 1;
    cursor = 0;
    drawMenu(menu, cursor);
    return;
  }

  // WOLだけ特別処理
  if (strcmp(item.path, "wol") == 0)
  {
    executeWOL();
    return;
  }

  // Shutdownだけconfirm
  if (strcmp(item.path, "shutdown") == 0)
  {
    confirmTitle = "Shutdown?";
    confirmPath = "shutdown";

    state = CONFIRM;
    confirmCursor = 0;

    drawConfirm(confirmTitle, confirmCursor);
    return;
  }

  // 通常API
  executeApi(item.label, item.path, item.showResponse);
}
