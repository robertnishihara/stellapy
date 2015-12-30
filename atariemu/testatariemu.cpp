#include "atariemu.h"
#include <stdio.h>
int main() {
	emuState* e = emuNewState("pong","/Users/joschu/Synced/Proj/control/domain_data/atari_roms");	
	if (!e) return 1;
	int reward;
	bool gameOver;
	bool roundOver;
	char imgBuf[160*210*3];
	char* ramBuf = 0;
	emuReset(e);
	emuAction a = {};
	emuStep(e,a,reward,gameOver,roundOver);
	printf("r: %i, go: %i, roundOver: %i\n",reward,gameOver,roundOver);
}