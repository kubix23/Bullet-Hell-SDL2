#pragma once
#ifndef FUNCTION_H
#define FUNCTION_H
enum playerMove {
	PLAYER_UP2,
	PLAYER_UP1,
	PLAYER_LEFT2,
	PLAYER_LEFT1,
	PLAYER_DOWN2,
	PLAYER_DOWN1,
	PLAYER_RIGHT2,
	PLAYER_RIGHT1,
};

void DrawString(SDL_Surface* screen, int x, int y, const char* text,
    SDL_Surface* charset);
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color, int opacity = 255);
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color, int opacity = 255);
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
    Uint32 outlineColor, Uint32 fillColor, int opacity = 255);
void DrawCircle(SDL_Surface* screen, int x, int y, int radius, Uint32 color, int opacity = 255);

Uint32 time_left(int time);
Uint32 time_difference(int time);
int pointDistance(int x1, int y1, int x2, int y2);
Uint32 oneSec(Uint32 interval, void* param);
bool loadBmp(SDL_Surface** turretBMP, SDL_Surface** missleBMP,
    SDL_Surface** ringTurretBMP, SDL_Surface** waveTurretBMP, 
    SDL_Surface** playerBMP , SDL_Surface** coursorBMP,
    SDL_Surface** potionBMP, SDL_Surface** bigMissleBMP,
	SDL_Surface** playerMoveBMP, SDL_Surface** turretMoveBMP,
	SDL_Surface** explodeBMP);

void printBullets(SDL_Surface* screen, SDL_Surface* bmpMissle, int inSeries, query* h);
void simpleShoot(int x, int y, int frontX, int forntY, double angle, query* bullets, int team);
void simpleUpdate(int x, int y, int frontX, int forntY, int shootSpeed, query* h, hitboxs* tableH, SDL_Rect bulletHitbox, int* score = NULL, int* series = NULL);
void simpleRemove(SDL_Surface* screen, int bulletRange, query* bullets, int* series = NULL);

void ringShoot(int x, int y, int frontX, int forntY, int numberInRing, query* bullets, int team);
void ringUpdate(int x, int y, int frontX, int forntY, int shootSpeed, int numberInRing, query* hh, hitboxs* tableH, SDL_Rect bulletHitbox, int* score = NULL, int* series = NULL);
void ringeRemove(SDL_Surface* screen, int bulletRange, int numberInRing, query* bullets, int* series = NULL);

void waveShoot(int x, int y, int frontX, int forntY, int numberInSeries, double angle, query* bullets, int team);
void waveUpdate(int x, int y, int frontX, int forntY, int shootSpeed, int numberInWave, query* hh, hitboxs* tableH, SDL_Rect bulletHitbox, int* score = NULL, int* series = NULL);
void waveRemove(SDL_Surface* screen, int bulletRange, int numberInWave, query* bullets, int* series = NULL);

void bigExplodeBulletShoot(int x, int y, int frontX, int forntY, double angle, query* bigBullets, int team);
void bigExplodeBulletUpdate(int x, int y, int frontX, int forntY, int shootSpeed, query* bigBullets, hitboxs* tableH, SDL_Rect bulletHitbox, int* score = NULL, int* series = NULL);
void bigExplodeBulletRemove(SDL_Surface* screen, int bulletRange, int numberInRing, query* bigBullets, query* bullets, int team, int* series = NULL);

void groundRing(SDL_Surface* screen, double gameTime, int x, int y, int radius, int team, hitboxs* tableH);
void groundRect(SDL_Surface* screen, double gameTime, int x, int y, int side, int team, hitboxs* tableH);

int loadGame(SDL_Surface* screen, SDL_Rect* camera, float gameTime, SDL_Surface* charset, int* gameIntervalTime, player_t* player, int* game);
int loadGui(SDL_Surface* gui, float gameTime, SDL_Surface* charset, int printInfo, int* frames, double* delta, double* fpsTimer, double* fps, player_t* player,int* game);

void explode(SDL_Surface** screen, SDL_Surface* explodeBMP, int x, int y);

#endif
