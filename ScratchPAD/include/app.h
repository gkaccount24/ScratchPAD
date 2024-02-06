#ifndef APP_H
#define APP_H

#include "defs.h"

#include "gfx.h"

#include "imgui_main_menubar.h"
#include "imgui_menu.h"
#include "imgui_table.h"

#include <SDL.h>

#include "./imgui/imgui.h"
#include "./imgui/backends/imgui_impl_sdl2.h"
#include "./imgui/backends/imgui_impl_sdlrenderer2.h"

#include <string_view>
#include <string>
#include <vector>

using std::string_view;
using std::string;
using std::vector;

#define APP_DEFAULT_WINDOW_XPOS SDL_WINDOWPOS_UNDEFINED
#define APP_DEFAULT_WINDOW_YPOS SDL_WINDOWPOS_UNDEFINED
#define APP_DEFAULT_WINDOW_WIDTH 800
#define APP_DEFAULT_WINDOW_HEIGHT 600

class app
{
public:
	bool Init(const string_view& Title, 
		      const geometry& WindowGeometry);

	bool Init(const string_view& Title);

	int Run();

private:
	void BuildMainMenubar();
	void BuildDirListingTable();
	void BuildUIManifest(const string_view& FilePath);

private:
	void Cleanup();

private:
	gfx				   Graphics;
	ui_manifest		   UIManifest;
	imgui_main_menubar MainMenubar;
	imgui_table		   DirListingTable;

	// exit status, 
	// error codes, etc..
	bool Exited;
	int  FailCode;
};

#endif
