#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

#define POINTS_NUM 16
#define FACES_NUM 24

// Vertices and indices values are generated using my program from
// tesseract-generator repository.

dgl_Real hypercube_vertices[] = {
	0, 0, 0, 0, //  0
	0, 0, 0, 1, //  1
	0, 0, 1, 0, //  2
	0, 0, 1, 1, //  3
	0, 1, 0, 0, //  4
	0, 1, 0, 1, //  5
	0, 1, 1, 0, //  6
	0, 1, 1, 1, //  7
	1, 0, 0, 0, //  8
	1, 0, 0, 1, //  9
	1, 0, 1, 0, // 10
	1, 0, 1, 1, // 11
	1, 1, 0, 0, // 12
	1, 1, 0, 1, // 13
	1, 1, 1, 0, // 14
	1, 1, 1, 1, // 15
};

// Indices form 4 triangles per face of hypercube.
int hypercube_indices[96*3] = {
	0, 8, 4,
	0, 8, 2,
	0, 8, 1,
	0, 4, 2,
	0, 4, 1,
	0, 2, 1,
	8, 0, 12,
	8, 0, 10,
	8, 0, 9,
	8, 12, 10,
	8, 12, 9,
	8, 10, 9,
	4, 12, 0,
	4, 12, 6,
	4, 12, 5,
	4, 0, 6,
	4, 0, 5,
	4, 6, 5,
	12, 4, 8,
	12, 4, 14,
	12, 4, 13,
	12, 8, 14,
	12, 8, 13,
	12, 14, 13,
	2, 10, 6,
	2, 10, 0,
	2, 10, 3,
	2, 6, 0,
	2, 6, 3,
	2, 0, 3,
	10, 2, 14,
	10, 2, 8,
	10, 2, 11,
	10, 14, 8,
	10, 14, 11,
	10, 8, 11,
	6, 14, 2,
	6, 14, 4,
	6, 14, 7,
	6, 2, 4,
	6, 2, 7,
	6, 4, 7,
	14, 6, 10,
	14, 6, 12,
	14, 6, 15,
	14, 10, 12,
	14, 10, 15,
	14, 12, 15,
	1, 9, 5,
	1, 9, 3,
	1, 9, 0,
	1, 5, 3,
	1, 5, 0,
	1, 3, 0,
	9, 1, 13,
	9, 1, 11,
	9, 1, 8,
	9, 13, 11,
	9, 13, 8,
	9, 11, 8,
	5, 13, 1,
	5, 13, 7,
	5, 13, 4,
	5, 1, 7,
	5, 1, 4,
	5, 7, 4,
	13, 5, 9,
	13, 5, 15,
	13, 5, 12,
	13, 9, 15,
	13, 9, 12,
	13, 15, 12,
	3, 11, 7,
	3, 11, 1,
	3, 11, 2,
	3, 7, 1,
	3, 7, 2,
	3, 1, 2,
	11, 3, 15,
	11, 3, 9,
	11, 3, 10,
	11, 15, 9,
	11, 15, 10,
	11, 9, 10,
	7, 15, 3,
	7, 15, 5,
	7, 15, 6,
	7, 3, 5,
	7, 3, 6,
	7, 5, 6,
	15, 7, 11,
	15, 7, 13,
	15, 7, 14,
	15, 11, 13,
	15, 11, 14,
	15, 13, 14,
};

dgl_Point3D hypercube_vertices_3D[POINTS_NUM] = {0};

// This is arbitrary rotation that I just found to be interesting when rendered.
void rotate_hypercube_4D(float angle, float dt) {
	// Rotation in yz plane.
	for(int i = 0; i < POINTS_NUM; ++i) {
		float x = hypercube_vertices[i*4];
		float w = hypercube_vertices[i*4+3];

		hypercube_vertices[i*4] = cos(angle*dt)*x - sin(angle*dt)*w;
		hypercube_vertices[i*4+3] = sin(angle*dt)*x + cos(angle*dt)*w;
	}

	// Rotation in xw plane.
	for(int i = 0; i < POINTS_NUM; ++i) {
		float y = hypercube_vertices[i*4+1];
		float z = hypercube_vertices[i*4+2];
	
		hypercube_vertices[i*4+1] = cos(angle*dt)*y - sin(angle*dt)*z;
		hypercube_vertices[i*4+2] = sin(angle*dt)*y + cos(angle*dt)*z;
	}

	// Rotation in xy plane.
	for(int i = 0; i < POINTS_NUM; ++i) {
		float z = hypercube_vertices[i*4+2];
		float w = hypercube_vertices[i*4+3];
	
		hypercube_vertices[i*4+2] = cos(angle*dt)*z - sin(angle*dt)*w;
		hypercube_vertices[i*4+3] = sin(angle*dt)*z + cos(angle*dt)*w;
	}
}

// Orthogonal projection.
void project_to_3D() {
	for(int i = 0; i < POINTS_NUM; ++i) {
		hypercube_vertices_3D[i].x = hypercube_vertices[i*4];
		hypercube_vertices_3D[i].y = hypercube_vertices[i*4+1];
		hypercube_vertices_3D[i].z = hypercube_vertices[i*4+2];
	}
}

dgl_Color colors[1] = {
	DGL_RGBA(0, 255, 0, 130),
};
dgl_Simple_Model sm = {
	.vertices = hypercube_vertices_3D,
	.indices = hypercube_indices,
	.colors = colors,
	.vertices_length = POINTS_NUM,
	// 4 triangles per face and 3 points per triangle.
	.indices_length = FACES_NUM*4*3,
	.colors_length = 1
};

void init() {
	init_window_params(800, 800);
	init_fps(-1);
}

void start() {}

void update(float dt) {
	rotate_hypercube_4D(0.5, dt);
	project_to_3D();
	dgl_scale_simple_model(&sm, 250.0f);
	dgl_translate_simple_model(&sm, (dgl_Point3D){.x = 400, .y = 400, .z = 0});
	dgl_draw_simple_model_mesh(&window.canvas, &sm, (dgl_Mat){0});
}

void end() {}
