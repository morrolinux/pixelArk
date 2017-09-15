/**
 * @file 
 * Load and saving functions for the grid status
 */

#include <iostream>
#include <fstream>
#include "datastruct.h"
#include "debug.h"

using namespace std;
using namespace sf;

extern int xoffset,yoffset;
static brick_t brick;

/**
  * load the grid form binary file into a temporary grid_t object.
  * when finished, swap each element from the temp to the final grid_t object, one by one.
  * this is safer than substituting the whole grid_t object once, because of 
  * possible memory access issues when attempting to read/write on a not (yet) allocated object,
  * (which is done by other functions like ::isGridPresent
  */
void load(grid_t &g)
{
    extern const int rows,columns;
    grid_t tmp;


     //Reading from file
     ifstream input_file("grid.dat", ios::binary);
     input_file.read((char*)&tmp, sizeof(grid_t)); 
     input_file.close();        

    
	for (int r = 0; r < rows; r++)  
		for (int c = 0; c < columns; c++){
		    g.grid[r][c]=tmp.grid[r][c];
            g.grid[r][c].s.setTexture(brick.t[tmp.grid[r][c].lives-1]);
        }

	
     D1(cout<<"matrix loaded (binary file)\n");
}

/**
  * Save the grid status to a binary file.
  */
void save(grid_t &g)
{

    // Serializing 
    ofstream output_file("grid.dat", ios::binary);
    output_file.write((char*)&g, sizeof(grid_t));
    output_file.close();
    D1(cout<<"\nbinary file saved\n"<<endl);
    
}
