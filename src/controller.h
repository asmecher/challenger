/**
 * Challenger
 * ==========
 * Copyright (c) 2012 by Alec Smecher
 * Distributed under the GNU GPL v2. For full terms see the file COPYING.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#define P1_LEFT		16384
#define P1_RIGHT	4096
#define P1_UP		2048
#define P1_DOWN		8192
#define P1_FIRE		4

#define SELECT		2
#define START		1

#define P2_LEFT		1024
#define P2_RIGHT	512
#define P2_UP		8
#define P2_DOWN		256
#define P2_FIRE		16

bool openController();
unsigned int readController();
void closeController();
bool controllerHasData();

#endif
