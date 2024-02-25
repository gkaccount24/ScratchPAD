#include "game_window.h"

namespace scratchpad
{
	game_window::game_window(const char* WindowTitle,
						     geometry WindowGeometry,
						     int WindowFlags,
						     scratchpad::game_diagnostics& GameDiag):
		Handle(nullptr),
		RenderHandle(nullptr),
		Geometry(WindowGeometry),
		Flags(WindowFlags),
		Diagnostics(GameDiag)
	{

	}

	game_window::~game_window()
	{
		Destroy();

	}

	bool game_window::Create()
	{
		Handle = SDL_CreateWindow(Title.c_str(),
								  Geometry.XPos, Geometry.YPos,
								  Geometry.Width, Geometry.Height,
								  Flags);
		if(!Handle)
		{
			Diagnostics.WriteErrorLog("failed to create SDL window\n");
			Diagnostics.WriteErrorLog(SDL_GetError());

			return false;
		}

		RenderHandle = SDL_CreateRenderer(Handle, -1, SDL_RENDERER_ACCELERATED);

		if(!RenderHandle)
		{
			Diagnostics.WriteErrorLog("failed to create SDL renderer\n");
			Diagnostics.WriteErrorLog(SDL_GetError());

			return false;
		}
		
		Diagnostics.WriteInfoLog("created SDL window and SDL renderer\n");

		return true;
	}

	void game_window::Destroy()
	{
		if(RenderHandle)
		{
			SDL_DestroyRenderer(RenderHandle);
			RenderHandle = nullptr;
		}

		if(Handle)
		{
			SDL_DestroyWindow(Handle);
			Handle = nullptr;
		}
	}
}