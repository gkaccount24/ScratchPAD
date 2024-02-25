#include "game.h"

namespace scratchpad
{
	game::game(const char* WindowTitle,
			   geometry WindowGeometry,
			   int WindowFlags):

		MessageBuffer(""),
		Diagnostics(),
		MessageHandler(MessageBuffer, Diagnostics),
		Window(WindowTitle,
			   WindowGeometry,
			   WindowFlags,
			   MessageBuffer)
	{
		if(!Window.Create())
		{
			WriteErrorLog("failed to create window");

		}
	}

	game::~game()
	{
		Window.Destroy();

	}

	int game::Play()
	{
		while(!Diagnostics.Done)
		{
			PollEvents();

		}

		return 0;
	}

	void game::WriteErrorLog(string Message)
	{

	}

	void game::WriteWarningLog(string Message)
	{

	}

	void game::WriteInfoLog(string Message)
	{

	}

	void game::PollEvents()
	{
		SDL_Event E { };

		while(SDL_PollEvent(&E))
		{
			MessageHandler.QMessage(E);
		}

		MessageHandler.ProcessMessageQ();
	}
}
