#include "./include/imgui_window.h"

void imgui_window::SetName(const string_view& WindowName)
{
	Name = WindowName;
}

void imgui_window::SetFlags(ImGuiWindowFlags WindowFlags)
{
	Flags = WindowFlags;
}

