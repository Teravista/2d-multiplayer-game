#include "GameplayController.h"
#include <time.h>
#include <list>
#include "StructStorage.h"


#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 20
#define PLAYER_INVINSIBILITY_FRAMES_DEFAULT 30

GameplayController::GameplayController(ConnectionHandler* connectionHandler, std::mutex* bulletMtx, std::map<SOCKET, Player*>* players)
{
    this->players = players;
    this->bulletMtx = bulletMtx;
    this->myConnectionH = connectionHandler;
}

bool GameplayController::CheckPlayerColission(std::map<SOCKET, Player*>* allivePlayers) {
    if (allivePlayers->empty())
        return NULL;
    std::map<SOCKET, Player*>::iterator iter;
    std::map<SOCKET, Player*>::iterator innerIter;
    Player* player1 = NULL;
    Player* player2 = NULL;
    for (iter = allivePlayers->begin(); iter != allivePlayers->end(); iter++)
    {
        player1 = iter->second;
        innerIter = iter;
        for (innerIter++; innerIter != allivePlayers->end(); innerIter++)
        {
            player2 = innerIter->second;

            if (player1->x + PLAYER_WIDTH >= player2->x && player1->x - PLAYER_WIDTH <= player2->x
                && player1->y + PLAYER_HEIGHT >= player2->y && player1->y - PLAYER_HEIGHT <= player2->y)

            {
                if (player1->invincebleFrames <= 0)
                {
                    player1->lifes--;
                    player1->invincebleFrames = PLAYER_INVINSIBILITY_FRAMES_DEFAULT;
                }
                if (player2->invincebleFrames <= 0)
                {
                    player2->lifes--;
                    player2->invincebleFrames = PLAYER_INVINSIBILITY_FRAMES_DEFAULT;
                }
            }
        }
    }
    return false;
}

bool GameplayController::CheckBulletColision(std::map<SOCKET, Player*>* allivePlayers, std::list<Bullets>* allBullets) {

    if (allBullets->empty()) return false;
    std::map<SOCKET, Player*>::iterator playerIter;
    Player* player = NULL;
    for (playerIter = allivePlayers->begin(); playerIter != allivePlayers->end(); playerIter++)
    {
        player = playerIter->second;
        if (player->invincebleFrames <= 0)
        {
            for (const auto& bulletIter : *allBullets) {

                if (player->x + PLAYER_WIDTH / 2 >= bulletIter.x && player->x - PLAYER_WIDTH / 2 <= bulletIter.x
                    && player->y + PLAYER_HEIGHT / 2 >= bulletIter.y && player->y - PLAYER_HEIGHT / 2 <= bulletIter.y)
                {
                    player->lifes--;
                    player->invincebleFrames = PLAYER_INVINSIBILITY_FRAMES_DEFAULT;
                }
            }
        }
    }
    return false;
}

void GameplayController::InvinsibleFrmesPropagator(std::map<SOCKET, Player*>* allivePlayers)
{
    std::map<SOCKET, Player*>::iterator iter;
    Player* player;
    for (iter = allivePlayers->begin(); iter != allivePlayers->end(); iter++)
    {
        player = iter->second;
        if (player->invincebleFrames > 0)
            player->invincebleFrames--;
    }
}

void BulletPropagator(std::list<Bullets>* allBullets)
{
    std::list<Bullets>::iterator bulletIterator = allBullets->begin();
    std::list<Bullets>::iterator bulletLast = allBullets->end();
    while (bulletIterator != bulletLast)
    {
        bulletIterator->x += bulletIterator->xSpeed * 5;
        bulletIterator->y += bulletIterator->ySpeed * 5;
        if (bulletIterator->x < 30 || bulletIterator->x>500 - 50 || bulletIterator->y < 30 || bulletIterator->y>400 - 50)
            bulletIterator = allBullets->erase(bulletIterator);
        else
            ++bulletIterator;
    }
}

void PlayerPropagator(std::map<SOCKET, Player*>* allivePlayers)
{
    std::map<SOCKET, Player*>::iterator iter;
    for (iter = allivePlayers->begin(); iter != allivePlayers->end(); iter++)
    {
        Player* player = iter->second;
        player->x += player->xSpeed * 2;
        player->y += player->ySpeed * 2;
        if (player->x < 30 || player->x>500 - 50)
            player->x -= player->xSpeed * 2;
        if (player->y < 30 || player->y>400 - 50)
            player->y -= player->ySpeed * 2;
    }
}

void GameplayController::PlayerSorter(std::map<SOCKET, Player*>* allivePlayers)
{
    allivePlayers->clear();
    std::map<SOCKET, Player*>::iterator iter;
    for (iter = this->players->begin(); iter != this->players->end(); iter++)
    {
        if (iter->second->lifes > 0)
            allivePlayers->insert(std::move(*iter));
    }
}

void GameplayController::SendPlayerPositionToClients(std::map<SOCKET, Player*>* allivePlayers, char* buf)
{
    std::map<SOCKET,Player*>::iterator iter;
    for (iter = allivePlayers->begin(); iter != allivePlayers->end(); iter++)
    {
        Player* player = iter->second;
        buf[0] = buf[1] = 'P';
        buf[2] = 1;//to remove
        buf[3] = int(player->x / 100);
        buf[4] = int(player->x / 10) % 10;
        buf[5] = (int(player->x) % 100) % 10;
        buf[6] = int(player->y / 100);
        buf[7] = int(player->y / 10) % 10;
        buf[8] = (int(player->y) % 100) % 10;
        buf[9] = player->lifes;
        buf[10] = player->invincebleFrames;
        buf[11] = '\0';
        this->myConnectionH->SendToClient(iter->first, buf, 12);
    }
}

void GameplayController::BulletSender(std::list<Bullets>* newBullets,char* buf,std::list<Bullets>* allBullets)
{
    std::list<Bullets>::iterator iter;
    for (iter = newBullets->begin(); iter != newBullets->end(); iter++)
    {
        Bullets bullet = Bullets(*iter);
        buf[0] = buf[1] = 'B';
        buf[3] = int(bullet.x / 100);
        buf[4] = int(bullet.x / 10) % 10;
        buf[5] = (int(bullet.x) % 100) % 10;
        buf[6] = int(bullet.y / 100);
        buf[7] = int(bullet.y / 10) % 10;
        buf[8] = (int(bullet.y) % 100) % 10;
        buf[9] = int(bullet.xSpeed);
        buf[10] = int(bullet.ySpeed);
        buf[11] = '\0';
        allBullets->push_front(bullet);
        this->myConnectionH->SendToAllClients(this->players, buf, 12);
    }
    this->bulletMtx->lock();
    newBullets->clear();
    this->bulletMtx->unlock();
}

void GameplayController::SendEnemyPositionToClients(std::map<SOCKET, Player*>* allivePlayers,char* buf)
//here we are using UDP because we dont need to always send  enemy infomration every tick
//we can afford to miss  a few
{
    std::map<SOCKET, Player*>::iterator iter;
    for (iter = allivePlayers->begin(); iter != allivePlayers->end(); iter++)
    {
        Player* player = iter->second;
        buf[0] = buf[1] = 'E';
        buf[2] = iter->first;
        buf[3] = int(player->x / 100);
        buf[4] = int(player->x / 10) % 10;
        buf[5] = (int(player->x) % 100) % 10;
        buf[6] = int(player->y / 100);
        buf[7] = int(player->y / 10) % 10;
        buf[8] = (int(player->y) % 100) % 10;
        buf[9] = player->lifes;
        buf[10] = player->invincebleFrames;
        buf[11] = '\0';
        this->myConnectionH->SendToAllClientsButOneUDP(this->players,iter->first, buf, 12);
    }
}
void GameplayController::GameStatePropagator(std::list<Bullets>* newBullets)
{
    std::list<Bullets> allBullets;
    std::map<SOCKET, Player*> allivePlayers;
    char buf[100];
    buf[0] = '0';
    int buff_length;
    clock_t t = clock();
    while (true)
    {
        if ((double)(clock() - t) / CLOCKS_PER_SEC > 0.02)
        {
            this->myConnectionH->CheckConnections(this->players);
            PlayerSorter(&allivePlayers);
            InvinsibleFrmesPropagator(&allivePlayers);

            BulletPropagator(&allBullets);
            PlayerPropagator(&allivePlayers);
            CheckBulletColision(&allivePlayers, &allBullets);
            CheckPlayerColission(&allivePlayers);

            SendPlayerPositionToClients(&allivePlayers,buf);
            SendEnemyPositionToClients(&allivePlayers,buf);
            BulletSender(newBullets,buf,&allBullets);
            
            t = clock();
        }
    }
}