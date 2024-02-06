#ifndef APP_GRAPHICS_H
#define APP_GRAPHICS_H

#include "defs.h"

#include "ui_manifest.h"

#include "imgui_main_menubar.h"
#include "imgui_table.h"

#include <SDL.h>

#include "./imgui/imgui.h"
#include "./imgui/backends/imgui_impl_sdl2.h"
#include "./imgui/backends/imgui_impl_sdlrenderer2.h"

#include <string_view>

#define GFX_DEFAULT_SDL_INIT_FLAGS (SDL_INIT_VIDEO | SDL_INIT_TIMER)
#define GFX_DEFAULT_SDL_WINDOW_FLAGS (SDL_WINDOW_ALLOW_HIGHDPI |\
									  SDL_WINDOW_MAXIMIZED |\
									  SDL_WINDOW_RESIZABLE |\
									  SDL_WINDOW_SHOWN)

#define GFX_DEFAULT_SDL_RENDERER_FLAGS SDL_RENDERER_ACCELERATED

using std::string_view;

class gfx
{
public:
     gfx() = default;
    ~gfx() = default;

public:
	// void DrawMainMenuBar(imgui_main_menubar* MainMenuBar);
	// void DrawTable(imgui_table* Table, geometry& Geometry);
	void DrawSplitterWindow();
	void Draw(ui_manifest* UI);

	void StartNewFrame();
	void SwapBuffers();

private:
	bool InitSDL(int Flags);

	bool CreateAppWindow(const string_view& Title, const geometry& Geometry, int SDLWindowMask);
	bool CreateAppWindow(const string_view& Title, const geometry& Geometry);

	bool CreateAppRenderer(int RenderFlags);
	bool CreateAppRenderer();

	void EnableImGuiIOKeyboardNav(ImGuiIO& IO);
	void InitImGui();

public:
	bool Init(const string_view& WindowTitle, const geometry& Geometry, int SDLWindowMask);
	bool Init(const string_view& WindowTitle, const geometry& Geometry);

	void Cleanup();

private:
	SDL_Window*		 WindowHandle;
	int              SDLWindowMask;
	ImGuiWindowFlags ImGuiWindowMask;
	geometry         WindowGeometry;
	string_view		 WindowTitle;

    SDL_Renderer*	 RenderHandle;
    int				 RenderFlags;
};

#endif
