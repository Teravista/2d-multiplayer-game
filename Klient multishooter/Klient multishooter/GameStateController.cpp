#include "GameStateController.h"
#include"./SDL2-2.0.10/include/SDL.h"
#include "StructStorage.h"

GameStateController::GameStateController(KeyboardHandler* keyboardHandler, WindowHandler* windowHandler, SocketHandler* socketHandler, std::mutex* bulletMtx)
{
    this->bulletMtx = bulletMtx;
    this->keyboardHandler = keyboardHandler;
    this->windowHandler = windowHandler;
    this->socketHandler = socketHandler;
}

std::string GameStateController::GameLoadScreen()
{
    bool pressedEnter = false;
    SDL_Event event;
    std::string input = "";
    double ticks = SDL_GetTicks();
    double ticks2 = SDL_GetTicks();
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
            char keyRead = keyboardHandler->InputAdress(event, &pressedEnter);

            if(keyRead == SDLK_BACKSPACE&&input.length()>0)
                input.pop_back();
            else if (keyRead != NULL&& keyRead != SDLK_BACKSPACE)
            {
                input.push_back(keyRead);
            }
        }
    }
    if (input.compare("local") == 0)
    {
        return std::string("127.0.0.1");
    }
    return std::string(input);
}

void GameStateController::FirstLevel(Player* P1,Player* enemies, std::list<Bullets>* bullets,int PlayerId)
{
    SDL_Event event;
    double ticks = SDL_GetTicks();
    double ticks2 = SDL_GetTicks();
    bool gamestate = true;
    while (gamestate)
    {
        if (ticks2 - ticks > 16.6)//co oko³o 1 milisekundy 60fps
        {
            this->BulletPropagator(bullets);
            windowHandler->TexturesUpdateLVL1(P1, enemies, *bullets);
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

void GameStateController::BulletPropagator(std::list<Bullets>* allBullets)
{
    std::list<Bullets>::iterator iter = allBullets->begin();
    std::list<Bullets>::iterator end = allBullets->end();
    this->bulletMtx->lock();
    while (iter != end)
    {
        iter->x += iter->xSpeed * 5;
        iter->y += iter->ySpeed * 5;
        if (iter->x < 30 || iter->x>500 - 50 || iter->y < 30 || iter->y>400 - 50)
            iter = allBullets->erase(iter);
        else
            ++iter;
    }
    this->bulletMtx->unlock();
}