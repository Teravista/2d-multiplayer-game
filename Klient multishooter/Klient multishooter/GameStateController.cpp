#include "GameStateController.h"
#include"./SDL2-2.0.10/include/SDL.h"
#include "StructStorage.h"

GameStateController::GameStateController(KeyboardHandler* keyboardHandler, WindowHandler* windowHandler, SocketHandler* socketHandler)
{
    this->keyboardHandler = keyboardHandler;
    this->windowHandler = windowHandler;
    this->socketHandler = socketHandler;
}

std::string GameStateController::GameLoadScreen()
{
    bool pressedEnter = false;
    SDL_Event event;
    char input[126];
    double ticks = SDL_GetTicks();
    double ticks2 = SDL_GetTicks();
    sprintf_s(input, "");
    while (!pressedEnter)
    {
        if (ticks2 - ticks > 16.6)//co oko³o 1 milisekundy 60fps
        {
            windowHandler->TexturesUpdateLoadScreen(input, ticks);
            ticks = ticks2 = SDL_GetTicks();
        }
        else
        {
            ticks2 = SDL_GetTicks();
        }
        while (SDL_PollEvent(&event))
        {
            int length = strlen(input);
            char keyRead = keyboardHandler->InputAdress(event, &pressedEnter);

            if(keyRead == SDLK_BACKSPACE&&length>0)
                input[length-1] = '\0';
            else if (keyRead != NULL&&keyRead != SDLK_BACKSPACE)
            {
                input[length] = keyRead;
                input[length+1] = '\0';
            }
        }
    }
    if (strcmp(input,"local") == 0)
    {
        sprintf_s(input, "127.0.0.1");
        return std::string(input);
    }
    return std::string(input);
}

void GameStateController::FirstLevel(Player* P1,Player* enemies, std::list<Bullets>* bulletsToDraw,int PlayerId)
{
    SDL_Event event;
    double ticks = SDL_GetTicks();
    double ticks2 = SDL_GetTicks();
    bool gamestate = true;
    while (gamestate)
    {
        if (ticks2 - ticks > 16.6)//co oko³o 1 milisekundy 60fps
        {
            windowHandler->TexturesUpdateLVL1(P1, enemies, *bulletsToDraw);
            P1->wasHit = false;
            ticks = ticks2 = SDL_GetTicks();
        }
        else
        {
            ticks2 = SDL_GetTicks();
        }
        while (SDL_PollEvent(&event))
        {
            keyboardHandler->EventGameHandler(event, P1, socketHandler->server_socket, &gamestate, PlayerId);
        }
    }
}