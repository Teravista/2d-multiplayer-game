#pragma once
#include "KeyboardHandler.h"
#include "WindowHandler.h"
#include "SocketHandler.h"
#include "StructStorage.h"
#include <list>
#include <mutex>
#include <string>
class GameStateController
{
public:
	GameStateController(KeyboardHandler* keyboardHandler,WindowHandler* windowHandler,SocketHandler* socketHandler, std::mutex* bulletMtx);
	std::string GameLoadScreen();
	void FirstLevel(Player* P1, Player* enemies, std::list<Bullets>* bullets, int PlayerId);
private:
	void BulletPropagator(std::list<Bullets>* allBullets);
	KeyboardHandler* keyboardHandler;
	WindowHandler* windowHandler;
	SocketHandler* socketHandler;
	std::mutex* bulletMtx;
};

