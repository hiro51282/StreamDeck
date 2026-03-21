#pragma once

struct MenuItem
{
  const char *label;
  void (*action)();
};

MenuItem *getCurrentMenu(int menu);
int getMenuSize(int menu);