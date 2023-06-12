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
// QUALITY AND IMPROVEMENT::
//3.improve movement handler seems junky and everything in keyboardHandler
//4.change method of comunication with server to bits : be smart use less data
// add smart pointers
// add graphics for player enemies bullets and some background
// some more gameplay mechanics like moving background bigger playspace than only screen some obstacles
// adding some basic level constructor
// CONNECTION
//5.use UDP instead of TCP for some non important data like current player location (we can skip a few frames)

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
    std::thread th1(&SocketHandler::ReciverFromServerTCP, &socketHandler,&P1,&bullets);
    std::thread th2(&SocketHandler::ReviferFromServerUDP, &socketHandler, &enemies);


    gamestateController.FirstLevel(&P1,&enemies,&bullets);

    th1.detach();
    th2.detach();
    windowHandler.FreeSurfaces();
    socketHandler.SocketClose();
    return 0;
}

