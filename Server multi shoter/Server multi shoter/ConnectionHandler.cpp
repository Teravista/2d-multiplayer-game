#include "ConnectionHandler.h"
#include "StructStorage.h"
#include <winsock.h>
#include <thread>
#include <list>

int ConnectionHandler::ServerInitialize()
{
    WSADATA wsas;
    int result = WSAStartup(MAKEWORD(1, 1), &wsas);
    if (result != 0)
    {
        printf("WSA failed to initialied");
        return -1;
    }
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socketAddr;
    memset((void*)(&socketAddr), 0, sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(8888);
    socketAddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(serverSocket, (struct sockaddr FAR*) & socketAddr, sizeof(socketAddr));
    if (result != 0)
    {
        printf("Server socket failed to initialize");
        return -1;
    }
    result = listen(serverSocket, 20);
    if (result != 0)
    {
        printf("Couldnt modify listeting size");
        return -1;
    }
    return 1;
}

void ConnectionHandler::ClientMessageReciver(SOCKET si, Player* P, std::list<Bullets>* bullets)
{
    char buf[100];
    int cur_player = socketCounter - 1;
    while (recv(si, buf, 100, 0) > 0)
    {
        if (buf[0] == 'L')
            P[cur_player].xSpeed = -1;
        if (buf[0] == 'R')
            P[cur_player].xSpeed = 1;
        if (buf[0] == 'U')
            P[cur_player].ySpeed = -1;
        if (buf[0] == 'N')
            P[cur_player].ySpeed = +1;
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
            bullets->push_back(bullet);
        }
    };
}

void ConnectionHandler::SendToClient(int ithSocket, char* buf, int buff_length)
{
    send(socketos[ithSocket], buf, buff_length, 0);
}

void ConnectionHandler::AcceptNewClient(Player* players, std::list<Bullets>* bullets)
{
    SOCKET cleintSocket;
    struct sockaddr_in sc;
    int lenc = sizeof(sc);
    cleintSocket = accept(serverSocket, (SOCKADDR*)&sc, &lenc);
    socketos[socketCounter] = cleintSocket;
    socketCounter++;
    printf("Client Connected: %i\n", socketCounter);
    std::thread th1(&ConnectionHandler::ClientMessageReciver,this, cleintSocket, players,bullets);
    th1.detach();
}

void ConnectionHandler::ServerShutdown()
{
    closesocket(serverSocket);
    WSACleanup();
}