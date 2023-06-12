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
	void ReciverFromServerTCP(Player* P1, std::list<Bullets>* bullets);
	void SendMessageToServer(char* message,int messageLength);
	void ReviferFromServerUDP(std::map<int, Player*>* enemies);
	void SocketClose();
private:
	std::string serverIP;
	std::mutex* bulletMtx;
};

