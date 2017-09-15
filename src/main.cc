/**
 * @mainpage PixelArk
 *
 * Yet another Arkanoid game, with few variations. 
 * PixelArk has a "classic" and an "editor" mode:
 * 
 * "Classic" mode draws a full grid of bricks to be destoryed,
 * and your prize is a random background image made of pixels 
 * which you can see behind the bricks...
 * 
 * "editor" mode lets you draw your own grid by choosing color 
 * and position of each single brick, using the in-game basic editor provided.
 * custom brick grids can be saved and played anytime,
 * just like you can do in classic mode by saving the grid status.
 *
 * @author Moreno Razzoli
 */

/**
 * @file 
 * Main file which declares all ariables and listeners
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>			//text drawing library
#include <cassert>
#include "datastruct.h"
#include "init.h"
#include "fileIO.h"
#include "controller.h"
#include "debug.h"

using namespace sf;
using namespace std;

extern const int rows,columns; 

gamemode gm;
gamestatus gs;
background_t background; 
caption_t middleScreen;

/**
 * Game flags for win, game over, ecc 
 */
bool gameOver=false;
bool wonGame=false;			
bool ps=false;					/** needed to make the user press space to continue */
bool fortunesEnabled=true;		/** enable or disable fortunes in classic player mode */

/** game strings */
String pausedHints="\t\t\t\tPixelArk by Morrolinux\n\n"
					"Q = Quit\n"
					"Space = pause/resume the game\n"
					"\n\t\t\t\t::Load - Save::\n\n"
					"S = Save grid status (also works in Editor mode)\n"
					"L = Load last saved grid\n"
					"\n\t\t\t\t::Editor mode::\n\n"
					"E = Enter editor mode\n";
					
String editorHints="\t\t\t\t::Editor Mode::\n\n"
					"Drag the mouse to Draw your own grids\n"
					"OR\n"
					"Click a brick to change its type\n\n"
					"P = play on this grid\n" 
					"S = save to file\n\n"
					"C = switch back to Classic mode";

String pressSpace="\t\t\t< Press space to continue >";
String pressC="\t\t< Game OVER - Press C to restart >";
String youWonTheGame="congrants! you won!\nHit C to restart or Q to quit";

int main()
{

	Clock clk;

	grid_t g;
	heart_t heart;						//needed to draw the lifebar

	graphics_t gfx;
	gfx_init(gfx);
	RenderWindow game(VideoMode(gfx.width, gfx.height), "PixelArk");
	game.setFramerateLimit(gfx.fps);

	paddle_t paddle;
	paddle_init(paddle,gfx);

	ball_t ball;
	ball_init(ball);

	fortunes_t fortunes;
	fortunes_init(fortunes,gfx);

	caption_t gsCaption;				//play-pause caption
	caption_init(gsCaption,10,3,20);

	caption_t hintsCaption;
	caption_init(hintsCaption,gfx.width/16,gfx.height/2-30,13);
	hintsCaption.c.setString(pausedHints);

	caption_t editorCaption;
	caption_init(editorCaption,gfx.width/16,gfx.height/2-30,15);
	editorCaption.c.setString(editorHints);

	caption_init(middleScreen,gfx.width/16,gfx.height/2+80,15);
	middleScreen.c.setString(pressSpace);		//general purpose middle-screen caption

	/** game mode and status initialization */
	gm=classic;
	init_mode(g,gm,ball);
	playPauseButton(pause);

	/** Main Loop */
	while (game.isOpen()) {
		Event e;
		
		/** Event loop for keypress(es) */
		while (game.pollEvent(e)) {
			if (e.type == Event::Closed)										// Window closed
				game.close();
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::Q)		// Quit action
				game.close();
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::S){		// Save button
				save(g);
				gsCaption.c.setString("Grid saved");
				clk.restart();
				D1(cout<<"\nFile Salvato\n");
				continue;
			}
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::L){		// Load button
				gm=custom;
				ball_init(ball);
				paddle_init(paddle,gfx);
				playPauseButton(play);						//due to function hinibition during pause state, 
				init_mode(g,gm,ball);						//we need to un-pause the game before doing anything
				playPauseButton(pause);						//and pause back again, even if it wasn't paused, 
				gsCaption.c.setString("Grid loaded");		//to give the user the time to set-up
				middleScreen.c.setString("");
				hintsCaption.c.setString(pausedHints);
				continue;
			}												
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::E){		// let the game switch to editor mode
				if(gameOver || wonGame) continue;
				gm=editor;			
				hintsCaption.c.setString("");
				editorCaption.c.setString(editorHints);	
				playPauseButton(play);
				init_mode(g,gm,ball);
			}
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::P){		// Run on-the-fly the just created grid.
				gm=run;															// This is needed in editor mode to start playing
				init_mode(g,gm,ball);											// without having to save and load first.
				continue;
			}
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::Up){		// Fast forward effect
				if(gameOver || wonGame) continue;
				faster(ball);
				gsCaption.c.setString("Speed Up");
				clk.restart();
				continue;
			}
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::Down){	// Slow-mo effect
				if(gameOver || wonGame) continue;
				slower(ball);
				gsCaption.c.setString("Slow down");
				clk.restart();
				continue;
			}
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::Space){	// Pause the game
				if(gm==editor)
					continue;
				if(gs==play){
					hintsCaption.c.setString(pausedHints);
					playPauseButton(pause);
					gsCaption.c.setString("paused");
				}else if(gs==pause){
					if(gameOver || wonGame) continue;
					hintsCaption.c.setString("");
					playPauseButton(play);
					gsCaption.c.setString("resumed");
					clk.restart();
				}
				continue;
			}
			if (e.type == Event::KeyPressed && e.key.code == Keyboard::C){		//reload game, start over
				gm=classic;
				gs=play;
				gameOver=false;
				wonGame=false;
				fortunesEnabled=true;
				middleScreen.c.setString("");
				hintsCaption.c.setString(pausedHints);
				init_mode(g,gm,ball);
				ball_init(ball);
				paddle_init(paddle,gfx);
				fortunes_init(fortunes,gfx);
				playPauseButton(pause);
				continue;
			}
			editorMode(g,game,e);
		}

		/** make the gsCaption disappear after N seconds */
		Time elapsed = clk.getElapsedTime();
		float t2 = elapsed.asSeconds();
		if(t2 > 1 && gs==play){
			gsCaption.c.setString("");
		}

		spinBall(ball);
		isBallColliding(ball,g.grid);	
		checkBallEscaping(ball,gfx,paddle);
		keyboardControl(paddle,gfx);
		isPaddleHitting(paddle,ball);

		/** check weather the user won the game or not */
		if( gm!=editor && isGridPresent(g.grid)==false ){
			ball.spinning=false;
			winTheGame();
		}


		game.clear();
		game.draw(background.s);

		if(fortunesEnabled)
			game.draw(fortunes.s);
		
		/** don't draw the ball when the game is paused or the player won */
		if(gs!=pause && !wonGame && gm!=editor)
			game.draw(ball.s);
		
		if( wonGame )
		{
			game.draw(middleScreen.c);
		}

		if( ps && gs==pause ){
			game.draw(middleScreen.c);
		}else{
			ps=false;
		}
	
		if(gm!=editor)
			game.draw(paddle.s);
	
		game.draw(gsCaption.c);

		if(!wonGame && !gameOver)
		game.draw(hintsCaption.c);

		if(gm==editor)
			game.draw(editorCaption.c);

		/** 
		  * draw a simple heart-composed life bar proportionally 
		  * to the number of lives left for the player. 
		  */
		if(gm!=editor){
			for(int i=0;i<paddle.lives;i++){
				heart.s.setPosition(500+i*heart.s.getGlobalBounds().width,3);
				game.draw(heart.s);
				//game.draw(lifebar.bar[i].s);
			}
		}

		/** draw the bricks grid */
		for (int i = 0; i < rows; i++) {   
  			for (int j = 0; j < columns; j++) {
				game.draw(g.grid[i][j].s);
			}
		}

		game.display();
	}

	return 0;
}
