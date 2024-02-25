#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include "game_diagnostics.h"
#include "game_types.h"

#include <SDL.h>

#include <sstream>
#include <string>

using std::stringstream;
using std::string;

namespace scratchpad
{
	class game_window 
	{

	public:

		 game_window(const char* WindowTitle, 
					 scratchpad::geometry WindowGeometry,
					 int WindowFlags,
					 scratchpad::game_diagnostics& GameDiag);
		~game_window();

	public:

		bool Create();
		void Destroy();

	public:
		/***
		**** SDL PTR HANDLES
		***/
		SDL_Window*	  Handle;
		SDL_Renderer* RenderHandle;

		/***
		**** WINDOW DATA MEMBERS
		***/
		string	      Title;
		geometry	  Geometry;
		int			  Flags;

		/***
		**** GAME DIAGNOSTIC UTILITY class
		***/
		scratchpad::game_diagnostics& Diagnostics;
	};
}

#endif
