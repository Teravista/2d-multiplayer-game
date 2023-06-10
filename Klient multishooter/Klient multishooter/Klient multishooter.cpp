#include <iostream>
#include <winsock.h>
#include <process.h>
#include "StructStorage.h"
#include"./SDL2-2.0.10/include/SDL.h"
#include "WindowHandler.h"
#include "KeyboardHandler.h"
#include <list>
#include <windows.h>
#include "SocketHandler.h"
#include "GameStateController.h"
#include <thread>
#include <string>
#include <map>

//TODO
// GAMPLAY::
//2.implement collisions and life coutners ect...
// QUALITY AND IMPROVEMENT::
//3.improve movement handler seems junky
//6. 
//4.change method of comunication with server to bits : be smart use less data
//10. move storing player info in client and server from simple arrays to more advanced and flexible collections
// CONNECTION
//5.use UDP instead of TCP for some non important data like current player location (we can skip a few frames)
//7.implement some kind of time out mechanizm for players who disconet to automaticly dissapear from map

int main(int argc, char** argv) 
{
    std::mutex bulletMtx;
    WindowHandler windowHandler = WindowHandler(&bulletMtx);
    SocketHandler socketHandler = SocketHandler(&bulletMtx);
    KeyboardHandler keyboardHandler = KeyboardHandler(&socketHandler);
    GameStateController gamestateController = GameStateController(&keyboardHandler, &windowHandler, &socketHandler,&bulletMtx);
    Player P1;
    std::map<int, Player*> enemies;
    std::list<Bullets> bullets;
    if (windowHandler.SDL_Initialize() == -1)
        return 0;
    std::string serverIP = "";
    do {//keep asking for IP till client enter valid server IP
        serverIP = gamestateController.GameLoadScreen();
        if (serverIP.compare("END") == 0) return 0;
    } while (socketHandler.SocketInitializer(serverIP) == -1);
    std::thread th1(&SocketHandler::ReciverFromServer, &socketHandler,&P1,&enemies,&bullets);

    gamestateController.FirstLevel(&P1,&enemies,&bullets);

    th1.detach();
    windowHandler.FreeSurfaces();
    socketHandler.SocketClose();
    return 0;
}

