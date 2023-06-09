#pragma once
#include <list>
#include"./SDL2-2.0.10/include/SDL.h"
class WindowHandler
{
public:
	int SDL_Initialize();
	void TexturesUpdate(struct Player* P1,struct Player* enemies, std::list<struct Bullets> bulletsToDraw);
	void FreeSurfaces();
private:
	struct Colors;
	SDL_Surface* screen;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
	void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color, char b);
	void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color, char b);
	void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
	void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,Uint32 outlineColor, Uint32 fillColor, char b);
	void DrawEntities(Player* P1, Player* enemies, std::list<struct Bullets> bulletsToDraw);
	void GetColors(SDL_Surface* screen);
};

