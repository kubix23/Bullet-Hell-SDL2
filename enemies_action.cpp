#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <time.h> 
#include <stdlib.h>


extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	1000
#define SCREEN_HEIGHT	1000
#define TICK_INTERVAL    20 //1000/THIS VALUE = FPS
#define SPEED			 5 //1-10
#define ALPHACOLOR		0xFEFEFE


#include "enemies.h";
#include "function.h";

void player_t::shoot(int gameTime) {
	simpleShoot(this->x, this->y, 0, this->bmpPlayer->h/2, this->angle, this->bullets, 2);
}
void player_t::printMissle(SDL_Surface* screen) {
	printBullets(screen, this->bmpMissle, 1, this->bullets);
}
void player_t::updatetMissle(SDL_Surface* screen, double gameTime, hitboxs* tableH) {
	simpleUpdate(this->x, this->y, 0, this->bmpPlayer->h, this->shootSpeed, this->bullets, tableH, this->bmpMissle->clip_rect, &this->score, &this->series);
}
void player_t::removeLastMissle(SDL_Surface* screen) {
	simpleRemove(screen, this->bulletRange, this->bullets,&this->series);
}
//--------------------------------------------
void turret_t::shoot(int gameTime) {
	srand(gameTime*50 + (int)time(NULL));
	if (gameTime % 4 < 2) {
		if (rand()%12 == 1 && gameTime < 20) {
			bigExplodeBulletShoot(this->x, this->y, 0, this->bmpTurret->h, this->angle, this->bigBullets, 1);
		}
		else {
			simpleShoot(this->x, this->y, 0, this->bmpTurret->h, this->angle + rand() % 180, this->bullets, 1);
		}
	}
	else {
		simpleShoot(this->x, this->y, 0, this->bmpTurret->h, this->angle + 180 + rand() % 180, this->bullets, 1);

	}
}
void turret_t::printMissle(SDL_Surface* screen) {
	printBullets(screen, this->bmpMissle, 1, this->bullets);
	printBullets(screen, this->bmpBigMissle, 1, this->bigBullets);
}
void turret_t::updatetMissle(SDL_Surface* screen, double gameTime, hitboxs* tableH) {
	simpleUpdate(this->x, this->y, 0, this->bmpTurret->h, this->shootSpeed, this->bullets, tableH, this->bmpMissle->clip_rect);
	bigExplodeBulletUpdate(this->x, this->y, 0, this->bmpTurret->h, this->shootSpeed, this->bigBullets, tableH, this->bmpMissle->clip_rect);
}
void turret_t::removeLastMissle(SDL_Surface* screen) {
	simpleRemove(screen, this->bulletRange, this->bullets);
	bigExplodeBulletRemove(screen, this->bulletRange / 2, this->numberSplitShoot, this->bigBullets, this->bullets, 1);
}
//------------------------------------------------------
void ringTurret_t::shoot( int gameTime) {
	ringShoot(this->x, this->y, 0, this->bmpTurret->h, this->numberInRing, this->bullets, 1);
}
void ringTurret_t::printMissle(SDL_Surface* screen) {
	printBullets(screen, this->bmpMissle, this->numberInRing, this->bullets);
}
void ringTurret_t::updatetMissle(SDL_Surface* screen, double gameTime, hitboxs* tableH) {
	if((int)gameTime < 20)groundRing(screen,gameTime,this->x,this->y,200,1, tableH);
	ringUpdate(this->x, this->y, 0, this->bmpTurret->h, this->shootSpeed, this->numberInRing,this->bullets, tableH, this->bmpMissle->clip_rect);
}
void ringTurret_t::removeLastMissle(SDL_Surface* screen) {
	ringeRemove(screen, this->bulletRange, this->numberInRing, this->bullets);
}
//---------------------------------------------------------
void waveTurret_t::shoot(int gameTime) {
	srand(time(NULL));
	waveShoot(this->x, this->y, 0, this->bmpTurret->h, this->numberInWave,(rand()%10)*36,this->bullets,1);
}
void waveTurret_t::printMissle(SDL_Surface* screen) {
	printBullets(screen, this->bmpMissle, this->numberInWave, this->bullets);
}
void waveTurret_t::updatetMissle(SDL_Surface* screen, double gameTime, hitboxs* tableH) {
	if ((int)gameTime < 20)groundRect(screen, gameTime + 1, this->x, this->y, 400, 1, tableH);
	waveUpdate(this->x, this->y, 0, this->bmpTurret->h, this->shootSpeed, this->numberInWave, this->bullets, tableH, this->bmpMissle->clip_rect);
}
void waveTurret_t::removeLastMissle(SDL_Surface* screen) {
	waveRemove(screen, this->bulletRange, this->numberInWave, this->bullets);
}