#pragma once
#include <winsock.h>
#include "StructStorage.h"
#include <list>
class ConnectionHandler
{
public:
	int ServerInitialize();
	void ServerShutdown();
	void AcceptNewClient(Player* players, std::list<Bullets>* bullets);
	void SendToClient(int ithSocket, char* buf, int buff_length);
	int socketCounter = 0;
private:
	SOCKET serverSocket;
	SOCKET socketos[10];
	void ClientMessageReciver(SOCKET si, Player* P, std::list<Bullets>* bullets);
};

