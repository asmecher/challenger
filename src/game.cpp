/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

#include "challenger.h"

const int NUM_IMAGES = 9;

const int NUM_CLOUDS = 20;
const int NUM_FIRE = 20;

const int BIG_BAD_FRAME = 800;

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a>b?b:a)

typedef struct {
	float x, y;
	float xSpeed, ySpeed;
	unsigned char ttl;
} Particle;

typedef struct {
	float score = 0;
	Particle clouds[NUM_CLOUDS];
	Particle fire[NUM_FIRE];
	bool doClouds = false;
	float shuttleY = 30, shuttleVelocity = 0;
	float shuttleSteer = 0;
	float shuttleX = 170;
	float padY = -70, padVelocity = 0;
	float gradientY = -1000, gradientVelocity = 0;
	bool doShuttle = true;
	bool leftDown = false, rightDown = false;
} Arena;

void updateArena(Arena *a) {
	a->shuttleY += a->shuttleVelocity;
	a->shuttleX += a->shuttleSteer;
	if (a->shuttleX <= 0) {
		a->shuttleX = a->shuttleSteer = 0; // Left-hand limit for shuttle
	}
	if (a->shuttleX >= 250) { // Right-hand limit for shuttle
		a->shuttleX = 250;
		a->shuttleSteer = 0;
	}

	a->padY += a->padVelocity;

	a->gradientY = MIN(0, a->gradientY + a->gradientVelocity);
}

void initFire(Arena *a) {
	for (int i=0; i<NUM_FIRE; i++) {
		a->fire[i].ttl = random() / (float) RAND_MAX * 48;
		a->fire[i].x = ((int) a->shuttleX) + (random() / (float) RAND_MAX * 80);
		a->fire[i].y = ((int) a->shuttleY) + (random() / (float) RAND_MAX * 110);
		a->fire[i].xSpeed = (random() / (float) RAND_MAX * 2) - 1;
		a->fire[i].ySpeed = (random() / (float) RAND_MAX * 2) - 1;
	}
}

int game(SDL_Surface *screen, SDL_Surface *vscreen, unsigned char players) {
	unsigned frameStart;
	TTF_Font *fontSmall = TTF_OpenFont("FreeSansBold.ttf", 10);
	TTF_Font *fontLarge = TTF_OpenFont("FreeSansBold.ttf", 42);
	if (!fontSmall || !fontLarge) {
		fprintf(stderr, "Could not load font.\n");
		return MENU_ERROR;
	}

	// Load images
	SDL_Surface *images[NUM_IMAGES];
	const char *imageNames[NUM_IMAGES] = {"gradient.png", "launch0.png", "launch1.png", "launch2.png", "launch3.png", "shuttle.png", "cloud.png", "smoke.png", "fire.png"};
	for (int i=0; i<NUM_IMAGES; i++) {
		SDL_Surface *loadedImage = IMG_Load(imageNames[i]);
		if (!loadedImage) {
			fprintf(stderr, "Could not load title image.\n");
			return MENU_ERROR;
		}
		images[i] = SDL_DisplayFormatAlpha(loadedImage);
		SDL_FreeSurface(loadedImage);
	}
	SDL_Surface *gradient = images[0], *launch0 = images[1], *launch1 = images[2], *launch2 = images[3], *launch3 = images[4], *shuttle = images[5], *cloudImage = images[6], *smokeImage = images[7], *fireImage = images[8];

	Mix_Chunk *boop = Mix_LoadWAV("boop.wav");
	Mix_Chunk *theme = Mix_LoadWAV("game.wav");
	Mix_Chunk *boom = Mix_LoadWAV("boom.wav");
	Mix_Chunk *noise = Mix_LoadWAV("noise.wav");
	if (!boop || !theme || !boom || !noise) {
		fprintf(stderr, "Could not load audio.\n");
		return MENU_ERROR;
	}

	SDL_Color counterFontColour;
	Uint32 blackColour = SDL_MapRGB(screen->format, 0, 0, 0);
	counterFontColour.r = 0xf6; counterFontColour.g = 0xf1; counterFontColour.b = 0x6e;

	SDL_FillRect(screen, NULL, blackColour);

	SDL_Rect miscRect, playerTextRect, p1DestRect;
	SDL_Surface *playerOneImage = TTF_RenderText_Solid(fontSmall, "PLAYER ONE", counterFontColour);
	SDL_Surface *playerTwoImage = TTF_RenderText_Solid(fontSmall, "PLAYER TWO", counterFontColour);
	playerTextRect.x = 0;
	playerTextRect.y = -2;
	p1DestRect.x = 0; p1DestRect.y = 240;

	Arena a[2];

	SDL_Rect gameOverRect;
	gameOverRect.x = 30;
	gameOverRect.y = 70;
	
	SDL_Rect counterRect;
	counterRect.x = 150;
	counterRect.y = 80;

	char textBuf[80];

	Mix_PlayChannel(-1, theme, -1);

	// Initialize the clouds and fire locations and speeds
	for (unsigned char player=0; player<2; player++) {
		for (int i=0; i<NUM_CLOUDS; i++) {
			a[player].clouds[i].ttl = 0;
		}
		for (int i=0; i<NUM_FIRE; i++) {
			a[player].fire[i].ttl = 0;
		}
	}

	SDL_Rect shuttleRect;

	SDL_Rect padRect;
	padRect.x = 0;

	SDL_Rect gradientRect;
	gradientRect.x = 0;

	bool leave = false;

	unsigned long frameCounter = 0;
	SDL_Surface *launchImage = launch0;
	SDL_Surface *counterImage = NULL;
	SDL_Surface *gameOverImage = NULL;

	while (!leave) {
		// Handle events
		frameStart = SDL_GetTicks();
		SDL_Event e;
		while (SDL_PollEvent(&e)) switch (e.type) {
			case SDL_KEYDOWN: switch (e.key.keysym.sym) {
				case SDLK_LEFT: a[0].leftDown = true; break;
				case SDLK_RIGHT: a[0].rightDown = true; break;
				case SDLK_LCTRL: a[1].leftDown = true; break;
				case SDLK_LALT: a[1].rightDown = true; break;
				case SDLK_ESCAPE: leave = true;
			} break;
			case SDL_KEYUP: switch (e.key.keysym.sym) {
				case SDLK_LEFT: a[0].leftDown = false; break;
				case SDLK_RIGHT: a[0].rightDown = false; break;
				case SDLK_LCTRL: a[1].leftDown = false; break;
				case SDLK_LALT: a[1].rightDown = false; break;
			} break;
			case SDL_QUIT:
				leave = true;
				break;
		}

		if (controllerHasData()) {
			unsigned int c = readController();
			a[0].leftDown = c & P1_LEFT;
			a[0].rightDown = c & P1_RIGHT;
			a[1].leftDown = c & P2_LEFT;
			a[1].rightDown = c & P2_RIGHT;
		}

		switch (frameCounter) {
			/* Countdown */
			case 24*10:
			case 24*9:
			case 24*8:
			case 24*7:
			case 24*6:
			case 24*5:
			case 24*4:
			case 24*3:
			case 24*2:
				counterRect.x = 160; // 10 is over; split the digit
			case 24*1: {
				int count;
				if (counterImage) SDL_FreeSurface(counterImage);

				count = ((240-frameCounter)/24) + 1;
				sprintf(textBuf, "%i", count);
				counterImage = TTF_RenderText_Solid(fontLarge, textBuf, counterFontColour);

				Mix_PlayChannel(-1, boop, 0);
			} break;
			/* Launchpad arm */
			case (int) (24*3.5): launchImage = launch1; break;
			case (int) (24*5.5): launchImage = launch2; break;
			case (int) (24*7.5): launchImage = launch3; break;
			/* Counter over */
			case 24*11:
				// Get rid of the counter
				SDL_FreeSurface(counterImage);
				counterImage = NULL;
				a[0].doClouds = a[1].doClouds = true;
				Mix_PlayChannel(-1, noise, -1);
				break;
			/* Accelerate */
			case 260:
				a[0].shuttleVelocity = a[1].shuttleVelocity = -0.1;
				a[0].padVelocity = a[1].padVelocity = .2;
				a[0].gradientVelocity = a[1].gradientVelocity = .2;
				break;
			case 290:
				a[0].shuttleVelocity = a[1].shuttleVelocity = -.1;
				a[0].padVelocity = a[1].padVelocity = 1;
				a[0].gradientVelocity = a[1].gradientVelocity = 1;
				break;
			case 310:
				a[0].shuttleVelocity = a[1].shuttleVelocity = 0;
				a[0].padVelocity = a[1].padVelocity = 2;
				a[0].gradientVelocity = a[1].gradientVelocity = 2;
				break;
			case 330:
			case 350:
			case 370:
			case 390:
				a[0].padVelocity *= 1.5;
				a[1].padVelocity *= 1.5;
				break;
			/* Gasket problem? */
			case BIG_BAD_FRAME:
				Mix_HaltChannel(-1);
				a[0].doClouds = a[0].doShuttle = false;
				Mix_PlayChannel(-1, boom, 0);
				initFire(&(a[0]));
				gameOverImage = TTF_RenderText_Solid(fontLarge, "GAME OVER", counterFontColour);
				break;
			case BIG_BAD_FRAME+10:
				if (players == 2) {
					a[1].doClouds = a[1].doShuttle = false;
					Mix_PlayChannel(-1, boom, 0);
					initFire(&(a[1]));
				} break;
			/* Back to main menu */
			case BIG_BAD_FRAME + (24 * 4):
				leave = true;
				break;
		}

		for (unsigned char player=0; player<players; player++) {
			updateArena(&(a[player]));
			shuttleRect.x = a[player].shuttleX;
			shuttleRect.y = a[player].shuttleY;
			padRect.y = a[player].padY;
			gradientRect.y = a[player].gradientY;

			if (a[player].doClouds) {
				// Move the thrust clouds around
				unsigned char r = (random() / (float) RAND_MAX) * NUM_CLOUDS;
				a[player].clouds[r].ttl = random() / (float) RAND_MAX * 20;
				a[player].clouds[r].x = shuttleRect.x + 20 + (random() / (float) RAND_MAX * 15);
				a[player].clouds[r].y = shuttleRect.y + 140;
				a[player].clouds[r].xSpeed = (random() / (float) RAND_MAX * 4) - 2;
				a[player].clouds[r].ySpeed = random() / (float) RAND_MAX * 2;
	
				// That also means the shuttle can move
				if (a[player].leftDown && shuttleRect.x > 0) a[player].shuttleSteer -= a[player].gradientVelocity / 50;
				if (a[player].rightDown && shuttleRect.x < SCREEN_WIDTH-1) a[player].shuttleSteer += a[player].gradientVelocity / 50;
			}
	
			SDL_BlitSurface(gradient, NULL, vscreen, &gradientRect);
			SDL_BlitSurface(launchImage, NULL, vscreen, &padRect);
			if (a[player].doShuttle) SDL_BlitSurface(shuttle, NULL, vscreen, &shuttleRect);
			if (counterImage) SDL_BlitSurface(counterImage, NULL, vscreen, &counterRect);

			// Draw the clouds
			for (int i=0; i<NUM_CLOUDS; i++) if (a[player].clouds[i].ttl) {
				a[player].clouds[i].ttl--;
				a[player].clouds[i].x += a[player].clouds[i].xSpeed;
				a[player].clouds[i].y += a[player].clouds[i].ySpeed;
	
				miscRect.x = a[player].clouds[i].x;
				miscRect.y = a[player].clouds[i].y;
				SDL_BlitSurface(cloudImage, NULL, vscreen, &miscRect);
			}

			// Draw the fire
			for (int i=0; i<NUM_FIRE; i++) if (a[player].fire[i].ttl) {
				a[player].fire[i].ttl--;
				a[player].fire[i].x += a[player].fire[i].xSpeed;
				a[player].fire[i].y += a[player].fire[i].ySpeed;
	
				miscRect.x = a[player].fire[i].x;
				miscRect.y = a[player].fire[i].y;
				SDL_BlitSurface(a[player].fire[i].ttl>10?fireImage:smokeImage, NULL, vscreen, &miscRect);
			}

			if (!a[player].doShuttle && gameOverImage) SDL_BlitSurface(gameOverImage, NULL, vscreen, &gameOverRect);

			// Update score
			if (frameCounter < BIG_BAD_FRAME) a[player].score += abs(a[player].gradientVelocity) + abs(a[player].shuttleSteer);

			// P1 text
			SDL_BlitSurface(player==0?playerOneImage:playerTwoImage, NULL, vscreen, &playerTextRect);
			sprintf(textBuf, "SCORE: %i", (int) a[0].score);
			SDL_Surface *scoreImage = TTF_RenderText_Solid(fontSmall, textBuf, counterFontColour);
			miscRect.x = 0;
			miscRect.y = playerOneImage->h - 3;
			SDL_BlitSurface(scoreImage, NULL, vscreen, &miscRect);
			SDL_FreeSurface(scoreImage);

			// Double up the resolution
			if (players == 1) {
				SDL_Surface *zoomed = zoomSurface(vscreen, 2, 2.2, 0);
				SDL_BlitSurface(zoomed, NULL, screen, NULL);
				SDL_FreeSurface(zoomed);
			} else switch(player) {
				case 0: {
					SDL_Surface *zoomed = zoomSurface(vscreen, 2, 1.15, 0);
					SDL_BlitSurface(zoomed, NULL, screen, &p1DestRect);
					SDL_FreeSurface(zoomed);
				} break;
				case 1: {
					SDL_Surface *zoomed = zoomSurface(vscreen, 2, 1.15, 0);
					SDL_Surface *rotated = rotozoomSurface(zoomed, 180, 1, 0);
					SDL_BlitSurface(rotated, NULL, screen, NULL);
					SDL_FreeSurface(zoomed);
					SDL_FreeSurface(rotated);
				} break;
			}
		}

		// Blit the frame
		SDL_Flip(screen);

		// Go to sleep
		SDL_Delay(FRAME_TICKS - (SDL_GetTicks() - frameStart));
		frameCounter++;
	}

	// Halt audio
	Mix_HaltChannel(-1);

	if (gameOverImage) SDL_FreeSurface(gameOverImage);

	// Free resources
	for (int i=0; i<6; i++) {
		SDL_FreeSurface(images[i]);
	}
	SDL_FreeSurface(playerOneImage);
	SDL_FreeSurface(playerTwoImage);

	TTF_CloseFont(fontSmall);
	TTF_CloseFont(fontLarge);

	Mix_FreeChunk(boop);
	Mix_FreeChunk(theme);
	Mix_FreeChunk(boom);
	Mix_FreeChunk(noise);

	// Finished
	return 0;
}
