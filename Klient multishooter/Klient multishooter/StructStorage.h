#pragma once
#include "StructStorage.h"
#include <winsock.h>
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1

struct Player
{
    double x = 200;
    double y = 200;
    double ySpeed = 0;
    double xSpeed = 0;
    double xShootSpeed = 0;
    double yShootSpeed = 0;
    int lifes = 0;
    int invincebleFrames = 0;
};

struct Bullets
{
    double x = 0;
    double y = 0;
    double xSpeed = 0;
    double ySpeed = 0;
};

struct Player_Socket {
    Player* P1 = NULL;
    SOCKET socket = NULL;
    Player* enemies = NULL;
};

