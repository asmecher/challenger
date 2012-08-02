/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

#include "controller.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int cfp;

unsigned int lastControllerKey = 0;

bool openController() {
	cfp = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY);
	if (cfp<0) return false;
	return true;
}

unsigned int readController() {
	unsigned char newData;
	unsigned char *oldDataAsPtr = (unsigned char *) (&lastControllerKey);
	if (read(cfp, &newData, sizeof(newData)) == 1) {
		oldDataAsPtr[(newData & 128) ? 0 : 1] = newData;
	}
	return lastControllerKey;
}

bool controllerHasData() {
	unsigned int previousLast = lastControllerKey;
	unsigned int thisOne = readController();
	return (thisOne != previousLast);
}

void closeController() {
	close(cfp);
}
