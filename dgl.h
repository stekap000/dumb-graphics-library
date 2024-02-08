/*
	DGL - Dumb Graphics Library
	
	This project was started for three reasons:
		1. To be playground for trying out different graphical concepts that are usually implemented directly on GPU
		2. To be playground for further experimentation (for example using 'winapi' to show the results)
		3. To allow easy graphics whenever I need them without many dependencies or setup time
	
	It is clear that it is impossible to expect high level of efficiency from graphics library implemented on CPU.
	However, with this in mind, I do try to optimize things over time within these confines, and this also represents
	another form of experimentation for me.
	
	During the time I was writing this, I stumbled upon 'tsoding daily' and his 'olivec' library. I picked up some
	things from him (for example deferred return in C that I didn't know about).
	
	Feel free to use this library in any way you want.
	
	stekap, 2023
	
	-----------------------------------------------------------------------------------------------------------------
	
	Naming convention:
		1. Everything that is part of the library starts with 'dgl' in upper or lower case
		2. Name/Identifier parts are separated by underscore ('_')
		3. Macros are fully upper case
		4. Types have capital letter after each underscore (like dgl_Font)
		5. Functions have all lower case letters
*/

#ifndef DGL_DECLARATION
#define DGL_DECLARATION
#define DGLAPI static

#include <stdlib.h>
#include <stdio.h>	// TODO: Maybe remove in the future, or let user choose with macro. This is only because we have functions that work with files
#include <string.h> // TODO: Maybe remove (currently it is here because of memcpy)
#include <stdint.h>
#include <errno.h>  // TODO: Remove in the future. We can do without this
#include <math.h>

typedef enum { false, true } dgl_Bool;
typedef float dgl_Real; // This is because we can use it to easily change precision
typedef int dgl_Errno;

#define DGL_PI 3.14159265358

#define DEFER_RETURN(v) do { DEFER_RESULT=v; goto DEFER; } while(0)

#ifdef DGL_TARGET_WINDOWS
    #define DGL_RGB(r, g, b)     (((b) & 0xFF) | (((g) & 0xFF) << 8) | (((r) & 0xFF) << 16) | 0xFF000000)
    #define DGL_RGBA(r, g, b, a) (((b) & 0xFF) | (((g) & 0xFF) << 8) | (((r) & 0xFF) << 16) | (((a) & 0xFF) << 24))
    #define DGL_GET_ALPHA(color)    (((color) >> 24) & 0xFF)
    #define DGL_GET_RED(color)      (((color) >> 16) & 0xFF)
    #define DGL_GET_GREEN(color)    (((color) >>  8) & 0xFF)
    #define DGL_GET_BLUE(color)		 ((color)        & 0xFF)
    
	#define DGL_WHITE	0xFFFFFFFF
	#define DGL_BLACK	0xFF000000
	#define DGL_RED 	0xFFFF0000
	#define DGL_GREEN 	0xFF00FF00
	#define DGL_BLUE 	0xFF0000FF
	#define DGL_YELLOW	0xFFFFFF00
	#define DGL_CYAN	0xFF00FFFF
	#define DGL_PINK	0xFFFF00FF
#else
    #define DGL_RGB(r, g, b)     (((r) & 0xFF) | (((g) & 0xFF) << 8) | (((b) & 0xFF) << 16) | 0xFF000000)
    #define DGL_RGBA(r, g, b, a) (((r) & 0xFF) | (((g) & 0xFF) << 8) | (((b) & 0xFF) << 16) | (((a) & 0xFF) << 24))
    #define DGL_GET_ALPHA(color)    (((color) >> 24) & 0xFF)
    #define DGL_GET_RED(color)		 ((color)        & 0xFF)
    #define DGL_GET_GREEN(color)    (((color) >>  8) & 0xFF)
    #define DGL_GET_BLUE(color)     (((color) >> 16) & 0xFF)

	#define DGL_WHITE	0xFFFFFFFF
	#define DGL_BLACK	0xFF000000
	#define DGL_RED 	0xFF0000FF
	#define DGL_GREEN 	0xFF00FF00
	#define DGL_BLUE 	0xFFFF0000
	#define DGL_YELLOW	0xFF00FFFF
	#define DGL_CYAN	0xFFFFFF00
	#define DGL_PINK	0xFFFF00FF
#endif

#define DGL_OFFSET_OF(s, f) ((size_t)&(((s*)0)->f))
#define DGL_SWAP(a, b, T) do { T t = a; a = b; b = t; } while(0)
#define DGL_LERP(a, b, t) a+(b-a)*t
#define DGL_SCALE_RGB(a, color) DGL_RGB((int)((a)*DGL_GET_RED(color)), (int)((a)*DGL_GET_GREEN(color)), (int)((a)*DGL_GET_BLUE(color)))
#define DGL_SCALE_RGBA(a, color) DGL_RGBA((int)((a)*DGL_GET_RED(color)), (int)((a)*DGL_GET_GREEN(color)), (int)((a)*DGL_GET_BLUE(color)), (int)((a)*DGL_GET_ALPHA(color)))
#define DGL_SUM_RGB(c1, c2) DGL_RGB(DGL_GET_RED(c1) + DGL_GET_RED(c2), DGL_GET_GREEN(c1) + DGL_GET_GREEN(c2), DGL_GET_BLUE(c1) + DGL_GET_BLUE(c2))
#define DGL_SUM_RGBA(c1, c2) DGL_RGBA(DGL_GET_RED(c1) + DGL_GET_RED(c2), DGL_GET_GREEN(c1) + DGL_GET_GREEN(c2), DGL_GET_BLUE(c1) + DGL_GET_BLUE(c2), DGL_GET_ALPHA(c1) + DGL_GET_ALPHA(c2))
// TODO: Fix weird behaviour
#define DGL_CLAMP(v, l, r) (((v) < (l)) ? (l) : (((v) > (r)) ? (r) : (v)))

#ifndef DGL_AALIAS_DEG
	#define DGL_AALIAS_DEG 2
#endif

#ifndef DGL_COORDINATE_CENTER_X
	#define DGL_COORDINATE_CENTER_X 0
#endif
#ifndef DGL_COORDINATE_CENTER_Y
	#define DGL_COORDINATE_CENTER_Y 0
#endif

#define DGL_TRANSFORM_COORDINATES_X(x) ((x) + DGL_COORDINATE_CENTER_X)
#define DGL_TRANSFORM_COORDINATES_Y(y, canvas_height) ((canvas_height)-(y) - DGL_COORDINATE_CENTER_Y)

typedef struct {
	dgl_Real *cells;
	int height;
	int width;
} dgl_Mat;

DGLAPI dgl_Mat dgl_mat_alloc(int height, int width){
	dgl_Mat m = {};
	m.cells = calloc(height*width, sizeof(dgl_Real));
	m.height = height;
	m.width = width;
	return m;
}

DGLAPI void dgl_mat_free(dgl_Mat m){
	free(m.cells);
}

DGLAPI void dgl_mat_scale(dgl_Mat m, dgl_Real s){
	for(int i = 0; i < m.width*m.height; ++i)
		m.cells[i] *= s;
}

DGLAPI void dgl_mat_add(dgl_Mat m1, dgl_Mat m2){
	if(m1.height == m2.height && m1.width == m2.width)
		for(int i = 0; i < m1.width*m1.height; ++i)
			m1.cells[i] += m2.cells[i];
}

DGLAPI dgl_Mat dgl_mat_mul(dgl_Mat m1, dgl_Mat m2){
	dgl_Mat m = {};
	if(m1.width == m2.height){
		m = dgl_mat_alloc(m1.height, m2.width);
		
		for(int i = 0; i < m1.height; ++i){
			for(int j = 0; j < m2.width; ++j){
				for(int k = 0; k < m1.width; ++k){
					m.cells[i*m.width + j] += m1.cells[i*m1.width + k]*m2.cells[k*m2.width + j];
				}					
			}
		} 
	}

	return m;
}

// TODO: Delete this in the future since it is just for testing
DGLAPI void dgl_mat_print(dgl_Mat m){
	for(int i = 0; i < m.height; ++i){
		for(int j = 0; j < m.width; ++j){
			printf("%f ", m.cells[i*m.width + j]);
		}
		printf("\n");
	}
}

typedef dgl_Real dgl_Mat4[16];
typedef dgl_Real dgl_Vec4[4];

DGLAPI void dgl_mat4_add(dgl_Mat4 m1, dgl_Mat4 m2){
#ifdef DGL_USE_SIMD

#else
	for(int i = 0; i < 16; ++i)
		m1[i] += m2[i];
#endif
}

DGLAPI void dgl_mat4_compose(dgl_Mat4 m1, dgl_Mat4 m2){
	dgl_Mat4 temp;
	memcpy(temp, m1, 64);
	
	for(int i = 0; i < 4; ++i){
		for(int j = 0; j < 4; ++j){
			for(int k = 0; k < 4; ++k){
				m1[i*4 + j] += temp[i*4 + k] * m2[k*4 + j];
			}					
		}
	}
}
// 
// dgl_Vec dgl_mat4_apply(dgl_Mat4 m, dgl_Vec4 v){
// 
// }

DGLAPI void dgl_init_proj_params(float fov_deg, float aspect_ratio, dgl_Real *l, dgl_Real *r, dgl_Real *t, dgl_Real *b, dgl_Real n, dgl_Real f){
	(*t) = n * tanf(fov_deg * 0.5 * DGL_PI / 180);
	(*b) = -(*t);
	(*r) = aspect_ratio * (*t);
	(*l) = -(*r);
}

// These params are initialized by specifying FOV and aspect-ratio
DGLAPI dgl_Mat dgl_mat_get_proj(dgl_Real l, dgl_Real r, dgl_Real t, dgl_Real b, dgl_Real n, dgl_Real f){
	dgl_Mat m = dgl_mat_alloc(4, 4);

	// This is OpenGL projection matrix
	m.cells[0]  = 2*n/(r-l);
	m.cells[2]  = (r+l)/(r-l);
	m.cells[5]  = 2*n/(t-b);
	m.cells[6]  = (t+b)/(t-b);
	m.cells[10] = -(f+n)/(f-n);
	m.cells[11] = -2*f*n/(f-n);
	m.cells[14] = -1;
	
	return m;
}

typedef union {
	struct { int x, y; };
	int v[2];
} dgl_Point2D;

typedef union {
	struct { dgl_Real x, y, z; };
	dgl_Real v[3];
} dgl_Point3D;

typedef struct { dgl_Point2D v[3];	} dgl_Triangle2D;
typedef struct { dgl_Point3D v[3];	} dgl_Triangle3D;

DGLAPI inline void dgl_scale_point_2D(dgl_Point2D *p, int scale);
DGLAPI inline void dgl_scale_point_3D(dgl_Point3D *p, dgl_Real scale);
DGLAPI inline void dgl_translate_point_2D(dgl_Point2D *p, dgl_Point2D t);
DGLAPI inline void dgl_translate_point_3D(dgl_Point3D *p, dgl_Point3D t);
DGLAPI void dgl_rotate_point_2D(dgl_Point2D *p, dgl_Real angle);
DGLAPI void dgl_rotate_point_3D(dgl_Point3D *p, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z);

// TODO: This model allows definition of colors for triangles (not interpolation based on vertices)
// It is assumed that model consists of triangles
// TODO: Right now, this type agregates information from various addresses in memory
//       It would be more efficient if it was actually continuous memory
typedef struct{
	dgl_Point3D *vertices;
	int *indices;
	uint32_t *colors;
	size_t vertices_length;
	size_t indices_length;
	size_t colors_length;
}dgl_Simple_Model;

// This is useful since it allows us to operate only on one portion of pixels (for example use dgl_clear just on one portion)
typedef struct{
	uint32_t *pixels;
	size_t width;
	size_t height;
	int stride;
}dgl_Canvas;

#define DGL_GET_PIXEL(canvas, x, y) (canvas).pixels[(y)*(canvas).stride + (x)]
#define DGL_SET_PIXEL(canvas, x, y, v) (canvas).pixels[(y)*(canvas).stride + (x)] = (v)

#define DGL_DEFAULT_FONT_WIDTH		6
#define DGL_DEFAULT_FONT_HEIGHT		7

char dgl_Default_Glyphs[128][DGL_DEFAULT_FONT_HEIGHT][DGL_DEFAULT_FONT_WIDTH] = {
	[' '] = {
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	[','] = {
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0}
	},
	['.'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 0, 0, 0},
		{0, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['0'] = {
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 1, 1, 0},
		{1, 0, 1, 0, 1, 0},
		{1, 1, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['1'] = {
		{0, 0, 1, 0, 0, 0},
		{0, 1, 1, 0, 0, 0},
		{1, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{1, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['2'] = {
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 0, 1, 1, 0, 0},
		{0, 1, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['3'] = {
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['4'] = {
		{0, 0, 0, 1, 0, 0},
		{0, 0, 1, 1, 0, 0},
		{0, 1, 0, 1, 0, 0},
		{1, 0, 0, 1, 0, 0},
		{1, 1, 1, 1, 1, 0},
		{0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['5'] = {
		{1, 1, 1, 1, 1, 0},
		{1, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['6'] = {
		{0, 1, 1, 1, 1, 0},
		{1, 0, 0, 0, 0, 0},
		{1, 0, 1, 1, 0, 0},
		{1, 1, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['7'] = {
		{1, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 1, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['8'] = {
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['9'] = {
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['a'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['b'] = {
		{1, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['c'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['d'] = {
		{0, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['e'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 1, 1, 1, 1, 0},
		{1, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['f'] = {
		{0, 0, 1, 1, 1, 0},
		{0, 0, 1, 0, 0, 0},
		{1, 1, 1, 1, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['g'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0}
	},
	['h'] = {
		{1, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['i'] = {
		{0, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['j'] = {
		{0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 0, 0},
		{0, 0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0, 0},
		{0, 1, 1, 1, 0, 0}
	},
	['k'] = {
		{1, 0, 0, 0, 0, 0},
		{1, 0, 0, 1, 0, 0},
		{1, 0, 1, 0, 0, 0},
		{1, 1, 0, 0, 0, 0},
		{1, 0, 1, 0, 0, 0},
		{1, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['l'] = {
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['m'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 0, 1, 0, 0},
		{1, 0, 1, 0, 1, 0},
		{1, 0, 1, 0, 1, 0},
		{1, 0, 1, 0, 1, 0},
		{1, 0, 1, 0, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['n'] = {
		{0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['o'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['p'] = {
		{0, 0, 0, 0, 0, 0},
		{1, 0, 1, 1, 0, 0},
		{1, 1, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 1, 1, 1, 0, 0},
		{1, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0}
	},
	['q'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 0, 1, 0},
		{1, 0, 0, 1, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 1, 0}
	},
	['r'] = {
		{0, 0, 0, 0, 0, 0},
		{1, 0, 1, 1, 1, 0},
		{1, 1, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['s'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0},
		{1, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 1, 0},
		{1, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['t'] = {
		{0, 0, 1, 0, 0, 0},
		{0, 1, 1, 1, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['u'] = {
		{0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['v'] = {
		{0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 0, 1, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['w'] = {
		{0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 1, 0},
		{1, 0, 1, 0, 1, 0},
		{1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['x'] = {
		{0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 1, 0, 1, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 1, 0, 1, 0, 0},
		{1, 0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0, 0}
	},
	['y'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 1, 0},
		{0, 1, 0, 0, 1, 0},
		{0, 1, 0, 0, 1, 0},
		{0, 0, 1, 1, 1, 0},
		{0, 0, 0, 0, 1, 0},
		{0, 1, 1, 1, 0, 0}
	},
	['z'] = {
		{0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 0},
		{0, 0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0, 0},
		{0, 1, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0}
	}
};


// This is a general font type
typedef struct{
	size_t width, height;	// This must match with glyph mesh dimensions
	char *glyphs;			// This allows us to iterate through 3D matrix character by character
}dgl_Font;

// This is default font provided with library
dgl_Font dgl_Default_Font = {
	.width = DGL_DEFAULT_FONT_WIDTH,
	.height = DGL_DEFAULT_FONT_HEIGHT,
	.glyphs = &dgl_Default_Glyphs[0][0][0]	// We could just assign dgl_Default_Glyphs, but compiler gives warning
};

DGLAPI dgl_Errno dgl_render_ppm(dgl_Canvas *canvas, char *filename);
DGLAPI dgl_Simple_Model *dgl_load_simple_model(char *filename, dgl_Bool has_quads);
DGLAPI int dgl_save_simple_model(dgl_Simple_Model *sm, char *filename);
DGLAPI void dgl_delete_simple_model(dgl_Simple_Model *sm);
DGLAPI dgl_Simple_Model *dgl_cull_reduce_simple_model(dgl_Simple_Model *sm, int stride);

DGLAPI void dgl_clear(dgl_Canvas *canvas, uint32_t color);
DGLAPI void dgl_fill_rect(dgl_Canvas *canvas, int top_left_x, int top_left_y, size_t w, size_t h, uint32_t color);
DGLAPI void dgl_draw_rect(dgl_Canvas *canvas, int top_left_x, int top_left_y, size_t w, size_t h, uint32_t color);
DGLAPI void dgl_fill_circle(dgl_Canvas *canvas, int center_x, int center_y, size_t r, uint32_t color);
DGLAPI void dgl_draw_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t color);
DGLAPI void dgl_fill_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t color);
DGLAPI void dgl_fill_triangle_bary_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t c1, uint32_t c2, uint32_t c3);
DGLAPI void dgl_draw_text(dgl_Canvas *canvas, const char *text, int x, int y, const dgl_Font *font, uint8_t scale, dgl_Bool vertical, uint32_t color);
DGLAPI void dgl_draw_line(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color);
DGLAPI void dgl_draw_line_bresenham(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color);
DGLAPI void dgl_draw_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, dgl_Mat proj_mat, uint32_t color);
DGLAPI void dgl_fill_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, dgl_Mat proj_mat, uint32_t color);

DGLAPI void dgl_sort3(int *a, int *b, int *c);
DGLAPI int dgl_min3(int a, int b, int c);
DGLAPI int dgl_max3(int a, int b, int c);
DGLAPI int dgl_clamp(int v, int left, int right);
DGLAPI uint32_t dgl_blend(uint32_t f, uint32_t b);
DGLAPI uint32_t dgl_bary_color(float s, float t, uint32_t c1, uint32_t c2, uint32_t c3);

DGLAPI void dgl_scale_point_2D(dgl_Point2D *p, int scale);
DGLAPI void dgl_scale_point_3D(dgl_Point3D *p, dgl_Real scale);
DGLAPI void dgl_translate_point_2D(dgl_Point2D *p, dgl_Point2D t);
DGLAPI void dgl_translate_point_3D(dgl_Point3D *p, dgl_Point3D t);
DGLAPI void dgl_rotate_point_2D(dgl_Point2D *p, dgl_Real angle);
DGLAPI void dgl_rotate_point_3D(dgl_Point3D *p, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z);

DGLAPI void dgl_draw_simple_model(dgl_Canvas *canvas, const dgl_Simple_Model *sm, dgl_Mat proj_mat);
DGLAPI void dgl_draw_simple_model_mesh(dgl_Canvas *canvas, const dgl_Simple_Model *sm, dgl_Mat proj_mat);
DGLAPI void dgl_scale_simple_model(dgl_Simple_Model *sm, dgl_Real scale);
DGLAPI void dgl_translate_simple_model(dgl_Simple_Model *sm, dgl_Point3D translation);
DGLAPI void dgl_rotate_simple_model(dgl_Simple_Model *sm, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z);

#endif // DGL_H

#ifdef DGL_IMPLEMENTATION

// TODO: We should use this to allow the choice of using SIMD
// Of course, on top of this, whenever we are using some simd instruction set, we would need to check if it
// is supported
#ifdef DGL_USE_SIMD
#include <immintrin.h>
typedef union { __m128 v; float s[4]; } dgl_M128;
typedef union { __m128i v; int s[4]; } dgl_M128i;
#endif

DGLAPI void dgl_scale_point_2D(dgl_Point2D *p, int scale){ p->x *= scale; p->y *= scale; }
DGLAPI void dgl_scale_point_3D(dgl_Point3D *p, dgl_Real scale){ p->x *= scale; p->y *= scale; p->z *= scale; }
DGLAPI void dgl_translate_point_2D(dgl_Point2D *p, dgl_Point2D t){ p->x += t.x; p->y += t.y; }
DGLAPI void dgl_translate_point_3D(dgl_Point3D *p, dgl_Point3D t){ p->x += t.x; p->y += t.y; p->z += t.z; }

// TODO: Rotation like this is of course not the best way but will do for now
DGLAPI void dgl_rotate_point_2D(dgl_Point2D *p, dgl_Real angle){
	dgl_Point2D tp = (*p);
	p->x = cos(angle)*tp.x - sin(angle)*tp.y;
	p->y = sin(angle)*tp.x + cos(angle)*tp.y;
}
DGLAPI void dgl_rotate_point_3D(dgl_Point3D *p, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z){
	dgl_Real sin_x = sin(angle_x);
	dgl_Real cos_x = cos(angle_x);
	dgl_Real sin_y = sin(angle_y);
	dgl_Real cos_y = cos(angle_y);
	dgl_Real sin_z = sin(angle_z);
	dgl_Real cos_z = cos(angle_z);
	
	dgl_Point3D tp = (*p);
	p->y = cos_x*tp.y - sin_x*tp.z;
	p->z = sin_x*tp.y + cos_x*tp.z;
	tp = (*p);
	p->x =  cos_y*tp.x + sin_y*tp.z;
	p->z = -sin_y*tp.x + cos_y*tp.z;
	tp = (*p);
	p->x = cos_z*tp.x - sin_z*tp.y;
	p->y = sin_z*tp.x + cos_z*tp.y;
}

// TODO: We can also write our own functionality in the future (strtok is just dumb)
// We can load models that contain triangles and quads, but not arbitrary faces (no need for now)
#include <string.h>
DGLAPI dgl_Simple_Model *dgl_load_simple_model(char *filename, dgl_Bool has_quads){
	FILE *file = fopen(filename, "r");
	if(file == NULL) return NULL;

	dgl_Simple_Model *sm = (dgl_Simple_Model *)calloc(1, sizeof(dgl_Simple_Model));
	// Just make space for one color that will be used and set it to white by default
	sm->colors_length = 1;
	sm->colors = (uint32_t*)malloc(sizeof(uint32_t));
	sm->colors[0] = DGL_WHITE;

	char line[100];
	
	// Go through file and see how many vertices and faces are there
	while(fgets(line, 100, file)){
		if(line[0] == 'v' && line[1] == ' ') ++(sm->vertices_length);
		else if(line[0] == 'f') sm->indices_length += (3 + has_quads*3);
	}

	// Allocate space for vertices and indices in simple model
	sm->vertices = (dgl_Point3D *)malloc(sizeof(dgl_Point3D) * sm->vertices_length);
	sm->indices = (int *)malloc(sizeof(int) * sm->indices_length);
	
	rewind(file);

   	// We are just looking for vertices and indices(faces)
	int vertex_index = 0;
	int index_index = 0;  // In the end, this also tells us how many indices we actually have
	char *f1, *f2, *f3, *f4;
	while(fgets(line, 100, file)){
		if(line[0] == 'v' && line[1] == ' '){
			// Take out 'v'
			strtok(line, " ");

			// For three vertex coordinates
			sm->vertices[vertex_index].x = atof(strtok(NULL, " "));
			sm->vertices[vertex_index].y = atof(strtok(NULL, " "));
			sm->vertices[vertex_index].z = atof(strtok(NULL, " "));

			++vertex_index;
		}
		else if(line[0] == 'f'){
   			// Take out 'f'
			strtok(line, " ");

			// Temporarily save three parts of 'f' line
			f1 = strtok(NULL, " ");
			f2 = strtok(NULL, " ");
			f3 = strtok(NULL, " ");
			// For potential quad
			f4 = strtok(NULL, " ");

			// -1 because .obj file stores indices starting from 1
   			sm->indices[index_index++] = atoi(strtok(f1, "/")) - 1;
		   	sm->indices[index_index++] = atoi(strtok(f2, "/")) - 1;
   			sm->indices[index_index++] = atoi(strtok(f3, "/")) - 1;

			// If face is a quad then form additional triangle
			if(has_quads && f4 != NULL && f4[0] != '\n'){
				sm->indices[index_index++] = atoi(f1) - 1;
				sm->indices[index_index++] = atoi(f3) - 1;
				sm->indices[index_index++] = atoi(strtok(f4, "/")) - 1;
			}
		}
	}
	
	// Reallocate indices by using malloc and memcpy since we know exact size (realloc is too slow for this)
	int *temp = (int *)calloc(index_index, sizeof(int));
	memcpy(temp, sm->indices, sizeof(int) * index_index);
	free(sm->indices);
	sm->indices_length = index_index;
	sm->indices = temp;
	
	fclose(file);
	return sm;
}

DGLAPI void dgl_delete_simple_model(dgl_Simple_Model *sm){
	free(sm->vertices);
	free(sm->indices);
	free(sm->colors);
	free(sm);
}

// This is a very forceful way to reduce model complexity (by making it disconnected)
// The reason it works is that removal of triangles will roughly be uniform because of how
// locally close triangles are defined locally close in files
DGLAPI dgl_Simple_Model *dgl_cull_reduce_simple_model(dgl_Simple_Model *sm, int stride){
	int temp_length = sm->indices_length/stride;
	int *temp = (int *)malloc(sizeof(int)*temp_length);

	int temp_index = 0;
	for(size_t i = 0; i < sm->indices_length/3 && temp_index < temp_length - 2; ++i){
		if(i % stride == 0){
			temp[temp_index++] = sm->indices[i*3];
			temp[temp_index++] = sm->indices[i*3 + 1];
			temp[temp_index++] = sm->indices[i*3 + 2];
		}
	}

	free(sm->indices);
	sm->indices_length /= stride;
	sm->indices = temp;

	return sm;
}

// TODO: Create function that will take model and reduce number of triangles but still keep
// the mesh connected (decimation algorithm)

DGLAPI int dgl_save_simple_model(dgl_Simple_Model *sm, char *filename){
	int DEFER_RESULT = 0;
	
	FILE *file = fopen(filename, "w");
	if(file == NULL) return -1;

	fprintf(file, "# Dgl Simple Model [vertices: %d, indices: %d]\n", sm->vertices_length, sm->indices_length);

	for(size_t i = 0; i < sm->vertices_length; ++i){
		fprintf(file, "v %lf %lf %lf\n", sm->vertices[i].x, sm->vertices[i].y, sm->vertices[i].z);
		if(ferror(file)) DEFER_RETURN(errno);
	}

	for(size_t i = 0; i < sm->indices_length; i += 3){
		// Add 1 because .obj uses indexing from 1
		fprintf(file, "f %d %d %d\n", sm->indices[i] + 1, sm->indices[i+1] + 1, sm->indices[i+2] + 1);
		if(ferror(file)) DEFER_RETURN(errno);
	}

DEFER:
	fclose(file);
	return DEFER_RESULT;
}

// TODO: Adapt this code, so it fits within specific mode of operation (like DGL_TARGET_WINDOWS)
DGLAPI dgl_Errno dgl_render_ppm(dgl_Canvas *canvas, char *filename){
	int DEFER_RESULT = 0;
	
	FILE *file = fopen(filename, "wb");
	if(file == NULL) return -1;
	
	fprintf(file, "P6\n%d %d\n255\n", (unsigned int)canvas->width, (unsigned int)canvas->height);
	if(ferror(file)) DEFER_RETURN(errno);
	
	for(size_t i = 0; i < canvas->width*canvas->height; ++i){
		fwrite(&canvas->pixels[i], sizeof(char), 3, file);
		if(ferror(file)) DEFER_RETURN(errno);
	}
	
DEFER:
	fclose(file);
	return DEFER_RESULT;
}
	
DGLAPI void dgl_clear(dgl_Canvas *canvas, uint32_t color){
	for(size_t y = 0; y < canvas->height; ++y)
		for(size_t x = 0; x < canvas->width; ++x)
			DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
}

DGLAPI void dgl_draw_text(dgl_Canvas *canvas, const char *text, int x, int y, const dgl_Font *font, uint8_t scale, dgl_Bool vertical, uint32_t color){
	// This is because we allow vertical and horizontal drawing
	int dy = ((vertical == true) ? font->height*scale : 0);
	int dx = ((vertical == false) ? font->width*scale : 0);
	
	// For condition is like this because this way we don't need to use 'strlen' and thus depend on 'string.h'
	for(unsigned int i = 0; text[i] != '\0'; ++i){
		// Top left corner of i-th character
		// - on cy because of transformation of coordinate systems by dgl_fill_rect
		int cy = y - i*dy;
		int cx = x + i*dx;
		
		const char *glyph = &font->glyphs[text[i]*sizeof(char)*font->width*font->height];
		
		// Iterate through glyph mesh
		for(unsigned int gy = 0; gy < font->height; ++gy){
			for(unsigned int gx = 0; gx < font->width; ++gx){
				
				// Top-left corner of rectangle of current glyph pixel (rectangle because it allows us to scale)
				// - on ry because of transformation of coordinate systems by dgl_fill_rect
				int ry = cy - gy*scale;
				int rx = cx + gx*scale;
				
				dgl_fill_rect(canvas, rx, ry, scale, scale, glyph[gy*font->width + gx] * color);
			}
		}
	}
}

// TODO: Maybe it is possible to optimize this further (important because it is also used to draw letters)
DGLAPI void dgl_fill_rect(dgl_Canvas *canvas, int x0, int y0, size_t w, size_t h, uint32_t color){
	x0 = DGL_TRANSFORM_COORDINATES_X(x0);
	y0 = DGL_TRANSFORM_COORDINATES_Y(y0, canvas->height);
	
	for(int y = y0; y < y0 + (int)h; ++y)
		if(y >= 0 && y < (int)canvas->height)
			for(int x = x0; x < x0 + (int)w; ++x)
				if(x >= 0 && x < (int)canvas->width)
					DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
}

DGLAPI void dgl_draw_rect(dgl_Canvas *canvas, int x0, int y0, size_t w, size_t h, uint32_t color){
	x0 = DGL_TRANSFORM_COORDINATES_X(x0);
	y0 = DGL_TRANSFORM_COORDINATES_Y(y0, canvas->height);

	dgl_draw_line_bresenham(canvas, x0, y0, x0 + w, y0, color);
	dgl_draw_line_bresenham(canvas, x0, y0, x0, y0 + h, color);
	dgl_draw_line_bresenham(canvas, x0 + w, y0 + h, x0, y0 + h, color);
	dgl_draw_line_bresenham(canvas, x0 + w, y0 + h, x0 + w, y0, color);
}

// TODO: Maybe it is possible to speed this up by iterating only on the portion of circle because of its symmetry
//		 (for example iterating over bounding box of quarter of the circle)
DGLAPI void dgl_fill_circle(dgl_Canvas *canvas, int center_x, int center_y, size_t r, uint32_t color){
	center_x = DGL_TRANSFORM_COORDINATES_X(center_x);
	center_y = DGL_TRANSFORM_COORDINATES_Y(center_y, canvas->height);
	
	uint32_t orig_color = color;
	
	int top_left_y = center_y - r;
	int top_left_x = center_x - r;
	for(size_t y = 0; y < 2*r; ++y)
		if(!(top_left_y + (int)y < 0) && !(top_left_y + (int)y >= (int)canvas->height))
			for(size_t x = 0; x < 2*r; ++x)
				if(!(top_left_x + (int)x < 0) && !(top_left_x + (int)x >= (int)canvas->width)){
					// For now we are just doing anti-aliasing with uniform subpixels whose
					// values we measure at their centers
					
					// INTEGER ARITHMETIC
					// Transformation to integer-only arithmetic
					// Y = y + (0.5 + sy)/DGL_AALIAS_DEG;
					// X = x + (0.5 + sx)/DGL_AALIAS_DEG;
					// Y = (2*y*DGL_AALIAS_DEG + 1 + 2*sy)/(2*DGL_AALIAS_DEG)
					// X = (2*x*DGL_AALIAS_DEG + 1 + 2*sx)/(2*DGL_AALIAS_DEG)
					// Y = A/C
					// X = B/C
					// Transformation of condition
					// ((A - r*C)*(A - r*C) + (B - r*C)*(B - r*C))/(C*C) <= r*r
					// (A - r*C)*(A - r*C) + (B - r*C)*(B - r*C) <= r*r*C*C
					// A*A - 2*A*r*C + r*r*C*C + B*B - 2*B*r*C + r*r*C*C <= r*r*C*C
					// A*A + B*B - 2*r*C*(A + B) + r*r*C*C <= 0
					// A*A + B*B + r*r*C*C <= 2*r*C*(A + B)

					// x and y length(distances) from the center coords for the current pixel
					int pxl = top_left_x + x - center_x;
					int pyl = top_left_y + y - center_y;
					unsigned int left_side = pxl*pxl + pyl*pyl;
					
					// If the pixel falls within portion of circle that does not need antialiasing
					if(left_side <= (r-1)*(r-1)){
						DGL_SET_PIXEL(*canvas, top_left_x + x, top_left_y + y, dgl_blend(color, DGL_GET_PIXEL(*canvas, top_left_x + x, top_left_y + y)));
					}
					else if(left_side <= (r+1)*(r+1)){
						int count = 0;
						for(int sy = 0; sy < DGL_AALIAS_DEG; ++sy){
							for(int sx = 0; sx < DGL_AALIAS_DEG; ++sx){
								uint32_t A = 2*y*DGL_AALIAS_DEG + 1 + 2*sy;
								uint32_t B = 2*x*DGL_AALIAS_DEG + 1 + 2*sx;
								uint32_t C = 2*DGL_AALIAS_DEG;
								uint32_t rC = r*C;
								
								if(A*A + B*B + rC*rC <= 2*rC*(A + B)) ++count;
							}
						}
						
						uint8_t new_alpha = count*(color >> 24)/(DGL_AALIAS_DEG*DGL_AALIAS_DEG);
						color &= 0x00FFFFFF;
						color |= (new_alpha << 24);
						
						DGL_SET_PIXEL(*canvas, top_left_x + x, top_left_y + y, dgl_blend(color, DGL_GET_PIXEL(*canvas, top_left_x + x, top_left_y + y)));
					
						color = orig_color;
					}
				}
}

DGLAPI void dgl_draw_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t color){
	// We don't need to transform coordinates because dgl_draw_line already does it

	dgl_draw_line_bresenham(canvas, t.v[0].x, t.v[0].y, t.v[1].x, t.v[1].y, color);
	dgl_draw_line_bresenham(canvas, t.v[0].x, t.v[0].y, t.v[2].x, t.v[2].y, color);
	dgl_draw_line_bresenham(canvas, t.v[1].x, t.v[1].y, t.v[2].x, t.v[2].y, color);
}

// TODO: Move vertex color information into triangle
// TODO: Feels like things can be optimized here, but it's not primary now
// TODO: See how to adapt this code to SIMD
// @Test It is roughly two times slower that other triangle filling function when using single color
DGLAPI void dgl_fill_triangle_bary_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t c1, uint32_t c2, uint32_t c3){
   	int x0 = DGL_TRANSFORM_COORDINATES_X(t.v[0].x);
	int y0 = DGL_TRANSFORM_COORDINATES_Y(t.v[0].y, canvas->height);
	int x1 = DGL_TRANSFORM_COORDINATES_X(t.v[1].x);
	int y1 = DGL_TRANSFORM_COORDINATES_Y(t.v[1].y, canvas->height);
	int x2 = DGL_TRANSFORM_COORDINATES_X(t.v[2].x);
	int y2 = DGL_TRANSFORM_COORDINATES_Y(t.v[2].y, canvas->height);

	// Find bounding box
	int bl = dgl_min3(x0, x1, x2);
	int br = dgl_max3(x0, x1, x2);
	int bt = dgl_max3(y0, y1, y2);
	int bb = dgl_min3(y0, y1, y2);

	float denom = (y1-y2)*(x0-x2) + (x2-x1)*(y0-y2);
	
	// Fill based on barycentric coords
	for(int y = bb; y <= bt; ++y)
		if(y > 0 && y < (int)canvas->height)
			for(int x = bl; x <= br; ++x)
				if(x > 0 && x < (int)canvas->width){
					float l1 = ((y1-y2)*(x-x2) + (x2-x1)*(y-y2)) / denom;
					float l2 = ((y2-y0)*(x-x2) + (x0-x2)*(y-y2)) / denom;
					//float l3 = 1 - l1 - l2;

					if(0 <= l1 && l1 <= 1 && 0 <= l2 && l2 <= 1 && (l1+l2) <= 1)
						DGL_SET_PIXEL(*canvas, x, y, dgl_blend(dgl_bary_color(l1, l2, c1, c2, c3), DGL_GET_PIXEL(*canvas, x, y)));
				}
}

// TODO: Find out the reason for weird shaded triangles apearing when rendering model with filled triangles
DGLAPI void dgl_fill_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t color){
	// We don't iterate through bounding square, but directly through triangle points
	// This way, we don't need to check whether point is in triangle and we loop through half as many points
	
	// We force triangle points to obey:
	// 1. (x0, y0) is the top point
	// 2. (x2, y2) is the bottom point
	
	int x0 = DGL_TRANSFORM_COORDINATES_X(t.v[0].x);
	int y0 = DGL_TRANSFORM_COORDINATES_Y(t.v[0].y, canvas->height);
	int x1 = DGL_TRANSFORM_COORDINATES_X(t.v[1].x);
	int y1 = DGL_TRANSFORM_COORDINATES_Y(t.v[1].y, canvas->height);
	int x2 = DGL_TRANSFORM_COORDINATES_X(t.v[2].x);
	int y2 = DGL_TRANSFORM_COORDINATES_Y(t.v[2].y, canvas->height);
	
	// Completely degenerate horizontal line triangle
	if(y0 == y1 && y0 == y2){
		// Horizontal line out of the screen
		if(y0 < 0 || y0 >= (int)canvas->height) return;
		
		// Force order x0 <= x1 <= x2
		if(x0 > x1) DGL_SWAP(x0, x1, int);
		if(x1 > x2) DGL_SWAP(x1, x2, int);
		if(x0 > x1) DGL_SWAP(x0, x1, int);
		
		// Draw horizontal line
		for(int x = x0; x <= x2; ++x)
			if(x >= 0 && x < (int)canvas->width)
				DGL_SET_PIXEL(*canvas, x, y0, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y0)));
		
		return;
	}
	
	// Completely degenerate vertical line triangle
	if(x0 == x1 && x0 == x2){
		// Vertical line out of the screen
		if(x0 < 0 || x0 >= (int)canvas->width) return;
		
		// Force order y0 <= y1 <= y2
		if(y0 > y1) DGL_SWAP(y0, y1, int);
		if(y1 > y2) DGL_SWAP(y1, y2, int);
		if(y0 > y1) DGL_SWAP(y0, y1, int);
		
		// Draw vertical line
		for(int y = y0; y <= y2; ++y)
			if(y >= 0 && y < (int)canvas->height)
				DGL_SET_PIXEL(*canvas, x0, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x0, y)));
		
		return;
	}
	
	// Since triangle primitive is frequent, we won't use sort3, but instead implement it directly
	// Force first two rules
	if(y0 > y1){
		DGL_SWAP(y0, y1, int);
		DGL_SWAP(x0, x1, int);
	}
	
	if(y1 > y2){
		DGL_SWAP(y1, y2, int);
		DGL_SWAP(x1, x2, int);
	}
	
	if(y0 > y1){
		DGL_SWAP(y0, y1, int);
		DGL_SWAP(x0, x1, int);
	}
	
	int dy10 = y1 - y0;
	int dx10 = x1 - x0;
	int dy20 = y2 - y0;
	int dx20 = x2 - x0;
	int dy21 = y2 - y1;
	int dx21 = x2 - x1;
	
	// Upper part of the triangle (bounded from below by horizontal line from (x1,y1) to line ((x0,y0),(x2,y2))

	if(dy10 != 0 && dy20 != 0){
		if(x1 > x0){
			DGL_SWAP(dy10, dy20, int);
			DGL_SWAP(dx10, dx20, int);
		}
	
		for(int y = y0; y < y1; ++y)
			if(y >= 0 && y < (int)canvas->height)
				for(int x = dx10*(y-y0)/dy10 + x0; x <= dx20*(y-y0)/dy20 + x0; ++x)
					if(x >= 0 && x < (int)canvas->width)
						DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
		
		if(x1 > x0){
			DGL_SWAP(dy10, dy20, int);
			DGL_SWAP(dx10, dx20, int);
		}
	}
	
	// Lower part of the triangle (bounded from above by horizontal line from (x1,y1) to line ((x0,y0),(x2,y2))
	if(dy21 != 0 && dy20 != 0){
		int y1_temp = y1;
		if(x1 > x0){
			DGL_SWAP(dy21, dy20, int);
			DGL_SWAP(dx21, dx20, int);
			DGL_SWAP(y1, y0, int);
			DGL_SWAP(x1, x0, int);
		}
	
		for(int y = y1_temp; y < y2; ++y)
			if(y >= 0 && y < (int)canvas->height)
				for(int x = dx21*(y-y1)/dy21 + x1; x <= dx20*(y-y0)/dy20 + x0; ++x) 
					if(x >= 0 && x < (int)canvas->width)
						DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
	}
}

// This is cheap but not great way to draw line since it struggles in cases of steep slopes
// Still, it is left here and can be used
DGLAPI void dgl_draw_line(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color){
	x0 = DGL_TRANSFORM_COORDINATES_X(x0);
	y0 = DGL_TRANSFORM_COORDINATES_Y(y0, canvas->height);
	x1 = DGL_TRANSFORM_COORDINATES_X(x1);
	y1 = DGL_TRANSFORM_COORDINATES_Y(y1, canvas->height);

	// Force (x0,y0) to be the point with smaller x coordinate
	if(x0 > x1){
		DGL_SWAP(x0, x1, int);
		DGL_SWAP(y0, y1, int);
	}
	
	if(x1 - x0 != 0){
		for(int x = x0; x <= x1; ++x){
			if(x >= 0 && x < (int)canvas->width){
				int y = ((y1 - y0)*x - (y1 - y0)*x0 + y0*(x1 - x0))/(x1 - x0);
				if(y >= 0 && y < (int)canvas->height)
					DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
			}
		}
	}
	else{
		if(x0 < 0 || x0 >= (int)canvas->width) return;
		
		for(int y = y0; y <= y1; ++y)
			if(y >= 0 && y < (int)canvas->height)
				DGL_SET_PIXEL(*canvas, x0, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x0, y)));
	}
}

// TODO: It might be possible to not duplicate code in if and else parts because of symmetry
// TODO: Maybe add antialiasing capability
DGLAPI void dgl_draw_line_bresenham(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color){
	x0 = DGL_TRANSFORM_COORDINATES_X(x0);
	y0 = DGL_TRANSFORM_COORDINATES_Y(y0, canvas->height);
	x1 = DGL_TRANSFORM_COORDINATES_X(x1);
	y1 = DGL_TRANSFORM_COORDINATES_Y(y1, canvas->height);
	
	// y = mx + c, m = dy/dx
	// (x_k, y_k)         <-- current pixel (lower left corner for standard coord system)
	// (x_(k+1), y_(k+1)) <-- next pixel (lower left corner for standard coord system)

	// CASE 1: m <= 1
	// x is always incrementing so x_(k+1) is always (x_k + 1) in this case

	// d1 = y - (y_k + 1/2)
	//    = m(x_(k+1)) + c - (y_k + 1/2)
	//    = m(x_k + 1) + c - (y_k + 1/2)
	
	// d2 = (y_(k+1) + 1/2) - y
	//    = (y_k + 1) + 1/2 - m(x_(k+1)) - c
	//    = (y_k + 1) + 1/2 - m(x_k + 1) - c
	
	// d1-d2 = 2m(x_k + 1) -2y_k + 2(c-1)
	//       = 2(dy/dx)(x_k + 1) -2y_k + 2(c-1)
	// dx(d1-d2) = 2dy(x_k + 1) -2dxy_k + 2dx(c-1)
	//           = 2dyx_k - 2dxy_k + (2dy - 2dx + 2dxc)
	//           = 2dyx_k - 2dxy_k + C
	
	// D_k = dx(d1-d2) = 2dyx_k - 2dxy_k + C <-- Decision parameter when we are at step k
	// D_(k+1) = 2dyx_(k+1) - 2dxy_(k+1) + C <-- Decision for next step
	// D_(k+1)-D_k =  2dy(x_(k+1) - x_k) - 2dx(y_(k+1) - y_k)
	// D_(k+1) = D_k + 2dy(x_(k+1) - x_k) - 2dx(y_(k+1) - y_k) <-- Next decision in terms of previous one
	//         = D_k + 2dy(x_k - x_k + 1) - 2dx(y_(k+1) - y_k)
	//         = D_k + 2dy - 2dx(y_(k+1) - y_k)

	// If D_k < 0: (d1-d2)<0 => d1<d2 => y_(k+1)=y_k     => D_(k+1) = D_k + 2dy       (choose right)
	// If D_k > 0: (d1-d2)>0 => d1>d2 => y_(k+1)=y_k + 1 => D_(k+1) = D_k + 2dy - 2dx (choose top-right)
	// Now we have all the decisions if we have initial one
	// From formula for D_k: D_0 = C => D_0 = 2dy - 2dx + 2dxc = 2dy + 2dx(c-1)
	// c = y_k - mx_k = y_k - (dy/dx)x_k => D_0 = 2dy + 2dx(y_k - (dy/dx)x_k - 1)
	// D_0 = 2dy + 2(dxy_k - dyx_k - dx) => D_0 = 2dy - 2dx
	

	// CASE 2: m > 1
	// We can make derivation for this case by expressing line as function x(y)
	// This is because it allows us to reduce this to the same thinking used in previous case
	// Then we would do derivation all over again within this context
	// Instead of doing this we can exploit symmetry
	// Line in this case has corresponding line in (CASE 1) that is it's reflection in line y=x
	// This means that by switching x and y and altering everything that depends on them accordingly
	// we can use everything that we already derived
	// x -> y, y -> x, dx -> dy, dy -> dx             <-- base changes
	// m = (dy/dx) -> (dx/dy) = (1/m), c -> -(1/m)c   <-- changes that follow
	// This means that the formulas will stay valid if we just switch (x and y) and (dx and dy)
	// Based on this, we have formulas for this case:

	// D_k = 2dxy_k - 2dyx_k + C'
	// D_(k+1) = D_k + 2dx - 2dy(x_(k+1)) - x_k)
	// If D_k < 0: D_(k+1) = D_k + 2dx
	// If D_k > 0: D_(k+1) = D_k + 2dx - 2dy
	// D_0 = 2dx - 2dy

	// If line is in 1 or -1 octant
	if(abs(y1-y0) < abs(x1-x0)){
		if(x0 > x1){
			DGL_SWAP(x0, x1, int);
			DGL_SWAP(y0, y1, int);
		}
	
		int dx = x1 - x0;
		int dy = y1 - y0;
		
		int y = y0;
		int y_increment = 1;

		// If line is in -1 octant
		if(dy < 0){
			// Essentialy make a reflection so that decision behaves just like for the 1 octant
			// with the difference of moving down
			y_increment = -1;
			dy = -dy;
		}
	
		// In my derivation I get -2dx because (x_k,y_k) doesn't lie in the middle but in lower left corner
		int D = 2*dy - dx;
	
		for(int x = x0; x <= x1; ++x){
			if(x >= 0 && x < (int)canvas->width && y >= 0 && y < (int)canvas->height)
				DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
			
			if(D > 0){
				y += y_increment;
				D -= 2*dx;
			}
			D += 2*dy;
		}
	}
	// If line is in 2 or -2 octant
	else{
		if(y0 > y1){
			DGL_SWAP(x0, x1, int);
			DGL_SWAP(y0, y1, int);
		}
	
		int dx = x1 - x0;
		int dy = y1 - y0;
		
		int x = x0;
		int x_increment = 1;

		// If line is in -1 octant
		if(dx < 0){
			// Essentialy make a reflection so that decision behaves just like for the 1 octant
			// with the difference of moving down
			x_increment = -1;
			dx = -dx;
		}
	
		// In my derivation I get -2dx because (x_k,y_k) doesn't lie in the middle but in lower left corner
		int D = 2*dx - dy;
	
		for(int y = y0; y <= y1; ++y){
			if(x >= 0 && x < (int)canvas->width && y >= 0 && y < (int)canvas->height)
				DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
			
			if(D > 0){
				x += x_increment;
				D -= 2*dy;
			}
			D += 2*dx;
		}
	}
}

// TODO: For now, we are just doing orthogonal projection because it requires minimal calculation. Move to perspective projection
// TODO: Maybe create projection screen type or something similar that will hold focus point and direction the screen is facing
DGLAPI void dgl_draw_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, dgl_Mat proj_mat, uint32_t color){
    dgl_Triangle2D pt;
	//dgl_Mat v = dgl_mat_alloc(4, 1);
	
	// Perspective projection (division by 'w' constructs NDC space (we don't need that?))
	// We should also add clipping check after vertices have been transformed?
	/*
	v.cells[0] = t.v[0].x;
	v.cells[1] = t.v[0].y;
	v.cells[2] = t.v[0].z;
	v.cells[3] = 1;
	
	v = dgl_mat_mul(proj_mat, v); 
	pt.v[0].x = v.cells[0];
	pt.v[0].y = v.cells[1];

	v.cells[0] = t.v[1].x;
	v.cells[1] = t.v[1].y;
	v.cells[2] = t.v[1].z;
	v.cells[3] = 1;
	
	v = dgl_mat_mul(proj_mat, v); 
	pt.v[1].x = v.cells[0];
	pt.v[1].y = v.cells[1];

	v.cells[0] = t.v[2].x;
	v.cells[1] = t.v[2].y;
	v.cells[2] = t.v[2].z;
	v.cells[3] = 1;
	
	v = dgl_mat_mul(proj_mat, v); 
	pt.v[2].x = v.cells[0];
	pt.v[2].y = v.cells[1];
	*/
	// Orthogonal projection
	pt.v[0].x = t.v[0].x;
	pt.v[0].y = t.v[0].y;
	pt.v[1].x = t.v[1].x;
	pt.v[1].y = t.v[1].y;
	pt.v[2].x = t.v[2].x;
	pt.v[2].y = t.v[2].y;
	dgl_draw_triangle_2D(canvas, pt, color);

	//dgl_mat_free(v);
}

DGLAPI void dgl_fill_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, dgl_Mat proj_mat, uint32_t color){
	// Perspective projection
	
	// Orthogonal projection (seems to work)
	dgl_Triangle2D pt;
	pt.v[0].x = t.v[0].x;
	pt.v[0].y = t.v[0].y;
	pt.v[1].x = t.v[1].x;
	pt.v[1].y = t.v[1].y;
	pt.v[2].x = t.v[2].x;
	pt.v[2].y = t.v[2].y;
	//dgl_fill_triangle_2D(canvas, pt, color);
	dgl_fill_triangle_bary_2D(canvas, pt, DGL_RED, DGL_GREEN, DGL_BLUE);
}

// Currently, there is no checking which triangles are further away
// Colors will be looped, so we don't need to provide as many colors as there are triangles
DGLAPI void dgl_draw_simple_model(dgl_Canvas *canvas, const dgl_Simple_Model *sm, dgl_Mat proj_mat){
	// TODO: Add some simple differentiation between closer and further triangles of the model
	dgl_Triangle3D t;
	
	for(size_t i = 0; i < sm->indices_length/3; ++i){
		for(size_t j = 0; j < 3; ++j)
			t.v[j] = sm->vertices[sm->indices[i*3 + j]];
		
		dgl_fill_triangle_3D(canvas, t, proj_mat, sm->colors[i % sm->colors_length]);
	}
}

// TODO: Make this more efficient by doing the projection of all points and then rendering
// 2D triangles instead of iterating over 3D triangles and rendering them.
DGLAPI void dgl_draw_simple_model_mesh(dgl_Canvas *canvas, const dgl_Simple_Model *sm, dgl_Mat proj_mat){
	dgl_Triangle3D t;

	for(size_t i = 0; i < sm->indices_length/3; ++i){
		for(size_t j = 0; j < 3; ++j)
			t.v[j] = sm->vertices[sm->indices[i*3 + j]];
		
		dgl_draw_triangle_3D(canvas, t, proj_mat, sm->colors[i % sm->colors_length]);
	}
}

DGLAPI void dgl_scale_simple_model(dgl_Simple_Model *sm, dgl_Real scale){
	for(size_t i = 0; i < sm->vertices_length; ++i)
		dgl_scale_point_3D(&(sm->vertices[i]), scale);
}

DGLAPI void dgl_translate_simple_model(dgl_Simple_Model *sm, dgl_Point3D translation){
	for(size_t i = 0; i < sm->vertices_length; ++i)
		dgl_translate_point_3D(&(sm->vertices[i]), translation);
}

DGLAPI void dgl_rotate_simple_model(dgl_Simple_Model *sm, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z){
	for(size_t i = 0; i < sm->vertices_length; ++i)
		dgl_rotate_point_3D(&(sm->vertices[i]), angle_x, angle_y, angle_z);
}

#ifdef DGL_USE_SIMD
DGLAPI __m128i dgl_mm_mul_epi32(__m128i a, __m128i b){
	__m128i a13    = _mm_shuffle_epi32(a, 0xF5);          // (-,a3,-,a1)
	__m128i b13    = _mm_shuffle_epi32(b, 0xF5);          // (-,b3,-,b1)
	__m128i prod02 = _mm_mul_epu32(a, b);                 // (-,a2*b2,-,a0*b0)
	__m128i prod13 = _mm_mul_epu32(a13, b13);             // (-,a3*b3,-,a1*b1)
	__m128i prod01 = _mm_unpacklo_epi32(prod02,prod13);   // (-,-,a1*b1,a0*b0) 
	__m128i prod23 = _mm_unpackhi_epi32(prod02,prod13);   // (-,-,a3*b3,a2*b2) 
	__m128i prod   = _mm_unpacklo_epi64(prod01,prod23);   // (ab3,ab2,ab1,ab0)
	return prod;
}
#endif

// TODO: It might be possible to speed this up further by expanding multiples of 255
// as series, measuring the error, and adjusting result by a constant factor so that
// we don't lose information

unsigned int vta1[4] __attribute__((aligned(16)));
unsigned int vta2[4] __attribute__((aligned(16)));
unsigned int vta3[4] __attribute__((aligned(16)));
unsigned int vta4[4] __attribute__((aligned(16)));
DGLAPI uint32_t dgl_blend(uint32_t f, uint32_t b){
	// These were used in dgl_Real arithmetic
	// uint32_t alpha 	= (uint32_t)((a1    +    a2*(1-a1))*255);
	// uint32_t red 	= (uint32_t)((a1*r1 + r2*a2*(1-a1))*255);
	// uint32_t green 	= (uint32_t)((a1*g1 + g2*a2*(1-a1))*255);
	// uint32_t blue 	= (uint32_t)((a1*b1 + b2*a2*(1-a1))*255);

	// ONLY WITH INTEGER ARITHMETIC
	uint8_t a1 = ((f >> 24) & 0xFF);
	uint8_t a2 = ((b >> 24) & 0xFF);
		
	uint8_t r1 = (f & 0xFF);
	uint8_t r2 = (b & 0xFF);
		
	uint8_t g1 = ((f >> 8) & 0xFF);
	uint8_t g2 = ((b >> 8) & 0xFF);
		
	uint8_t b1 = ((f >> 16) & 0xFF);
	uint8_t b2 = ((b >> 16) & 0xFF);

	// TODO: Check if it is faster to precompute these shared parts
	// (maybe not if the compiler does is silently)
	// uint16_t t1 = 255*a1;
	// uint16_t t2 = (255-a1)*a2;

	// Instead of dividing by 65025 or 255, we accept the error and approximate it with shift
	// I feel like error can be adjusted by a constant factor so that we still cover the whole color range
	// after this step
	uint8_t red   = (255*a1*r1 + (255-a1)*a2*r2) >> 16;
	uint8_t green = (255*a1*g1 + (255-a1)*a2*g2) >> 16;
	uint8_t blue  = (255*a1*b1 + (255-a1)*a2*b2) >> 16;
	uint8_t alpha = (255*a1    + (255-a1)*a2   ) >> 8;
	  	
	return (alpha << 24) | (blue << 16) | (green << 8) | red;
}

DGLAPI void dgl_sort3(int *a, int *b, int *c){
	if(*a > *b) DGL_SWAP(*a, *b, int);
	if(*b > *c) DGL_SWAP(*b, *c, int);
	if(*a > *b) DGL_SWAP(*a, *b, int);
}

DGLAPI int dgl_min3(int a, int b, int c){
	if(a < b){
		if(a < c) return a;
		else return c;
	}
	else{
		if(b < c) return b;
		else return c;
	}
}

DGLAPI int dgl_max3(int a, int b, int c){
	if(a > b){
		if(a > c) return a;
		else return c;
	}
	else{
		if(b > c) return b;
		else return c;
	}
}

DGLAPI uint32_t dgl_bary_color(float s, float t, uint32_t c1, uint32_t c2, uint32_t c3){
	uint32_t a = DGL_SCALE_RGB(s, c1);
	uint32_t b = DGL_SCALE_RGB(t, c2);
	uint32_t c = DGL_SCALE_RGB(1-s-t, c3);
	return DGL_SUM_RGB(DGL_SUM_RGB(a, b), c);
}

// TODO: Maybe make it a macro
// TODO: Maybe make it branchless
DGLAPI int dgl_clamp(int v, int left, int right){
	if(v < left) return left;
	if(v > right) return right;
	return v;
}

DGLAPI inline uint32_t DGL_RGBA_TO_BGRA(uint32_t v){
	uint8_t t = v & 0xFF;
	v &= 0xFFFFFF00;
	v |= ((v & 0x00FF0000) >> 16);
	v &= 0xFF00FFFF;
	v |= (t << 16);
	return v;
}

// TODO: See the point of adding static inline to functions (with some macro) that are part of the library
// TODO: Add some basic matrix support with SIMD
// TODO: Add ability to triangulate and render surfaces
// TODO: Create UI elements like buttons

#endif // DGL_IMPLEMENTATION

#ifdef DGL_USE_ENGINE

#define DEFAULT_WINDOW_WIDTH  800
#define DEFAULT_WINDOW_HEIGHT 600

typedef struct {
	int width;
	int height;
	dgl_Canvas canvas;
	dgl_Canvas back_canvas;
} dgl_Window;

dgl_Window window = {};
unsigned int fps = 60;

void init_window_params(int width, int height){
	window.width = width;
	window.height = height;
}

void init_fps(unsigned int v){
	fps = v;
}

#ifdef DGL_TARGET_WINDOWS
#include <windows.h>
#include <windowsx.h>

typedef struct{
	HBITMAP hbm;
	uint32_t* data;
	BITMAPINFO bmi;
}dgl_Dib_Buffer;

BITMAPINFO dgl_get_bitmap_info(int width, int height) {
	BITMAPINFOHEADER bmi_h = {};
	bmi_h.biSize = sizeof(BITMAPINFOHEADER);
	bmi_h.biWidth = width;
	bmi_h.biHeight = -height;
	bmi_h.biPlanes = 1;
	bmi_h.biBitCount = 32;
	bmi_h.biCompression = BI_RGB;
	
	BITMAPINFO bmi = {};
	bmi.bmiHeader = bmi_h;

	return bmi;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void init();
void start();
void update(float dt);
void end();

void windows_init(){
	init();

	if(window.width == 0) window.width = DEFAULT_WINDOW_WIDTH;
	if(window.height == 0) window.height = DEFAULT_WINDOW_HEIGHT;
	
	window.canvas.pixels = malloc(sizeof(uint32_t) * window.width * window.height);
	window.canvas.width = window.width;
	window.canvas.height = window.height;
	window.canvas.stride = window.width;

	window.back_canvas.pixels = malloc(sizeof(uint32_t) * window.width * window.height);
	window.back_canvas.width = window.width;
	window.back_canvas.height = window.height;
	window.back_canvas.stride = window.width;
}

void windows_start(){
	start();
}

LARGE_INTEGER var_fps_ticks_start;
LARGE_INTEGER var_fps_ticks_end;
POINT cursorPos = {};
void windows_update(HWND hwnd, float dt){
	//QueryPerformanceCounter(&var_fps_ticks_start);

	GetCursorPos(&cursorPos);
	ScreenToClient(hwnd, &cursorPos);

	memcpy(window.canvas.pixels, window.back_canvas.pixels, sizeof(uint32_t)*window.canvas.width*window.canvas.height);
	
	update(dt);

	//QueryPerformanceCounter(&var_fps_ticks_end);
}

void windows_end(){
	end();
}

// TODO: Window drawing area should be adjusted so that user defined width and height
// don't include window borders.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow){
	LARGE_INTEGER performance_frequency;
	QueryPerformanceFrequency(&performance_frequency);

	windows_init();

	MSG msg;
	HWND hwnd;
	WNDCLASSW wc;
	
	wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.lpszClassName = L"Window";
    wc.hInstance     = hInstance;
    wc.lpszMenuName  = NULL;
    wc.lpfnWndProc   = WndProc;

	RegisterClassW(&wc);

	// TODO: Change to CreateWindowEx version
	hwnd = CreateWindowW(wc.lpszClassName, L"Engine",
						 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						 100, 100, window.width, window.height, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);

	LARGE_INTEGER ticks_start;
	LARGE_INTEGER ticks_end;
	
	QueryPerformanceCounter(&ticks_start);
	QueryPerformanceCounter(&ticks_end);

	float time_acc = 0;
	float time_slice = 0;

	time_slice = 1.0/fps;
	time_slice -= 0.01*time_slice;	// THIS IS EXPERIMENTAL ADJUSTMENT (so that we get precisely defined FPS)
	
	while(msg.message != WM_QUIT){
		// PeekMessage will not block if there are no messages (GetMessage will)
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			// Most messages are sent directly to window procedure
			// Those that are not include: WM_PAINT, keyboard related messages, mouse related messages
			// When key is pressed, it is sent through queue as a virtual key code
			// The purpose of TranslateMessage is to take that code and generate corresponding message of type WM_CHAR
			// This way, we can have WM_CHAR case in window procedure, and get its data from WPARAM and LPARAM
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else{
			// Accumulate time to roughly match expected frame duration for the given FPS
			ticks_start = ticks_end;
			QueryPerformanceCounter(&ticks_end);

			time_acc += (float)(ticks_end.QuadPart - ticks_start.QuadPart)/(float)performance_frequency.QuadPart;
			
			// Update things after accumulated time and prepare for next accumulation
			if(time_acc > time_slice){
				windows_update(hwnd, time_acc);
				InvalidateRect(hwnd, NULL, FALSE);
				
				time_acc = 0;
			}
		}
	}
	
	return (int)msg.wParam;
}

PAINTSTRUCT	 ps;
HDC			 hdc;
BITMAPINFO   bmi;
RECT         rect;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg) {
	case WM_CREATE:
		{
			windows_start();
			bmi = dgl_get_bitmap_info(window.width, window.height);
		} break;
	case WM_PAINT:
		{
			GetClientRect(hwnd, &rect);
			window.width = rect.right - rect.left;
			window.height = rect.bottom - rect.top;

			hdc = BeginPaint(hwnd, &ps);
			StretchDIBits(hdc,
						  0, 0, window.width, window.height,
						  0, 0, window.canvas.width, window.canvas.height,
						  window.canvas.pixels, &bmi,
						  DIB_RGB_COLORS, SRCCOPY);
			EndPaint(hwnd, &ps);		
		} break;
	case WM_DESTROY:	// Used to terminate window
		{
			PostQuitMessage(0);	// Generate WM_QUIT message and send it to queue (used to terminate application)
			windows_end();
		} break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif // DGL_USE_ENGINE::DGL_TARGET_WINDOWS

#endif // DGL_USE_ENGINE

