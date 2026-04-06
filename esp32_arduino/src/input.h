#pragma once

extern bool rotatedRight;
extern bool rotatedLeft;
extern bool pressed;
extern bool backPressed;

void initInput();
void readEncoder();
void readButtons();