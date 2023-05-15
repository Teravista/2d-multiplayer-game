#include <iostream>
#include <winsock.h>
#include <process.h>
#include"./SDL2-2.0.10/include/SDL.h"
#include <list>
#include <windows.h>

#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
struct Color
{
    int black, green, red, blue;
};

struct Bullets
{
    double x, y;
};

struct Player
{
    double x, xBorderMovement;
    double y, yBorderMovement;
    double ySpeed;
    double xSpeed;
    int currentEnemies;
    double xShootSpeed;
    double yShootSpeed;
    bool allive = false;
};



struct Player_Socket {
    Player* P1 = NULL;
    SOCKET socket = NULL;
    Player* enemies = NULL;
};

int playerID = -1;
std::list<Bullets> bullets;
std::list<Bullets> bulletsToDraw;
bool shot = false;
HANDLE myMtx;


unsigned __stdcall ReciverFromServer(void* data)
{
    Player_Socket *si = (Player_Socket*)data;
    char buf[100];
    printf("got in\n");
    while (recv(si->socket, buf, 10, 0) > 0)
    {
        int x = 0;
        int y = 0;
        if (strcmp(buf, " KONIEC ") == 0)
        {
            closesocket(si->socket);
            WSACleanup();
            return 0;
        }
        if (buf[0] == 'P'&&buf[1]=='P') {
            x = buf[3] * 100;
            x += buf[4] * 10;
            x += buf[5];
            y = buf[6] * 100;
            y += buf[7] * 10;
            y += buf[8];
            si->P1->x = x;
            si->P1->y = y;
            playerID = buf[2];
            WaitForSingleObject(myMtx, INFINITE);
            std::list<Bullets> bulletsToDraww(bullets);
            bulletsToDraw = bulletsToDraww;
            bullets.clear();
            ReleaseMutex(myMtx);
        }
        else if (buf[0] == 'B'&&buf[1] == 'B')
        {
            x = buf[3] * 100;
            x += buf[4] * 10;
            x += buf[5];
            y = buf[6] * 100;
            y += buf[7] * 10;
            y += buf[8];
            Bullets bullet;
            bullet.x = x;
            bullet.y = y;
            bullets.push_back(bullet);
        }
        else if(buf[0]=='E'&&buf[1]=='E')
        {
            int sock = buf[2];
            x = buf[3] * 100;
            x += buf[4] * 10;
            x += buf[5];
            y = buf[6] * 100;
            y += buf[7] * 10;
            y += buf[8];
            si->enemies[sock].allive =true;
            si->enemies[sock].x = x;
            si->enemies[sock].y = y;
        }


    };
}

void EventHandler(SDL_Event event, Player* P1,SOCKET server_socket,bool* gameState)
{
    char buf[80];
    buf[0] = '0';
    int buff_length;
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) *gameState = false;
            if (event.key.keysym.sym == SDLK_UP) { buf[0] = 'U'; P1->ySpeed = -1; }
            else if (event.key.keysym.sym == SDLK_DOWN){ buf[0] = 'N'; P1->ySpeed = 1; }
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

void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color,char b) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32*)p = color;
};


void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color,char b) {
    for (int i = 0; i < l; i++) {
        DrawPixel(screen, x, y, color,b);
        x += dx;
        y += dy;
    };
};

void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
    SDL_Rect dest;
    dest.x = x - sprite->w / 2;
    dest.y = y - sprite->h / 2;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite, NULL, screen, &dest);
};

void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
    Uint32 outlineColor, Uint32 fillColor,char b) {
    int i;
    DrawLine(screen, x, y, k, 0, 1, outlineColor,b);
    DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor,b);
    DrawLine(screen, x, y, l, 1, 0, outlineColor,b);
    DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor,b);
    for (i = y + 1; i < y + k - 1; i++)
        DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor,b);
};

void FreeSurfaces(SDL_Surface** screen, SDL_Texture** scrtex, SDL_Window** window, SDL_Renderer** renderer)
{
    SDL_FreeSurface(*screen);
    SDL_DestroyTexture(*scrtex);
    SDL_DestroyWindow(*window);
    SDL_DestroyRenderer(*renderer);
    SDL_Quit();
};

void GetColors(Color* colors, SDL_Surface* screen)
{
    colors->black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
    colors->green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
    colors->red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
    colors->blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
};

void DrawMostForward(Color colors, SDL_Surface** screen, Player* P1, Player* enemies)
{
    // rysowanie mojego gracza eti
    // tekst informacyjny / info text
   
    for (int i = 0; i < 10; i++)
    {
        if (enemies[i].allive == true)
        {
            DrawRectangle(*screen, enemies[i].x, enemies[i].y, 20, 20, colors.blue, colors.red,'E');
        }
    }
        for (auto  bullet : bulletsToDraw)
        {
            DrawRectangle(*screen, bullet.x, bullet.y, 5, 5, colors.green, colors.red,'B');
        }
    DrawRectangle(*screen, P1->x, P1->y, 20, 20, colors.red, colors.blue,'P');
    //            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"

};

void playerInitializer(Player* players)
{
    players->x = 200;
    players->y = 200;
    players->xShootSpeed = 0;
    players->yShootSpeed = 0;
    players->xSpeed = 0;
    players->ySpeed = 0;
    players->xBorderMovement = 0;
    players->yBorderMovement = 0;
    players->allive = false;
}

int main(int argc, char** argv) 
{
    myMtx=CreateMutex(NULL, FALSE,L"MyMutex");
    SDL_Event event;
    SDL_Surface* screen = NULL;
    SDL_Texture* scrtex = NULL;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    Color colors;
    Player P1;
    Player enemies[10];

    playerInitializer(&P1);
    for (int i = 0; i < 10; i++)
        playerInitializer(&enemies[i]);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 0;
    }
    int rc = SDL_CreateWindowAndRenderer(500, 400, 0,
        &window, &renderer);
    if (rc != 0) {
        SDL_Quit();
        printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
        return 0;
    };
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, 500, 400);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetWindowTitle(window, "GRA NYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    //SDL_SetColorKey(images->charset, true, 0x000000);

    screen = SDL_CreateRGBSurface(0, 500, 400, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 500, 400);
    // wy³¹czenie widocznoœci kursora myszy
    SDL_ShowCursor(SDL_DISABLE);
    GetColors(&colors, screen);

    SOCKET server_socket;
    struct sockaddr_in sa;
    WSADATA wsas;
    WORD wersja;
    wersja = MAKEWORD(2, 0);
    WSAStartup(wersja, &wsas);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    memset((void*)(&sa), 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(8888);
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");

    int result;
    result = connect(server_socket, (struct sockaddr FAR*) & sa, sizeof(sa));
    if (result == SOCKET_ERROR)
    {
        printf("\ nBlad polaczenia !");
        return 0;
    }
    unsigned threadID, threadID2;
    double ticks =  SDL_GetTicks();
    double ticks2 = SDL_GetTicks();


    Player_Socket PS;
    Player_Socket* PS_pointer = &PS;
    PS_pointer->P1 = &P1;
    PS_pointer->socket = server_socket;
    PS_pointer->enemies = enemies;
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ReciverFromServer, (void*)PS_pointer, 0, &threadID);
    bool gamestate = true;
    while(gamestate)
    { 
        if (ticks2 - ticks > 16.6)//co oko³o 1 milisekundy 60fps
        {
            WaitForSingleObject(myMtx, INFINITE);
            SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
            SDL_RenderCopy(renderer, scrtex, NULL, NULL);
            SDL_RenderPresent(renderer);          
            SDL_FillRect(screen, NULL, colors.black);
            DrawMostForward(colors, &screen, &P1, enemies);
            ReleaseMutex(myMtx);
            ticks=ticks2= SDL_GetTicks();
        }
        else
        {
            ticks2= SDL_GetTicks();
        }
        while (SDL_PollEvent(&event))
        {
            EventHandler(event, &P1, server_socket, &gamestate);
        };
    }
    closesocket(server_socket);
    FreeSurfaces(&screen, &scrtex, &window, &renderer);
    WSACleanup();
    return 0;
}

