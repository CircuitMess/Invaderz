/*
    Invaderz
    Copyright (C) 2019 CircuitMess

    Original game:
    Invaders by Yoda Zhang
    http://www.yodasvideoarcade.com/images/gamebuino-invaders.zip

    Ported to MAKERphone by CircuitMess

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
//----------------------------------------------------------------------------    
// define variables and constants
//----------------------------------------------------------------------------    
#include <MAKERphone.h>
#include "Star.h"
MAKERphone mp;
# define Maxwhidth 74
uint32_t pixelsTimer=0;
bool pixelsState=0;
String gamestatus;
int score;
int lives;
int gamelevel;
int shipx;
int invaderx[55];
int invadery[55];
int invaders[55];
int invaderframe[55];
int invaderanz;
int invaderctr;
int invadersound;
int checkdir;
int nextxdir;
int nextydir;
int invaderxr;
int invaderyr;
int invadershotx[4];
int invadershoty[4];
int invadershotframe;
int invadershots;
int invadershottimer;
int bunkers[4];
int shotx;
int shoty;
int checkl;
int checkr;
int checkt;
int checkb;
int yeahtimer;
int infoshow;
int deadcounter;
int saucerx;
int saucerdir;
int saucers;
int saucertimer;
int saucerwait;
int delayBip;
/*
//----------------------------------------------------------------------------
// setup
//----------------------------------------------------------------------------
*/

void setup() {
  Serial.begin(115200);
  mp.begin();
  mp.setResolution(1);
  mp.display.fillScreen(TFT_BLACK);
  mp.display.setFreeFont(TT1);
  starsSetup();
  gamestatus = "title";
}

//----------------------------------------------------------------------------    
// loop
//----------------------------------------------------------------------------    
void loop() {

	if (mp.update()) {

    if (gamestatus == "newgame") { newgame(); } // new game

		if (gamestatus == "newlevel") { newlevel(); } // new level

		if (gamestatus == "running") { // game running loop
			mp.display.fillScreen(TFT_BLACK);
      mp.update();
      if (mp.buttons.released(BTN_B))
      {
        gamestatus = "paused";
        while(!mp.update());
      }
			showscore();
			checkbuttons(); // check buttons and move playership
			drawplayership(); // draw player ship
			drawplayershot(); // move + draw player shoot
			invaderlogic(); // invader logic
			drawinvaders(); // draw invaders
			invadershot(); // invaders shoot
			nextlevelcheck(); // next level?
			drawbunkers(); // draw bunkers & check collission with player shot
			saucerappears(); // saucer appears?
			movesaucer(); // move saucer
			drawsaucer(); // draw saucer & remove if hit
			showscore(); // show lives, score, level
       // end of: gamestatus = running
		}
   
		if (gamestatus == "title") { 
      showtitle();
      if (mp.buttons.released(BTN_B))
        mp.loader();
    }

		if (gamestatus == "gameover") { // game over
			mp.display.fillScreen(TFT_BLACK);
      for (int i = 0; i < 40; i++) {
        if (invaders[i] != -1) {
          mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_WHITE);
        }
      }
		  mp.display.fillRect(6, 25, 68, 14, TFT_WHITE);
		  mp.display.setCursor(7, 37);
		  mp.display.setTextSize(2);
		  mp.display.setTextColor(TFT_BLACK);
		  mp.display.print("GAME OVER");
		  mp.display.setTextSize(1);
      if(mp.buttons.released(BTN_B) || mp.buttons.released(BTN_A))
      {
        while(!mp.update());
        gamestatus="title";
      }
        
 
		
		  
			
			//saveHighscore(score);
		}
	  if(gamestatus == "paused")
    {
      mp.display.fillScreen(TFT_BLACK);
      mp.display.setCursor(0, mp.display.height()/2 - 12);
      mp.display.setTextFont(2);
      mp.display.setTextSize(1);
      mp.display.printCenter("Paused");
      mp.display.setCursor(2, 55);
      mp.display.setFreeFont(TT1);
      mp.display.printCenter("A: resume    B: quit");
      while(!mp.update());
      while (!mp.buttons.released(BTN_A)) {
        if (mp.buttons.released(BTN_B))
        {
          gamestatus = "title";
          while (!mp.update());
          break;
        }
        while (!mp.update());
        gamestatus = "running";
      }
      
    }
	} // end of update
}
