#pragma once
#include <list>
#include "StructStorage.h"
#include "ConnectionHandler.h"
#include <mutex>
class GameplayController
{
public:
	GameplayController(ConnectionHandler* connectionHandler, std::mutex* bulletMtx);
	void GameStatePropagator(std::list<Bullets>* newBullets, Player* P);
private:
	std::mutex* bulletMtx;
	bool BulletColision(Player player, std::list<Bullets> newBullets);
	bool DetectColission(Player P1, Player P2);
	ConnectionHandler* myConnectionH;
};

