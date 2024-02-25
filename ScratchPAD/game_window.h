#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <SDL.h>

#include <sstream>
#include <string>

using std::stringstream;
using std::string;

namespace scratchpad
{
	struct geometry
	{
		int XPos; 
		int YPos;
		int Width;
		int Height;
	};

	class game_window 
	{

	public:

		 game_window(const char* WindowTitle, 
					 geometry WindowGeometry,
					 int WindowFlags,
					 stringstream& GameMessageBuffer);

		~game_window();

	public:
		bool Create();
		void Destroy();

	public:
		SDL_Window* Handle;
		SDL_Renderer* RenderHandle;

		string Title;
		geometry Geometry;
		int Flags;

		stringstream& MessageBuffer;
	};
}

#endif
