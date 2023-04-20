#pragma once
#ifndef ENEMIES_H
#define ENEMIES_H

typedef struct query {
	double x = 0;
	double y = 0;
	double angle = 0;
	int team = 0;
	int distance = 0;

	query* last = this;
	query* next = NULL;

	void add(int posX, int posY, int angle, int team) {
		query* l = this;
		query* x = (query*)malloc(sizeof(query));

		x->x = posX;
		x->y = posY;
		x->angle = angle;
		x->distance = 0;
		x->team = team;
		x->next = NULL;
		l->last->next = x;
		l->last = x;

	}
	void removeFirst() {
		query* l = this;
		if (l->next != NULL) {
			query* x = l->next;
			l->next = l->next->next;
			free(x);
		}
		if (l->next == NULL) {
			l->last = this;
			l->next = NULL;
		}
	}
	void remove(query* bullet) {
		query* l = bullet;
		if (l->next != NULL) {
			query* x = l->next;
			l->next = l->next->next;
			free(x);
		}
		if (l->next == NULL) {
			l->last = this;
			l->next = NULL;
		}
	}
};

typedef struct hitboxs {
	int number = 0;
	int* life = NULL;
	SDL_Rect rect = { 0,0,0,0 };
	double* groundDemageTime = 0;
	double* iFrame = 0;
	int team = 0;
	hitboxs* next = NULL;

	void add(SDL_Rect rect, int* life, int team, double* ground, double* iFrame) {
		hitboxs* x = (hitboxs*)malloc(sizeof(hitboxs));
		x->number = this->next !=  NULL ? this->next->number + 1 : 1;
		x->life = life;
		x->rect = rect;
		x->team = team;
		x->groundDemageTime = ground;
		x->iFrame = iFrame;

		x->next = this->next != NULL ? this->next : NULL;
		this->next = x;
	}
	void remove(int num) {
		hitboxs* k = this;
		while (k->number != num) {
			k = k->next;
		}
		if (k->next != NULL) {
			hitboxs* u = k->next;
			k->next = k->next->next;
			free(u);
		}
	}
	void clear() {
		hitboxs* h = this;
		while (h->next != NULL) {
			hitboxs* u = h->next->next;
			free(h->next);
			h->next = u;
		}
	}

	void updateLife(query* bullets, SDL_Rect bulletHitbox, int* score = NULL, int* series = NULL) {
		hitboxs* k = this;
		bool skip = false;
		while (k->next != NULL ) {
			if (k->next->iFrame != NULL) {
				query* b = bullets;
				while (b->next != NULL && *(k->next)->iFrame <= 0.0) {
					if (b->next->x - bulletHitbox.w / 2 < k->next->rect.x + k->next->rect.w &&
						b->next->x + bulletHitbox.w / 2 > k->next->rect.x &&
						b->next->y - bulletHitbox.h / 2 < k->next->rect.y + k->next->rect.h &&
						b->next->y + bulletHitbox.h / 2 > k->next->rect.y &&
						b->next->team != k->next->team) {
						b->remove(b);
						if (series != NULL) {
							*series += 1;
							*score += *series * 3;
						}

						if (*(k->next)->life <= 0) {
							this->remove(k->number);
							if (k == NULL) {
								break;
							}
							skip = true;
							break;
						}
						else {
							*(k->next)->life -= 7;
							switch (k->next->team) {
								case 1:
									*(k->next)->iFrame = 1.0;
								break;
								case 2:
									*(k->next)->iFrame = 10.0;
								break;
							}
							break;
						}
					}
					b = b->next;
				}
				if (skip) {
					skip = false;
					continue;
				}
			}
			k = k->next;
		}
	}
};

typedef struct healthPotion_t {
	SDL_Surface* bmpPotion;
	SDL_Rect position;
	int healthUp = 10;
	int scoreUp = 100;
	healthPotion_t* next;

	void add(SDL_Rect rect, int healthUp, int scoreUp) {
		healthPotion_t* x = (healthPotion_t*)malloc(sizeof(healthPotion_t));

		x->position = rect;
		x->healthUp = healthUp;
		x->scoreUp = scoreUp;
		x->bmpPotion = this->bmpPotion;
		x->next = this->next != NULL ? this->next : NULL;
		this->next = x;
	}
	void updateTarget(hitboxs* tableH, int* score = NULL) {
		hitboxs* k = tableH;
		while (k->next != NULL) {
			healthPotion_t* p = this;
			while (p->next != NULL) {
				if (SDL_HasIntersection(&p->next->position, &k->next->rect)) {
					*score += p->next->scoreUp;
					*(k->next)->life += p->next->healthUp;

					healthPotion_t* temp = p->next->next;
					free(p->next);
					p->next = temp;
					break;
				}
				p = p->next;
			}
			k = k->next;
		}
	}
	void print(SDL_Surface* screen) {
		healthPotion_t* p = this;
		while (p->next != NULL) {
			SDL_BlitSurface(p->bmpPotion, NULL, screen, &p->next->position);
			p = p->next;
		}
	}
};

typedef struct player_t {
	player_t() {
		this->bullets = (query*)malloc(sizeof(query) * 1);
		for (int i = 0; i < 1; i++) {
			(this->bullets + i)->x = 0;
			(this->bullets + i)->y = 0;
			(this->bullets + i)->distance = 0;
			(this->bullets + i)->last = this->bullets + i;
			(this->bullets + i)->next = NULL;
		}
		this->bmpAnimation = SDL_CreateRGBSurface(0, 100, 100, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		this->bmpExplode = SDL_CreateRGBSurface(0, 100, 100, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	};
	SDL_Surface* bmpPlayer = NULL;
	SDL_Surface* bmpMissle = NULL;
	SDL_Surface* bmpAnimation = NULL;
	SDL_Surface* bmpExplode = NULL;


	int x = SCREEN_WIDTH / 2;
	int y = SCREEN_HEIGHT / 2;
	int health = 100;
	int maxHealth = 100;
	int score = 15000;
	int series = 0;
	double groundDamage = 0;
	double iFrames = 0;
	int shootSpeed = 2;
	int angle = 0;
	int bulletRange = 800;
	query* bullets;	

	void shoot(int gameTime);
	void printMissle(SDL_Surface* screen);
	void updatetMissle(SDL_Surface* screen, double gameTime, hitboxs* tableH);
	void removeLastMissle(SDL_Surface* screen);
};

typedef struct turret_t {
	turret_t(int movedx, int movedy, int angle, int bulletRange, int numberSplitShoot) :
		movedx(movedx* SPEED), movedy(movedy* SPEED), angle(angle), bulletRange(bulletRange), numberSplitShoot(numberSplitShoot){
		this->bullets = (query*)malloc(sizeof(query) * numberSplitShoot);
		this->bigBullets = (query*)malloc(sizeof(query));
		for (int i = 0; i < numberSplitShoot; i++) {
			(this->bullets + i)->x = 0;
			(this->bullets + i)->y = 0;
			(this->bullets + i)->distance = 0;
			(this->bullets + i)->last = this->bullets + i;
			(this->bullets + i)->next = NULL;
		}
		this->bigBullets->x = 0;
		this->bigBullets->y = 0;
		this->bigBullets->distance = 0;
		this->bigBullets->last = this->bigBullets;
		this->bigBullets->next = NULL;
		this->bmpAnimation = SDL_CreateRGBSurface(0, 100, 100, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		this->bmpExplode = SDL_CreateRGBSurface(0, 100, 100, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	};
	SDL_Surface* bmpTurret = NULL;
	SDL_Surface* bmpMissle = NULL;
	SDL_Surface* bmpBigMissle = NULL;
	SDL_Surface* bmpAnimation = NULL;
	SDL_Surface* bmpExplode = NULL;



	int x = -1;
	int y = -1;
	int movedx = 0;
	int movedy = 0;
	int health = 100;
	int maxHealth = 100;
	int shootSpeed = 2;
	int score = 15000;
	int series = 0;
	int numberSplitShoot = 8;
	int RoF = 1000;
	double timer = SDL_GetTicks();
	double groundDamage = 0;
	double iFrames = 0;
	int angleTurret = 0;
	int angle = 0;
	int bulletRange = 500;
	query* bullets;
	query* bigBullets;

	void shoot(int gameTime);
	void printMissle(SDL_Surface* screen);
	void updatetMissle(SDL_Surface* screen, double gameTime, hitboxs* tableH);
	void removeLastMissle(SDL_Surface* screen);
};

typedef struct ringTurret_t {
	ringTurret_t(int movedx, int movedy, int numberInRing, int bulletRange) :
		movedx(movedx* SPEED), movedy(movedy* SPEED), numberInRing(numberInRing), bulletRange(bulletRange) {
		this->bullets = (query*)malloc(sizeof(query) * numberInRing);
		for (int i = 0; i < numberInRing; i++) {
			(this->bullets + i)->x = 0;
			(this->bullets + i)->y = 0;
			(this->bullets + i)->distance = 0;
			(this->bullets + i)->last = this->bullets + i;
			(this->bullets + i)->next = NULL;
		}
		this->bmpAnimation = SDL_CreateRGBSurface(0, 100, 100, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		this->bmpExplode = SDL_CreateRGBSurface(0, 100, 100, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	};
	SDL_Surface* bmpTurret = NULL;
	SDL_Surface* bmpMissle = NULL;
	SDL_Surface* bmpAnimation = NULL;
	SDL_Surface* bmpExplode = NULL;


	int x = 0;
	int y = 0;
	int movedx = 0;
	int movedy = 0;
	int health = 100;
	int maxHealth = 100;
	int score = 15000;
	int series = 0;
	double timer = SDL_GetTicks();
	int RoF = 5000;
	double groundDamage = 0;
	double iFrames = 0;
	int shootSpeed = 2;
	int numberInRing = 5;
	int bulletRange = 500;
	query *bullets;

	void shoot(int gameTime);
	void printMissle(SDL_Surface* screen);
	void updatetMissle(SDL_Surface* screen, double gameTime, hitboxs* tableH);
	void removeLastMissle(SDL_Surface* screen);
};

typedef struct waveTurret_t {
	waveTurret_t(int movedx, int movedy, int numberInWave, int bulletRange) :
		movedx(movedx* SPEED), movedy(movedy* SPEED), numberInWave(numberInWave), bulletRange(bulletRange) {
		this->bullets = (query*)malloc(sizeof(query) * numberInWave);
		for (int i = 0; i < numberInWave; i++) {
			(this->bullets + i)->x = 0;
			(this->bullets + i)->y = 0;
			(this->bullets + i)->distance = 0;
			(this->bullets + i)->last = this->bullets + i;
			(this->bullets + i)->next = NULL;
		}
		this->bmpAnimation = SDL_CreateRGBSurface(0, 100, 100, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		this->bmpExplode = SDL_CreateRGBSurface(0, 100, 100, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	};
	SDL_Surface* bmpTurret = NULL;
	SDL_Surface* bmpMissle = NULL;
	SDL_Surface* bmpAnimation = NULL;
	SDL_Surface* bmpExplode = NULL;

	int x = 0;
	int y = 0;
	int movedx = 0;
	int movedy = 0;
	int health = 100;
	int maxHealth = 100;
	int score = 15000;
	int series = 0;
	int RoF = 5000;
	int angle = 0;
	double timer = SDL_GetTicks();
	double groundDamage = 0;
	double iFrames = 0;
	int shootSpeed = 2;
	int numberInWave = 5;
	int bulletRange = 500;
	query* bullets;

	void shoot(int gameTime);
	void printMissle(SDL_Surface* screen);
	void updatetMissle(SDL_Surface* screen, double gameTime, hitboxs* tableH);
	void removeLastMissle(SDL_Surface* screen);
};

#endif