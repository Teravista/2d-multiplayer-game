#pragma once
#include "StructStorage.h"
#include <list>
#include <winsock.h>
#include <thread>
#include <string>
#include <mutex>
#include <map>
class SocketHandler
{
public:
	SOCKET server_socket;
	SocketHandler(std::mutex* bulletMtx);
	int SocketInitializer(std::string serverIP);
	void ReciverFromServer(Player* P1, std::map<int, Player*>* enemies, std::list<Bullets>* bullets);
	void SendMessageToServer(char* message,int messageLength);
	void SocketClose();
private:
	std::mutex* bulletMtx;
};

