#ifndef GAME_DIAGNOSTICS_H
#define GAME_DIAGNOSTICS_H

#include <string>

using std::string;

namespace scratchpad
{
	class game_diagnostics
	{

	public:

		 game_diagnostics();
		~game_diagnostics();

	public:

		void SetExitError(int GameExitCode, string GameExitMessage);

	public:
		int ExitCode;
		string ExitMessage;
		bool Error;
		bool Done;

	};
}

#endif
