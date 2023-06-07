#pragma once
#include <list>
#include "StructStorage.h"
#include "ConnectionHandler.h"
class GameplayController
{
public:
	GameplayController(ConnectionHandler* connectionHandler);
	void GameStatePropagator(std::list<Bullets>* bullets, Player* P);
private:
	bool BulletColision(Player player, std::list<Bullets> bullets);
	bool DetectColission(Player P1, Player P2);
	ConnectionHandler* myConnectionH;
};

