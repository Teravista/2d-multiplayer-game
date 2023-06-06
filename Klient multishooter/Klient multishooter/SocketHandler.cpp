#include "SocketHandler.h"
#include "StructStorage.h"
#include <winsock.h>
#include <stdio.h>
#include <thread>
#include <list>

void SocketHandler::ReciverFromServer( Player* P1, Player* enemies, int* playerID, std::list<Bullets>* bullets, std::list<Bullets>* bulletsToDraw)
{
    char buf[100];
    printf("got in\n");
    while (recv(server_socket, buf, 10, 0) > 0)
    {
        int x = 0;
        int y = 0;
        if (strcmp(buf, " KONIEC ") == 0)
        {
            closesocket(server_socket);
            WSACleanup();
            return;
        }
        if (buf[0] == 'P' && buf[1] == 'P') {
            x = buf[3] * 100;
            x += buf[4] * 10;
            x += buf[5];
            y = buf[6] * 100;
            y += buf[7] * 10;
            y += buf[8];
            P1->x = x;
            P1->y = y;
            *playerID = buf[2];
            std::list<Bullets> bulletsToDraww(*bullets);
            *bulletsToDraw = bulletsToDraww;
            bullets->clear();
        }
        else if (buf[0] == 'B' && buf[1] == 'B')
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
            bullets->push_back(bullet);
        }
        else if (buf[0] == 'E' && buf[1] == 'E')
        {
            int sock = buf[2];
            x = buf[3] * 100;
            x += buf[4] * 10;
            x += buf[5];
            y = buf[6] * 100;
            y += buf[7] * 10;
            y += buf[8];
            enemies[sock].allive = true;
            enemies[sock].x = x;
            enemies[sock].y = y;
        }
        else if (buf[0] == 'H' && buf[1] == 'H')
        {
            if (buf[2])P1->wasHit = true;
        }
        else if (buf[0] == 'C' && buf[1] == 'C')
        {
            if (buf[2])P1->wasHit = true;
        }
    };
}

int SocketHandler::SocketInitializer()
{
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
    int result = connect(server_socket, (struct sockaddr FAR*) & sa, sizeof(sa));
    if (result == SOCKET_ERROR)
    {
        printf("\n Blad polaczenia !");
        return -1;
    }
    return 1;
}

void SocketHandler::SocketClose()
{
    closesocket(server_socket);
}