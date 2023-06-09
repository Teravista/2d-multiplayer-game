#include "GameplayController.h"
#include <time.h>
#include <list>
#include "StructStorage.h"


#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 20

GameplayController::GameplayController(ConnectionHandler* connectionHandler)
{
    this->myConnectionH = connectionHandler;
}

bool GameplayController::DetectColission(Player P1, Player P2) {
    if (P1.x + PLAYER_WIDTH >= P2.x && P1.x - PLAYER_WIDTH <= P2.x
        && P1.y + PLAYER_HEIGHT >= P2.y && P1.y - PLAYER_HEIGHT <= P2.y)
        return true;
    return false;
}

bool GameplayController::BulletColision(Player player, std::list<Bullets> bullets) {

    if (bullets.empty()) return false;
    for (const auto& iter : bullets) {
        if (player.x + PLAYER_WIDTH / 2 >= iter.x && player.x - PLAYER_WIDTH / 2 <= iter.x
            && player.y + PLAYER_HEIGHT / 2 >= iter.y && player.y - PLAYER_HEIGHT / 2 <= iter.y)
            return true;
    }
    return false;
}

void GameplayController::GameStatePropagator(std::list<Bullets>* bullets,Player* P)
{
    char buf[100];
    buf[0] = '0';
    int buff_length;
    clock_t t = clock();
    while (true)
    {
        if ((double)(clock() - t) / CLOCKS_PER_SEC > 0.02)
        {
            std::list<Bullets>::iterator iter = bullets->begin();
            std::list<Bullets>::iterator end = bullets->end();
            while (iter != end)
            {
                iter->x += iter->xSpeed * 5;
                iter->y += iter->ySpeed * 5;
                if (iter->x < 30 || iter->x>500 - 50 || iter->y < 30 || iter->y>400 - 50)
                    iter = bullets->erase(iter);
                else
                    ++iter;
            }
            for (int i = 0; i < this->myConnectionH->socketCounter; i++)
            {
                P[i].x += P[i].xSpeed * 2;
                P[i].y += P[i].ySpeed * 2;
                if (P[i].x < 30 || P[i].x>500 - 50)
                    P[i].x -= P[i].xSpeed * 2;
                if (P[i].y < 30 || P[i].y>400 - 50)
                    P[i].y -= P[i].ySpeed * 2;
                buf[0] = buf[1] = 'P';
                buf[2] = this->myConnectionH->socketCounter;
                buf[3] = int(P[i].x / 100);
                buf[4] = int(P[i].x / 10) % 10;
                buf[5] = (int(P[i].x) % 100) % 10;
                buf[6] = int(P[i].y / 100);
                buf[7] = int(P[i].y / 10) % 10;
                buf[8] = (int(P[i].y) % 100) % 10;
                buf[9] = '\0';
                this->myConnectionH->SendToClient(i,buf,10);
                for (Bullets bullet : *bullets)
                {
                    buf[0] = buf[1] = 'B';
                    buf[3] = int(bullet.x / 100);
                    buf[4] = int(bullet.x / 10) % 10;
                    buf[5] = (int(bullet.x) % 100) % 10;
                    buf[6] = int(bullet.y / 100);
                    buf[7] = int(bullet.y / 10) % 10;
                    buf[8] = (int(bullet.y) % 100) % 10;
                    buf[9] = '\0';
                    this->myConnectionH->SendToClient(i, buf, 10);

                }
                for (int v = 0; v < this->myConnectionH->socketCounter; v++)
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
                        this->myConnectionH->SendToClient(i, buf, 10);
                    }
                }
                for (int v = 0; v < this->myConnectionH->socketCounter; v++) {
                    if (v != i) {
                        buf[0] = buf[1] = 'H';
                        buf[2] = DetectColission(P[i], P[v]);
                        this->myConnectionH->SendToClient(i, buf, 3);
                    }
                }

                buf[0] = buf[1] = 'C';
                buf[2] = BulletColision(P[i], *bullets);
                this->myConnectionH->SendToClient(i, buf, 3);

            }

            t = clock();
        }
    }
}