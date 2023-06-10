#pragma once
struct Player
{
    int x = 200;
    int y = 200;
    int ySpeed = 0;
    int xSpeed = 0;
    int xShootSpeed = 0;
    int yShootSpeed = 0;
    int lifes = 3;
    int invincebleFrames = 0;
};

struct Bullets
{
    int x = 0;
    int y = 0;
    int xSpeed = 0;
    int ySpeed = 0;
};