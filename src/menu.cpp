/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

#include "challenger.h"

int menu(SDL_Surface *screen, SDL_Surface *vscreen) {
	unsigned frameStart;
	bool leave = false;

	// Load and prepare the background image
	SDL_Surface *loadedImage = IMG_Load("title.png");
	if (!loadedImage) {
		fprintf(stderr, "Could not load title image.\n");
		return MENU_ERROR;
	}
	SDL_Surface *background = SDL_DisplayFormatAlpha(loadedImage);
	SDL_FreeSurface(loadedImage);

	TTF_Font *fontSmall = TTF_OpenFont("FreeSansBold.ttf", 10);
	TTF_Font *fontLarge = TTF_OpenFont("FreeSansBold.ttf", 12);
	if (!fontSmall || !fontLarge) {
		fprintf(stderr, "Could not load font.\n");
		return MENU_ERROR;
	}

	SDL_Color playerFontColour, autuinFontColour;
	playerFontColour.r = playerFontColour.g = playerFontColour.b = 230;
	autuinFontColour.r = autuinFontColour.g = autuinFontColour.b = 200;

	SDL_Surface *onePlayerTextSmall = TTF_RenderText_Solid(fontSmall, "ONE PLAYER", playerFontColour);
	SDL_Surface *twoPlayerTextSmall = TTF_RenderText_Solid(fontSmall, "TWO PLAYER", playerFontColour);
	SDL_Surface *onePlayerTextLarge = TTF_RenderText_Solid(fontLarge, "ONE PLAYER", playerFontColour);
	SDL_Surface *twoPlayerTextLarge = TTF_RenderText_Solid(fontLarge, "TWO PLAYER", playerFontColour);
	SDL_Surface *autuinText = TTF_RenderText_Solid(fontSmall, "(C) 2012 AUTOMATIC TURQUOISE INTERNATIONAL", autuinFontColour);
	TTF_CloseFont(fontSmall);
	TTF_CloseFont(fontLarge);

	SDL_Rect onePlayerRectSmall, onePlayerRectLarge;
	SDL_Rect twoPlayerRectSmall, twoPlayerRectLarge;
	onePlayerRectSmall.x = 60; onePlayerRectLarge.x = 50;
	twoPlayerRectSmall.x = 190; twoPlayerRectLarge.x = 180;
	onePlayerRectSmall.y = twoPlayerRectSmall.y = 172;
	onePlayerRectLarge.y = twoPlayerRectLarge.y = 170;

	SDL_Rect autuinRect;
	autuinRect.x = 35;
	autuinRect.y = 187;

	Uint32 black = SDL_MapRGB(vscreen->format, 0, 0, 0);

	Mix_Chunk *boop = Mix_LoadWAV("boop.wav");
	Mix_Chunk *theme = Mix_LoadWAV("title.wav");
	if (!boop || !theme) {
		fprintf(stderr, "Could not load audio.\n");
		return MENU_ERROR;
	}

	// Initialize the star locations and speeds
	Star stars[NUM_STARS];
	for (int i=0; i<NUM_STARS; i++) {
		stars[i].x = random() / ((float) RAND_MAX) * SCREEN_WIDTH;
		stars[i].y = random() / ((float) RAND_MAX) * SCREEN_HEIGHT;
		float intensity = random() / (float) RAND_MAX;
		unsigned char colourIntensity = (intensity * 155) + 100;
		stars[i].speed = intensity * .2;
		stars[i].colour = SDL_MapRGB(vscreen->format, colourIntensity, colourIntensity, colourIntensity);
	}

	Mix_PlayChannel(-1, theme, -1);

	char selectedOption = 0;
	unsigned int frame = 0;

	while (!leave) {
		// Handle events
		frameStart = SDL_GetTicks();
		SDL_Event e;
		while (SDL_PollEvent(&e)) switch (e.type) {
			case SDL_KEYDOWN: switch (e.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_LEFT:
					if (selectedOption != 0) Mix_PlayChannel(-1, boop, 0);
					selectedOption = 0;
					frame = 0;
					break;
				case SDLK_DOWN:
				case SDLK_RIGHT:
					if (selectedOption != 1) Mix_PlayChannel(-1, boop, 0);
					selectedOption = 1;
					frame = 0;
					break;
				case SDLK_ESCAPE:
					selectedOption = -1;
					leave = true;
					break;
				case SDLK_UNKNOWN: // Suspend button
					selectedOption = -2;
					leave = true;
					break;
				case SDLK_RETURN:
					leave = true;
					break;
			case SDL_QUIT:
				selectedOption = -1;
				leave = true;
				break;
			}
		}

		// Debounce any keys currently down (wait 1 sec)
		if (frame >= FPS && controllerHasData()) {
			unsigned int c = readController();
			if ((c & P1_LEFT) && selectedOption != 0) {
				Mix_PlayChannel(-1, boop, 0);
				selectedOption = 0;
			} else if ((c & P1_RIGHT) && selectedOption != 1) {
				Mix_PlayChannel(-1, boop, 0);
				selectedOption = 1;
			} else if (c & P1_FIRE) {
				leave = true;
			} else if (c & SELECT) {
				selectedOption = -1;
				leave = true;
			}
			frame = FPS;
		}

		// Draw the frame
		SDL_FillRect(vscreen, NULL, black);
		SDL_LockSurface(vscreen);
		Uint32 *s = (Uint32 *) vscreen->pixels;
		for (unsigned int i=0; i<NUM_STARS; i++) {
			unsigned x = stars[i].x, y = stars[i].y;
			*(s + (y * SCREEN_WIDTH) + x) = stars[i].colour;
			stars[i].y += stars[i].speed;
			if (stars[i].y >= SCREEN_HEIGHT) stars[i].y = 0;
		}
		SDL_UnlockSurface(vscreen);
		SDL_BlitSurface(background, NULL, vscreen, NULL);
		SDL_BlitSurface(autuinText, NULL, vscreen, &autuinRect);
		SDL_BlitSurface(
			selectedOption==0?onePlayerTextLarge:onePlayerTextSmall,
			NULL,
			vscreen,
			selectedOption==0?&onePlayerRectLarge:&onePlayerRectSmall
		);

		SDL_BlitSurface(
			selectedOption==1?twoPlayerTextLarge:twoPlayerTextSmall,
			NULL,
			vscreen,
			selectedOption==1?&twoPlayerRectLarge:&twoPlayerRectSmall
		);

		// Double up the resolution
		SDL_Surface *zoomed = zoomSurface(vscreen, 2, 2.31, 0);
		SDL_BlitSurface(zoomed, NULL, screen, NULL);
		SDL_FreeSurface(zoomed);

		// Blit the frame
		SDL_Flip(screen);

		// Go to sleep
		SDL_Delay(FRAME_TICKS - (SDL_GetTicks() - frameStart));
		frame++;

		if (frame > FPS * 30) {
			attractor(screen, vscreen);
			frame = 0;
		}
	}

	// Stop the audio
	Mix_HaltChannel(-1);

	// One last boop for the quit/play action
	Mix_PlayChannel(-1, boop, 0);
	while(Mix_Playing(-1) != 0) SDL_Delay(10);

	// Free resources
	SDL_FreeSurface (background);
	SDL_FreeSurface (autuinText);
	SDL_FreeSurface (onePlayerTextSmall);
	SDL_FreeSurface (twoPlayerTextSmall);
	SDL_FreeSurface (onePlayerTextLarge);
	SDL_FreeSurface (twoPlayerTextLarge);
	Mix_FreeChunk(boop);
	Mix_FreeChunk(theme);

	switch (selectedOption) {
		case -2: return MENU_SUSPEND;
		case -1: return MENU_QUIT;
		case 0: return MENU_ONEPLAYER;
		case 1: return MENU_TWOPLAYER;
	}
}
