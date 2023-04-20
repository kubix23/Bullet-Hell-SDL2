#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	1000
#define SCREEN_HEIGHT	1000
#define TICK_INTERVAL    20 //1000/THIS VALUE = FPS
#define SPEED			 5 //1-10

#include "enemies.h";
#include "function.h";



// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	if (x <= screen->w + sprite->w && y <= screen->h + sprite->h) {
		SDL_Rect dest;
		dest.x = x - sprite->w / 2;
		dest.y = y - sprite->h / 2;
		dest.w = sprite->w;
		dest.h = sprite->h;
		SDL_BlitSurface(sprite, NULL, screen, &dest);
	}
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color, int opacity) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	if (opacity == 255) {	
		*(Uint32*)p = color;
	}
	else if (opacity == 0) {
		return;
	}
	else {
		Uint8 a = (((Uint8)(color >> 24)) * (opacity)+(*(p + 3)) * (256 - opacity)) >> 8;
		Uint8 r = (((Uint8)(color >> 16)) * (opacity)+(*(p + 2)) * (256 - opacity)) >> 8;
		Uint8 g = (((Uint8)(color >> 8)) * (opacity)+(*(p + 1)) * (256 - opacity)) >> 8;
		Uint8 b = (((Uint8)(color)) * (opacity)+*(p) * (256 - opacity)) >> 8;
		*(Uint32*)p = (Uint32)(b | g << 8 | r << 16 | a << 24);
	}
	

};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color, int opacity) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color, opacity);
		x += dx;
		y += dy;
	};
};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor, int opacity) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

void DrawCircle(SDL_Surface* screen, int x, int y, int radius, Uint32 color, int opacity){
	for (int i = 0; i < radius; i++) {
		int a = sqrt(-pow(i, 2) + pow(radius, 2));
		for (int yy = 0; yy <= a; yy++) {
			if(x + i > 0 && y + yy > 0 && x + i < screen->w && y + yy < screen->h)DrawPixel(screen, x + i, y + yy, color,opacity);
			if (x - i > 0 && y + yy > 0 && x - i < screen->w && y + yy < screen->h)DrawPixel(screen, x - i, y + yy, color,opacity);
			if (x + i > 0 && y - yy > 0 && x + i < screen->w && y - yy < screen->h)DrawPixel(screen, x + i, y - yy, color,opacity);
			if (x - i > 0 && y - yy > 0 && x - i < screen->w && y - yy < screen->h)DrawPixel(screen, x - i, y - yy, color,opacity);
		}
	}
};