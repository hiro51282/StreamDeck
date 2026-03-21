#include <LiquidCrystal_I2C.h>
#include "display.h"
#include "menu.h"

// main.cppのlcdを参照
extern LiquidCrystal_I2C lcd;

// =========================
// 初期化
// =========================
void initDisplay()
{
    lcd.init();
    lcd.backlight();
}

// =========================
// メッセージ表示
// =========================
void showMessage(const char *line1, const char *line2)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

// =========================
// メニュー描画
// =========================
void drawMenu(int menu, int cursor)
{
    lcd.clear();

    MenuItem *items = getCurrentMenu(menu);

    lcd.setCursor(0, 0);
    lcd.print(cursor == 0 ? ">" : " ");
    lcd.print(items[0].label);

    lcd.setCursor(0, 1);
    lcd.print(cursor == 1 ? ">" : " ");
    lcd.print(items[1].label);
}

// =========================
// drawConfirm
// =========================
void drawConfirm(const char *title, int cursor)
{
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(title);

    lcd.setCursor(0, 1);

    if (cursor == 0)
        lcd.print("> NO   YES");
    else
        lcd.print("  NO  >YES");
}