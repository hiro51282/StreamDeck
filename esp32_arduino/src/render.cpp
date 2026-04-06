#include <LiquidCrystal_I2C.h>
#include "render.h"

extern LiquidCrystal_I2C lcd;
extern String lastLine1;
extern String lastLine2;

void show(const char *l1, const char *l2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2);
}

void render(State &state) {
  int total = state.items.size();
  if (total == 0) return;

  int top = (state.cursor == 0) ? 0 : state.cursor - 1;

  String line1 = "";
  String line2 = "";

  if (top < total) {
    line1 += (top == state.cursor) ? ">" : " ";
    line1 += state.items[top].name;
  }

  if (top + 1 < total) {
    line2 += (top + 1 == state.cursor) ? ">" : " ";
    line2 += state.items[top + 1].name;
  }

  if (line1 != lastLine1) {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lastLine1 = line1;
  }

  if (line2 != lastLine2) {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lastLine2 = line2;
  }
}