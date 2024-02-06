#ifndef IMGUI_MAIN_MENU_BAR_H
#define IMGUI_MAIN_MENU_BAR_H

#include "defs.h"

#include "imgui_menu.h"

#include <string>
#include <string_view>
#include <cstdint>

using std::string;
using std::string_view;

#define IMGUI_MAX_MENU_COUNT 8 

class imgui_main_menubar
{
public:
     imgui_main_menubar() = default;
    ~imgui_main_menubar() = default;

public:
    void AddMenu(const string_view& MenuName, bool Active);
    imgui_menu* GetMenu(int MenuIndex);

public:
    void Hide();
    void Show();

private:
    imgui_menu Menus[IMGUI_MAX_MENU_COUNT];
    int        MenuCount;

    geometry   Geometry;
    bool       Visible;
};

#endif
