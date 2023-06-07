#include <winsock.h>
#include <iostream>
#include <process.h>
#include <time.h>
#include <list>
#include <thread>
#include "StructStorage.h"
#include "ConnectionHandler.h"
#include "GameplayController.h"

Player P[10];
std::list<Bullets> bullets;

int main()
{
    ConnectionHandler connectionHandler = ConnectionHandler();
    GameplayController gameplayController = GameplayController(&connectionHandler);
    if (connectionHandler.ServerInitialize() == -1)
        return 0;
    std::thread thForwarder(&GameplayController::GameStatePropagator,&gameplayController,&bullets,P);
    thForwarder.detach();
    printf("Server operating correctly\n");
    for (;;)
    {
        connectionHandler.AcceptNewClient(P,&bullets);
    }
    connectionHandler.ServerShutdown();
    return 0;
}


