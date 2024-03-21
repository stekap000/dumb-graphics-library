#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

//typedef struct{
//	dgl_Point3D *vertices;
//	int *indices;
//	dgl_Color *colors;
//	size_t vertices_length;
//	size_t indices_length;
//	size_t colors_length;
//} dgl_Simple_Model;

#define POINTS_NUM 16
#define HYPERCUBE_FACES_NUM 24

dgl_Real hypercube[POINTS_NUM * 4] = {
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

dgl_Real hypercube_indices[HYPERCUBE_FACE_NUM] = {
	0, 1, 2,
	0, 1, 4,
	0, 1, 8,
	0, 1, 3,
	0, 1, 5,
	0, 1, 9,
	0, 2, 4,
	0, 2, 8,
	0, 2, 3,
	0, 2, 6,
	0, 2, 10,
}

dgl_Point3D hypercube_3D[POINTS_NUM] = {0};

// Projections are orthogonal.

void project_to_3D() {
	for(int i = 0; i < POINTS_NUM; ++i) {
		hypercube_3D[i].x = hypercube[i*4];
		hypercube_3D[i].y = hypercube[i*4+1];
		hypercube_3D[i].z = hypercube[i*4+2];
	}
}

void init() {

}

void start() {

}

void update(float dt) {

}

void end() {

}
