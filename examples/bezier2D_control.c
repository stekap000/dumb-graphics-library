#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

int width = 400;
int height = 400;

float t = 0.0f;
float speed = 0.2;

dgl_V3 p0 = {
	.x = 50,
	.y = 150,
	.z = 0
};
dgl_V3 p1 = {0};
dgl_V3 p2 = {
	.x = 350,
	.y = 200,
	.z = 0
};
dgl_V3 p = {0};

void init() {
	init_window_params(width, height);
	init_fps(-1);
}

void start() {}

void update(float dt) {
	dgl_clear(&window.canvas, DGL_BLACK);

	p1.x = cursor_pos.x;
	p1.y = cursor_pos.y;

	// Draw bezier curve for current cursor position
	for(; t <= 1.0; t += 0.02) {
		dgl_V3 v1 = dgl_v3_add(dgl_v3_scale(p0, 1-t), dgl_v3_scale(p1, t));
		dgl_V3 v2 = dgl_v3_add(dgl_v3_scale(p1, 1-t), dgl_v3_scale(p2, t));
		p = dgl_v3_add(dgl_v3_scale(v1, 1-t), dgl_v3_scale(v2, t));
		
		dgl_fill_circle(&window.canvas, p.x, p.y, 2, DGL_GREEN);
	}

	// Draw three main points
	dgl_fill_circle(&window.canvas, p0.x, p0.y, 5, DGL_RED);
	dgl_fill_circle(&window.canvas, p1.x, p1.y, 5, DGL_GREEN);
	dgl_fill_circle(&window.canvas, p2.x, p2.y, 5, DGL_BLUE);
	
	t = 0;
}

void end() {}
