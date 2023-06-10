#pragma once
#include <list>
#include "StructStorage.h"
#include "ConnectionHandler.h"
#include <mutex>
class GameplayController
{
public:
	GameplayController(ConnectionHandler* connectionHandler, std::mutex* bulletMtx, std::map<SOCKET, Player*>* players);
	void GameStatePropagator(std::list<Bullets>* newBullets);
private:
	std::mutex* bulletMtx;
	std::map<SOCKET, Player*>* players;
	bool CheckBulletColision(std::map<SOCKET, Player*>* allivePlayers, std::list<Bullets>* allBullets);
	void SendPlayerPositionToClients(std::map<SOCKET, Player*>* allivePlayers, char* buf);
	void SendEnemyPositionToClients(std::map<SOCKET, Player*>* allivePlayers, char* buf);
	void BulletSender(std::list<Bullets>* newBullets, char* buf, std::list<Bullets>* allBullets);
	void PlayerSorter(std::map<SOCKET, Player*>* allivePlayers);
	void InvinsibleFrmesPropagator(std::map<SOCKET, Player*>* allivePlayers);
	bool CheckPlayerColission(std::map<SOCKET, Player*>* allivePlayers);
	ConnectionHandler* myConnectionH;
};

