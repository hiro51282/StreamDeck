#pragma once
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>

extern LiquidCrystal_I2C lcd;
extern WiFiUDP udp;

void executeWOL();
void executeAction(const char *title, const char *url, bool showResponse);
void actionWOL();
void actionShutdown();
void actionStatus();
void actionTemp();
void actionMemory();
void actionPing();
void actionUptime();
