#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_COORDINATE_CENTER_X 200
#define DGL_COORDINATE_CENTER_Y 200
#define DGL_IMPLEMENTATION
#include "dgl.h"

int width = 400;
int height = 400;

dgl_Point2D v3_to_point2d(dgl_V3 v) {
	return (dgl_Point2D){ .x = v.x, .y = v.y };
}

typedef struct {
	int n;
	dgl_V3 *vertices;
} convex_shape;

convex_shape shape1;
convex_shape shape2;
convex_shape minkowski_sum;

void draw_convex_shape(convex_shape shape, dgl_Color color) {
	for(int i = 0; i < shape.n; ++i) {
		for(int j = 0; j < shape.n; ++j) {
			dgl_draw_line_bresenham(&window.canvas,
									shape.vertices[i].x,
									shape.vertices[i].y,
									shape.vertices[j].x,
									shape.vertices[j].y,
									color);
		}
	}
}

convex_shape minkowski_sum_shape(convex_shape shape1, convex_shape shape2) {
	convex_shape shape_sum;
	shape_sum.n = shape1.n * shape2.n;
	shape_sum.vertices = malloc(shape_sum.n * sizeof(dgl_V3));

	for(int i = 0; i < shape1.n; ++i) {
		for(int j = 0; j < shape2.n; ++j) {
			shape_sum.vertices[i*shape2.n + j] = dgl_v3_add(shape1.vertices[i],
															shape2.vertices[j]);
		}
	}
	
	return shape_sum;
}

void init() {
	init_window_params(width, height);
	init_fps(-1);
}

void start() {
	shape1.n = 3;
	shape2.n = 4;

	shape1.vertices = malloc(shape1.n * sizeof(dgl_V3));
	shape2.vertices = malloc(shape2.n * sizeof(dgl_V3));

	shape1.vertices[0] = (dgl_V3){ .x = -30, .y = 0, .z = 0 };
	shape1.vertices[1] = (dgl_V3){ .x = 30, .y = 0, .z = 0 };
	shape1.vertices[2] = (dgl_V3){ .x = 0, .y = 40, .z = 0 };

	shape2.vertices[0] = (dgl_V3){ .x = -20, .y = 20, .z = 0 };
	shape2.vertices[1] = (dgl_V3){ .x = 20, .y = 20, .z = 0 };
	shape2.vertices[2] = (dgl_V3){ .x = 20, .y = -20, .z = 0 };
	shape2.vertices[3] = (dgl_V3){ .x = -20, .y = -20, .z = 0 };

	minkowski_sum = minkowski_sum_shape(shape1, shape2);
}

void update(float dt) {
	dgl_clear(&window.canvas, DGL_BLACK);
	draw_convex_shape(shape1, DGL_RED);
	draw_convex_shape(shape2, DGL_GREEN);
	draw_convex_shape(minkowski_sum, DGL_BLUE);
	//p1.x = cursor_pos.x;
	//p1.y = cursor_pos.y;
}

void end() {}
