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
#include <thread>

//TODO
//1.move propagations of bullets solely to clietn side (implement mutex to by sure bullet list doesnt collide[when adding new bullets and drawing])
//2.implement collisions and life coutners ect...
//3.improve movement handler seems junky
//4.change method of comunication with server to bits : be smart use less data
//5.use UDP instead of TCP for some non important data like current player location (we can skip a few frames)
//6.implement some kind of popup to choose manual ipadrres to connect to server or local host
//7.implement some kind of time out mechanizm for players who disconet to automaticly dissapear from map

int main(int argc, char** argv) 
{
    WindowHandler windowHandler = WindowHandler();
    SocketHandler socketHandler = SocketHandler();
    KeyboardHandler ketboardHandler = KeyboardHandler();
    SDL_Event event;
    Player P1;
    Player enemies[10];
    std::list<Bullets> bullets;
    std::list<Bullets> bulletsToDraw;
    int playerID = -1;
    if (windowHandler.SDL_Initialize() == -1)
        return 0;
    if (socketHandler.SocketInitializer() == -1)
        return 0;
    std::thread th1(&SocketHandler::ReciverFromServer, &socketHandler,&P1,enemies,&playerID,&bullets,&bulletsToDraw);
    double ticks =  SDL_GetTicks();
    double ticks2 = SDL_GetTicks();
    bool gamestate = true;
    while(gamestate)
    { 
        if (ticks2 - ticks > 16.6)//co oko³o 1 milisekundy 60fps
        {
            windowHandler.TexturesUpdate(&P1,enemies, bulletsToDraw);
            P1.wasHit = false;
            ticks = ticks2 = SDL_GetTicks();
        }
        else
        {
            ticks2 = SDL_GetTicks();
        }
        while (SDL_PollEvent(&event))
        {
            ketboardHandler.EventHandler(event, &P1, socketHandler.server_socket, &gamestate,playerID);
        }
    }
    th1.detach();
    windowHandler.FreeSurfaces();
    socketHandler.SocketClose();
    WSACleanup();
    return 0;
}

