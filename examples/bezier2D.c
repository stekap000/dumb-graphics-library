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
	.y = 50,
	.z = 0
};
dgl_V3 p1 = {
	.x = 200,
	.y = 350,
	.z = 0
};
dgl_V3 p2 = {
	.x = 350,
	.y = 100,
	.z = 0
};
dgl_V3 p = {0};

void init() {
	init_window_params(width, height);
	init_fps(-1);
}

void start() {}

void update(float dt) {
	// Calculate bezier point for current value of t
	dgl_V3 v1 = dgl_v3_add(dgl_v3_scale(p0, 1-t), dgl_v3_scale(p1, t));
	dgl_V3 v2 = dgl_v3_add(dgl_v3_scale(p1, 1-t), dgl_v3_scale(p2, t));
	p = dgl_v3_add(dgl_v3_scale(v1, 1-t), dgl_v3_scale(v2, t));

	// Draw moving point and its trail
	dgl_fill_circle(&window.back_canvas, p.x, p.y, 2, DGL_RGBA(0, 120, 0, 80));
	dgl_fill_circle(&window.canvas, p.x, p.y, 3, DGL_GREEN);

	// Draw three endpoints
	dgl_fill_circle(&window.canvas, p0.x, p0.y, 5, DGL_RED);
	dgl_fill_circle(&window.canvas, p1.x, p1.y, 5, DGL_GREEN);
	dgl_fill_circle(&window.canvas, p2.x, p2.y, 5, DGL_BLUE);

	// If moving point gets to the end, reverse the direction and move center point
	if(t < 0 || t > 1) {
		p1.x = ((int)p1.x + 80) % width;
		speed = -speed;
	}
	
	t += speed * dt;
}

void end() {}
