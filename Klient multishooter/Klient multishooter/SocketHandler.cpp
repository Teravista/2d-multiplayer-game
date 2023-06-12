#include "SocketHandler.h"
#include "StructStorage.h"
#include <winsock.h>
#include <stdio.h>
#include <thread>
#include <list>
#include <string>

SocketHandler::SocketHandler(std::mutex* bulletMtx)
{
    this->bulletMtx = bulletMtx;
}
void SocketHandler::ReviferFromServerUDP(std::map<int, Player*>* enemies)
{
    SOCKET UDPsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in RecvAddr;
    memset((void*)(&RecvAddr), 0, sizeof(RecvAddr));
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(9999);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int iResult = bind(UDPsocket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
    if (iResult != 0) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        return;
    }
    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);


    char buf[15];
    int bytesrev;
    while (bytesrev=recvfrom(UDPsocket, buf, 12, 0 /* no flags*/, (SOCKADDR*)&SenderAddr, &SenderAddrSize)) {
        int val;
        if (bytesrev == SOCKET_ERROR) {
            val = WSAGetLastError();
        }
       int x = 0;
        int y = 0;
        if (buf[0] == 'E' && buf[1] == 'E')
        {
             int sock = buf[2];
             Player* newPlayer;
             if (enemies->find(sock) != enemies->end()) {
                 auto iterator = enemies->find(sock);
                 newPlayer = iterator->second;
             }
             else
             {
                 newPlayer = new Player;
                 enemies->insert({ sock,newPlayer });
             }
             x = buf[3] * 100;
             x += buf[4] * 10;
             x += buf[5];
             y = buf[6] * 100;
             y += buf[7] * 10;
             y += buf[8];
             newPlayer->lifes = buf[9];
             newPlayer->invincebleFrames = buf[10];
             newPlayer->x = x;
             newPlayer->y = y;
            }
    }
}

void SocketHandler::ReciverFromServerTCP( Player* P1, std::list<Bullets>* bullets)
{
    char buf[100];
    printf("got in\n");
    while (recv(server_socket, buf, 12, 0) > 0)
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
            P1->lifes = buf[9];
            P1->invincebleFrames = buf[10];
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
            bullet.xSpeed = buf[9];
            bullet.ySpeed = buf[10];
            this->bulletMtx->lock();
            bullets->push_back(bullet);
            this->bulletMtx->unlock();
        }
       
    };
}

int SocketHandler::SocketInitializer(std::string serverIP)
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
    sa.sin_addr.s_addr = inet_addr(serverIP.c_str());
    this->serverIP = serverIP;
    
    int result = connect(server_socket, (struct sockaddr FAR*) & sa, sizeof(sa));
    if (result == SOCKET_ERROR)
    {
        printf("\n Blad polaczenia !");
        return -1;
    }
    return 1;
}

void SocketHandler::SendMessageToServer(char* message, int messageLength)
{
    send(this->server_socket, message, messageLength, 0);
}

void SocketHandler::SocketClose()
{
    closesocket(server_socket);
    WSACleanup();
}