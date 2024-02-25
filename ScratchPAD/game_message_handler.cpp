#include "game_message_handler.h"


namespace scratchpad
{
	game_message_handler::game_message_handler(stringstream& GameMessageBuffer, 
											   game_diagnostics& GameDiag):
		Diag(GameDiag),
		MessageBuffer(GameMessageBuffer)
	{
	}

	game_message_handler::~game_message_handler()
	{
		if(MQueue.empty())
		{
			game_message* Message = MQueue.front();

			if(Message)
			{
				delete Message;
				Message = nullptr;
			}

			MQueue.pop();
		}
	}

	void game_message_handler::ProcessMessageQ()
	{
		if(MQueue.empty())
		{
			game_message* Message = MQueue.front();

			Message->Process(MessageBuffer, Diag);

			MQueue.pop();

			delete Message;
			Message = nullptr;
		}
	}

	void game_message_handler::QMessage(SDL_Event& GameEvt)
	{
		switch(GameEvt.type)
		{
			case SDL_MOUSEMOTION:
			{
				MQueue.push(new mouse_move(GameEvt));

				break;
			}
			case SDL_QUIT:
			{
				MQueue.push(new quit_message(Diag.ExitMessage, Diag.ExitCode, GameEvt));

				break;
			}
		}
	}
}
