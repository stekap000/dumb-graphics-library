#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_COORDINATE_CENTER_X 350
#define DGL_COORDINATE_CENTER_Y 350
#define DGL_IMPLEMENTATION
#include "dgl.h"

// Program created for the purpose of visualizing minkowski sum/diff in order
// to get intuitive sense about it's behaviour.

int width = 700;
int height = 700;

dgl_Point2D v3_to_point2d(dgl_V3 v) {
	return (dgl_Point2D){ .x = v.x, .y = v.y };
}

typedef struct {
	int n;
	dgl_V3 *vertices;
} convex_shape;

convex_shape shape1;
convex_shape shape2;
convex_shape shape2_inverted;
convex_shape minkowski_sum = {0};
convex_shape minkowski_diff = {0};

void scale_shape(convex_shape *shape, float f) {
	for(int i = 0; i < shape->n; ++i) dgl_v3_scale_mut(&shape->vertices[i], f);
}

void translate_shape(convex_shape *shape, dgl_V3 v) {
	for(int i = 0; i < shape->n; ++i) dgl_v3_add_mut(&shape->vertices[i], v);
}

convex_shape deep_copy_shape(convex_shape shape) {
	convex_shape temp = {0};
	temp.n = shape.n;
	temp.vertices = malloc(temp.n * sizeof(dgl_V3));
	for(int i = 0; i < temp.n; ++i) temp.vertices[i] = shape.vertices[i];
	return temp;
}

// Draw shape by drawing lines between all possible point.
// Not efficient, but easy.
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

	shape1.vertices[0] = (dgl_V3){ .x = -20, .y = 0, .z = 0 };
	shape1.vertices[1] = (dgl_V3){ .x = 20, .y = 0, .z = 0 };
	shape1.vertices[2] = (dgl_V3){ .x = 0, .y = 40, .z = 0 };

	shape2.vertices[0] = (dgl_V3){ .x = -20, .y = 20, .z = 0 };
	shape2.vertices[1] = (dgl_V3){ .x = 20, .y = 20, .z = 0 };
	shape2.vertices[2] = (dgl_V3){ .x = 20, .y = -20, .z = 0 };
	shape2.vertices[3] = (dgl_V3){ .x = -20, .y = -20, .z = 0 };

	scale_shape(&shape1, 3);
	scale_shape(&shape2, 3);

	translate_shape(&shape2, (dgl_V3){120, 120, 0});
	
	shape2_inverted = deep_copy_shape(shape2);
	scale_shape(&shape2_inverted, -1);
	
	minkowski_sum_shape_alloc(&minkowski_sum, shape1, shape2);
	minkowski_sum_shape_alloc(&minkowski_diff, shape1, shape2_inverted);
}

void update(float dt) {
	dgl_clear(&window.canvas, DGL_BLACK);
	dgl_draw_vertical_line(&window.canvas, 0, DGL_RGBA(255, 255, 255, 120));
	dgl_draw_horizontal_line(&window.canvas, 0, DGL_RGBA(255, 255, 255, 120));
	dgl_fill_circle(&window.canvas, cursor_pos.x, cursor_pos.y, 5, DGL_YELLOW);
	
	translate_shape(&shape1, (dgl_V3){ cursor_pos.x, cursor_pos.y, 0});
	
	draw_convex_shape(shape1, DGL_WHITE);
	minkowski_sum_shape_update(&minkowski_sum, shape1, shape2);
	minkowski_sum_shape_update(&minkowski_diff, shape1, shape2_inverted);
	draw_convex_shape(minkowski_sum, DGL_RGBA(0, 0, 255, 80));
	draw_convex_shape(minkowski_diff, DGL_RGBA(255, 255, 0, 80));
	draw_convex_shape(shape2, DGL_GREEN);
	
	translate_shape(&shape1, (dgl_V3){ -cursor_pos.x, -cursor_pos.y, 0});
}

void end() {}
