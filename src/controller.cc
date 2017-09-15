/**
 * @file 
 * This file contains much of the fun: all user controls, physics and game checks are done here.
 */

#include <SFML/Graphics.hpp>
#include "datastruct.h"
#include "init.h"
#include "controller.h"
#include "debug.h"
#include <iostream>
#include <math.h>

using namespace sf;
using namespace std;

#define MAXSPEED 7	/**< Max ball speed for bounce and/or ::faster function */
#define BR 1		/**< Brick or Ball RADIUS. needed to check collisions more accurately */

/** 
  * slows down the ball achieving a nice slow-mo effect.
  * the ball should never stop. hence, some checks are required.
  */
void slower(ball_t &ball)
{
	extern gamestatus gs;
	if(gs==pause) return;

	if( abs(ball.dx) > 1 && abs(ball.dy) > 1 ) {
		ball.dx/=2;
		ball.dy/=2;
	}else if( ball.dx == 0 && abs(ball.dy) > 1 )
		ball.dy/=2;
	D2(cout<<ball.dx<<" "<<ball.dy<<"\n");		
}

/** 
  * twices the speed of the ball, limiting the maximum gained speed 
  * to the double of the setted value ( ::MAXSPEED -1)
  */
void faster(ball_t &ball)
{
	extern gamestatus gs;
	if(gs==pause) return;

	if( abs(ball.dx) < MAXSPEED && abs(ball.dy) < MAXSPEED ){
		ball.dx*=2;
		ball.dy*=2;
	}
	D2(cout<<"ball speed x: "<<ball.dx<<" y: "<<ball.dy<<"\n");
}

/**
  * spins the ball if spinning==true,
  */
void spinBall(ball_t &ball)
{
	extern gamestatus gs;
	if(gs==pause) return;

	if(ball.spinning){
		/** spin the ball */
		ball.y += ball.dy;
		ball.x += ball.dx;
	}
	ball.s.setPosition(ball.x, ball.y);
}

/**
  * @return true if the sprite passed by argument is being clicked. false otherwise
  * @param[in] spr The sprite to be checked 
  * @param[in] game the RenderWindow of the game
  * @param[in] e The generated event
  */
bool isSpriteClicked ( Sprite &spr , RenderWindow &game ,Event e)
{
		if (e.type == Event::MouseButtonPressed){
			if (e.mouseButton.button == sf::Mouse::Left){
				if( spr.getGlobalBounds().contains(e.mouseButton.x,e.mouseButton.y)){
					return true;
				}
			}
		}	
		return false;
}

/**
  * @return true if a sprite is right-clicked 
  */
bool isSpriteRightClicked ( Sprite &spr , RenderWindow &game ,Event e)
{
		if (e.type == Event::MouseButtonPressed){
			if (e.mouseButton.button == sf::Mouse::Right){
				if( spr.getGlobalBounds().contains(e.mouseButton.x,e.mouseButton.y)){
					return true;
				}
			}
		}	
		return false;
}

/**
  * @return true if mouse is moving over the sprite passed by argument, false otherwise
  */
bool isMouseOver(Sprite &spr , RenderWindow &game ,Event e)
{
	if (e.type == Event::MouseMoved)
		{
			if( spr.getGlobalBounds().contains(e.mouseMove.x,e.mouseMove.y) ){
				return true;
			}
		}
	return false;
}

/**
  *  @return true if mouse button is pressed and mouse is over the passed sprite, false otherwise
  */
bool isMouseDragging(Sprite &spr , RenderWindow &game ,Event e)
{
	if(Mouse::isButtonPressed(Mouse::Left) && isMouseOver(spr,game,e)){
		return true;
	}
	return false;
}

/**
  * checks if the brick went under 1 and, if so, moves it out of the game area,
  * making it unplayable. 
  * @return true if brick is being eliminated, false otherwise
  * @param[in] grid The matrix containing all the bricks 
  * @param[in] i,j The coordinates of the brick to be checked.
  */
bool checkBrickDeath(brick_t grid[rows][columns] ,int &i, int &j)
{
    if(grid[i][j].lives<1){
    	grid[i][j].s.setPosition(-100, 0);
    	grid[i][j].s.setColor(Color(0,0,0,0));
        return true;
    }
    return false;
}

/** 
  * blinks the paddle n times specified by param.
  * should be useful expectially for death
  */
void blink_sprite(Sprite &s, int & times)
{
	extern ball_t ball;
	extern paddle_t paddle;
	static Clock clk1;
	static bool lastPos=true;

	if (times > 0){
		Time elapsed1 = clk1.getElapsedTime();
		float t1 = elapsed1.asSeconds() * 32;
		float sint = sin(t1);

		if(sint < 0 && lastPos){
			times--;
		}

		if(sint > 0){
			s.setColor(Color(0,0,0,0));
			lastPos=true;
		}else{
			s.setColor(Color(255,255,255,255));
			lastPos=false;
		}
	}else{
		clk1.restart();
	}
}

/**
  * actions to accomplish when death occurs
  */
void check_paddle_death(paddle_t &paddle)
{
	extern bool gameOver;
	extern caption_t middleScreen;
	extern String pressC;
	extern background_t background;

	if(paddle.lives > 0){
		return;
	}else{
		if(!gameOver){
			background_init(background,2);
			gameOver=true;
			middleScreen.c.setString(pressC);
		}
		//cout<<"DEAD!\n";
	}
}

/** 
  * checks weather the ball is exiting the game area or not. 
  * if it is, bounce it to the opposite side. 
  * needs the ball coordinates and the screen resolution to be passed
  * via two struct parameters 
  *
  */
void checkBallEscaping(ball_t &ball, graphics_t &gfx, paddle_t & paddle)
{
	extern bool ps;
	static int blinks=0;
	//ball.s.getGlobalBounds().width
	/** set screen bounds */
	if (ball.left() <= 1){ 
		ball.dx = abs(ball.dx);
	}
	if(ball.right() >= gfx.width-1){
		ball.dx = -abs(ball.dx);
	}
	if (ball.y <= 0){
		ball.dy = abs(ball.dy);
	}

	/** set death */
	if(ball.y > gfx.height){
		ball.dy=-ball.dy;   
		blinks=5;
		paddle.lives--;
		D1(cout<<"lives: "<<paddle.lives<<"\n");
		ball_init(ball);
		paddle.s.setPosition(gfx.width/2 - paddle.s.getGlobalBounds().width/2 , 
			gfx.height - paddle.s.getGlobalBounds().height);
		playPauseButton(pause);
		ps=true;
		//playPauseButton(play,ball,paddle);
	}
	blink_sprite(paddle.s,blinks);
	check_paddle_death(paddle);

}

/**
  * set all needed flags and caption to win the game
  */
void winTheGame()
{	
	extern bool wonGame;
	extern String youWonTheGame;
	extern background_t background;
	if(wonGame) return;
	background.s.setTexture(background.t[3]);
	extern caption_t middleScreen;
	wonGame=true;
	middleScreen.c.setString(youWonTheGame);
	playPauseButton(pause);
}

/** 
  * checks if at least one brick is present on the grid, 
  * meaning the game has not finished
  * @return true if as least one brick is present
  */
bool isGridPresent(brick_t grid[rows][columns])
{
	extern gamestatus gs;
	if (gs!=play) return true;
    for (int r = 0; r < rows; r++) {   
        for (int c = 0; c < columns; c++) {
			if (grid[r][c].lives > 0){
				return true;
			}
		}
	}
	return false;	
}

/**
 * fix the ball bouncing when side-hitting a brick while 
 * falling in a perfect perpendicular angulation 
 */
 void bounceFix(ball_t &ball,brick_t grid)
 {
	if(ball.dx > -1 && ball.dx < 1){
		if(ball.x <= grid.s.getPosition().x)
			ball.dx = -2;
		else
			ball.dx = 2;
	}
 }
/** 
  * this function loops thru the matrix to check if the ball collides a brick in it.
  * if so, bounces the ball with the opposite delta x or delta y values. 
  * the lives of that brick are decremented and ::checkBrickDeath is called to decide 
  * weather or not to remove the brick from the field.
  * @return true if a collision between the ball and a brick has happened.
  */
bool isBallColliding(ball_t &ball, brick_t grid[rows][columns])
{
        bool collision=false;
    extern const int columns, rows;
        static float ballWidth=ball.s.getGlobalBounds().width;
        static float ballHeight=ball.s.getGlobalBounds().height;

/* scan the brick_t matrix looking for collisions */
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
				
            if(grid[r][c].lives == 0) continue;
           	collision=false;

            if (FloatRect(ball.x-BR, ball.yCenter()-1, ballWidth/2, 3).intersects(
                        	grid[r][c].s.getGlobalBounds()) ){              // (<)
                D2(cout<<"left HIT: dx:"<<ball.dx<<" --> ");
                ball.dx = abs(ball.dx);
                bounceFix(ball,grid[r][c]);
                D2(cout<<ball.dx<<"\n");
                collision=true;
            }
			if (FloatRect(ball.xCenter()+BR, ball.yCenter()-1, ballWidth/2, 3).intersects(
                            grid[r][c].s.getGlobalBounds()) ){              // (>)
                D2(cout<<"right HIT: dx:"<<ball.dx<<" --> ");
                ball.dx = -abs(ball.dx);
                bounceFix(ball,grid[r][c]);
                D2(cout<<ball.dx<<"\n");
                collision=true;
            }
			if (FloatRect(ball.xCenter()-BR, ball.y-1, 3 , ballHeight/2 ).intersects(
                        	grid[r][c].s.getGlobalBounds()) ) {             // (^)
                D2(cout<<"top HIT: dy:"<<ball.dy<<" --> ");
                ball.dy = abs(ball.dy);
                D2(cout<<ball.dy<<"\n");
                collision=true;
            }if (FloatRect(ball.xCenter()-BR, ball.yCenter()+1, 3 , ballHeight/2 ).intersects(
                            grid[r][c].s.getGlobalBounds()) ) {             // (_)
                D2(cout<<"bottom HIT: dy:"<<ball.dy<<" --> ");
                ball.dy = -abs(ball.dy);
                D2(cout<<ball.dy<<"\n");
                collision=true;
            }
            if(collision){
            	grid[r][c].lives--;
            	checkBrickDeath(grid,r,c);
            	return collision;
            }

        }
    }
        return collision;
}

/**
  * pause the game and freeze its elements.
  * ball and paddle objects are needed in order to set the speed,
  * the game status gs has to be accessible in order to make other operations aware.
  * param[in] newStatus The status which has been requested (Play|Pause)
  */
void playPauseButton(gamestatus newStatus){
	extern gamestatus gs;
	
	if(gs==newStatus)
		return;
	if(newStatus==pause)
		D1(cout<<"paused\n");
	if(newStatus==play)
		D1(cout<<"resumed\n");

	gs=newStatus;

}

/**
  * stops the game and let the user draw his own puzzle.
  * Hit P (to play what you just drawed) or S to save to file.
  */
void editorMode(grid_t &g, RenderWindow &game ,Event e)
{
	static brick_t brick;	//static visibility
	extern gamemode gm;
	//extern int columns,rows;
	if(gm!=editor){
		return;
	}

	for(int i=0;i<rows;i++){
		for(int j=0;j<columns;j++){
		    if(isSpriteClicked( g.grid[i][j].s , game, e)){	
				if(g.grid[i][j].lives < 5){
					g.grid[i][j].lives+=1;
			    	g.grid[i][j].s.setTexture(brick.t[g.grid[i][j].lives-1]);
		    		g.grid[i][j].s.setColor(Color(255,255,255,255));
				}else{
					g.grid[i][j].lives=0;
			    	g.grid[i][j].s.setTexture(brick.t[g.grid[i][j].lives]);
		    		g.grid[i][j].s.setColor(Color(0,0,0,0));
				}
		    }
			if(isSpriteRightClicked(g.grid[i][j].s , game, e)){
				g.grid[i][j].lives=0;
		    	g.grid[i][j].s.setTexture(brick.t[g.grid[i][j].lives]);
	    		g.grid[i][j].s.setColor(Color(0,0,0,0));		
			}

		    if(isMouseOver( g.grid[i][j].s , game, e) && g.grid[i][j].s.getColor() != Color(255,255,255,255) ){
		    	g.grid[i][j].s.setColor(Color(0,255,255,100));				
		    }else if(!isMouseOver( g.grid[i][j].s , game, e) && g.grid[i][j].s.getColor() != Color(255,255,255,255))
		    	g.grid[i][j].s.setColor(Color(0,0,0,0));

		    if(isMouseDragging(g.grid[i][j].s , game, e)){
				g.grid[i][j].s.setTexture(brick.t[0]);
		    	g.grid[i][j].s.setColor(Color(255,255,255,255));
				g.grid[i][j].lives=1;
		    }			
		}
	}

}

/**
  * keyboard control function to move the paddle.
  * needs the paddle and game area to be passed via appropriate struct elements
  */
void keyboardControl(paddle_t &paddle,graphics_t &gfx)
{
	extern gamestatus gs;
	if (gs==pause) return;
    /** keyboard controls */
    if (Keyboard::isKeyPressed(Keyboard::Right) && paddle.s.getPosition().x+90 < gfx.width)
    	paddle.s.move(paddle.speed, 0);
    if (Keyboard::isKeyPressed(Keyboard::Left) && paddle.s.getPosition().x > 0)
    	paddle.s.move(-paddle.speed, 0);
}


/**
  * checks if the ball is hitting the paddle and calculates the angulation to be assigned to the ball
  * based upon the relative hit coordinates on the paddle.
  * @return True if the paddle was hit. false otherwise.
  *
  */
bool isPaddleHitting(paddle_t &paddle, ball_t &ball)
{	
	float ballWidth=ball.s.getGlobalBounds().width;
	float ballHeight=ball.s.getGlobalBounds().height;

	if (FloatRect(ball.x, ball.y, ballWidth,ballHeight).intersects(paddle.s.getGlobalBounds())){	
		float midPaddle=paddle.s.getPosition().x+paddle.s.getGlobalBounds().width/2;
		int angularity=20;
		int angulation=abs((int)(((midPaddle-ball.x)/100)*angularity));	/* -*-|-- difference between collision point and paddle center */

		if (angulation > MAXSPEED)
			angulation=MAXSPEED;

		if(ball.x < midPaddle){		
			ball.dx = -angulation;	/* invert the angulation to bounce the ball to the left */
			ball.dy = -ball.dy;
		}else{ 				/* the ball must bounce to the right if the collision is on the right */
			ball.dx = abs(angulation);
			ball.dy = -ball.dy;
		}
		return true;
	}
		return false;		
}
