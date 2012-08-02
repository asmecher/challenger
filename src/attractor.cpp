/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

#include "challenger.h"

void attractor(SDL_Surface *screen, SDL_Surface *vscreen) {
	// Load and prepare the background image
	SDL_Surface *credits[3];
	const char *imageNames[] = {"credits.png", "credits2.png", "credits3.png"};

	for (unsigned char i=0; i<3; i++) {
		credits[i] = IMG_Load(imageNames[i]);
		if (!credits[i]) {
			fprintf(stderr, "Could not load credit image %i.\n", imageNames[i]);
			return;
		}
	}

	Uint32 black = SDL_MapRGB(vscreen->format, 0, 0, 0);

	unsigned int frameStart;
	bool leave = false;
	unsigned int frame = 0;
	unsigned int imageNum = 0;

	while (!leave) {
		// Handle events
		frameStart = SDL_GetTicks();
		SDL_Event e;
		while (SDL_PollEvent(&e)) switch (e.type) {
			case SDL_KEYDOWN: leave = true; break;
			case SDL_QUIT: leave = true; break;
		}

		if (controllerHasData()) leave = true;

		// Draw the frame
		SDL_FillRect(vscreen, NULL, black);
		SDL_BlitSurface(credits[imageNum], NULL, vscreen, NULL);

		// Double up the resolution
		SDL_Surface *zoomed = zoomSurface(vscreen, 2, 2.31, 0);
		SDL_BlitSurface(zoomed, NULL, screen, NULL);
		SDL_FreeSurface(zoomed);

		// Blit the frame
		SDL_Flip(screen);

		// Go to sleep
		SDL_Delay(FRAME_TICKS - (SDL_GetTicks() - frameStart));
		frame++;

		if (frame > FPS * 20) {
			imageNum = (imageNum + 1) % 3;
			frame = 0;
		}
	}

	// Free resources
	for (unsigned char i=0; i<3; i++) {
		SDL_FreeSurface (credits[i]);
	}
}
