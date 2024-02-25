#ifndef GAME_MESSAGE_HANDLER_H
#define GAME_MESSAGE_HANDLER_H

#include "game_diagnostics.h"
#include "game_message.h"

#include <SDL.h>

#include <queue>
#include <sstream>

using std::stringstream;
using std::queue;

namespace scratchpad
{
	class game_message_handler
	{
		
	public:
		 game_message_handler(stringstream& GameMessageBuffer, 
							  game_diagnostics& GameDiag);
		~game_message_handler();

		void QMessage(SDL_Event& E);
		void ProcessMessageQ();

	private:
		game_diagnostics& Diag;
		stringstream& MessageBuffer;

		queue<game_message*> MQueue;
	};
}

#endif
