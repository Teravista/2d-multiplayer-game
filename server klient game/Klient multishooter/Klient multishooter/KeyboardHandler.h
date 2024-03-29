#pragma once
#include"./SDL2-2.0.10/include/SDL.h"
#include "StructStorage.h"
#include "SocketHandler.h"
class KeyboardHandler
{
public:
	KeyboardHandler(SocketHandler* socketHandler);
	void EventGameHandler(SDL_Event event, Player* P1, SOCKET server_socket, bool* gameState);
	char InputAdress(SDL_Event event, bool* pressedEnter);
private:
	SocketHandler* socketHandler;
};

