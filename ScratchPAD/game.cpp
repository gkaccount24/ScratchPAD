#include "game.h"

namespace globals
{
	static string_view MESSAGE_BUFFER_HEADER
	{
		"_______    ________    ________    _____      _____  ___________\n"
		"\\      \\ / _____/   /  _____/   /  _  \\ / \\ \\_   _____/\n"
		"/   |   \\ / \\  ___ / \\  ___  /  /_\\  \\ / \\ / \\ | __)_\n"
		"/    |    \\    \\_\\  \\ \\    \\_\\  \\/    |    \\ / Y    \\ | \\\n"
		"\\____ | __ / \\______ / \\______ / \\____ | __ / \\____ | __ / _______ /\n"
		"\\ / \\ / \\ / \\ / \\ / \\ /\n"
	};
}

namespace scratchpad
{
	game::game(const char* WindowTitle,
			   geometry WindowGeometry,
			   int WindowFlags):

		MessageBuffer(globals::MESSAGE_BUFFER_HEADER.data()),
		Diagnostics(MessageBuffer),
		MessageHandler(Diagnostics),
		Window(WindowTitle,
			   WindowGeometry,
			   WindowFlags,
			   Diagnostics)
	{
		if(!Window.Create())
		{
			/***
			***** LEAVE DIAG STATE IN UNREADY STATE
			****/
			Diagnostics.GameState = game_diagnostic_states::UnReady;

			/***
			***** OUTPUT GAME MESSAGE BUFFER
			****/
			cout << MessageBuffer.str() << endl;
		}
		else
		{
			/***
			***** DIAG STATE IN SET TO READY STATE
			****/
			Diagnostics.SetReadyState();
		}
	}

	game::~game()
	{
		Window.Destroy();

	}

	inline bool game::Playing()
	{
		return Diagnostics.GameState == game_diagnostic_states::Playing;
	}

	inline bool game::Paused()
	{
		return Diagnostics.GameState == game_diagnostic_states::Paused;
	}

	inline bool game::Ready()
	{
		return Diagnostics.GameState == game_diagnostic_states::Ready;
	}

	void game::Draw()
	{

		SDL_RenderPresent(Window.RenderHandle);
	}

	int game::Play()
	{
		while(Playing())
		{
			PollEvents();

			Draw();
		}

		return 0;
	}

	inline void game::PollEvents()
	{
		SDL_Event E { };

		while(SDL_PollEvent(&E))
		{
			switch(E.type)
			{
				case SDL_QUIT:
				{
					Diagnostics.StopGame();

					break;
				}
			}
		}

		MessageHandler.ProcessMessageQ();
	}
}
