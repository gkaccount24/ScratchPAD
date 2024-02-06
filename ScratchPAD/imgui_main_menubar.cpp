#include "./include/imgui_main_menubar.h"

#define ERROR_MAX_MENUS_REACHED -100

void imgui_main_menubar::Hide()
{
	Visible = false;
}

void imgui_main_menubar::Show()
{
	Visible = true;
}

imgui_menu* imgui_main_menubar::GetMenu(int MenuIndex)
{
	if(MenuIndex < IMGUI_MAX_MENU_COUNT)
	{
		imgui_menu* Menu = &Menus[MenuIndex];

		return Menu;
	}

	return nullptr;
}

void imgui_main_menubar::AddMenu(const string_view& MenuName, bool MenuActive)
{
	if(MenuCount < IMGUI_MAX_MENU_COUNT)
	{
		Menus[MenuCount].SetName(MenuName);
		Menus[MenuCount].SetActive(MenuActive);

		MenuCount++;
	}
}
