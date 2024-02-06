#ifndef IMGUI_MENU_H
#define IMGUI_MENU_H

#include <string_view>
#include <string>

using std::string_view;
using std::string;

#define MAX_MENU_ITEM_COUNT 64

enum imgui_menu_item_behavior
{
    OpensSubmenu,
    RunsAnAction
};

struct imgui_menu_item
{
    /***
     *
     * some notes:
     * a menu item can point to a menu, or
     * a menu item when clicked can run an action
    */

    imgui_menu_item_behavior Behavior;
    string                   Text;
};

class imgui_menu
{

public:
     imgui_menu() = default;
    ~imgui_menu() = default;

public:
    void SetName(const string_view& MenuName);
    void SetActive(bool MenuActive);

private:
    string          Name;
    bool            Active;

    imgui_menu_item MenuItems[MAX_MENU_ITEM_COUNT];
    size_t          MenuItemCount;
};

#endif
