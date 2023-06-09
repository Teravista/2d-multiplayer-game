#pragma once
#include "KeyboardHandler.h"
#include "WindowHandler.h"
#include "SocketHandler.h"
#include "StructStorage.h"
#include <list>
#include <string>
class GameStateController
{
public:
	GameStateController(KeyboardHandler* keyboardHandler,WindowHandler* windowHandler,SocketHandler* socketHandler);
	std::string GameLoadScreen();
	void FirstLevel(Player* P1, Player* enemies, std::list<Bullets>* bulletsToDraw, int PlayerId);
private:
	KeyboardHandler* keyboardHandler;
	WindowHandler* windowHandler;
	SocketHandler* socketHandler;
};

