//----------------------------------------------------------------------------
void newgame() {
	score = 0;
	lives = 3;
	gamelevel = 0;
	shipx = 80;
	shotx = -1;
	shoty = -1;
	deadcounter = -1;
	saucers = -1;
  removeTrack(titleMusic);
  addTrack(mainMusic);
  mainMusic->play();
  addTrack(shootSound);
  addTrack(invaderDestroyed);
  addTrack(playerDestroyed);
	for (int i = 0; i < 4; i++) {
		invadershotx[i] = -1;
		invadershoty[i] = -1;
	}
	gamestatus = "newlevel";
}
//----------------------------------------------------------------------------
void newlevel() {
	invaderanz = 40;
	invaderctr = 39;
	invaderxr = 4;
	invaderyr = 4;
	checkdir = 0;
	nextxdir = 4;
	nextydir = 0;
	yeahtimer = 0;
	delayBip = 0;
	invadershottimer = 60;
	saucertimer = 240;
	int down = gamelevel;
	if (gamelevel > 8) { down = 16*2; }
	for (int i = 0; i < 8; i++) {
		invaderx[i] = 10 + i * 8*2;
		invaderx[i + 8] = 10 + i * 8*2;
		invaderx[i + 16] = 10 + i * 8*2;
		invaderx[i + 24] = 10 + i * 8*2;
		invaderx[i + 32] = 10 + i * 8*2;
		invadery[i] = 14 + down;
		invadery[i + 8] = 13*2 + down;
		invadery[i + 16] = 19*2 + down;
		invadery[i + 24] = 25*2 + down;
		invadery[i + 32] = 31*2 + down;
		invaders[i] = 4;
		invaders[i + 8] = 2;
		invaders[i + 16] = 2;
		invaders[i + 24] = 0;
		invaders[i + 32] = 0;
		invaderframe[i] = 0;
		invaderframe[i + 8] = 0;
		invaderframe[i + 16] = 0;
		invaderframe[i + 24] = 0;
		invaderframe[i + 32] = 0;
	}
	for (int i = 0; i < 4; i++) {
		bunkers[i] = 0;
		if (gamelevel > 5) { bunkers[i] = -1; }
	}
	gamestatus = "running";
}
//----------------------------------------------------------------------------
void showscore() {
	if (infoshow == 1 and saucers == -1) {
		if (lives > 1) { mp.display.drawBitmap(0, 0, playership[0], TFT_WHITE, 2); }
		if (lives > 2) { mp.display.drawBitmap(18, 0, playership[0], TFT_WHITE, 2); }
		mp.display.cursor_x= 42*2 - 4 * (score > 9) - 4 * (score > 99) - 4 * (score > 999);
		mp.display.cursor_y = 10;
		mp.display.print(score);
		mp.display.cursor_x = 144;
		mp.display.cursor_y = 10;
		mp.display.print(gamelevel + 1);
	}
}
//----------------------------------------------------------------------------
void nextlevelcheck() {
	// increment timer after all invaders killed
	if (invaderanz == 0) {
		yeahtimer++;
		if (yeahtimer >= 90) {
			gamelevel++;
			gamestatus = "newlevel";
		}
	}
}
//----------------------------------------------------------------------------
void handledeath() {
	deadcounter--;
	if (deadcounter == 0) {
		deadcounter = -1;
		lives--;
		shipx = 0;
		if (lives == 0) { gamestatus = "gameover"; }
	}
}