#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_COORDINATE_CENTER_X 250
#define DGL_COORDINATE_CENTER_Y 250
#define DGL_IMPLEMENTATION
#include "dgl.h"

int width = 500;
int height = 500;

dgl_Point2D v3_to_point2d(dgl_V3 v) {
	return (dgl_Point2D){ .x = v.x, .y = v.y };
}

typedef struct {
	int n;
	dgl_V3 *vertices;
} convex_shape;

convex_shape shape1;
convex_shape shape2;
convex_shape minkowski_sum = {0};

void scale_shape(convex_shape *shape, float f) {
	for(int i = 0; i < shape->n; ++i) dgl_v3_scale_mut(&shape->vertices[i], f);
}

void translate_shape(convex_shape *shape, dgl_V3 v) {
	for(int i = 0; i < shape->n; ++i) dgl_v3_add_mut(&shape->vertices[i], v);
}

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

void minkowski_sum_shape_alloc(convex_shape *shape_sum, convex_shape shape1, convex_shape shape2) {
	shape_sum->n = shape1.n * shape2.n;
	shape_sum->vertices = malloc(shape_sum->n * sizeof(dgl_V3));
}

void minkowski_sum_shape_update(convex_shape *shape_sum, convex_shape shape1, convex_shape shape2) {
	for(int i = 0; i < shape1.n; ++i) {
		for(int j = 0; j < shape2.n; ++j) {
			shape_sum->vertices[i*shape2.n + j] = dgl_v3_add(shape1.vertices[i],
															 shape2.vertices[j]);
		}
	}
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

	scale_shape(&shape1, 3);
	scale_shape(&shape2, 3);
	
	minkowski_sum_shape_alloc(&minkowski_sum, shape1, shape2);
}

convex_shape temp;
void update(float dt) {
	dgl_clear(&window.canvas, DGL_BLACK);
	dgl_draw_vertical_line(&window.canvas, 0, DGL_RGBA(255, 255, 255, 120));
	dgl_draw_horizontal_line(&window.canvas, 0, DGL_RGBA(255, 255, 255, 120));
	dgl_fill_circle(&window.canvas, cursor_pos.x, cursor_pos.y, 3, DGL_YELLOW);
	
	temp = shape1;
	
	translate_shape(&temp, (dgl_V3){ cursor_pos.x, cursor_pos.y, 0});
	
	draw_convex_shape(temp, DGL_RGBA(255, 0, 0, 120));
	draw_convex_shape(shape2, DGL_RGBA(0, 255, 0, 120));
	minkowski_sum_shape_update(&minkowski_sum, shape1, shape2);
	draw_convex_shape(minkowski_sum, DGL_RGBA(0, 0, 255, 120));
	
	translate_shape(&temp, (dgl_V3){ -cursor_pos.x, -cursor_pos.y, 0});
}

void end() {}
