#include "game_diagnostics.h"

/***
**** HELPER MACROS
***/
#define EMPTY_MESSAGE ""
#define EXIT_CODE 0

namespace scratchpad
{
	game_diagnostics::game_diagnostics(stringstream& GameMessageBuffer):
		GameState(game_diagnostic_states::UnReady),
		MessageBuffer(GameMessageBuffer),
		ExitCode(EXIT_CODE), 
		ExitMessage(EMPTY_MESSAGE)
	{

	}

	game_diagnostics::~game_diagnostics()
	{
		ExitMessage.clear();
		ExitMessage.shrink_to_fit();
	}

	void game_diagnostics::Quit()
	{

	}

	void game_diagnostics::SetReadyState()
	{
		if(GameState == game_diagnostic_states::UnReady)
		{
			WriteInfoLog("set ready state to true, ready to start game.\n");

		}
		else
		{
			WriteErrorLog("failed to set ready state.\n");

		}
	}

	void game_diagnostics::StartGame()
	{
		if(GameState == game_diagnostic_states::Ready)
		{
			WriteInfoLog("starting game...\n");

			/**
			***SET DONE BOOLEAN FLAG
			***/
			GameState = game_diagnostic_states::Playing;
		}
		else
		{
			WriteErrorLog("failed to start game.\n");

		}
	}

	void game_diagnostics::PauseGame()
	{
		if(GameState == game_diagnostic_states::Playing)
		{
			WriteInfoLog("pausing game...\n");

			/**
			***SET DONE BOOLEAN FLAG
			***/
			GameState = game_diagnostic_states::Paused;
		}
		else
		{
			WriteErrorLog("failed to pause game.\n");

		}
	}

	void game_diagnostics::StopGame()
	{
		if(GameState == game_diagnostic_states::Playing)
		{
			WriteInfoLog("stopping game...\n");

			/**
			*** SET DONE BOOLEAN FLAG
			***/
			GameState = game_diagnostic_states::Stopped;
		}
		else
		{
			WriteErrorLog("failed to stop game.\n");

		}
	}

	void game_diagnostics::SetExitError(int GameExitCode, string GameExitMessage)
	{
		WriteInfoLog("setting exit state...\n");

		ExitCode = GameExitCode;
		ExitMessage = GameExitMessage;
	}

	void game_diagnostics::WriteErrorLog(string Message) 
	{
		MessageBuffer << "[ERROR] " << Message;
	}

	void game_diagnostics::WriteWarningLog(string Message) 
	{
		MessageBuffer << "[WARNING] " << Message;
	}

	void game_diagnostics::WriteInfoLog(string Message) 
	{
		MessageBuffer << "[INFO] " << Message;
	}
}


