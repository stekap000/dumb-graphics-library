#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_USE_AUTOSCALE
#define DGL_IMPLEMENTATION
#include "dgl.h"

#include <time.h>

#define width 600
#define height 600
#define cell_width 5
#define cell_height 5
#define grid_height height/cell_height
#define grid_width width/cell_width

int grid[grid_height][grid_width] = {};

#define white_saturation 0.6

#define loop(iterator, n) for(int iterator = 0; iterator < n; ++iterator)

void random_grid(int grid[grid_height][grid_width])
{
	loop(y, grid_height)
	{
		loop(x, grid_width)
		{
			grid[y][x] = (rand() > (RAND_MAX*white_saturation)) ? 1 : 0;
		}
	}
}

void draw_grid(int grid[grid_height][grid_width])
{
	loop(y, grid_height)
	{
		loop(x, grid_width)
		{
			uint32_t color = (grid[y][x] == 0) ? DGL_BLACK : DGL_RGB(0, 150, 0);
			dgl_fill_rect(&window.canvas, x*cell_width, (y+1)*cell_height, cell_width, cell_height, color);
		}
	}
}

int next_state(int grid[grid_height][grid_width], int x, int y)
{
	int alive = grid[y][x];
	int alive_neighbours = 0;

	for(int i = -1; i <= 1; ++i)
	{
		for(int j = -1; j <= 1; ++j)
		{
			if(i == 0 && j == 0) continue;
				
			int Y = y + i;
			int X = x + j;

			if(Y >= 0 && Y < grid_height && X >= 0 && X < grid_width)
				alive_neighbours += grid[Y][X];
		}
	}

	if(alive_neighbours == 3) return 1;
	if(alive_neighbours == 2 && alive) return 1;
	
	return 0;
}

void update_grid(int grid[grid_height][grid_width])
{
	int temp[grid_height][grid_width] = {};

	loop(y, grid_height)
	{
		loop(x, grid_width)
		{
			temp[y][x] = next_state(grid, x, y);
		}
	}

	memcpy(grid, temp, grid_width*grid_height*4);
}

void init()
{
	init_window_params(width, height);
	init_fps(120);
	
	srand(time(0));
	
   	random_grid(grid);
}

void start(){}

void update(float dt)
{
	draw_grid(grid);
	update_grid(grid);
}

void end(){}
