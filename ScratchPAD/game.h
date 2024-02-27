#ifndef GAME_H
#define GAME_H

#include "game_message_handler.h"
#include "game_diagnostics.h"
#include "game_window.h"

#include <string_view>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

using std::string_view;
using std::stringstream;

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
		void Draw();

	private:
		/**
		*** HELPER/CONVENIENCE METHODS
		**/
		bool Playing();
		bool Paused();
		bool Ready();

		bool LoadTextures(string Path);
		bool LoadImages(string Path);
		bool LoadAudio(string Path);

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
	};
}

#endif
