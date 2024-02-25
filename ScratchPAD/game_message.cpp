#include "game_message.h"

namespace scratchpad
{
	 game_message::game_message(SDL_Event& GameEvt):
		 Evt(GameEvt)
	 {
	 }

	 game_message::~game_message() 
	 {
	 }

	 /***
	 **** GAME MOUSE MOVE MESSAGE 
	 ***/
	 mouse_move::mouse_move(SDL_Event& GameEvt):
		 game_message(GameEvt)
	 {

	 }

	 mouse_move::~mouse_move()
	 {

	 }

	 void mouse_move::Process(stringstream& MessageBuffer, game_diagnostics& GameDiag)
	 {
		 MessageBuffer << "[MQUEUE] mouse motion\n";

		 MessageBuffer << "[MQUEUE] mouse pos x " << Evt.motion.x;
		 MessageBuffer << "[MQUEUE] mouse pos y " << Evt.motion.y;

		 MessageBuffer << "[MQUEUE] mouse rel x " << Evt.motion.xrel;
		 MessageBuffer << "[MQUEUE] mouse rel y " << Evt.motion.yrel;
	 }

	 /***
	 **** GAME QUIT MESSAGE 
	 ***/
	 quit_message::quit_message(string QuitMessage, int QuitCode, SDL_Event& GameEvt):
		 Message(QuitMessage),
		 Code(QuitCode),
		 game_message(GameEvt)
	 { }

	 quit_message::~quit_message() 
	 {
		 Code = 0;

		 if(!Message.empty())
		 {
			 Message.clear();
			 Message.shrink_to_fit();
		 }
	 }

	 void quit_message::Process(stringstream& MessageBuffer, game_diagnostics& GameDiag)
	 {
		 MessageBuffer << "[MQUEUE] game quit message received\n";
		 MessageBuffer << "[MQUEUE] game quit code: " << Code << "\n";
		 MessageBuffer << "[MQUEUE] game quit message string: " << Message << "\n";

		 GameDiag.Done = true;
	 }
}
