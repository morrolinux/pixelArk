/**
 * @file 
 * This file contains all the Initialization functions needed for the game to start.
 */

#include <SFML/Graphics.hpp>
#include <time.h>
#include <fstream>
#include <iostream>
#include "datastruct.h"
#include "fileIO.h"

using namespace sf;
using namespace std;

extern const int rows,columns;	

/** x and y size of the brick. This information is given by the loaded sprite proprieties.*/
int xSize;
int ySize;

/** 
  * position offset of the first brick to be placed on the grid.
  * Useful for defining the spacing of the brick grid from the game boundaries.
  */
int xoffset=40;
int yoffset=30;

/**
 * Background initialization function 
 * @param[in] bgNumber The background to be loaded
 */
void background_init(background_t &background, int bgNumber)
{
	assert(&background != 0);
	D4(cout<<"initializing background "<<bgNumber<<" \n");
	background.number=bgNumber;
	background.s.setTexture(background.t[bgNumber]);
}

/**
 * Sets all the variables regarding game window and frames per second
 */
void gfx_init(graphics_t &gfx)
{
	assert(&gfx != 0);
	D4(cout<<"initializing gfx...\n");
	gfx.width=640;
	gfx.height=480;
	gfx.fps=60;	
}

/**
 * set paddle params
 */
void paddle_init(paddle_t &paddle,graphics_t gfx)
{
	assert(&paddle != 0);
	D4(cout<<"initializing paddle...\n");
	paddle.type=0;
	paddle.speed=9;
	paddle.lives=4;
	paddle.s.setTexture(paddle.t);

	/** start position */
	paddle.s.setPosition(gfx.width/2 - paddle.s.getGlobalBounds().width/2 , gfx.height - paddle.s.getGlobalBounds().height);
}

/**
 * set ball params
 */
void ball_init(ball_t &ball)
{
	assert(&ball != 0);
	D4(cout<<"initializing ball...\n");
	srand(time(0));
	ball.type=0;
	ball.s.setTexture(ball.t[ball.type]);

	/** ball starting speed and position */
	ball.x=300; //595; //30; //291; //300;
	ball.y=220;

	ball.dx=(rand()%11)-5; //6;
	ball.dy=6;
}

/**
 * init captions 
 */
void caption_init(caption_t &caption,int xposition, int yposition, int size)
{
	assert(&caption != 0);
	D4(cout<<"initializing caption...\n");
	caption.c.setFont(caption.font);
	caption.c.setCharacterSize(size);
	caption.c.setStyle(Text::Bold);
	caption.c.setColor(Color::White);
	caption.c.setPosition(xposition,yposition);
}

/**
 * init fortunes and get a random one.
 */
void fortunes_init(fortunes_t &fortunes, graphics_t gfx)
{
	assert(&fortunes != 0);
	fortunes_t tmp;
	D4(cout<<"initializing fortunes...\n");
	srand(time(0));
	int r=(rand())%fortunes.nt;
	D4(cout<<"loading fortune "<<r<<"\n");
	tmp.s.setTexture(fortunes.t[r]);
	tmp.s.setColor(Color(255,255,255,255));
	tmp.s.setPosition(gfx.width/2 - (tmp.s.getGlobalBounds().width/2) ,gfx.height/16+3);

	fortunes = tmp;
}

/**
  * initialize a grid by specific type passed by argument. 
  * full = standard full grid
  * file = load grid scheme from binary file
  * blank = load a full grid of transparent bricks to be drawed
  * actual = loops trough the grid to remove transparent bricks, which are not to be used.
  */
void init_grid(grid_t &g, gridtype type)
{
	assert(&g != 0);
	D4(cout<<"initializing sample brick...\n");
	static brick_t brick;
	brick.s.setTexture(brick.t[0]);
	xSize=brick.s.getGlobalBounds().width+1;
  	ySize=brick.s.getGlobalBounds().height;
  	D4(cout<<xSize<<" "<<ySize<<endl);
	
	D4(cout<<"initializing grid...\n");

	/** creates a standard full grid of bricks */
	if(type==full){
		D4(cout<<"grid type = FULL\n");
		for (int r = 0; r < rows; r++) {   
  			for (int c = 0; c < columns; c++) {
				  //int l=rand() % 5;
    			  assert(&g.grid[r][c] != 0);
				  int l=1; //r%5;
				  g.grid[r][c].s.setTexture(brick.t[l-1]);
				  D4(cout<<"Texture: Ok "<<r<<" "<<c<<"\n");
				  g.grid[r][c].s.setColor(Color(255,255,255,255));
  				  D4(cout<<"Color: Ok "<<r<<" "<<c<<"\n");
				  g.grid[r][c].s.setPosition(c * xSize+xoffset, r * ySize+yoffset);
				  D4(cout<<"Position: Ok "<<r<<" "<<c<<"\n");
				  g.grid[r][c].lives=l;
			}
		}		
	/** loads from file, and make sure blank bricks are not in the playable area (type=actual) */
	}else if(type==file){
		D4(cout<<"grid type = Load from file\n");
		load(g);
 	    assert(&g != 0);
		type=actual;
	/** creates an "empty" grid of blank bricks which the user can paint on his own. */ 
	}else if(type==blank){
		D4(cout<<"grid type = BLANK\n");
		for (int r = 0; r < rows; r++) {   
  			for (int c = 0; c < columns; c++) {
    			  assert(&g.grid[r][c] != 0);
				  g.grid[r][c].s.setTexture(brick.t[0]);
				  g.grid[r][c].s.setColor(Color(0, 0, 0, 0));
				  g.grid[r][c].s.setPosition(c * xSize+xoffset, r * ySize+yoffset);
				  g.grid[r][c].lives=0;
			}
		}	
	}
	/** loops through the matrix moving apart the blank bricks, letting the grid be playable. */
	if(type==actual){
		D4(cout<<"grid type = ACTUAL\n");
		for (int r = 0; r < rows; r++){  
  			for (int c = 0; c < columns; c++){
   			  	assert(&g.grid[r][c] != 0);
			  	if (g.grid[r][c].lives==0){
					g.grid[r][c].s.setPosition(-100,0);
			  	}
			}
		}
	}
	D4(cout<<"matrix created \n");
}	

/**
  * sets everything is needed to enter a specific game mode passed by argument
  * then initialize the grid accordingly
  */
void init_mode(grid_t &g, gamemode gm,ball_t &ball)
{
	extern background_t background;
	extern bool fortunesEnabled;
	extern gamestatus gs;
	if(gs==pause) return;
	if(gm==classic){
		init_grid(g,full);
		ball.spinning=true;
	}
	if(gm==editor){
		init_grid(g,blank);
		background_init(background,1);
		ball.spinning=false;
		fortunesEnabled=false;
	}
	if(gm==custom){
		init_grid(g,file);
		fortunesEnabled=false;
	}
	if(gm==run){
		init_grid(g,actual);
		ball_init(ball);
		ball.spinning=true;
	}
	if(gm!=editor){
		background_init(background,0);
	}

}

