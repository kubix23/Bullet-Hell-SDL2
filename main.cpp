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
#define SCREEN_HEIGHT	700
#define TICK_INTERVAL   20 //1000/THIS VALUE = FPS
#define SPEED			5 //1-10
#define ALPHACOLOR		0xFEFEFE




#include "enemies.h";
#include "function.h";

Uint32 time_left(int time){
	 Uint32 now = SDL_GetTicks();
	if (time <= now)
		return 0;
	else
		return time - now;
}

Uint32 time_difference(int time) {
	Uint32 now = SDL_GetTicks();
	return now - time;
}

int pointDistance(int x1,int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

Uint32 oneSec(Uint32 interval, void* param) {
	bool* myParam = (bool*)param;
	*myParam = true;
	return interval;
}

bool loadBmp(SDL_Surface** turretBMP, SDL_Surface** missleBMP,
	SDL_Surface** ringTurretBMP, SDL_Surface** waveTurretBMP,
	SDL_Surface** playerBMP, SDL_Surface** coursorBMP,
	SDL_Surface** potionBMP, SDL_Surface** bigMissleBMP,
	SDL_Surface** playerMoveBMP, SDL_Surface** turretMoveBMP,
	SDL_Surface** explodeBMP) {

	*turretBMP = SDL_LoadBMP("./turret.bmp");
	*missleBMP = SDL_LoadBMP("./Missle.bmp");
	*ringTurretBMP = SDL_LoadBMP("./ringTurret.bmp");
	*waveTurretBMP = SDL_LoadBMP("./wave_turret.bmp");
	*playerBMP = SDL_LoadBMP("./center.bmp");
	*coursorBMP = SDL_LoadBMP("./coursor.bmp");
	*potionBMP = SDL_LoadBMP("./potion.bmp");
	*bigMissleBMP = SDL_LoadBMP("./bigMissle.bmp");
	*playerMoveBMP = SDL_LoadBMP("./Player_move/playermove.bmp");
	*turretMoveBMP = SDL_LoadBMP("./Player_move/turretMove.bmp");
	*explodeBMP = SDL_LoadBMP("./Player_move/Explode.bmp");

	if (*explodeBMP == NULL || *turretMoveBMP == NULL ||
		*playerMoveBMP == NULL || *bigMissleBMP == NULL || 
		*bigMissleBMP == NULL || *potionBMP == NULL ||
		*coursorBMP == NULL || *playerBMP == NULL || 
		*waveTurretBMP == NULL || *ringTurretBMP == NULL ||
		*missleBMP == NULL || *turretBMP == NULL) {
		printf("Image load error: %s\n", SDL_GetError());
		SDL_FreeSurface(*turretBMP);
		SDL_FreeSurface(*missleBMP);
		SDL_FreeSurface(*ringTurretBMP);
		SDL_FreeSurface(*waveTurretBMP);
		SDL_FreeSurface(*playerBMP);
		SDL_FreeSurface(*potionBMP);
		SDL_FreeSurface(*bigMissleBMP);
		SDL_FreeSurface(*playerMoveBMP);
		SDL_FreeSurface(*turretMoveBMP);


		SDL_Quit();
		return true;
	};
	if (!SDL_HasColorKey(*turretBMP)) {
		SDL_SetColorKey(*turretBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*missleBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*ringTurretBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*waveTurretBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*playerBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*coursorBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*potionBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*bigMissleBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*playerMoveBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*turretMoveBMP, SDL_TRUE, ALPHACOLOR);
		SDL_SetColorKey(*explodeBMP, SDL_TRUE, ALPHACOLOR);
	}

	return false;
}



//PRINT BULLETS
void printBullets(SDL_Surface* screen, SDL_Surface* bmpMissle, int inSeries, query* hh) {	
	for (int i = 0; i < inSeries; i++) {
		query* h = hh + i;
		h = h->next;
		while (h != NULL) {
			DrawSurface(screen, bmpMissle, h->x, h->y);
			h = h->next;
		}
	}
}

//SHOOT PATERN
void simpleShoot(int x, int y, int frontX, int forntY, double angle, query* bullets, int team) {
	x = x + ((frontX)*cos(angle * (-2 * M_PI / 360)) - (forntY)*sin(angle * (-2 * M_PI / 360)));
	y = y + ((frontX)*sin(angle * (-2 * M_PI / 360)) + (forntY)*cos(angle * (-2 * M_PI / 360)));
	bullets->add(x, y,angle,team);
}

void ringShoot(int x, int y, int frontX, int forntY, int numberInRing, query* bullets, int team) {
	for (double angle = 0, i = 0; angle < 360; angle += 360.0/ numberInRing, i++) {
		simpleShoot(x, y, frontX, forntY, angle, bullets, team);
		bullets++;
	}
}
void waveShoot(int x, int y, int frontX, int forntY, int numberInSeries, double angle, query* bullets, int team) {
	for (int number = 0, i = 0; number < numberInSeries; number++, i++) {
		simpleShoot(x, y, (-numberInSeries/2.0 + number)*30, forntY, angle, bullets , team);
		bullets++;
	}
}
void bigExplodeBulletShoot(int x, int y, int frontX, int forntY, double angle, query* bigBullets, int team) {
	simpleShoot(x, y, frontX, forntY, angle, bigBullets, team);
}


//UPDATE PATTERN
void simpleUpdate(int x, int y, int frontX, int forntY, int shootSpeed, query* h, hitboxs* tableH, SDL_Rect bulletHitbox, int* score, int* series)	 {
	query* l = h->next;
	while (l != NULL) {
		l->distance += shootSpeed *SPEED;
		l->x = l->x - (shootSpeed * SPEED) * sin(l->angle * (-2 * M_PI / 360));
		l->y = l->y + (shootSpeed * SPEED) * cos(l->angle * (-2 * M_PI / 360));
		l = l->next;
	}
	(*tableH).updateLife(h, bulletHitbox, score, series);
}

void ringUpdate(int x, int y, int frontX, int forntY, int shootSpeed, int numberInRing, query* hh, hitboxs* tableH, SDL_Rect bulletHitbox, int* score, int* series) {

	for (double angle = 0; angle < 360; angle += 360.0 / numberInRing) {
		simpleUpdate(x, y, frontX, forntY, shootSpeed, hh, tableH, bulletHitbox, score, series);
		hh++;
	}
}
void waveUpdate(int x, int y, int frontX, int forntY, int shootSpeed, int numberInSeries, query* hh, hitboxs* tableH, SDL_Rect bulletHitbox, int* score, int* series) {
	for (int number = 0; number < numberInSeries; number++) {
		simpleUpdate(x, y, (-numberInSeries/2 + number)*30, forntY, shootSpeed, hh, tableH, bulletHitbox, score, series);
		hh++;
	}
}
void bigExplodeBulletUpdate(int x, int y, int frontX, int forntY, int shootSpeed, query* bigBullets, hitboxs* tableH, SDL_Rect bulletHitbox, int* score, int* series) {
	simpleUpdate(x, y, frontX, forntY, shootSpeed, bigBullets, tableH, bulletHitbox, score, series);
}

//REMOVE PATTERN
void simpleRemove(SDL_Surface* screen, int bulletRange, query* bullets, int* series) {
	if (bullets->next != NULL) {
		if (bullets->next->distance >= bulletRange) {
			bullets->removeFirst();
			if(series != NULL)*series = 0;
		}
		else if (!(bullets->next->x > 0 && bullets->next->y > 0 && bullets->next->x < screen->w && bullets->next->y < screen->h)) {
			bullets->removeFirst();
			if (series != NULL)*series = 0;
		}

	}
}

void ringeRemove(SDL_Surface* screen, int bulletRange, int numberInRing, query* bullets, int* series) {
	for (double angle = 0; angle < 360; angle += 360.0 / numberInRing) {
		simpleRemove(screen, bulletRange, bullets, series);
		bullets++;
	}
}
void waveRemove(SDL_Surface* screen, int bulletRange, int numberInSeries, query* bullets, int* series) {
	for (int number = 0; number < numberInSeries; number++) {
		simpleRemove(screen, bulletRange, bullets, series);
		bullets++;
	}
}
void bigExplodeBulletRemove(SDL_Surface* screen, int bulletRange, int numberInRing, query* bigBullets, query* bullets, int team, int* series) {
	if (bigBullets->next != NULL)
		if (bigBullets->next->distance >= bulletRange)
			for (double angle = 0, i = 0; angle < 360; angle += 360.0 / numberInRing, i++) {
				simpleShoot(bigBullets->next->x, bigBullets->next->y, 0, 0, angle, bullets, team);
			}
	simpleRemove(screen, bulletRange, bigBullets, NULL);
}


//SPECIAL PATTERN
void groundRing(SDL_Surface* screen, double gameTime, int x, int y, int radius, int team, hitboxs* tableH) {
	double a = ((int)gameTime % 10) + gameTime - floor(gameTime);
	if (a > 3.0) {
		if (a > 6.2) {
			DrawCircle(screen, x, y, radius, SDL_MapRGB(screen->format, 0, 0, 255));

			hitboxs* k = tableH ->next;
			while (k != NULL) {

				if (pow((k->rect.x + k->rect.w / 2) - x, 2) + pow((k->rect.y + k->rect.h / 2) - y, 2) <= pow(radius,2) && team != k->team) {

					if (*k->groundDemageTime > 0) {
						if (a - *k->groundDemageTime >= 1.5) {
							*k->life -= 20;
							*k->groundDemageTime = 0;
						}
					} else {
						*k->groundDemageTime = a;
					}
				}
				else if (pow(k->rect.x + k->rect.w/2 - x, 2) + pow(k->rect.y + k->rect.h/2 - y, 2) < pow(radius + 10, 2) && team != k->team) {
					*k->groundDemageTime = 0;
				}
				if (a >= 9.9) {
					*k->groundDemageTime = 0;
				}
				k = k->next;
			}

		}
		else if (cos(200 / (a + 1)) < 0) {
			DrawCircle(screen, x, y, radius, SDL_MapRGB(screen->format, 50, 50, 50),120);
		}
	}

}
void groundRect(SDL_Surface* screen, double gameTime, int x, int y, int side, int team, hitboxs* tableH) {
	double a = ((int)gameTime % 10) + gameTime - floor(gameTime);
	if (a > 3.0) {
		if (a > 6.2) {
			DrawRectangle(screen, x - side / 2, y - side / 2, side, side, SDL_MapRGB(screen->format, 120, 40, 50), SDL_MapRGB(screen->format, 120, 40, 50));

			SDL_Rect rect = {x - side /2, y - side /2, side, side};
			hitboxs* k = tableH->next;
			while (k != NULL) {
				if (SDL_HasIntersection(&k->rect,&rect) && team != k->team) {
					if (*k->groundDemageTime > 0) {

						if (a - *k->groundDemageTime >= 1.5) {
							*k->life -= 20;
							*k->groundDemageTime = 0;
						}
					}
					else {
						*k->groundDemageTime = a;
					}
				}
				else if (k->rect.x < rect.x -10 + rect.w + 20 &&
						k->rect.x + k->rect.w > rect.x -10 &&
						k->rect.y < rect.y -10 + rect.h + 20 &&
						k->rect.y + k->rect.h > rect.y -10 &&
						k->team != team) {
						*k->groundDemageTime = 0;
				}

				if (a >= 9.9) {
					*k->groundDemageTime = 0;
				}
				k = k->next;
			}

		}
		else if (cos(200 / (a + 1)) < 0) {
			DrawRectangle(screen, x- side /2, y- side /2, side, side, SDL_MapRGB(screen->format, 50, 50, 50), SDL_MapRGB(screen->format, 50, 50, 50), 120);
		}
	}

}


void explode(SDL_Surface** screen,SDL_Surface* explodeBMP, int x, int y) {
	SDL_Rect animation = { 0,0,100,100 };
	SDL_Rect target = { x - 50,y - 50,100,100 };
	animation.x = ((int)(SDL_GetTicks() * 8) % 3) * 100;
	SDL_BlitSurface(explodeBMP, &animation, *screen, &target);
}


int loadGame(SDL_Surface* screen, SDL_Rect* camera, float gameTime, SDL_Surface *charset, int *gameIntervalTime, player_t* player, int *game) {
	static int seed = time(NULL);
	srand(seed++);
	static SDL_Surface* playerBMP;
	static SDL_Surface* coursorBMP;
	static SDL_Surface* playerMoveBMP;


	static SDL_Surface* turretBMP;
	static SDL_Surface* ringTurretBMP;
	static SDL_Surface* waveTurretBMP;
	static SDL_Surface* turretMoveBMP;


	static SDL_Surface* missleBMP;
	static SDL_Surface* bigMissleBMP;
	static SDL_Surface* potionBMP;
	static SDL_Surface* explodeBMP;



	hitboxs tableH;

	static turret_t* turret = new turret_t(2, 2, 0, 400, 8);
	static ringTurret_t* ringTurret = new ringTurret_t(0, 3, 21, 700);
	static waveTurret_t* waveTurret = new waveTurret_t(0, 0, 10, 400);
	static healthPotion_t* potion = new healthPotion_t();


	char text[128];
	int mx, my, rmx, rmy, shoot;
	static int destruction[2] = { 0,0 };
	static int explodeTimer;
	SDL_Rect animation = { 100,0,100,100 };
	shoot = false;

	if (loadBmp(&turretBMP, &missleBMP,
		&ringTurretBMP, &waveTurretBMP,
		&playerBMP, &coursorBMP,
		&potionBMP, &bigMissleBMP,
		&playerMoveBMP, &turretMoveBMP,
		&explodeBMP)) {
			return 1;
		}

	potion->bmpPotion = potionBMP;
	if(potion->next == NULL)potion->add({(rand()%screen->w) + potionBMP->w,(rand()%screen->h) + potionBMP->h, potionBMP->w, potionBMP->h}, 3, 100);
	potion->print(screen);

	//=====================================
	player->bmpPlayer = playerBMP;
	player->bmpMissle = missleBMP;
	SDL_FillRect(player->bmpAnimation, NULL, ALPHACOLOR);

	tableH.add({ player->x - player->bmpPlayer->w / 2, player->y - player->bmpPlayer->h / 2, player->bmpPlayer->w, player->bmpPlayer->h }, &player->health, 2, &player->groundDamage, &player->iFrames);
	if (time_difference(*gameIntervalTime) % 200 < 80)player->iFrames -= 1;

	

	if (player->x > SCREEN_WIDTH / 2 &&
		player->x < screen->w - SCREEN_WIDTH / 2) {
		*camera = { player->x - SCREEN_WIDTH / 2, camera->y,SCREEN_WIDTH,SCREEN_HEIGHT };
	}
	if (player->y > SCREEN_HEIGHT / 2 &&
		player->y < screen->h - SCREEN_HEIGHT / 2) {
		*camera = {camera->x, player->y - SCREEN_HEIGHT / 2,SCREEN_WIDTH,SCREEN_HEIGHT };
	}

	const Uint8* curentClick = SDL_GetKeyboardState(NULL);
	const Uint32 buttons = SDL_GetMouseState(&mx, &my);

	if ((curentClick[SDL_SCANCODE_W] || curentClick[SDL_SCANCODE_UP]) && player->y > 0) {
		player->y += -SPEED;
		animation.x = PLAYER_UP1 * 100;
		SDL_BlitSurface(playerMoveBMP, &animation, player->bmpAnimation, NULL);
	}
	if ((curentClick[SDL_SCANCODE_S] || curentClick[SDL_SCANCODE_DOWN]) && player->y < screen->h) {
		player->y += SPEED;
		animation.x = PLAYER_DOWN1 * 100;
		SDL_BlitSurface(playerMoveBMP, &animation, player->bmpAnimation, NULL);

	}
	if ((curentClick[SDL_SCANCODE_A] || curentClick[SDL_SCANCODE_LEFT]) && player->x > 0) {
		player->x += -SPEED;
		animation.x = PLAYER_LEFT1 * 100;
		SDL_BlitSurface(playerMoveBMP, &animation, player->bmpAnimation, NULL);

	}
	if ((curentClick[SDL_SCANCODE_D] || curentClick[SDL_SCANCODE_RIGHT]) && player->x < screen->w) {
		player->x += SPEED;
		animation.x = PLAYER_RIGHT1 * 100;
		SDL_BlitSurface(playerMoveBMP, &animation, player->bmpAnimation, NULL);
	}
	if ((buttons & SDL_BUTTON_LMASK) != 0) shoot = true;
	

	//=====================================

	if (*game == 1) {
		if (turret->health <= 0 && turret->health >= -1000) {
			destruction[0] = turret->x;
			destruction[1] = turret->y;
			explodeTimer = SDL_GetTicks();
			delete turret;
			(*game)++;
			SDL_Delay(50);
		}
		if (turret->health <= 0) turret = new turret_t(2, 2, 0, 400, 8);
		SDL_FillRect(turret->bmpAnimation, NULL, ALPHACOLOR);
		turret->bmpTurret = turretBMP;
		turret->bmpMissle = missleBMP;
		turret->bmpBigMissle = bigMissleBMP;

		turret->x = 900 + gameTime * turret->movedx;
		turret->y = 200 + gameTime * turret->movedy;

		animation.x = ((int)(gameTime*8)%3) * 100;
		SDL_BlitSurface(turretMoveBMP, &animation, turret->bmpAnimation, NULL);
		double angle = atan2(turret->movedy, turret->movedx) + M_PI/2;

		int x = turret->x - (turret->bmpTurret->h/2)*sin(angle);
		int y = turret->y + (turret->bmpTurret->h / 2)*cos(angle);
		DrawSurface(screen, turret->bmpAnimation, x, y);

		if (turret->health > 0) tableH.add({ turret->x - turret->bmpTurret->w / 2, turret->y - turret->bmpTurret->h / 2, turret->bmpTurret->w, turret->bmpTurret->h }, &turret->health, 1, &turret->groundDamage, &turret->iFrames);
		if (time_difference(turret->timer) >= turret->RoF / SPEED) {
			if (turret->health > 0)turret->shoot(gameTime);
			turret->timer = SDL_GetTicks();
		}

		if (turret->health > 0) {
			turret->updatetMissle(screen, gameTime, &tableH);
			turret->printMissle(screen);
			turret->removeLastMissle(screen);
			DrawRectangle(screen, turret->x - turret->maxHealth / 2, turret->y - turret->bmpTurret->h / 2 - 20, turret->health, turret->maxHealth / 10, SDL_MapRGB(screen->format, 0, 0, 0), SDL_MapRGB(screen->format, 255, 0, 0));
			DrawSurface(screen, turret->bmpTurret, turret->x, turret->y);
			if (time_difference(*gameIntervalTime)%200 < 80)turret->iFrames -= 1;

		}


	}
	if (turret != NULL) {
		if (turret->health < turret->maxHealth && gameTime < 0.1) {
			delete turret;
		}
	}

	if (*game == 2) {
		if (ringTurret->health <= 0 && ringTurret->health >= -1000) {
			destruction[0] = ringTurret->x;
			destruction[1] = ringTurret->y;
			explodeTimer = SDL_GetTicks();
			delete ringTurret;
			(*game)++;
			SDL_Delay(50);
		}
		if (ringTurret->health <= 0)ringTurret = new ringTurret_t(0, 3, 21, 700);
		ringTurret->bmpTurret = ringTurretBMP;
		ringTurret->bmpMissle = missleBMP;;
		ringTurret->bmpAnimation = turretMoveBMP;

		ringTurret->x = 500 + gameTime * ringTurret->movedx;
		ringTurret->y = 500 + gameTime * ringTurret->movedy;
		if (ringTurret->health > 0) tableH.add({ ringTurret->x - ringTurret->bmpTurret->w / 2, ringTurret->y - ringTurret->bmpTurret->h / 2, ringTurret->bmpTurret->w, ringTurret->bmpTurret->h }, &ringTurret->health, 1, &ringTurret->groundDamage, &ringTurret->iFrames);
		
		
		if (time_difference(ringTurret->timer) >= ringTurret->RoF / SPEED) {
			if (ringTurret->health > 0)ringTurret->shoot(gameTime);
			ringTurret->timer = SDL_GetTicks();
		}
		if (ringTurret->health > 0) {
			ringTurret->updatetMissle(screen, gameTime, &tableH);
			ringTurret->removeLastMissle(screen);
			DrawRectangle(screen, ringTurret->x - ringTurret->maxHealth / 2, ringTurret->y - ringTurret->bmpTurret->h / 2 - 20, ringTurret->health, ringTurret->maxHealth / 10, SDL_MapRGB(screen->format, 0, 0, 0), SDL_MapRGB(screen->format, 255, 0, 0));
			DrawSurface(screen, ringTurret->bmpTurret, ringTurret->x, ringTurret->y);
			if (time_difference(*gameIntervalTime) % 200 < 80)ringTurret->iFrames -= 1;
		}
		ringTurret->printMissle(screen);

	}
	if (ringTurret != NULL) {
		if (ringTurret->health < ringTurret->maxHealth && gameTime < 0.1) {
			delete ringTurret;
		}
	}

	if (*game == 3) {
		if (waveTurret->health <= 0 && waveTurret->health >= -1000) {
			destruction[0] = waveTurret->x;
			destruction[1] = waveTurret->y;
			explodeTimer = SDL_GetTicks();
			delete waveTurret;
			(*game)++;
			SDL_Delay(50);
		}
		if (waveTurret->health <= 0)waveTurret = new waveTurret_t(0, 0, 10, 400);
		waveTurret->bmpTurret = waveTurretBMP;
		waveTurret->bmpMissle = missleBMP;
		waveTurret->bmpAnimation = turretMoveBMP;

		waveTurret->x = 300 + gameTime * waveTurret->movedx;
		waveTurret->y = 700 + gameTime * waveTurret->movedy;
		if (waveTurret->health > 0) tableH.add({ waveTurret->x - waveTurret->bmpTurret->w / 2, waveTurret->y - waveTurret->bmpTurret->h / 2, waveTurret->bmpTurret->w, waveTurret->bmpTurret->h }, &waveTurret->health, 1, &waveTurret->groundDamage, &waveTurret->iFrames);
		if (time_difference(waveTurret->timer) >= waveTurret->RoF / SPEED) {
			if (waveTurret->health > 0)waveTurret->shoot(gameTime);
			waveTurret->timer = SDL_GetTicks();
		}
		if (waveTurret->health > 0) {
			waveTurret->updatetMissle(screen, gameTime, &tableH);
			waveTurret->removeLastMissle(screen);
			DrawRectangle(screen, waveTurret->x - waveTurret->maxHealth / 2, waveTurret->y - waveTurret->bmpTurret->h / 2 - 20, waveTurret->health, waveTurret->maxHealth / 10, SDL_MapRGB(screen->format, 0, 0, 0), SDL_MapRGB(screen->format, 255, 0, 0));
			DrawSurface(screen, waveTurret->bmpTurret, waveTurret->x, waveTurret->y);
			if (time_difference(*gameIntervalTime) % 200 < 80)waveTurret->iFrames -= 1;
		}
		waveTurret->printMissle(screen);

	}
	if (waveTurret != NULL) {
		if (waveTurret->health < waveTurret->maxHealth && gameTime < 0.1) {
			delete waveTurret;
		}
	}

	if (time_difference(*gameIntervalTime) >= 5000 / SPEED) {
		*gameIntervalTime = SDL_GetTicks();
		player->score -= player->maxHealth*10/player->health;
	}


	
	//================================

	if (shoot) {
		static int shootTime = 0;
		if (time_difference(shootTime) >= 1000 / SPEED) {
			player->shoot(gameTime);
			shootTime = SDL_GetTicks();
		}
	}

	if (player->bmpPlayer != NULL)DrawSurface(screen, player->bmpPlayer, player->x, player->y);
	if(player->bmpAnimation != NULL)DrawSurface(screen, player->bmpAnimation, player->x, player->y);
	if (coursorBMP != NULL)DrawSurface(screen, coursorBMP, mx  + camera->x, my + camera->y);

	player->angle = atan2(mx + camera->x - player->x, my + camera->y - player->y) / M_PI * 180;
	player->updatetMissle(screen, gameTime, &tableH);
	player->removeLastMissle(screen);
	player->printMissle(screen);
	potion->updateTarget(&tableH,&player->score);

	if (destruction[0] && destruction[1]) {
		int diffrence = (int)time_difference(explodeTimer);

		if (diffrence <= 1000) {
			explode(&screen, explodeBMP, destruction[0], destruction[1]);
		}
		else{
			destruction[0] = 0;
			destruction[1] = 0;
		}
	}

	tableH.clear();
	SDL_FreeSurface(playerMoveBMP);
	SDL_FreeSurface(turretMoveBMP);
	SDL_FreeSurface(explodeBMP);
	SDL_FreeSurface(playerBMP);
	SDL_FreeSurface(coursorBMP);
	SDL_FreeSurface(turretBMP);
	SDL_FreeSurface(potionBMP);
	SDL_FreeSurface(ringTurretBMP);
	SDL_FreeSurface(waveTurretBMP);
	SDL_FreeSurface(missleBMP);

	return 0;
}

int loadGui(SDL_Surface* screen, float gameTime, SDL_Surface* charset, int printInfo, int* frames, double* delta, double* fpsTimer, double* fps, player_t* player,int* game) {
	char text[128];
	static int score = 0;
	char grade;

	if (player->health <= 0 || *game == 4) {
		SDL_Surface* temp = SDL_CreateRGBSurface(0, screen->w, screen->h, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

		SDL_FillRect(screen, NULL, SDL_MapRGBA(screen->format,0,0,0,150));


		sprintf(text, "GAME OVER");
		int scale1 = 8;
		SDL_Rect from1 = { 0,0, strlen(text) * 8, 8 };
		SDL_Rect to1 = { screen->w / 2 - strlen(text) * 8 * scale1 / 2, screen->h / 2 - 8 * scale1 / 2, strlen(text) * 8 * scale1, 8 * scale1 };
		DrawString(temp, from1.x, from1.y, text, charset);

		sprintf(text, "SCORE: %i", player->score);
		SDL_Rect from2 = { 0,8, strlen(text) * 8, 8 };
		SDL_Rect to2 = { screen->w / 2 - strlen(text) * 8 * scale1 / 2, screen->h / 2 + 8 * scale1 / 2, strlen(text) * 8 * scale1, 8 * scale1 };
		DrawString(temp, from2.x, from2.y, text, charset);


		if(score < player->score)score += 50;

		grade = 'A' + (int)((score + 3000) / 10000 + 100000 / (abs(score) + 3000));
		sprintf(text, "%C", grade);
		int scale2 = 35;
		SDL_Rect from3 = {0,16, 8, 8};
		SDL_Rect to3 = {screen->w / 2 - 8 * scale2 / 2, screen->h / 2 - 8 * scale2 / 2, 8 * strlen(text) * scale2, 8 * scale2};
		DrawString(temp, from3.x, from3.y, text, charset);

		sprintf(text, "MENU - ESC   PLAY AGAIN - R");
		int scale4 = 4;
		SDL_Rect from4 = { 0,24, strlen(text) * 8, 8 };
		SDL_Rect to4 = { screen->w / 2 - strlen(text) * 8 * scale4 / 2, screen->h / 2 + 30 * scale4, strlen(text) * 8 * scale4, 8 * scale4 };
		DrawString(temp, from4.x, from4.y, text, charset);

		SDL_BlitScaled(temp, &from1, screen, &to1);
		SDL_BlitScaled(temp, &from2, screen, &to2);
		SDL_BlitScaled(temp, &from4, screen, &to4);
		SDL_SetSurfaceColorMod(temp, 255, 0, 0);
		SDL_SetSurfaceAlphaMod(temp, 150);
		SDL_BlitScaled(temp, &from3, screen, &to3);

		SDL_FillRect(screen, &from1, ALPHACOLOR);
		SDL_FillRect(screen, &from2, ALPHACOLOR);
		SDL_FillRect(screen, &from3, ALPHACOLOR);
		SDL_FreeSurface(temp);
	}
	else {
		SDL_Surface* heartFull = SDL_LoadBMP("./fullheart.bmp");
		SDL_Surface* heartHalf = SDL_LoadBMP("./halfheart.bmp");
		if (!SDL_HasColorKey(heartFull)) {
			SDL_SetColorKey(heartFull, SDL_TRUE, ALPHACOLOR);
			SDL_SetColorKey(heartHalf, SDL_TRUE, ALPHACOLOR);
		}
		score = 0;


		if (printInfo) {
			*fpsTimer += *delta;
			if (*fpsTimer > 0.5) {
				*fps = *frames * 2;
				*frames = 0;
				*fpsTimer -= 0.5;
			};
			sprintf(text, "Szablon drugiego zadania, %.0lf klatek / s", *fps);
			DrawRectangle(screen, 10, 40, strlen(text) * 9, 36, SDL_MapRGB(screen->format, 150, 0, 0), SDL_MapRGB(screen->format, 31, 140, 200));
			DrawString(screen, 10 + strlen(text) / 4, 56, text, charset);
		}

		sprintf(text, "CZAS GRY= %.1lf s", gameTime);
		DrawString(screen, 10, 20, text, charset);

		sprintf(text, "SCORE: %i", player->score);
		DrawString(screen, screen->w/2, 10, text, charset);

		sprintf(text, "%i", player->health);
		DrawString(screen, screen->w - heartFull->w * 6 - strlen(text) * 8, 20, text, charset);
		SDL_Rect health = { screen->w - heartFull->w * 6 - strlen(text) * 8, 20, strlen(text) * 8, 8 };
		SDL_Rect healthTo = { screen->w - heartFull->w * 6 - strlen(text) * 8 * 3, 20 * 2, strlen(text) * 8 * 3, 8 * 3 };
		SDL_BlitScaled(screen, &health, screen, &healthTo);
		SDL_FillRect(screen, &health, ALPHACOLOR);


		for (int i = player->health, k = 1; i > 0; i -= 50, k++) {
			for (int j = i, o = 1; j > i - 50 && j > 0; j -= 10, o++) {
				if (j < 10 && j % 10 < 10) {
					DrawSurface(screen, heartHalf, screen->w - heartFull->w * o, heartFull->h * k);
				}
				else {
					DrawSurface(screen, heartFull, screen->w - heartFull->w * o, heartFull->h * k);
				}
			}
		}

		SDL_FreeSurface(heartFull);
		SDL_FreeSurface(heartHalf);
	}

	return 0;
}


// main
#ifdef __cplusplus
extern "C"
#endif

int init(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** scrtex, SDL_Surface** screen,
	SDL_Surface** gui, SDL_Surface* background,int* czarny,int* zielony,int* czerwony,int* niebieski) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&*window, &*renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(*window, "Bullet Hell");


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	*scrtex = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	*screen = SDL_CreateRGBSurface(0, background->w, background->h, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	*gui = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	*czarny = SDL_MapRGB((*screen)->format, 0x00, 0x00, 0x00);
	*zielony = SDL_MapRGB((*screen)->format, 0x00, 0xFF, 0x00);
	*czerwony = SDL_MapRGB((*screen)->format, 0xFF, 0x00, 0x00);
	*niebieski = SDL_MapRGB((*screen)->format, 0x11, 0x11, 0xCC);
}

bool loadBmpMain(SDL_Surface** charset, SDL_Surface** eti,
	SDL_Surface** background, SDL_Surface** center,
	SDL_Surface** menu) {

	*charset = SDL_LoadBMP("./cs8x8.bmp");
	*eti = SDL_LoadBMP("./eti.bmp");
	*background = SDL_LoadBMP("./background.bmp");
	*center = SDL_LoadBMP("./center.bmp");
	*menu = SDL_LoadBMP("./Menu.bmp");


	if (*charset == NULL || *eti == NULL ||
		*background == NULL || *center == NULL ||
		*menu == NULL) {
		printf("Image load error: %s\n", SDL_GetError());
		SDL_FreeSurface(*charset);
		SDL_FreeSurface(*eti);
		SDL_FreeSurface(*background);
		SDL_FreeSurface(*center);
		SDL_FreeSurface(*menu);
		SDL_Quit();
		return true;
	};
	if (!SDL_HasColorKey(*charset)) {
		SDL_SetColorKey(*charset, true, 0x000000);
		SDL_SetColorKey(*center, SDL_TRUE, ALPHACOLOR);
	}


	// wczytanie obrazka cs8x8.bmp


	

	



	


	return false;
}

void variableInit(int* frames, double* fpsTimer, double* fps,int *quit, 
	double* worldTime, double* gameTime , double* distance, 
	int* printInfo, int* game, int* menuSwitch, SDL_Rect* camera) {

	*frames = 0;
	*fpsTimer = 0;
	*fps = 0;
	*quit = 0;
	*worldTime = 0;
	*gameTime = 0;
	*distance = 0;
	*printInfo = 0;
	*game = 0;
	*menuSwitch = 0;
	*camera = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
}

int main(int argc, char **argv) {
	int t1, t2, tg1, tg2, gameIntervalTime, framesTimeControl, quit, frames, printInfo, game, menuSwitch;
	double delta, delta2, worldTime,gameTime, fpsTimer, fps, distance;
	int czarny, zielony, czerwony, niebieski;
	char text[128];

	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti;
	SDL_Surface	*gui;
	SDL_Surface* menu;

	SDL_Texture *scrtex;

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Rect camera;
	player_t* player = new player_t();
	

	SDL_Surface* background;
	SDL_Surface* center;
	loadBmpMain(&charset, &eti, &background, &center, &menu);

	quit = init(&window,&renderer, &scrtex,&screen, &gui, background, &czarny, &zielony, &czerwony, &niebieski);

	t1 = SDL_GetTicks();
	framesTimeControl = SDL_GetTicks() + TICK_INTERVAL;

	variableInit(&frames, &fpsTimer, &fps, &quit,
		&worldTime, &gameTime, &distance,
		&printInfo, &game, &menuSwitch, &camera);


	while(!quit) {
		t2 = SDL_GetTicks();

		SDL_FillRect(screen, NULL, czarny);
		SDL_FillRect(gui, NULL, ALPHACOLOR);

		// w tym momencie t2-t1 to czas w milisekundach,
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		worldTime += delta;
		
		if ((game == 0 ||  menuSwitch == 0) && game >= 0) {
			DrawSurface(gui,menu, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

		}else{
			tg2= SDL_GetTicks();
			delta2 = (tg2 - tg1) * 0.001;
			tg1 = tg2;
			gameTime += delta2;

			DrawSurface(screen, background, background->w / 2, background->h / 2);
			if(player->health > 0)quit = loadGame(screen, &camera, gameTime, charset, &gameIntervalTime,&*player,&game);
			quit = loadGui(gui, gameTime, charset, printInfo, &frames, &delta, &fpsTimer, &fps, &*player, &game);
		}

		SDL_BlitSurface(screen, &camera, screen, NULL);
		SDL_BlitSurface(gui, NULL, screen, NULL);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_FillRect(screen, NULL, czarny);

		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		SDL_RenderClear(renderer);


		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)

		while(SDL_PollEvent(&event)) {

			switch(event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_F1) {
						printInfo = printInfo ? 0 : 1;

					}else if(SDL_GetKeyboardState(NULL)[SDL_SCANCODE_N] && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_1] && menuSwitch == 0) {
						game = 1;
						menuSwitch = 1;
						tg1 = SDL_GetTicks();
						gameIntervalTime = SDL_GetTicks();
						delete player;
						player = new player_t();
						gameTime = 0;
						camera = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

					}else if(SDL_GetKeyboardState(NULL)[SDL_SCANCODE_N] && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_2] && menuSwitch == 0) {
						game = 2;
						menuSwitch = 1;
						tg1 = SDL_GetTicks();
						gameIntervalTime = SDL_GetTicks();
						delete player;
						player = new player_t();
						gameTime = 0;
						camera = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

					}else if(SDL_GetKeyboardState(NULL)[SDL_SCANCODE_N] && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_3] && menuSwitch == 0) {
						game = 3;
						menuSwitch = 1;
						tg1 = SDL_GetTicks();
						gameIntervalTime = SDL_GetTicks();
						delete player;
						player = new player_t();
						gameTime = 0;
						camera = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

					}else if(event.key.keysym.sym == SDLK_q && (game == 0 || menuSwitch == 0)) {
						quit = 1;
						menuSwitch = 0;

					}else if(event.key.keysym.sym == SDLK_ESCAPE && game != 0 && menuSwitch != 0) {
						menuSwitch = 0;
						tg1 = SDL_GetTicks();
						gameIntervalTime = SDL_GetTicks();

					} else if (event.key.keysym.sym == SDLK_ESCAPE && menuSwitch == 0 && game != 0) {
						menuSwitch = 1;
					} else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_R] && player->health <= 0 && game != 0) {
						menuSwitch = 1;
						tg1 = SDL_GetTicks();	
						gameIntervalTime = SDL_GetTicks();
						delete player;
						player = new player_t();
						gameTime = 0;
						camera = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
					}
					break;
				case SDL_KEYUP:

					break;
				case SDL_QUIT:
					if (game == 0) {
						quit = 1;
					}
					break;
				};
			};

		frames++;
		SDL_Delay(time_left(framesTimeControl));
		framesTimeControl += TICK_INTERVAL;
		};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(gui);
	SDL_FreeSurface(center);
	SDL_FreeSurface(menu);
	SDL_FreeSurface(background);

	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
