/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

#include "challenger.h"

int main(int argc, char* args[]) {
	// Open controller
	if (!openController()) {
		fprintf(stderr, "Could not open controller.\n");
		return -1;
	}

	// Start SDL
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		return -1;
	}
	SDL_Surface *screen = SDL_SetVideoMode( SCREEN_XRES, SCREEN_YRES, SCREEN_BPP, SDL_HWSURFACE | SDL_FULLSCREEN);
	SDL_Surface *vscreen = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0, 0, 0);
	TTF_Init();
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		return -1;
	}

	SDL_ShowCursor(0);

	bool quit = false, suspend = false;
	while (!quit && !suspend) {
		switch (menu(screen, vscreen)) {
			case MENU_ONEPLAYER:
				game(screen, vscreen, 1);
				break;
			case MENU_TWOPLAYER:
				game(screen, vscreen, 2);
				break;
			case MENU_ATTRACTOR:
				quit = true;
				break;
			case MENU_ERROR:
			case MENU_QUIT:
				quit = true;
				break;
			case MENU_SUSPEND:
				suspend = true;
				break;
		}
	}

	// Shut down
	SDL_FreeSurface(vscreen);
	SDL_Quit();
	TTF_Quit();
	Mix_CloseAudio();
	closeController();

	return suspend?-2:0;
}
