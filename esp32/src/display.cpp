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
    int size = getMenuSize(menu);

    // スクロール開始位置
    int start = cursor;
    if (cursor > 0)
    {
        start = cursor - 1;
    }

    // 範囲補正（はみ出し防止）
    if (start < 0)
        start = 0;
    if (start >= size)
        start = size - 1;

    // --- 1行目 ---
    lcd.setCursor(0, 0);
    if (start < size)
    {
        lcd.print(cursor == start ? ">" : " ");
        lcd.print(items[start].label);
    }

    // --- 2行目 ---
    lcd.setCursor(0, 1);
    if (start + 1 < size)
    {
        lcd.print(cursor == start + 1 ? ">" : " ");
        lcd.print(items[start + 1].label);
    }
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