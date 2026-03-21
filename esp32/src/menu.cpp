#include "menu.h"

// actions.cppの関数を使う
void actionWOL();
void actionShutdown();
void actionStatus();
void actionTemp();

MenuItem mainMenu[] = {
    {"Power", nullptr},
    {"Monitor", nullptr}};

MenuItem powerMenu[] = {
    {"Start", actionWOL},
    {"Shutdown", actionShutdown}};

MenuItem monitorMenu[] = {
    {"CPU", actionStatus},
    {"Temp", actionTemp}};

MenuItem *getCurrentMenu(int menu)
{
    if (menu == 0)
        return mainMenu;
    if (menu == 1)
        return powerMenu;
    if (menu == 2)
        return monitorMenu;
    return nullptr;
}

int getMenuSize(int menu)
{
    if (menu == 0)
        return sizeof(mainMenu) / sizeof(MenuItem);
    if (menu == 1)
        return sizeof(powerMenu) / sizeof(MenuItem);
    if (menu == 2)
        return sizeof(monitorMenu) / sizeof(MenuItem);
    return 0;
}