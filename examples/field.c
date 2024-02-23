#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_AALIAS_DEG 4
#define DGL_IMPLEMENTATION
#include "dgl.h"

int width = 500;
int height = 500;

float constant_scaling = 1;

dgl_V3 p1 = {
	.x = 200,
	.y = 200,
	.z = 0
};
dgl_V3 p2 = {0};

// Draw some weird field when given two same sources.
void draw_field(dgl_Canvas *canvas, dgl_V3 p1, dgl_V3 p2) {
	for(int i = 0; i < canvas->height; ++i) {
		for(int j = 0; j < canvas->width; ++j) {
			dgl_V3 temp = { .x = j, .y = i, .z = 0 };
			
			float dist_squared_1 = dgl_v3_lensq(dgl_v3_sub(temp, p1));
			float dist_squared_2 = dgl_v3_lensq(dgl_v3_sub(temp, p2));

			float field_scaling = constant_scaling * width * width / 8;
			float scaling_1 = DGL_LERP_INVERSE(0, field_scaling, dist_squared_1);
			float scaling_2 = DGL_LERP_INVERSE(0, field_scaling, dist_squared_2);

			float red_1 = 255 * (1 - scaling_1);
			float red_2 = 255 * (1 - scaling_2);

			double red = red_1 + red_2;
			if(red > 255) red = 255;

			// When writing custom function and working with canvas coordinates
			// we need to transform them to currently set coordinate system before
			// writing them to canvas.
			DGL_SET_PIXEL(*canvas,
						  DGL_TRANSFORM_COORDINATES_X(j),
						  DGL_TRANSFORM_COORDINATES_Y(i, canvas->height),
						  DGL_RGB((int)red, 0, 0));
		}
	}
}

void init(){
	init_window_params(width, height);
	init_fps(-1);
}

void start(){}

void update(float dt) {
	p2.x = cursor_pos.x;
	p2.y = cursor_pos.y;
	
	draw_field(&window.back_canvas, p1, p2);
	
	dgl_fill_circle(&window.canvas, p1.x, p1.y, 5, DGL_BLUE);
	dgl_fill_circle(&window.canvas, p2.x, p2.y, 5, DGL_GREEN);
}

void end() {}
