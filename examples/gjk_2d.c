#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#define DGL_COORDINATE_CENTER_X 250
#define DGL_COORDINATE_CENTER_Y 250
#include "dgl.h"

int width = 500;
int height = 500;

dgl_Simple_Model shape1;
dgl_Simple_Model shape2;

dgl_V3 shape1_center;
dgl_V3 shape2_center;

typedef struct {
	int n;
	dgl_V3 points[4];
} simplex;

dgl_V3 support(dgl_Simple_Model s, dgl_V3 direction) {
	float max = -999999;
	float temp = 0;
	int temp_i = 0;
	for(int i = 0; i < s.vertices_length; ++i) {
		temp = dgl_v3_dot(s.vertices[i], direction);
		if(temp > max) {
			max = temp;
			temp_i = i;
		}
	}

	return s.vertices[temp_i];
}

dgl_V3 minkowski_diff_support(dgl_Simple_Model s1, dgl_Simple_Model s2, dgl_V3 direction) {
	return dgl_v3_sub(support(s1, direction), support(s2, dgl_v3_scale(direction, -1)));
}

void add_point_to_simplex(simplex *s, dgl_V3 point) {
	s->points[s->n++] = point;
}

dgl_Bool update_simplex_and_direction_2d(simplex *s, dgl_V3 *d) {
	// One point case doesn't happen.

	// Two point case is determined because of how we found support point and
	// because of check if support point didn't reach origin (in the main loop).
	switch(s->n) {
	case 2: {
		dgl_V3 temp = dgl_v3_sub(s->points[0], s->points[1]);
		*d = dgl_v3_cross(dgl_v3_cross(temp, dgl_v3_scale(s->points[1], -1)), temp);
	}
	case 3: {
		dgl_V3 temp1 = dgl_v3_sub(s->points[0], s->points[2]); // AC
		dgl_V3 temp2 = dgl_v3_sub(s->points[1], s->points[2]); // AB
		dgl_V3 N = dgl_v3_cross(temp2, temp1);

		if(dgl_v3_dot(dgl_v3_cross(N, temp1), dgl_v3_scale(s->points[2], -1)) > 0) {
			s->n = 2;
			s->points[1] = s->points[2];
			*d = dgl_v3_cross(dgl_v3_cross(temp1, dgl_v3_scale(s->points[2], -1)), temp1);
		}
		else {
		    if(dgl_v3_dot(dgl_v3_cross(temp2, N), dgl_v3_scale(s->points[2], -1)) > 0) {
				s->n = 2;
				s->points[0] = s->points[2];
				*d = dgl_v3_cross(dgl_v3_cross(temp2, dgl_v3_scale(s->points[2], -1)), temp2);
			}
			else {
				return true;
			}
		}

		/*
		if(dgl_v3_dot(dgl_v3_cross(N, temp1), dgl_v3_scale(s->points[2], -1)) > 0) {
			if(dgl_v3_dot(temp1, dgl_v3_scale(s->points[2], -1)) > 0) {
				s->n = 2;
				s->points[1] = s->points[2];
				*d = dgl_v3_cross(dgl_v3_cross(temp1, dgl_v3_scale(s->points[2], -1)), temp1);
			}
			else {
				if(dgl_v3_dot(temp2, dgl_v3_scale(s->points[2], -1)) > 0) {
					s->n = 2;
					s->points[0] = s->points[2];
					*d = dgl_v3_cross(dgl_v3_cross(temp2, dgl_v3_scale(s->points[2], -1)), temp2);
				}
				else {
					s->n = 1;
					s->points[0] = s->points[2];
					*d = dgl_v3_scale(s->points[2], -1);
				}
			}
		}
		else {
			if(dgl_v3_dot(dgl_v3_cross(temp2, N), dgl_v3_scale(s->points[2], -1)) > 0) {
				if(dgl_v3_dot(temp2, dgl_v3_scale(s->points[2], -1)) > 0) {
					s->n = 2;
					s->points[0] = s->points[2];
					*d = dgl_v3_cross(dgl_v3_cross(temp2, dgl_v3_scale(s->points[2], -1)), temp2);
				}
				else {
					s->n = 1;
					s->points[0] = s->points[2];
					*d = dgl_v3_scale(s->points[2], -1);
				}
			}
			else {
				return true;
			}
		}
		*/
	}
	}

	return false;
}

dgl_Bool gjk_2d(dgl_Simple_Model s1, dgl_Simple_Model s2) {
	simplex s = {0};
	dgl_V3 direction = {1, 0, 0};
	dgl_V3 support_point = {0};
	
	// Start out with some point in minkowski diff.
	add_point_to_simplex(&s, minkowski_diff_support(s1, s2, direction));
	direction = dgl_v3_scale(s.points[0], -1);
	
	while(true) {
		// Furthest point in opposite direction (direction towards origin).
		support_point = minkowski_diff_support(s1, s2, direction);

		// Support point didn't reach origin.
		if(dgl_v3_dot(support_point, direction) < 0) return false;

		add_point_to_simplex(&s, support_point);

		if (update_simplex_and_direction_2d(&s, &direction)) return true;
	}
}

void init() {
	init_fps(-1);
	init_window_params(width, height);
}

dgl_Point3D shape1_vertices[] = {
	{-0.5,  0.5, 0},
	{ 0.5,  0.5, 0},
	{ 0.5, -0.5, 0},
	{-0.5, -0.5, 0},
};

int shape1_indices[] = {
	0, 1, 2,
	0, 2, 3
};

dgl_Color shape1_colors[] = {
	DGL_GREEN
};

dgl_Point3D shape2_vertices[] = {
	{-0.5, -0.5, 0},
	{ 0.0,  0.5, 0},
	{ 0.5, -0.5, 0},
};

int shape2_indices[] = {
	0, 1, 2,
};

dgl_Color shape2_colors[] = {
	DGL_RED
};

void start() {
	shape1 = (dgl_Simple_Model){
		.vertices = shape1_vertices,
		.indices = shape1_indices,
		.colors = shape1_colors,
		.vertices_length = 4,
		.indices_length = 6,
		.colors_length = 1
	};
	shape1_center = (dgl_V3){100, 0, 0};
	dgl_scale_simple_model(&shape1, 100);
	//dgl_translate_simple_model(&shape1, shape1_center);

	shape2 = (dgl_Simple_Model){
		.vertices = shape2_vertices,
		.indices = shape2_indices,
		.colors = shape2_colors,
		.vertices_length = 3,
		.indices_length = 3,
		.colors_length = 1
	};
	shape2_center = (dgl_V3){0, 0, 0};
	dgl_scale_simple_model(&shape2, 100);
}

dgl_V3 Z = {0, 0, 1};

void update(float dt) {
	//dgl_V3 movement_direction = dgl_v3_cross(Z, shape1_center);
	//dgl_translate_simple_model(&shape1, dgl_v3_scale(movement_direction, dt*0.5));
	//dgl_v3_add_mut(&shape1_center, dgl_v3_scale(movement_direction, dt*0.5));
	dgl_draw_simple_model_mesh(&window.canvas, &shape1, (dgl_Mat){0});

	dgl_translate_simple_model(&shape2, (dgl_V3){cursor_pos.x, cursor_pos.y, 0});
	dgl_draw_simple_model_mesh(&window.canvas, &shape2, (dgl_Mat){0});

	dgl_V3 sp1 = support(shape1, (dgl_V3){cursor_pos.x, cursor_pos.y, 0});
	dgl_fill_circle(&window.canvas, sp1.x, sp1.y, 5, DGL_YELLOW);

	dgl_V3 sp2 = support(shape2, (dgl_V3){cursor_pos.x, cursor_pos.y, 0});
	dgl_fill_circle(&window.canvas, sp2.x, sp2.y, 5, DGL_YELLOW);

	dgl_V3 sp = minkowski_diff_support(shape1, shape2, (dgl_V3){cursor_pos.x, cursor_pos.y, 0});
	dgl_fill_circle(&window.canvas, sp.x, sp.y, 5, DGL_YELLOW);
 
	if(gjk_2d(shape1, shape2)) {
		shape1.colors[0] = DGL_BLUE;
	}
	else {
		shape1.colors[0] = DGL_GREEN;
	}

	dgl_translate_simple_model(&shape2, (dgl_V3){-cursor_pos.x, -cursor_pos.y, 0});
}

void end() {}
