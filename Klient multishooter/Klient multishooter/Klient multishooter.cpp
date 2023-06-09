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

//TODO
//--->1.move propagations of bullets solely to clietn side (implement mutex to by sure bullet list doesnt collide[when adding new bullets and drawing])
//2.implement collisions and life coutners ect...
//3.improve movement handler seems junky
//4.change method of comunication with server to bits : be smart use less data
//5.use UDP instead of TCP for some non important data like current player location (we can skip a few frames)
//7.implement some kind of time out mechanizm for players who disconet to automaticly dissapear from map
//8.remove bullets to draw from main move it to SFL drawer and there create copy
//9. add mutex to adding/drawing bullets
//10. move storing player info in client and server from simple arrays to more advanced and flexible collections

int main(int argc, char** argv) 
{
    std::mutex bulletMtx;
    WindowHandler windowHandler = WindowHandler(&bulletMtx);
    SocketHandler socketHandler = SocketHandler(&bulletMtx);
    KeyboardHandler keyboardHandler = KeyboardHandler();
    GameStateController gamestateController = GameStateController(&keyboardHandler, &windowHandler, &socketHandler,&bulletMtx);
    Player P1;
    Player enemies[10];
    std::list<Bullets> bullets;
    int playerID = -1;
    if (windowHandler.SDL_Initialize() == -1)
        return 0;
    std::string serverIP = "";
    do {//keep asking for IP till client enter valid server IP
        serverIP = gamestateController.GameLoadScreen();
    } while (socketHandler.SocketInitializer(serverIP) == -1);
    std::thread th1(&SocketHandler::ReciverFromServer, &socketHandler,&P1,enemies,&playerID,&bullets);

    gamestateController.FirstLevel(&P1,enemies,&bullets,playerID);

    th1.detach();
    windowHandler.FreeSurfaces();
    socketHandler.SocketClose();
    return 0;
}

