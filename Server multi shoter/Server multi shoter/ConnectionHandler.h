#pragma once
#include <winsock.h>
#include "StructStorage.h"
#include <list>
#include <mutex>
#include <map>
class ConnectionHandler
{
public:
	ConnectionHandler(std::mutex* bulletMtx, std::map<SOCKET, Player*>* players);
	int ServerInitialize();
	void ServerShutdown();
	void AcceptNewClient(std::list<Bullets>* newBullets);
	void SendToClient(SOCKET socket, char* buf, int buff_length);
	void SendToAllClients(std::map<SOCKET, Player*>* players, char* buf, int buff_length);
	void DeletePlayer(SOCKET socketOfDeleted);
	void CheckConnections(std::map<SOCKET, Player*>* connectedCleints);
	void SendToAllClientsButOne(std::map<SOCKET, Player*>* players,SOCKET dontSend, char* buf, int buff_length);
	int socketCounter = 0;
private:
	std::mutex* bulletMtx;
	SOCKET serverSocket;
	std::map<SOCKET, Player*>* players;
	void ClientMessageReciver(SOCKET si, Player* player, std::list<Bullets>* newBullets);
};

