#pragma once
#include"./SDL2-2.0.10/include/SDL.h"
#include "StructStorage.h"
class KeyboardHandler
{
public:
	void EventGameHandler(SDL_Event event, Player* P1, SOCKET server_socket, bool* gameState, int playerID);
	char InputAdress(SDL_Event event, bool* pressedEnter);
};

