#pragma once
#include "StructStorage.h"
#include <list>
#include <winsock.h>
#include <thread>
#include <string>
class SocketHandler
{
public:
	SOCKET server_socket;
	int SocketInitializer(std::string serverIP);
	void ReciverFromServer(Player* P1, Player* enemies, int* playerID, std::list<Bullets>* bullets, std::list<Bullets>* bulletsToDraw);
	void SocketClose();
};

