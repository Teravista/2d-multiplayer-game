#pragma once
#include "StructStorage.h"
#include <list>
#include <winsock.h>
#include <thread>
class SocketHandler
{
public:
	SOCKET server_socket;
	int SocketInitializer();
	void ReciverFromServer(Player* P1, Player* enemies, int* playerID, std::list<Bullets>* bullets, std::list<Bullets>* bulletsToDraw);
	void SocketClose();
};

