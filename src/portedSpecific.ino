//----------------------------------------------------------------------------
void checkbuttons() {
	if (shipx < 0) shipx = 0;

	if (mp.buttons.repeat(BTN_LEFT, 1) && shipx > 0 && deadcounter == -1) { shipx-=2; }
	if (mp.buttons.repeat(BTN_RIGHT, 1) and shipx < 143 and deadcounter == -1) { shipx+=2; }

	if (mp.buttons.pressed(BTN_A) and shotx == -1 and deadcounter == -1) {
		shotx = shipx + 6;
		shoty = 106;
    shootSound->play();
		for (uint8_t i = 0; i < 8; i++){
      mp.leds[i] = CRGB(255,0,0);
    pixelsTimer=millis();
    pixelsState=1;  
	  }
	}
  if(millis()-pixelsTimer >= 50 && pixelsState==1){
    FastLED.clear();
  }
}
//----------------------------------------------------------------------------
void drawplayership() {
	if (deadcounter == -1) {
		mp.display.drawBitmap(shipx, 110, playership[0], TFT_WHITE, 2);
	}
	else {
		mp.display.drawBitmap(shipx, 110, playership[1 + invadershotframe], TFT_YELLOW, 2);
		handledeath();
	}
}
//----------------------------------------------------------------------------
void drawplayershot() {
	if (shotx != -1) {
		shoty = shoty - 4;
		mp.display.drawLine(shotx, shoty, shotx, shoty + 6, TFT_YELLOW);
    mp.display.drawLine(shotx+1, shoty, shotx+1, shoty + 6, TFT_YELLOW);
		if (shoty < 0) {
			shotx = -1;
			shoty = -1;
		}
	}
}
//----------------------------------------------------------------------------
void invaderlogic() {
	// increment invader counter
	if (invaderanz > 0) {
		checkdir = 0;
		do {
			invaderctr++;
			if (invaderctr > 39) {
				invaderctr = 0;
				checkdir = 1;
				invadersound = ++invadersound % 4;
				if (delayBip <= 0) {
					//gb.sound.fx(inv_move);
					if (invaderanz < 6) {
						delayBip = 5;
					}
					else if (invaderanz < 11) {
						delayBip = 3;
					} if (invaderanz < 21) {
						delayBip = 2;
					}
				}
				else { delayBip--; }
			}
		} while (invaders[invaderctr] == -1);

		// change direction?
		if (checkdir == 1) {
			if (nextydir != 0) {
				invaderxr = 0;
				invaderyr = 2;
			}
			else {
				invaderxr = nextxdir;
				invaderyr = 0;
			}
			checkdir = 0;
		}

		// change invader position
		invaderx[invaderctr] = invaderx[invaderctr] + invaderxr;
		invadery[invaderctr] = invadery[invaderctr] + invaderyr;

		// determine bunker removal if invaders are too low
		if (invadery[invaderctr] > 80) {
			for (int i = 0; i < 4; i++) {
				bunkers[i] = -1;
			}
		}

		// determine game over if invaders reach bottom
		if (invadery[invaderctr] > 100) {
			gamestatus = "gameover";
		}

		// determine screen border hit -> go down, then change direction
		if (invaderx[invaderctr] > 140 and invaderxr > 0) {
			nextxdir = -4;
			nextydir = 4;
		}
		if (invaderx[invaderctr] < 4 and invaderxr < 0) {
			nextxdir = 4;
			nextydir = 4;
		}
		if (invaderyr != 0) { nextydir = 0; }

		//change invader shape
		invaderframe[invaderctr] = ++invaderframe[invaderctr] % 2;

		// remove killed invader
		if (invaders[invaderctr] == 6) {
			invaders[invaderctr] = -1;
			invaderanz--;
		}

		// release invadershoot
		if (invadershottimer <= 0 and invadershots < gamelevel + 1 and invadershots < 4 and invadery[invaderctr] < 80) {
			invadershottimer = 40 - gamelevel * 10;
			invadershots++;
			int flag = 0;
			for (int u = 0; u < 4; u++) {
				if (flag == 0 and invadershotx[u] == -1) {
					invadershotx[u] = invaderx[invaderctr] + 2;
					invadershoty[u] = invadery[invaderctr];
					flag = 1;
				}
			}
		}
	}
}
//----------------------------------------------------------------------------
void drawinvaders() {
	infoshow = 1;
	for (int i = 0; i < 40; i++) {
		if (invaders[i] != -1) {
			if (invaders[i] == 0) mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_ORANGE, 2);
			if (invaders[i] == 2) mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_PINK, 2);
			if (invaders[i] == 4) mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_BLUE, 2);
			if (invaders[i] == 6) mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_YELLOW, 2);
			
			if (invadery[i] < 5) {
				infoshow = 0;
			}
		}

		// determine collission: invader & player shoot
		checkl = invaderx[i];
		checkr = invaderx[i] + 12;
		checkt = invadery[i];
		checkb = invadery[i] + 8;
		if (invaders[i] == 4) {
			checkl++;
			checkr--;
		}
		if (invaders[i] != -1 and invaders[i] != 6 and shotx >= checkl and shotx <= checkr and shoty + 2 >= checkt and shoty <= checkb) {
			score = score + invaders[i] * 10 + 10;
			invaders[i] = 6;
			shotx = -1;
			shoty = -1;
			invaderDestroyed->play();
		}
	}
}
//----------------------------------------------------------------------------
void invadershot() {
	// handle invadershoot timer & framecounter
	invadershottimer--;
  
	invadershotframe = ++invadershotframe % 2;

	// move invadershots, draw & check collission
	for (int i = 0; i < 4; i++) {
		if (invadershotx[i] != -1) {
			invadershoty[i] = invadershoty[i] + 2;
			mp.display.drawBitmap(invadershotx[i], invadershoty[i], bomb[invadershotframe], TFT_RED, 2);

			// check collission: invadershot & bunker
			for (int u = 0; u < 4; u++) {
        
				checkl = 22 + u * 36;
        checkr = 22 + u * 36 + 14;
        checkt = 90;
        checkb = 100;
				if (bunkers[u] != -1 and invadershotx[i] + 1 >= checkl and invadershotx[i] <= checkr and invadershoty[i] + 3 >= checkt and invadershoty[i] <= checkb) {
					bunkers[u]++;
					if (bunkers[u] > 4) { bunkers[u] = -1; }
					invadershotx[i] = -1;
					invadershoty[i] = -1;
					invadershots--;
				}
			}

			// check collission: invadershot & player
			checkl = shipx;
			checkr = shipx + 12;
			checkt = 107;
			checkb = 110;
			if (deadcounter == -1 and invadershotx[i] + 1 >= checkl and invadershotx[i] <= checkr and invadershoty[i] + 3 >= checkt and invadershoty[i] <= checkb) {
				deadcounter = 60;
				playerDestroyed->play();
			}

			//check collission: invadershot & playershoot
			checkl = invadershotx[i];
			checkr = invadershotx[i] + 1;
			checkt = invadershoty[i];
			checkb = invadershoty[i] + 3;
			if (shotx >= checkl and shotx <= checkr and shoty + 2 >= checkt and shoty <= checkb) {
				shotx = -1;
				shoty = -1;
				invadershotx[i] = -1;
				invadershoty[i] = -1;
				invadershots--;
			}

			// invadershoot on bottom off screen?
			if (invadershoty[i] > 128) {
				invadershotx[i] = -1;
				invadershoty[i] = -1;
				invadershots--;
			}
		}
	}
}
//----------------------------------------------------------------------------
void drawbunkers() {
	for (int i = 0; i < 4; i++) {
		checkl = 22 + i * 36;
		checkr = 22 + i * 36 + 14;
		checkt = 90;
		checkb = 100;
		if (bunkers[i] != -1 and shotx >= checkl and shotx <= checkr and shoty + 2 >= checkt and shoty <= checkb) {
			bunkers[i]++;
			shotx = -1;
			shoty = -1;
			if (bunkers[i] > 4) { bunkers[i] = -1; }
		}

		if (bunkers[i] != -1) {
			mp.display.drawBitmap(22 + i * 36, 90, bunker[bunkers[i]], TFT_GREEN, 2);
		}
	}
}
//----------------------------------------------------------------------------
void saucerappears() {
	saucertimer--;
	if (saucertimer <= 0) {
		saucertimer = 240;
		if (infoshow == 1 and saucers == -1) {
      mainMusic->pause();
      removeTrack(mainMusic);
      addTrack(ufoSound);
      ufoSound->setSpeed(2);
      ufoSound->setRepeat(1);
      ufoSound->play();
      
			saucers = 0;
			int i = random(2);
			if (i == 0) {
				saucerx = 0;
				saucerdir = 2;
			}
			else {
				saucerx = 146;
				saucerdir = -2;
			}
		}
	}
}
//----------------------------------------------------------------------------
void movesaucer() {
	if (saucers == 0) {
		saucerx = saucerx + saucerdir;
		if (saucerx <= 0 or saucerx >= 146) {
      removeTrack(ufoSound);
      mainMusic->resume();
      addTrack(mainMusic);
			saucers = -1;
		}

		// check collission: player shot & saucer  
		checkl = saucerx;
		checkr = saucerx + 20;
		checkt = 0;
		checkb = 6;
		if (shotx >= checkl and shotx <= checkr and shoty + 2 >= checkt and shoty <= checkb) {
			score += 100;
			saucers = 1;
			shotx = -1;
			shoty = -1;
			saucerwait = 30;
      removeTrack(ufoSound);
      invaderDestroyed->play();
      mainMusic->resume();
      addTrack(mainMusic);
		}
	}
}
//----------------------------------------------------------------------------
void drawsaucer() {
	if (saucers != -1) {
		mp.display.drawBitmap(saucerx, 0, saucer[saucers], TFT_RED, 2);
		if (saucers == 1) {
			saucerwait--;
			if (saucerwait <= 0) {
        removeTrack(ufoSound);
        addTrack(mainMusic);
        mainMusic->play();
        mainMusic->setRepeat(1);
				saucers = -1;
			}
		}
	}
}
#define STAR_COUNT 50            // Number of stars on the screen. Arduino UNO maxes out around 250.
#define BACKGROUND_COLOR 0x0000   // Background color in hex. 0x0000 is black.
#define STAR_SPEED_MIN 2          // Minimum movement in pixels per update. (value is inclusive)
#define STAR_SPEED_MAX 6         // Maximum movement in pixels per update. (value is inclusive)
#define STAR_COLOR 0xffff  
Star stars[STAR_COUNT]; 
void starsSetup()
{
    // Loop through each star.
    for(int i = 0; i < STAR_COUNT; i++)
    {
        // Randomize its position and speed.
        stars[i].randomize(0, mp.display.width(), 0, mp.display.height(), STAR_SPEED_MIN, STAR_SPEED_MAX);
    }
} 
//----------------------------------------------------------------------------
void eraseData()
{
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	while(1)
	{
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setTextFont(2);

		if (millis() - elapsedMillis >= multi_tap_threshold) {
			elapsedMillis = millis();
			blinkState = !blinkState;
		}

		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(4, 5);
		mp.display.printCenter("ARE YOU SURE?");
		mp.display.setCursor(4, 25);
		mp.display.printCenter("This cannot be reverted");

		if (blinkState){
			mp.display.drawRect((mp.display.width() - 60)/2, 102, 30*2, 9*2, TFT_RED);
			mp.display.setTextColor(TFT_RED);
			mp.display.setCursor(28*2, 103);
			mp.display.printCenter("DELETE");
		}
		else {
			mp.display.fillRect((mp.display.width() - 60)/2, 102, 30*2, 9*2, TFT_RED);
			mp.display.setTextColor(TFT_WHITE);
			mp.display.setCursor(28*2, 103);
			mp.display.printCenter("DELETE");
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			Serial.println("Go back");
			while (!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_A)) // DELETE
		{
			while(!mp.update());
			JsonArray &empty = mp.jb.createArray();
			mp.saveJSONtoSAV(highscoresPath, empty);
			gamestatus = "title";
			break;
		}
		mp.update();
	}
	while(!mp.update());
}

void dataDisplay()
{
	mp.homePopupEnable(0);
	shootSound->stop();
	titleMusic->stop();
	invaderDestroyed->stop();
	playerDestroyed->stop();
	removeTrack(shootSound);
	removeTrack(titleMusic);
	removeTrack(invaderDestroyed);
	removeTrack(playerDestroyed);
	mp.display.fillScreen(TFT_BLACK);
	mp.display.setCursor(32, -2);
	mp.display.setTextSize(1);
	mp.display.setTextFont(2);
	mp.display.setTextColor(TFT_RED);
	mp.display.printCenter("HIGHSCORES");
	mp.display.setCursor(3, 110);
	JsonArray &hiscores = mp.getJSONfromSAV(highscoresPath);
	for (int i = 1; i < 6;i++)
	{
		mp.display.setCursor(24, i * 20);
		if(i <= hiscores.size())
		{
			for(JsonObject& element:hiscores)
			{
				if(element["Rank"] == i)
					mp.display.printf("%d.   %.3s    %04d", i, element["Name"].as<char*>(), element["Score"].as<uint16_t>());
			}
		}
		else
			mp.display.printf("%d.    ---   ----", i);
	}
	mp.display.setCursor(2, 115);
	mp.display.print("Erase");
	while(1)
	{
		key = mp.buttons.kpdNum.getKey();
		if (mp.buttons.released(BTN_B) || mp.buttons.released(BTN_A))
		{
			while (!mp.update());          
			gamestatus = "title";
			break;
		}
		if (key == 'C')
		{
			eraseData();
			break;
		}
		mp.update();
	}
	mp.homePopupEnable(1);
	addTrack(titleMusic);
	titleMusic->play();
	titleMusic->setRepeat(1);
}
void showtitle() {
	cursor = 0;
	uint32_t blinkMillis = millis();
	bool blinkState = 0;
	while(!mp.buttons.released(BTN_A))
	{
		if(mp.update())
		{
			mp.display.fillScreen(TFT_BLACK);
			for(int i = 0; i < STAR_COUNT; i++)
			{
					// Remove the star from the screen by changing its pixel to the background color.
					mp.display.drawPixel(stars[i].x, stars[i].y, BACKGROUND_COLOR);

					// Update the position of the star.
					stars[i].update();

					// If the star's Y position is off the screen after the update.
					if(stars[i].y >= mp.display.height())
					{
							// Randomize its position.
							stars[i].randomize(0, mp.display.width(), 0, mp.display.height(), STAR_SPEED_MIN, STAR_SPEED_MAX);
							// Set its Y position back to the top.
							stars[i].y = 0;
					}

					// Draw the star at its new coordinate.
					mp.display.fillRect(stars[i].x, stars[i].y, 2, 2, STAR_COLOR);
			}
			if(millis() - blinkMillis >= 250)
			{
				blinkMillis = millis();
				blinkState = !blinkState;
			}
			mp.display.setTextColor(TFT_WHITE);
			mp.display.drawIcon(titleLogo, 7, 10, 73, 21, 2, TFT_WHITE);
			mp.display.setTextColor(TFT_RED);
			mp.display.setFreeFont(TT1);
			mp.display.setTextSize(2);
			mp.display.setCursor(10, 80);
			mp.display.printCenter("START");
			mp.display.setCursor(10, 100);
			mp.display.printCenter("HIGHSCORES");
			mp.display.setCursor(10, 120);
			mp.display.printCenter("QUIT");
			if(blinkState)
			{
				mp.display.drawRect(25, 67 + cursor * 20, 110, 16, TFT_RED);
				mp.display.drawRect(24, 66 + cursor * 20, 112, 18, TFT_RED);
			}
			if(mp.buttons.released(BTN_UP) && cursor > 0)
			{
				cursor--;
				while(!mp.update());
				blinkMillis = millis();
				blinkState = 1;
			}
			if(mp.buttons.released(BTN_DOWN) && cursor < 2)
			{
				cursor++;
				while(!mp.update());
				blinkMillis = millis();
				blinkState = 1;
			}		
		}
	}
	while(!mp.update());
	switch (cursor)
	{
		case 0:
			gamestatus = "newgame";
			break;
		case 1:
			dataDisplay();
			break;
		case 2:
			mp.loader();
			break;
	}
}

void enterInitials() {
	mp.homePopupEnable(0);
  name = "";
  String previous = "";
  uint32_t elapsedMillis = millis();
  uint32_t hiscoreMillis = millis();
  bool blinkState = 1;
  bool hiscoreBlink = 0;
  mp.textPointer = 0;
  while (!mp.buttons.released(BTN_A) || name.length() != 3) {
    if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine
		{
			elapsedMillis = millis();
			blinkState = !blinkState;
		}
    if(millis()-hiscoreMillis >= 1000)
    {
      hiscoreMillis = millis();
      hiscoreBlink = !hiscoreBlink;
    }
    previous = name;
   
    name = mp.textInput(name, 3);
    if(name.indexOf(' ') != -1)
      name = name.substring(0, name.length() - 1);
    if (previous != name)
    {
      blinkState = 1;
      elapsedMillis = millis();
    }
    
    mp.display.fillScreen(TFT_BLACK);
    mp.display.setCursor(16, 8);
    mp.display.setTextFont(2);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setTextSize(1);
    mp.display.printCenter("ENTER NAME");
    mp.display.setCursor(39, 80);
    
    if(hiscoreBlink && score > tempScore)
      mp.display.printCenter("NEW HIGH!");
    else
      mp.display.printf("SCORE: %04d", score);

    mp.display.setCursor(50, 40);
    mp.display.printCenter(name);
    if(blinkState)
      mp.display.drawFastVLine(mp.display.getCursorX(), mp.display.getCursorY()+3, 10, TFT_WHITE);
    mp.display.drawRect(30, 38, 100, 20, TFT_WHITE);
		mp.update();

  }
	mp.homePopupEnable(1);
  while(!mp.update());
}