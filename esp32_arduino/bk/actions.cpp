#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include "wol_config.h"
#include "wifi_config.h"
#include "actions.h"
#include "display.h"

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
    showMessage(title, "Sending...");

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
        showMessage(title, payload.c_str());
    }
    else
    {
        showMessage(title, "HTTP FAIL");
    }

    http.end();

    state = EXECUTING;
    actionStart = millis();
}

String buildUrl(const char *path)
{
    return String("http://") + SERVER_IP + ":5000/" + path + "?token=" + TOKEN;
}

void executeApi(const char *title, const char *path, bool showResponse)
{
    String url = buildUrl(path);
    executeAction(title, url.c_str(), showResponse);
}
