#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_COORDINATE_CENTER_X 300
#define DGL_COORDINATE_CENTER_Y 0
#define DGL_IMPLEMENTATION
#include "dgl.h"

#include <time.h>

#define WIDTH 800
#define HEIGHT 800

void init() {
	init_window_params(WIDTH, HEIGHT);
	init_fps(-1);

	srand(time(0));
}

void start() {
	dgl_clear(&window.back_canvas, 0xff181818);
}

float x = 0;
float y = 0;
float scale_x = 100;
float scale_y = 70;
dgl_Color color = DGL_RGBA(0, 255, 0, 150);

void iterate(float *x, float *y) {
	float v = (float)rand() / (float)RAND_MAX;
	if(v < 0.01) 		{ *x = 0;                          *y = (0.16f*(*y));                      }
	else if(v < 0.86)	{ *x = (0.85f*(*x) + 0.04f*(*y));  *y = (-0.04f*(*x) + 0.85f*(*y) + 1.6f); }
	else if(v < 0.93)	{ *x = (0.2f*(*x) - 0.26f*(*y));   *y = (0.23f*(*x) + 0.22f*(*y) + 1.6f);  }
	else				{ *x = (-0.15f*(*x) + 0.28f*(*y)); *y = (0.26f*(*x) + 0.24f*(*y) + 0.44f); }
}

void update(float dt) {
	dgl_fill_pixel(&window.back_canvas, (int)(x*scale_x), (int)(y*scale_y), color);
	iterate(&x, &y);
}

void end() {}
