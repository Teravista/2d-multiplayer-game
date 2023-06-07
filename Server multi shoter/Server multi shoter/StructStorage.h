#pragma once
struct Player
{
    double x = 200;
    double y = 200;
    double ySpeed = 0;
    double xSpeed = 0;
    double xShootSpeed = 0;
    double yShootSpeed = 0;
    bool allive = false;
    bool wasHit = false;
};

struct Bullets
{
    double x = 0;
    double y = 0;
    double xSpeed = 0;
    double ySpeed = 0;
};