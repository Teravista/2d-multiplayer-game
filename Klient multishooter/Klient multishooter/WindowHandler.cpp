#include "WindowHandler.h"
#include "StructStorage.h"
#include "KeyboardHandler.h"
#include"./SDL2-2.0.10/include/SDL.h"
#include <list>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
struct WindowHandler::Colors
{
    int red;
    int green;
    int blue;
    int black;
}myColors;

WindowHandler::WindowHandler(std::mutex* bulletMtx)
{
    this->bulletMtx = bulletMtx;
}

int WindowHandler::SDL_Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return -1;
    }
    int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
        &window, &renderer);
    if (rc != 0) {
        SDL_Quit();
        printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
        return -1;
    };
    charset = SDL_LoadBMP("./Resources/cs8x8.bmp");
    if (charset == NULL)
    {
        printf("SDL_LoadBMP error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetWindowTitle(window, "GRA NYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    //SDL_SetColorKey(images->charset, true, 0x000000);

    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    // wy³¹czenie widocznoœci kursora myszy
    SDL_ShowCursor(SDL_DISABLE);
    GetColors(screen);
    return 1;
}

void WindowHandler::RefreshWindow()
{
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void WindowHandler::FreeSurfaces()
{
    SDL_FreeSurface(screen);
    SDL_DestroyTexture(scrtex);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
};

void WindowHandler::DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32*)p = color;
};

void WindowHandler::DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    for (int i = 0; i < l; i++) {
        DrawPixel(screen, x, y, color);
        x += dx;
        y += dy;
    };
};

void WindowHandler::DrawString(SDL_Surface* screen, double x, double y, std::string textString, SDL_Surface* charset) {
    const char* text = textString.c_str();
    //TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24);
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8;
    d.h = 8;
    while (*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_BlitSurface(charset, &s, screen, &d);
        x += 8;
        text++;
    };
};

void WindowHandler::DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
    SDL_Rect dest;
    dest.x = x - sprite->w / 2;
    dest.y = y - sprite->h / 2;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite, NULL, screen, &dest);
};

void WindowHandler::DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
    Uint32 outlineColor, Uint32 fillColor) {
    int i;
    DrawLine(screen, x, y, k, 0, 1, outlineColor);
    DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
    DrawLine(screen, x, y, l, 1, 0, outlineColor);
    DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
    for (i = y + 1; i < y + k - 1; i++)
        DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

void WindowHandler::DrawEntities(Player* P1, Player* enemies, std::list<Bullets> bullets)//TODO optimazations kinda junky
{
    // drawing player enemies and bullets
    //
    for (int i = 0; i < 10; i++)
    {
        if (enemies[i].allive == true)
        {
            DrawRectangle(screen, enemies[i].x, enemies[i].y, 20, 20, myColors.blue, myColors.red);
        }
    }
    this->bulletMtx->lock();
    for (const Bullets& bullet : bullets)
    {
        DrawRectangle(screen, bullet.x, bullet.y, 5, 5, myColors.green, myColors.red);
    }
    this->bulletMtx->unlock();
    int playerColor = (P1->wasHit) ? myColors.green : myColors.blue;
    DrawRectangle(screen, P1->x, P1->y, 20, 20, myColors.red, playerColor);
};

void WindowHandler::TexturesUpdateLVL1(Player* P1, Player* enemies, std::list<Bullets> bullets)
{
    SDL_FillRect(screen, NULL, myColors.black);
    DrawEntities(P1, enemies, bullets);
    this->RefreshWindow();
}

void WindowHandler::TexturesUpdateLoadScreen(std::string input,int clockTircks)
{
    SDL_FillRect(screen, NULL, myColors.black);
    std::string text = "Type server IP you want connect to";
    DrawString(screen, screen->w / 2 - text.length() * 8 / 2, 26, text, charset);
    text = "or you can type 'local' to connect to localhost";
    DrawString(screen, screen->w / 2 - text.length() * 8 / 2, 46, text, charset);
    DrawString(screen, screen->w / 2 - input.length() * 8 / 2, 66, input, charset);
    DrawSpinningLogo(clockTircks);
    this->RefreshWindow();
}

void WindowHandler::DrawSpinningLogo(double clockTircks)
{
    double i = clockTircks; //using time to create continously spinning logo
    // something nice to look at while typing in server sIP
    const int xMiddle = SCREEN_WIDTH/2;
    const int yMiddle = SCREEN_HEIGHT / 2;
    for (int x = 0; x < 200; x++)
    {
        DrawPixel(screen, xMiddle +sin(x + i / 2000)*100, yMiddle + cos(x+i/1000) * 100,myColors.red);
        DrawPixel(screen, xMiddle + sin(x + i / 2000) * 100 - cos(x + i / 1000) * 100, yMiddle - cos(x + i / 1000) * 100, myColors.green);
        DrawPixel(screen, xMiddle - cos(x + i / 1000) * 100, yMiddle + sin(x + i / 2000) * 100 - cos(x + i / 1000) * 100, myColors.blue);
    }
}

void WindowHandler::GetColors(SDL_Surface* screen)
{
    myColors.black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
    myColors.green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
    myColors.red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
    myColors.blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
};

