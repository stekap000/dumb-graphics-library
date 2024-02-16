#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_USE_AUTOSCALE
#define DGL_IMPLEMENTATION
#include "dgl.h"

// If you want to make your own function that does some kind of rendering
// you just need to operate on "window.canvas" or "window.back_canvas" directly.

void custom_clear(dgl_Canvas *canvas, uint32_t color, int offset) {
	for(int i = 0; i < canvas->height; ++i) {
		for(int j = 0; j < canvas->width; ++j) {
			DGL_SET_PIXEL(*canvas, j, i, DGL_RGB((i+offset) % 256, (j+offset) % 256, (i+j+offset) % 256));
		}
	}
}

void init() {
	init_window_params(500, 500);
}

void start() {}

unsigned int offset = 0;
void update(float dt) {
	offset += 1;
	custom_clear(&window.canvas, DGL_WHITE, offset);
}

void end() {}
