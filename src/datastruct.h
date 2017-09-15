/**
 * @file 
 * Contains all the definitions for struct objects such as paddle, ball, brick etc..
 */

#include <SFML/Graphics.hpp>
#include <sstream>
#include <iostream>
#include "debug.h"
using namespace sf;
using namespace std;

/**
  * - Full grid: mostly used in classic mode to load a grid full of bricks to be destroyed.
  * - File: load a dummy grid object and load from file the actual grid (further explained in ::load function)
  * - blank: load a full grid of transparent bricks with 0 lives. useful for ::editorMode 
  * - actual: run through the brick matrix and place out of gaming area the bricks with 0 lives. 
  *   this function was made to ensure there was not whatsoever issue with misplaced 0-lives bricks after designing mode or loaded from file grids.
  */
enum gridtype {full,file,blank,actual}; 	
/**
  * Those flags are useful for letting other functions know what the user is playing at, and determine if they can or cannot be exectuted.
  * - Classic gamemode is inserted when playing in classic mode (I.E: when user launches the game)
  * - Custom: the user is playing on a grid which has been loaded from file.
  * - Editor: means we are still in the editor, designing a custom grid. therefore, there should be no action whatsoever from the ball or the paddle either.
  * - Run: the user is playing directly from an editor-designed grid. (no file has been loaded)
  */
enum gamemode {classic,custom,editor,run};

enum gamestatus {play,pause};

/** number of rows and columns to be drawn in the game */
static const int rows=8,columns=13;

/**
  * Contains widh, height and frames per second attributes for the game window
  */
struct graphics_t{
	int width;
	int height;
	int fps;
};

/**
  * A ball is made of all the elements and functions inside this class.
  */
struct ball_t{
	Sprite s;
	static const int nt=1;	/**< number of available textures */
	bool spinning;
	int type;		/**< There may be different types of balls with different features */
	Texture t[nt];
	int x,y;		/**< Ball x and y position */
	int dx,dy;		/**< Ball x and y acceleration */

	ball_t()
	{
		t[0].loadFromFile("../media/images/ball.png");
	}
	
	float left()
	{
		return this->x;
	}
	float right()
	{
		return (this->x + this->s.getGlobalBounds().width);
	}
	float up()
	{
		return this->y;
	}
	float down()
	{
		return (this->y + this->s.getGlobalBounds().height);
	}
	float xCenter()
	{
		return this->x+this->s.getGlobalBounds().width/2;
	}
	float yCenter()
	{
		return this->y+this->s.getGlobalBounds().height/2;

	}

};
/**
  * Describes the paddle object to be moved by the player 
  */
struct paddle_t{
	Sprite s;
	int type;
	int lives;
	Texture t;

	int speed;

	paddle_t()
	{
		t.loadFromFile("../media/images/paddle.png");
	}
};

/**
  * Background object made of multiple background images to be loaded
  */
struct background_t{
	Sprite s;
	static const int nt=4;
	int number;
	Texture t[nt];

	background_t()
	{
		t[0].loadFromFile("../media/images/bg-clean.jpg");
		t[1].loadFromFile("../media/images/bg-designer.jpg");
		t[2].loadFromFile("../media/images/bg-gameover.jpg");
		t[3].loadFromFile("../media/images/bg-win.jpg");
	}
};

/**
  * Same as a background, (random images like chineese cookie fortunes) 
  * but designed to be placed behind the blocks in the grid area.
  */
struct fortunes_t{
	Sprite s;
	static const int nt=6;
	Texture t[nt];
	fortunes_t()
	{
		t[0].loadFromFile("../media/images/fortune00.png");
		t[1].loadFromFile("../media/images/fortune01.png");
		t[2].loadFromFile("../media/images/fortune02.png");
		t[3].loadFromFile("../media/images/fortune03.png");		
		t[4].loadFromFile("../media/images/fortune04.png");		
		t[5].loadFromFile("../media/images/fortune05.png");		
	}
};

/**
  * Describes a single Brick.
  */
struct brick_t{
	Sprite s;
	static const int nt=5;
	int type;
	int lives;
	Texture t[nt];

	brick_t()
	{
		t[0].loadFromFile("../media/images/block01.png");
		t[1].loadFromFile("../media/images/block02.png");
		t[2].loadFromFile("../media/images/block03.png");
		t[3].loadFromFile("../media/images/block04.png");
		t[4].loadFromFile("../media/images/block05.png");
	}
};

/**
  * Describes a grid made of bricks 
  */
struct grid_t{
		brick_t grid [rows][columns]; /**< static matrix of bricks composing the game grid */
};

/**
  * A caption is a text area to be placed within the game drawable space
  */
struct caption_t{
	Font font;
	Text c;
	ostringstream ss;		//useful for appending text to a caption

	caption_t()
	{
		if (!font.loadFromFile("../media/fonts/bp.otf"))
		    D1(cout << "Error loading font\n");
	}
};

/**
  * Describes a single heart (useful for drawing a lifebar)
  */
struct heart_t{
	Sprite s;
	Texture t;

	heart_t()
	{
		t.loadFromFile("../media/images/heart.png");
		this->s.setTexture(this->t);
	}
	
};
