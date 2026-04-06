#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>

#include "api.h"
#include "render.h"
#include "input.h"
#include "model.h"

// ===== グローバル =====
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP udp;

String lastLine1 = "";
String lastLine2 = "";
String pendingAction = "";

unsigned long lastFetch = 0;

#define I2C_SCL 22
#define I2C_SDA 23

void setup()
{
  // ----------------------------------
  initInput();
  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  show("Booting...", "");
  delay(100);

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

  delay(500);
}

void loop()
{
  readEncoder();
  readButtons();

  if (rotatedRight)
    pendingAction = "down";
  if (rotatedLeft)
    pendingAction = "up";
  if (pressed)
    pendingAction = "select";
  if (backPressed)
    pendingAction = "back";

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