#include <Arduino.h>
#include "input.h"

// ピン
#define SW 14
#define DT 27
#define CLK 26
#define BACK 21

bool rotatedRight = false;
bool rotatedLeft = false;
bool pressed = false;
bool backPressed = false;

int lastCLK = HIGH;
int lastSW = HIGH;
int lastBack = HIGH;

unsigned long lastRotateTime = 0;
const int ROTATE_DELAY = 120;

void initInput() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(BACK, INPUT_PULLUP);
}

void readEncoder() {
  rotatedRight = false;
  rotatedLeft = false;

  int clk = digitalRead(CLK);

  if (lastCLK == HIGH && clk == LOW) {
    if (millis() - lastRotateTime > ROTATE_DELAY) {
      int dt = digitalRead(DT);

      if (dt == LOW) rotatedRight = true;
      else rotatedLeft = true;

      lastRotateTime = millis();
    }
  }

  lastCLK = clk;
}

void readButtons() {
  pressed = false;
  backPressed = false;

  int sw = digitalRead(SW);
  int back = digitalRead(BACK);

  if (lastSW == HIGH && sw == LOW) pressed = true;
  if (lastBack == HIGH && back == LOW) backPressed = true;

  lastSW = sw;
  lastBack = back;
}