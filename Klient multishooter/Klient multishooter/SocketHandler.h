#pragma once
#include "StructStorage.h"
#include <list>
#include <winsock.h>
#include <thread>
#include <string>
#include <mutex>
class SocketHandler
{
public:
	SOCKET server_socket;
	SocketHandler(std::mutex* bulletMtx);
	int SocketInitializer(std::string serverIP);
	void ReciverFromServer(Player* P1, Player* enemies, int* playerID, std::list<Bullets>* bullets);
	void SocketClose();
private:
	std::mutex* bulletMtx;
};

