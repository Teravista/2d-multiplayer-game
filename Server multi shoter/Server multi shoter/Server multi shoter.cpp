#include <winsock.h>
#include <iostream>
#include <process.h>
#include <time.h>
#include <list>
struct Player
{
    double x=200, xBorderMovement;
    double y=200, yBorderMovement;
    double ySpeed;
    double xSpeed;
    int currentEnemies;
    double xShootSpeed;
    double yShootSpeed;
};

struct Bullets
{
    double x=0, y=0;
    double xSpeed=0, ySpeed=0;
};
int socketCounter = 0;
SOCKET socketos[10];
Player P[10];
std::list<Bullets> bullets;

void PlayerInitializer(Player* players)
{
    players->x = 200;
    players->y = 200;
    players->xShootSpeed = 0;
    players->yShootSpeed = 0;
    players->xSpeed = 0;
    players->ySpeed = 0;
    players->xBorderMovement = 0;
    players->yBorderMovement = 0;
}

unsigned __stdcall TickHandler(void* data)
{
    char buf[100];
    buf[0] = '0';
    int buff_length;
    clock_t t = clock();
    while (true)
    {   
        if ((double)(clock() - t)/ CLOCKS_PER_SEC>0.02)
        {
            std::list<Bullets>::iterator iter = bullets.begin();
            std::list<Bullets>::iterator end = bullets.end();
            while (iter != end)
            {
                iter->x += iter->xSpeed * 5;
                iter->y += iter->ySpeed * 5;
                if (iter->x < 30 || iter->x>500 - 50 || iter->y < 30 || iter->y>400 - 50)
                    iter = bullets.erase(iter);
                else
                    ++iter;
            }
            for (int i = 0; i < socketCounter; i++)
            {
                P[i].x += P[i].xSpeed * 2;
                P[i].y += P[i].ySpeed * 2;
                if(P[i].x<30||P[i].x>500-50)
                    P[i].x -= P[i].xSpeed * 2;
                if (P[i].y < 30 || P[i].y>400 - 50)
                    P[i].y -= P[i].ySpeed * 2;
                buf[0]=buf[1] = 'P';
                buf[2] = socketCounter;
                buf[3] = int(P[i].x / 100);
                buf[4] = int(P[i].x / 10)%10;
                buf[5] = (int(P[i].x ) % 100)%10;
                buf[6] = int(P[i].y / 100);
                buf[7] = int(P[i].y / 10) % 10;
                buf[8] = (int(P[i].y) % 100) % 10;
                buf[9] = '\0';
                buff_length = 10;
                send(socketos[i], buf, buff_length, 0);
                for (Bullets bullet : bullets)
                {
                    buf[0]= buf[1] = 'B';
                    buf[3] = int(bullet.x / 100);
                    buf[4] = int(bullet.x / 10) % 10;
                    buf[5] = (int(bullet.x) % 100) % 10;
                    buf[6] = int(bullet.y / 100);
                    buf[7] = int(bullet.y / 10) % 10;
                    buf[8] = (int(bullet.y) % 100) % 10;
                    buf[9] = '\0';
                    buff_length = 10;
                    send(socketos[i], buf, buff_length, 0);
                }
                for (int v = 0; v < socketCounter; v++)
                {
                    if (v != i)
                    {
                        buf[0] = buf[1] = 'E';
                        buf[2] = v;
                        buf[3] = int(P[v].x / 100);
                        buf[4] = int(P[v].x / 10) % 10;
                        buf[5] = (int(P[v].x) % 100) % 10;
                        buf[6] = int(P[v].y / 100);
                        buf[7] = int(P[v].y / 10) % 10;
                        buf[8] = (int(P[v].y) % 100) % 10;
                        buf[9] = '\0';
                        buff_length = 10;
                        send(socketos[i], buf, buff_length, 0);
                    }
                }
                
                
            }

            t = clock();
        }
    }
}
unsigned __stdcall MovementHandler(void* data)
{
    SOCKET si = (SOCKET)data;
    char buf[100];
    int cur_player = socketCounter-1;
    while (recv(si, buf, 100, 0) > 0)
    {
        if (buf[0] == 'L')
            P[cur_player].xSpeed=-1;
        if (buf[0] == 'R')
            P[cur_player].xSpeed=1;
        if (buf[0] == 'U')
            P[cur_player].ySpeed=-1;
        if (buf[0] == 'N')
            P[cur_player].ySpeed=+1;
        if (buf[0] == 'Y')
            P[cur_player].ySpeed = 0;
        if (buf[0] == 'X')
            P[cur_player].xSpeed = 0;
        if (buf[0] == 'W' || buf[0] == 'A' || buf[0] == 'S' || buf[0] == 'D')
        {
            Bullets bullet;
            bullet.x = P[cur_player].x;
            bullet.y = P[cur_player].y;
            if (buf[0] == 'W')
                bullet.ySpeed = -1;
            if (buf[0] == 'S')
                bullet.ySpeed = 1;
            if (buf[0] == 'A')
                bullet.xSpeed = -1;
            if (buf[0] == 'D')
                bullet.xSpeed = +1;
            bullet.x += bullet.xSpeed * 30;
            bullet.y += bullet.ySpeed * 30;
            bullets.push_back(bullet);
        }
    };
    return 0;
}
int main()
{

    WSADATA wsas;
    int result;
    WORD wersja;
    wersja = MAKEWORD(1, 1);
    result = WSAStartup(wersja, &wsas);
    //Nastepnie tworzone jest gniazdko za pomoca funkcji socket
    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);


    struct sockaddr_in sa;
    memset((void*)(&sa), 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(8888);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);


    result = bind(s, (struct sockaddr FAR*) & sa, sizeof(sa));
    unsigned threadID1;
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &TickHandler, 0, 0, &threadID1);
    struct sockaddr {
        u_short sa_family;
        char sa_data[14];
    };
    result = listen(s, 5);
    SOCKET si;
    struct sockaddr_in sc;
    int lenc;
    for (;;)
    {
        lenc = sizeof(sc);
        si = accept(s, (SOCKADDR*)&sc, &lenc);
        socketos[socketCounter] = si;
        socketCounter++;
        printf("Client Connected: %i\n", socketCounter);
        unsigned threadID;
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &MovementHandler, (void*)si, 0, &threadID);
    }
    closesocket(s);
    WSACleanup();
    return 0;
}


