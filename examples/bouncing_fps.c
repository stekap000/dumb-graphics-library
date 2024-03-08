#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

void init() {
	init_window_params(300, 400);
	init_fps(-1);
}

void start() {

}

float x = 0;
float y = 0;
float x_offset = 1;
float y_offset = 1;
char buff[30] = {0};
void update(float dt) {
	x += x_offset;
	y += y_offset;
	
	int var_fps = (int)(1.0/dt);
	dgl_draw_text(&window.canvas, strcat(itoa(var_fps, buff, 10), " fps"), (int)x, (int)y, &dgl_default_font, 2, false, DGL_GREEN);

	if(x < 0 || x >= window.canvas.width) x_offset = -x_offset;
	if(y < 0 || y >= window.canvas.height) y_offset = -y_offset;
}

void end() {

}
