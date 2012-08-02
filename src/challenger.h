/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

#ifndef CHALLENGER_H
#define CHALLENGER_H

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_timer.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"

#include "controller.h"

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 200;
const int SCREEN_XRES = 640;
const int SCREEN_YRES = 480;
const int SCREEN_BPP = 32;

const int FPS = 24;
const int FRAME_TICKS = 1000 / FPS; // Milliseconds per frame

const int MENU_SUSPEND = -2;
const int MENU_ERROR = -1;
const int MENU_QUIT = 1;
const int MENU_ONEPLAYER = 2;
const int MENU_TWOPLAYER = 3;
const int MENU_ATTRACTOR = 4;

const int NUM_STARS = 100;

typedef struct {
	float x, y;
	float speed;
	Uint32 colour;
} Star;

int menu(SDL_Surface *screen, SDL_Surface *vscreen);
void attractor(SDL_Surface *screen, SDL_Surface *vscreen);
int game(SDL_Surface *screen, SDL_Surface *vscreen, unsigned char players);

#endif
