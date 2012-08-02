/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

#include "controller.h"

#include <stdio.h>

int main(int argc, char **argv) {
	if (!openController()) {
		fprintf(stderr, "Could not open controller.\r\n");
		return(-1);
	}
	unsigned int last=0;
	while (true) {
		if (controllerHasData()) {
			unsigned int c = readController();
			if (c & P1_LEFT) printf("P1_LEFT ");
			if (c & P1_RIGHT) printf("P1_RIGHT ");
			if (c & P1_UP) printf("P1_UP ");
			if (c & P1_DOWN) printf("P1_DOWN ");
			if (c & P2_LEFT) printf("P2_LEFT ");
			if (c & P2_RIGHT) printf("P2_RIGHT ");
			if (c & P2_UP) printf("P2_UP ");
			if (c & P2_DOWN) printf("P2_DOWN ");
			if (c & START) printf("START ");
			if (c & SELECT) printf("SELECT ");
			if (c & P1_FIRE) printf("P1_FIRE ");
			if (c & P2_FIRE) printf("P2_FIRE ");
			printf("%i (%i)\n", c, c-last);
			fflush(stdout);
			last = c;
		}
	}
}
