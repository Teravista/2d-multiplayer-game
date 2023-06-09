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

Player P[10];

int main()
{
    std::mutex bulletMtx;
    std::list<Bullets> newBullets;
    ConnectionHandler connectionHandler = ConnectionHandler(&bulletMtx);
    GameplayController gameplayController = GameplayController(&connectionHandler,&bulletMtx    );
    if (connectionHandler.ServerInitialize() == -1)
        return 0;
    std::thread thForwarder(&GameplayController::GameStatePropagator,&gameplayController,&newBullets,P);
    thForwarder.detach();
    printf("Server operating correctly\n");
    for (;;)
    {
        connectionHandler.AcceptNewClient(P,&newBullets);
    }
    connectionHandler.ServerShutdown();
    return 0;
}


