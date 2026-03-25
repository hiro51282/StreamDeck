#include "menu.h"

MenuItem mainMenu[] = {
    {"Power", nullptr, false},
    {"Monitor", nullptr, false}};

MenuItem powerMenu[] = {
    {"Start", "wol", false},
    {"Shutdown", "shutdown", false}};

MenuItem monitorMenu[] = {
    {"CPU", "status", true},
    {"Temp", "temp", true},
    {"Mem", "memory", true},
    {"Ping", "ping", true},
    {"Up", "uptime", true}};

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