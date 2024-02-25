#include "game.h"
#include "game_types.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <iostream>
#include <sstream>
#include <string_view>
#include <string>

using std::cout;
using std::endl;

using std::stringstream;
using std::string_view;
using std::string;

static const string_view WindowTitle = "Sidescroller";
static const scratchpad::geometry WindowGeometry = { };
static const int WindowFlags = SDL_WINDOW_MAXIMIZED | 
							   SDL_WINDOW_SHOWN;
#undef main

int main(int ArgCount, char* ArgV[])
{
	scratchpad::game Sidescroller(WindowTitle.data(), 
								  WindowGeometry, 
								  WindowFlags);
	return Sidescroller.Play();
}