#include "ConnectionHandler.h"
#include "StructStorage.h"
#include <winsock.h>
#include <thread>
#include <list>
#include <map>

ConnectionHandler::ConnectionHandler(std::mutex* bulletMtx, std::map<SOCKET, Player*>* players)
{
    this->players = players;
    this->bulletMtx = bulletMtx;
}

int ConnectionHandler::ServerInitialize()
{
    WSADATA wsas;
    int result = WSAStartup(MAKEWORD(2, 2), &wsas);
    if (result != 0)
    {
        printf("WSA failed to initialied");
        return -1;
    }
    serverSocketTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverSocketUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in socketAddrTCP;
    memset((void*)(&socketAddrTCP), 0, sizeof(socketAddrTCP));
    socketAddrTCP.sin_family = AF_INET;
    socketAddrTCP.sin_port = htons(8888);
    socketAddrTCP.sin_addr.s_addr = INADDR_ANY;

    

    result = bind(serverSocketTCP, (struct sockaddr FAR*) & socketAddrTCP, sizeof(socketAddrTCP));
    if (result != 0)
    {
        printf("Server socket TCP failed to initialize");
        return -1;
    }

    

    result = listen(serverSocketTCP, 20);
    if (result != 0)
    {
        printf("Couldnt modify TCP listeting size");
        return -1;
    }
    return 1;
}

void ConnectionHandler::ClientMessageReciver(SOCKET si, Player* player, std::list<Bullets>* newBullets)
{
    char buf[100];
    int cur_player = socketCounter - 1;
    while (recv(si, buf, 100, 0) > 0)
    {
        if (player->lifes > 0) {
            if (buf[0] == 'L')
                player->xSpeed = -1;
            if (buf[0] == 'R')
                player->xSpeed = 1;
            if (buf[0] == 'U')
                player->ySpeed = -1;
            if (buf[0] == 'N')
                player->ySpeed = +1;
            if (buf[0] == 'Y')
                player->ySpeed = 0;
            if (buf[0] == 'X')
                player->xSpeed = 0;
            if (buf[0] == 'W' || buf[0] == 'A' || buf[0] == 'S' || buf[0] == 'D')
            {
                Bullets bullet;
                bullet.x = player->x;
                bullet.y = player->y;
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
                this->bulletMtx->lock();
                newBullets->push_back(bullet);
                this->bulletMtx->unlock();
            }
        }
    };
}

void ConnectionHandler::SendToClient(SOCKET socket, char* buf, int buff_length)
{
    send(socket, buf, buff_length, 0);
}

void ConnectionHandler::SendToAllClients(std::map<SOCKET, Player*>* players, char* buf, int buff_length)
{
    std::map<SOCKET, Player*>::iterator iter;
    for (iter = players->begin(); iter != players->end(); iter++)
    {
        SendToClient(iter->first, buf, buff_length);
    }
}
void ConnectionHandler::SendToAllClientsButOneUDP(std::map<SOCKET, Player*>* players, SOCKET dontSend, char* buf, int buff_length)
{
    std::map<SOCKET, Player*>::iterator iter;

 

    for (iter = players->begin(); iter != players->end(); iter++)
    {
        if (iter->first != dontSend)    
        {
            struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&iter->first;
            std::string s = inet_ntoa(pV4Addr->sin_addr);
            struct sockaddr_in socketAddrUDP;
            socketAddrUDP.sin_family = AF_INET;
            socketAddrUDP.sin_port = htons(9999);
            socketAddrUDP.sin_addr.s_addr = inet_addr("127.0.0.1");
            int iResult = sendto(serverSocketUDP,
                buf, buff_length, 0, (SOCKADDR*)&socketAddrUDP, sizeof(socketAddrUDP));
            if (iResult == SOCKET_ERROR) {
                printf("Sending back response got an error: %d\n", WSAGetLastError());
            }
        }
    }
}

void ConnectionHandler::AcceptNewClient(std::list<Bullets>* newBullets)
{
    SOCKET cleintSocket;
    struct sockaddr_in sc;
    int lenc = sizeof(sc);
    cleintSocket = accept(serverSocketTCP, (SOCKADDR*)&sc, &lenc);


    Player* newPlayer = new Player();
    printf("Client Connected: %i\n", socketCounter);
    this->players->insert({ cleintSocket, newPlayer });
    std::thread th1(&ConnectionHandler::ClientMessageReciver,this, cleintSocket, newPlayer, newBullets);
    th1.detach();
}

void ConnectionHandler::DeletePlayer(SOCKET socketOfDeleted)
{
    char buf[12];
    buf[0] = buf[1] = 'E';
    buf[2] = socketOfDeleted;
    buf[9] = 0;
    SendToAllClients(this->players,buf,12);
}

void ConnectionHandler::CheckConnections(std::map<SOCKET, Player*>* connectedCleints)//remove any data connected to disconnected cleints
{
    std::map<SOCKET, Player*>::iterator iterator = connectedCleints->begin();
    std::map<SOCKET, Player*>::iterator iteratorEnd = connectedCleints->end();
    char testText[2]; testText[0] = 'Z'; testText[1] = 'Z';
    while (iterator != iteratorEnd)
    {
        int nSendBytes = send(iterator->first, testText, 2, 0);
        if (nSendBytes == SOCKET_ERROR)
        {
            SOCKET socket = iterator->first;
             closesocket(socket);
             delete(iterator->second);
             iterator = connectedCleints->erase(iterator);
             DeletePlayer(socket);
        }
        else
            ++iterator;
    }
}

void ConnectionHandler::ServerShutdown()
{
    closesocket(serverSocketTCP);
    WSACleanup();
}