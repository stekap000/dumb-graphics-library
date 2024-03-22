#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

#define POINTS_NUM 16
#define HYPERCUBE_FACES_NUM 24

dgl_Real hypercube_vertices[POINTS_NUM * 4] = {
	0, 0, 0, 0, // 0
	1, 0, 0, 0,	// 1
	0, 1, 0, 0,	// 2
	1, 1, 0, 0,	// 3
	0, 0, 1, 0,	// 4
	1, 0, 1, 0,	// 5
	0, 1, 1, 0,	// 6
	1, 1, 1, 0,	// 7
	0, 0, 0, 1,	// 8
	1, 0, 0, 1,	// 9
	0, 1, 0, 1,	// 10
	1, 1, 0, 1,	// 11
	0, 0, 1, 1,	// 12
	1, 0, 1, 1,	// 13
	0, 1, 1, 1,	// 14
	1, 1, 1, 1  // 15
};

int hypercube_indices[] = {
	0, 1, 2,  // 0
	0, 1, 4,  // 1
	0, 1, 8,  // 2
	0, 1, 3,  // 3
	0, 1, 5,  // 4
	0, 1, 9,  // 5
	0, 2, 4,  // 6
	0, 2, 8,  // 7
	0, 2, 3,  // 8
	0, 2, 6,  // 9
	0, 2, 10, // 10
	0, 4, 8,  // 11
	0, 4, 5,  // 12
	0, 4, 6,  // 13
	0, 4, 12, // 14
	0, 8, 9,  // 15
	0, 8, 10, // 16
	0, 8, 12, // 17
	1, 2, 3,  // 18
	1, 2, 4,  // 19
	1, 2, 5,  // 20
	1, 2, 8,  // 21
	1, 2, 9,  // 22
	1, 2, 6,  // 23
	1, 2, 10, // 24
	1, 4, 3,  // 25
	1, 4, 5,  // 26
	1, 4, 8,  // 27
	1, 4, 9,  // 28
	1, 4, 6,  // 29
	1, 4, 12, // 30
	1, 8, 3,  // 31
	1, 8, 5,  // 32
	1, 8, 9,  // 33
	1, 8, 10, // 34
	1, 8, 12, // 35
	1, 3, 5,  // 36
	1, 3, 9,  // 37
	1, 3, 7,  // 38
	1, 3, 11, // 39
	1, 3, 11, // 39
	1, 3, 11, // 39
};

//	0, 0, 0, 0, // 0
//	1, 0, 0, 0,	// 1
//	0, 1, 0, 0,	// 2
//	1, 1, 0, 0,	// 3
//	0, 0, 1, 0,	// 4
//	1, 0, 1, 0,	// 5
//	0, 1, 1, 0,	// 6
//	1, 1, 1, 0,	// 7
//	0, 0, 0, 1,	// 8
//	1, 0, 0, 1,	// 9
//	0, 1, 0, 1,	// 10
//	1, 1, 0, 1,	// 11
//	0, 0, 1, 1,	// 12
//	1, 0, 1, 1,	// 13
//	0, 1, 1, 1,	// 14
//	1, 1, 1, 1  // 15

dgl_Point3D hypercube_vertices_3D[POINTS_NUM] = {0};

void rotate_4D(float angle, float dt) {
	// Rotation in yz plane.
	
	for(int i = 0; i < POINTS_NUM; ++i) {
		float x = hypercube_vertices[i*4];
		float y = hypercube_vertices[i*4+1];
		float z = hypercube_vertices[i*4+2];
		float w = hypercube_vertices[i*4+3];

		hypercube_vertices[i*4] = cos(angle*dt)*x - sin(angle*dt)*w;
		hypercube_vertices[i*4+3] = sin(angle*dt)*x + cos(angle*dt)*w;
	}
}

void project_to_3D() {
	// Orthogonal projection.
	for(int i = 0; i < POINTS_NUM; ++i) {
		hypercube_vertices_3D[i].x = hypercube_vertices[i*4];
		hypercube_vertices_3D[i].y = hypercube_vertices[i*4+1];
		hypercube_vertices_3D[i].z = hypercube_vertices[i*4+2];
	}
}

dgl_Color colors[1] = { DGL_GREEN };
dgl_Simple_Model sm = {
	.vertices = hypercube_vertices_3D,
	.indices = hypercube_indices,
	.colors = colors,
	.vertices_length = POINTS_NUM,
	.indices_length = 42,
	.colors_length = 1
};

void init() {
	init_window_params(500, 500);
	init_fps(-1);
}

void start() {}

void update(float dt) {
	rotate_4D(2, dt);
	project_to_3D();
	dgl_scale_simple_model(&sm, 200.0f);
	dgl_translate_simple_model(&sm, (dgl_Point3D){.x = 150, .y = 150, .z = 0});
	dgl_draw_simple_model_mesh(&window.canvas, &sm, (dgl_Mat){0});
}

void end() {}
