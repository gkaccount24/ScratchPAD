#include "game_diagnostics.h"


namespace scratchpad
{
	game_diagnostics::game_diagnostics():
		ExitCode(0),
		ExitMessage(""),
		Done(true),
		Error(false)
	{

	}

	game_diagnostics::~game_diagnostics()
	{
		ExitMessage.clear();
		ExitMessage.shrink_to_fit();

		Done = true;
		Error = false;
	}

	void game_diagnostics::SetExitError(int GameExitCode, string GameExitMessage)
	{
		ExitCode = GameExitCode;
		ExitMessage = GameExitMessage;
	}

}


