#pragma once
#include <Arduino.h>
#include <vector>

struct Item {
  String name;
};

struct State {
  String menu;
  int cursor;
  std::vector<Item> items;
};