#ifndef GAME_MESSAGE_H
#define GAME_MESSAGE_H

#include "game_diagnostics.h"

#include <SDL.h>

#include <string>
#include <sstream>
#include <mutex>

using std::to_string;
using std::stringstream;
using std::string;
using std::mutex;

namespace scratchpad 
{
	/***
	**** GAME MESSAGE 
	***/
	class game_message
	{

	public:
				 game_message(SDL_Event& GameEvt);
		virtual ~game_message();

		virtual void Process(scratchpad::game_diagnostics& GameDiag) = 0;

	protected:
		SDL_Event&		   Evt;
	};

	/***
	**** GAME MOUSE MOVE MESSAGE 
	***/
	class mouse_move: public game_message
	{

	public:
				 mouse_move(SDL_Event& GameEvt);
		virtual ~mouse_move();

		virtual void Process(scratchpad::game_diagnostics& GameDiag);
	};

	/***
	**** GAME QUIT MESSAGE 
	***/

	class quit_message: public game_message
	{
	public:
				 quit_message(string ExitMessage, int ExitCode, SDL_Event& GameEvt);
		virtual ~quit_message();

		virtual void Process(scratchpad::game_diagnostics& GameDiag);

	public:
		string Message;
		int	   Code;

	};

}

#endif
