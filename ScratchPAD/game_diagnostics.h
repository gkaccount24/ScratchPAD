#ifndef GAME_DIAGNOSTICS_H
#define GAME_DIAGNOSTICS_H

#include <sstream>
#include <string>

using std::string;
using std::stringstream;

namespace scratchpad
{
	enum class game_diagnostic_states
	{
		UnReady = 0,
		Ready = 1,
		Playing = 2,
		Paused = 3,
		Stopped = 4,

		// final count of diagnostic
		// states
		Count = 5
	};

	enum class game_diagnostic_errors
	{
		SDLWindowCreationFailed = 0,
		SDLRendererCreationFailed = 1,

		UserDefinedError = 2,
		Count = 3
	};

	struct game_diagnostic_error
	{
		game_diagnostic_errors ID;
		string Message;
	};

	class game_diagnostics
	{

	public:

		game_diagnostics(stringstream& GameMessageBuffer);
		~game_diagnostics();

	public:

		void SetExitError(int GameExitCode,
						  string GameExitMessage);
		void StartGame();
		void PauseGame();
		void StopGame();

		void Quit();

		/**
		*** HELPER LOGGING METHODS
		**/
		void WriteErrorLog(string Message);
		void WriteWarningLog(string Message);
		void WriteInfoLog(string Message);

		/**
		*** HELPER/CONVENIENCE METHODS
		**/
		void SetReadyState();

	public:
		/***
		**** GAME STATE FLAGS/BOOLEANS
		***/
		game_diagnostic_states GameState;

		/***
		**** GAME'S MAIN MESSAGE BUFFER
		***/
		stringstream& MessageBuffer;

		/***
		**** EXIT STATE DATA MEMBERS
		***/
		int			  ExitCode;
		string		  ExitMessage;
	};
};

#endif
