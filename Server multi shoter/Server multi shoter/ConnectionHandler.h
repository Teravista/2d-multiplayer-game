#pragma once
#include <winsock.h>
#include "StructStorage.h"
#include <list>
#include <mutex>
class ConnectionHandler
{
public:
	ConnectionHandler(std::mutex* bulletMtx);
	int ServerInitialize();
	void ServerShutdown();
	void AcceptNewClient(Player* players, std::list<Bullets>* newBullets);
	void SendToClient(int ithSocket, char* buf, int buff_length);
	int socketCounter = 0;
private:
	std::mutex* bulletMtx;
	SOCKET serverSocket;
	SOCKET socketos[10];
	void ClientMessageReciver(SOCKET si, Player* P, std::list<Bullets>* newBullets);
};

