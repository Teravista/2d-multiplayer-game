#pragma once
#include <list>
#include <string>
#include"./SDL2-2.0.10/include/SDL.h"
class WindowHandler
{
public:
	int SDL_Initialize();
	void TexturesUpdateLVL1(struct Player* P1,struct Player* enemies, std::list<struct Bullets> bulletsToDraw);
	void TexturesUpdateLoadScreen(std::string input,int clockTicks);
	void FreeSurfaces();
private:
	struct Colors;
	SDL_Surface* screen;
	SDL_Surface* charset;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
	void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
	void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
	void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
	void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,Uint32 outlineColor, Uint32 fillColor);
	void DrawString(SDL_Surface* screen, double x, double y, std::string text, SDL_Surface* charset);
	void DrawEntities(Player* P1, Player* enemies, std::list<struct Bullets> bulletsToDraw);
	void DrawSpinningLogo(double i);
	void RefreshWindow();
	void GetColors(SDL_Surface* screen);
};

