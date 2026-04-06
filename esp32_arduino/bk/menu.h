#pragma once

struct MenuItem
{
    const char *label;
    const char *path;
    bool showResponse;
};

MenuItem *getCurrentMenu(int menu);
int getMenuSize(int menu);