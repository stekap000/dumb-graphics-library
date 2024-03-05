#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

#define WIDTH 600
#define HEIGHT 600

#define NUM_POINTS 4
dgl_V3 points[NUM_POINTS] = {
	{
		.x = 100,
		.y = 100,
		.z = 0
	},
	{
		.x = 500,
		.y = 200,
		.z = 0
	},
	{
		.x = 300,
		.y = 500,
		.z = 0
	},
	{
		.x = 250,
		.y = 350,
		.z = 0
	}
};

uint32_t colors[NUM_POINTS] = { DGL_RED, DGL_GREEN, DGL_BLUE, DGL_YELLOW};

void draw_points() {
	for(int i = 0; i < NUM_POINTS; ++i) {
		dgl_fill_circle(&window.canvas, points[i].x, points[i].y, 5, DGL_BLACK);
	}
}

void draw_voronoi() {
	float d = 0;
	int pi = 0;
	float min_d = WIDTH*HEIGHT;
	dgl_V3 p = {0};
	for(int y = 0; y < HEIGHT; ++y) {
		for(int x = 0; x < WIDTH; ++x) {
			min_d = WIDTH*HEIGHT;
			p.x = x;
			p.y = y;
			for(int i = 0; i < NUM_POINTS; ++i) {
				d = dgl_v3_len(dgl_v3_sub(p, points[i]));
				if(d < min_d) {
					min_d = d;
					pi = i;
				}
			}

			dgl_fill_pixel(&window.canvas, x, y, colors[pi]);
		}
	}
}

void init() {
	init_window_params(WIDTH, HEIGHT);
	init_fps(-1);
}

void start() {}

void update(float dt) {
	points[NUM_POINTS - 1].x = cursor_pos.x;
	points[NUM_POINTS - 1].y = cursor_pos.y;
	draw_voronoi();
	draw_points();
}

void end() {}
