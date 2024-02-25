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

	 void mouse_move::Process(game_diagnostics& GameDiag)
	 {
		 GameDiag.WriteInfoLog("[MQUEUE] mouse motion message\n");

		 GameDiag.WriteInfoLog("[MQUEUE] mouse pos x ");
		 GameDiag.WriteInfoLog("[MQUEUE] mouse pos y ");

		 GameDiag.WriteInfoLog("[MQUEUE] mouse pos rel x ");
		 GameDiag.WriteInfoLog("[MQUEUE] mouse pos rel y ");
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

	 void quit_message::Process(game_diagnostics& GameDiag)
	 {
		 GameDiag.WriteInfoLog("game quit message received\n");
		 GameDiag.WriteInfoLog("game quit code: ");
		 GameDiag.WriteInfoLog(to_string(Code));
		 GameDiag.WriteInfoLog("\n");

		 GameDiag.WriteInfoLog("game quit message string: ");
		 GameDiag.WriteInfoLog(Message);
	 }
}
