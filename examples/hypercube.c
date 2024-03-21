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

dgl_Real hypercube[POINTS_NUM * 4] = {
	0, 0, 0, 0,
	1, 0, 0, 0,
	0, 1, 0, 0,
	1, 1, 0, 0,
	0, 0, 1, 0,
	1, 0, 1, 0,
	0, 1, 1, 0,
	1, 1, 1, 0,
	0, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 0, 1,
	1, 1, 0, 1,
	0, 0, 1, 1,
	1, 0, 1, 1,
	0, 1, 1, 1,
	1, 1, 1, 1
};

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
