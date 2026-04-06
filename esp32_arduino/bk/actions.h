#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>

extern LiquidCrystal_I2C lcd;
extern WiFiUDP udp;

void executeWOL();
void executeAction(const char *title, const char *url, bool showResponse);
String buildUrl(const char *path);
void executeApi(const char *title, const char *path, bool showResponse);
