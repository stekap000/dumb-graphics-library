#define DGL_IMPLEMENTATION
#include "dgl.h"

#define WIDTH 500
#define HEIGHT 500

int main() {
	void *pixels = malloc(sizeof(dgl_Color) * WIDTH * HEIGHT);
	dgl_Canvas canvas = {
		.pixels = pixels,
		.width = WIDTH,
		.height = HEIGHT,
		.stride = WIDTH
	};
	dgl_fill_circle(&canvas, 200, 200, 100, DGL_RGBA(255, 0, 0, 100));
	dgl_fill_circle(&canvas, 300, 200, 100, DGL_RGBA(0, 255, 0, 100));
	dgl_fill_circle(&canvas, 250, 286, 100, DGL_RGBA(0, 0, 255, 100));
 
	dgl_draw_text(&canvas, "dgl", 217, 240, &dgl_default_font, 4, false, DGL_GREEN);
	dgl_render_ppm(&canvas, "test.ppm");
	
	return 0;
}
