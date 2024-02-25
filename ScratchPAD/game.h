#ifndef GAME_H
#define GAME_H

#include "game_message_handler.h"
#include "game_diagnostics.h"
#include "game_window.h"

namespace scratchpad
{
	class game
	{

	public:

		game(const char* WindowTitle,
			 geometry WindowGeometry,
			 int WindowFlags);

		~game();

	public:

		int Play();

	private:
		void PollEvents();

	private:
		/**
		*** HELPER LOGGING METHODS
		**/
		void WriteErrorLog(string Message);
		void WriteWarningLog(string Message);
		void WriteInfoLog(string Message);

	private:
		/**
		*** GENERIC Message Buffer
		**/
		stringstream MessageBuffer;

		/***
		**** GAME DIAG SYSTEM
		***/
		scratchpad::game_diagnostics Diagnostics;


		/***
		**** GAME MESSAGE SYSTEM
		***/
		scratchpad::game_message_handler MessageHandler;

		/**
		*** WINDOW FUNCTIONALITY
		**/
		scratchpad::game_window Window;

		/**
		*** Game STATE MEMBERS
		**/
		bool Done;
	};
}

#endif
