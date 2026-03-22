#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include "wol_config.h"
#include "wifi_config.h"

// 外部参照（main.cppにあるやつ）
extern LiquidCrystal_I2C lcd;
extern WiFiUDP udp;

extern int state;
extern unsigned long actionStart;

#define EXECUTING 1

// =========================
// sendWOL
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
// executeWOL
// =========================
void executeWOL()
{
    lcd.clear();
    lcd.print("Sending WOL...");
    sendWOL();

    lcd.setCursor(0, 1);
    lcd.print("Done");

    state = EXECUTING;
    actionStart = millis();
}

// =========================
// executeAction
// =========================
void executeAction(const char *title, const char *url, bool showResponse)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(title);
    lcd.setCursor(0, 1);
    lcd.print("Sending...");

    HTTPClient http;
    http.begin(url);
    http.setTimeout(2000);

    int httpCode = http.GET();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(title);

    lcd.setCursor(0, 1);

    if (httpCode > 0)
    {
        String payload = http.getString();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(title);

        lcd.setCursor(0, 1);
        lcd.print(payload);
    }
    else
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(title);

        lcd.setCursor(0, 1);
        lcd.print("HTTP FAIL");
    }

    http.end();

    state = EXECUTING;
    actionStart = millis();
}

void actionWOL()
{
  executeWOL();
}

void actionShutdown()
{
    executeAction("Shutdown", "http://" SERVER_IP ":5000/shutdown?token=" TOKEN, false);
}

void actionStatus()
{
    executeAction("Status", "http://" SERVER_IP ":5000/status?token=" TOKEN, true);
}

void actionTemp()
{
    executeAction("Temp", "http://" SERVER_IP ":5000/temp?token=" TOKEN, true);
}

void actionMemory()
{
    String url = "http://" + String(SERVER_IP) + ":5000/memory?token=" + String(TOKEN);
    executeAction("Memory", url.c_str(), true);
}

void actionPing()
{
    String url = "http://" + String(SERVER_IP) + ":5000/ping?token=" + String(TOKEN);
    executeAction("Ping", url.c_str(), true);
}

void actionUptime()
{
    String url = "http://" + String(SERVER_IP) + ":5000/uptime?token=" + String(TOKEN);
    executeAction("Uptime", url.c_str(), true);
}