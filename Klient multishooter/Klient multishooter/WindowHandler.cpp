#include "WindowHandler.h"
#include "StructStorage.h"
#include "KeyboardHandler.h"
#include"./SDL2-2.0.10/include/SDL.h"
#include <list>

struct WindowHandler::Colors
{
    int red;
    int green;
    int blue;
    int black;
}myColors;

int WindowHandler::SDL_Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return -1;
    }
    int rc = SDL_CreateWindowAndRenderer(600, 600, 0,
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
    SDL_RenderSetLogicalSize(renderer, 600, 600);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetWindowTitle(window, "GRA NYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    //SDL_SetColorKey(images->charset, true, 0x000000);

    screen = SDL_CreateRGBSurface(0, 600, 600, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 600, 600);
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

void WindowHandler::DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color, char b) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32*)p = color;
};

void WindowHandler::DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color, char b) {
    for (int i = 0; i < l; i++) {
        DrawPixel(screen, x, y, color, b);
        x += dx;
        y += dy;
    };
};

void DrawString(SDL_Surface* screen, double x, double y, const char* text, SDL_Surface* charset) {
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
    Uint32 outlineColor, Uint32 fillColor, char b) {
    int i;
    DrawLine(screen, x, y, k, 0, 1, outlineColor, b);
    DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor, b);
    DrawLine(screen, x, y, l, 1, 0, outlineColor, b);
    DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor, b);
    for (i = y + 1; i < y + k - 1; i++)
        DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor, b);
};

void WindowHandler::DrawEntities(Player* P1, Player* enemies, std::list<Bullets> bulletsToDraw)//TODO optimazations kinda junky
{
    // drawing player enemies and bullets
    //
    for (int i = 0; i < 10; i++)
    {
        if (enemies[i].allive == true)
        {
            DrawRectangle(screen, enemies[i].x, enemies[i].y, 20, 20, myColors.blue, myColors.red, 'E');
        }
    }
    for (auto bullet : bulletsToDraw)
    {
        DrawRectangle(screen, bullet.x, bullet.y, 5, 5, myColors.green, myColors.red, 'B');
    }
    int playerColor = (P1->wasHit) ? myColors.green : myColors.blue;
    DrawRectangle(screen, P1->x, P1->y, 20, 20, myColors.red, playerColor, 'P');
};

void WindowHandler::TexturesUpdateLVL1(Player* P1, Player* enemies, std::list<Bullets> bulletsToDraw)
{
    SDL_FillRect(screen, NULL, myColors.black);
    DrawEntities(P1, enemies, bulletsToDraw);
    this->RefreshWindow();
}

void WindowHandler::TexturesUpdateLoadScreen(char* input,int i)
{
    SDL_FillRect(screen, NULL, myColors.black);
    char text[128];
    sprintf_s(text, "Wpisz IP serwera do ktorego sie pol¹czysz");
    DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
    sprintf_s(text, "'127.0.0.1' lub wpisz 'local' by po³¹czyæ siê do localhosta");
    DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 46, text, charset);
    DrawString(screen, screen->w / 2 - strlen(input) * 8 / 2, 66, input, charset);
    DrawSpinningLogo(i);
    this->RefreshWindow();
}

void WindowHandler::DrawSpinningLogo(double i)
{
    int center = 300;
    for (int x = 0; x < 360; x++)
    {
        DrawPixel(screen, center +sin(x + i / 2000)*100, center + cos(x+i/1000) * 100,myColors.red, 'b');
        DrawPixel(screen, center + sin(x + i / 2000) * 100 - cos(x + i / 1000) * 100, center - cos(x + i / 1000) * 100, myColors.green, 'b');
        DrawPixel(screen, center - cos(x + i / 1000) * 100, center + sin(x + i / 2000) * 100 - cos(x + i / 1000) * 100, myColors.blue, 'b');
    }
}

void WindowHandler::GetColors(SDL_Surface* screen)
{
    myColors.black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
    myColors.green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
    myColors.red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
    myColors.blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
};

