#include "./include/imgui_menu.h"

void imgui_menu::SetName(const string_view& MenuName) 
{
	Name = MenuName;
}

void imgui_menu::SetActive(bool MenuActive)
{
	Active = MenuActive;
}
