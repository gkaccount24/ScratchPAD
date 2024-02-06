#ifndef IMGUI_WINDOW_H
#define IMGUI_WINDOW_H

#include <SDL.h>

#include "./imgui/imgui.h"
#include "./imgui/backends/imgui_impl_sdl2.h"
#include "./imgui/backends/imgui_impl_sdlrenderer2.h"

#include <string_view>
#include <string>

using std::string_view;
using std::string;

class imgui_window
{

public:
	 imgui_window() = default;
	~imgui_window() = default;

public:

	void SetName(const string_view& Name);
	void SetFlags(ImGuiWindowFlags WindowFlags);

private:
	string			 Name;
	ImGuiWindowFlags Flags;

};

#endif
