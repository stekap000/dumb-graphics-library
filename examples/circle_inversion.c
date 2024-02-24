#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

// Every inner point that corresponds to some point outside window will be black in inversion.
// Artifacts in inversion are a result of float-int casts and the fact that we are simply
// setting points which are integers, instead of drawing shapes or also coloring neighbours.
// If you move shape, your eyes will interpolate artifacts for you.

int width = 600;
int height = 600;
int cell_dimension = 50;

int cx = 0;
int cy = 0;
int circle_radius = 120;

void draw_grid() {
	// Draw horizontal lines
	for(int i = cell_dimension; i < height; i += cell_dimension)
		dgl_draw_line_bresenham(&window.back_canvas, 0, i, width, i, DGL_WHITE);

	// Draw vertical lines
	for(int i = cell_dimension; i < height; i += cell_dimension)
		dgl_draw_line_bresenham(&window.back_canvas, i, 0, i, height, DGL_WHITE);
}

void draw_inversion(int center_x, int center_y, int r) {
	dgl_V3 center = { .x = center_x, .y = center_y, .z = 0 };

	// Top left of circle bounding box
	int top_left_y = center_y - r;
	int top_left_x = center_x - r;

	// Loop over bounding box of this circle
	for(int y = 0; y < 2*r; ++y) {
		if(!(top_left_y + (int)y < 0) && !(top_left_y + (int)y >= (int)window.canvas.height)){
			for(int x = 0; x < 2*r; ++x) {
				if(!(top_left_x + (int)x < 0) && !(top_left_x + (int)x >= (int)window.canvas.width)){
					int pxl = top_left_x + x - center_x;
					int pyl = top_left_y + y - center_y;
					unsigned int left_side = pxl*pxl + pyl*pyl;

					// If we are in the circle
					if(left_side < (r-1)*(r-1)){
						// Create vector that points from center to current (inner) point
						dgl_V3 r1 = { .x = top_left_x + x, .y = top_left_y + y, .z = 0 };
						r1 = dgl_v3_sub(r1, center);
						r1.z = 0;

						// Find distance of corresponding outer point from the center
						float r2 = (r*r) / dgl_v3_len(r1);

						// Create vector that goes from center to outer point
						// After that, add it to center so that we get absolute canvas coords
						dgl_V3 temp = dgl_v3_scale(dgl_v3_unit(r1), r2);
						dgl_v3_add_mut(&temp, center);

						// If our vector lies inside canvas
						if(temp.y >= 0 && temp.y < (int)window.canvas.height) {
							if(temp.x >=0 && temp.x < (int)window.canvas.width) {
								// Set current (inner) point color to corresponding outer point
								DGL_SET_PIXEL(window.canvas,
											  DGL_TRANSFORM_COORDINATES_X(top_left_x + x),
											  DGL_TRANSFORM_COORDINATES_Y(top_left_y + y, window.canvas.height),
											  DGL_GET_PIXEL(window.canvas, (int)temp.x, (int)temp.y));
							}
						}
					}
				}
			}
		}
	}
}

void init() {
	init_window_params(width, height);
	init_fps(-1);
}

void start() {
	draw_grid();
}

void update(float dt) {
	cx = cursor_pos.x;
	cy = cursor_pos.y;

	dgl_draw_circle(&window.canvas, cx, cy, circle_radius, DGL_WHITE);
	dgl_fill_circle(&window.canvas, cx, cy, circle_radius - 2, DGL_BLACK);
	draw_inversion(cx, cy, circle_radius);
}

void end() {}
