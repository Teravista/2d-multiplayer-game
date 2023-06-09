#include "KeyboardHandler.h"
#include"./SDL2-2.0.10/include/SDL.h"
#include "StructStorage.h"
void KeyboardHandler::EventGameHandler(SDL_Event event, Player* P1, SOCKET server_socket, bool* gameState,int playerID)
{
    char buf[80];
    buf[0] = '0';
    int buff_length;
    switch (event.type)
    {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) *gameState = false;
        if (event.key.keysym.sym == SDLK_UP) { buf[0] = 'U'; P1->ySpeed = -1; }
        else if (event.key.keysym.sym == SDLK_DOWN) { buf[0] = 'N'; P1->ySpeed = 1; }
        else if (event.key.keysym.sym == SDLK_LEFT) { buf[0] = 'L'; P1->xSpeed = -1; }
        else if (event.key.keysym.sym == SDLK_RIGHT) { buf[0] = 'R'; P1->xSpeed = 1; }
        if (event.key.keysym.sym == SDLK_w) { buf[0] = 'W'; P1->yShootSpeed = UP; }
        else if (event.key.keysym.sym == SDLK_s) { buf[0] = 'S'; P1->yShootSpeed = DOWN; }
        else if (event.key.keysym.sym == SDLK_a) { buf[0] = 'A'; P1->yShootSpeed = LEFT; }
        else if (event.key.keysym.sym == SDLK_d) { buf[0] = 'D'; P1->yShootSpeed = RIGHT; }
        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_UP && P1->ySpeed == UP) { buf[0] = 'Y'; P1->ySpeed = 0; }
        else if (event.key.keysym.sym == SDLK_DOWN && P1->ySpeed == DOWN) { buf[0] = 'Y'; P1->ySpeed = 0; }
        else if (event.key.keysym.sym == SDLK_LEFT && P1->xSpeed == LEFT) { buf[0] = 'X'; P1->xSpeed = 0; }
        else if (event.key.keysym.sym == SDLK_RIGHT && P1->xSpeed == RIGHT) { buf[0] = 'X'; P1->xSpeed = 0; }
        if (event.key.keysym.sym == SDLK_w) P1->yShootSpeed = 0;
        else if (event.key.keysym.sym == SDLK_s) P1->yShootSpeed = 0;
        else if (event.key.keysym.sym == SDLK_a) P1->xShootSpeed = 0;
        else if (event.key.keysym.sym == SDLK_d) P1->xShootSpeed = 0;
        break;
    case SDL_QUIT:
        *gameState = false;
        break;
    }
    if (buf[0] != '0')
    {
        buf[1] = playerID;
        buf[2] = '\0';
        buff_length = 3;
        send(server_socket, buf, buff_length, 0);
    }
}

char KeyboardHandler::InputAdress(SDL_Event event, bool* pressedEnter)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        int value = event.key.keysym.sym;
        if (event.key.keysym.sym == SDLK_RETURN)
        {
            *pressedEnter = true;
        }
        else if((SDLK_a <= event.key.keysym.sym && event.key.keysym.sym <= SDLK_z) || 
            (SDLK_0 <=event.key.keysym.sym&& event.key.keysym.sym <= SDLK_9)|| event.key.keysym.sym == SDLK_PERIOD)
        {
            return event.key.keysym.sym;
        }
        else if (event.key.keysym.sym == SDLK_BACKSPACE)
        {
            return SDLK_BACKSPACE;
        }
        break;
    }
    return NULL;
}