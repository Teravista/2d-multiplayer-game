#include <winsock.h>
#include <iostream>
#include <process.h>
#include <time.h>
#include <list>
#include <thread>
#include "StructStorage.h"
#include "ConnectionHandler.h"
#include "GameplayController.h"
#include <mutex>
#include <map>


int main()
{
    std::mutex bulletMtx;
    std::list<Bullets> newBullets;
    std::map<SOCKET, Player*> players;
    ConnectionHandler connectionHandler = ConnectionHandler(&bulletMtx,&players);
    GameplayController gameplayController = GameplayController(&connectionHandler,&bulletMtx,&players);
    if (connectionHandler.ServerInitialize() == -1)
        return 0;
    std::thread thForwarder(&GameplayController::GameStatePropagator,&gameplayController,&newBullets);
    thForwarder.detach();
    printf("Server operating correctly\n");
    for (;;)
    {
        connectionHandler.AcceptNewClient(&newBullets);
    }
    connectionHandler.ServerShutdown();
    return 0;
}


