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

#ifndef DGL_HEADER_DECLARATION
#define DGL_HEADER_DECLARATION

#include <stdio.h>	// TODO: Maybe remove in the future, or let user choose with macro. This is only because we have functions that work with files

#include <stdint.h>
#include <errno.h>  // TODO: Remove in the future. We can do without this

typedef enum { false, true } dgl_Bool;
typedef float dgl_Real; // This is because we can use it to easily change precision
typedef int dgl_Errno;
// Saw this cool trick from 'tsoding'
#define DEFER_RETURN(v) do { DEFER_RESULT=v; goto DEFER; } while(0)

#define DGL_OFFSET_OF(s, f) ((size_t)&(((s*)0)->f))
#define DGL_SWAP(a, b, T) do { T t = a; a = b; b = t; } while(0)
#define DGL_LERP(a, b, t) a+(b-a)*t
// TODO: Fix weird behaviour
#define DGL_CLAMP(v, l, r) (((v) < (l)) ? (l) : (((v) > (r)) ? (r) : (v)))

#ifdef DGL_TARGET_WINDOWS
    #define DGL_RGB(r, g, b) (((b) & 0xFF) | (((g) & 0xFF) << 8) | (((r) & 0xFF) << 16) | 0xFF000000)
    #define DGL_RGBA(r, g, b, a) (((b) & 0xFF) | (((g) & 0xFF) << 8) | (((r) & 0xFF) << 16) | (((a) & 0xFF) << 24))
	
	#define DGL_WHITE	0xFFFFFFFF
	#define DGL_BLACK	0xFF000000
	#define DGL_RED 	0xFFFF0000
	#define DGL_GREEN 	0xFF00FF00
	#define DGL_BLUE 	0xFF0000FF
	#define DGL_YELLOW	0xFFFFFF00
	#define DGL_CYAN	0xFF00FFFF
	#define DGL_PINK	0xFFFF00FF
#else
    #define DGL_RGB(r, g, b) (((r) & 0xFF) | (((g) & 0xFF) << 8) | (((b) & 0xFF) << 16) | 0xFF000000)
    #define DGL_RGBA(r, g, b, a) (((r) & 0xFF) | (((g) & 0xFF) << 8) | (((b) & 0xFF) << 16) | (((a) & 0xFF) << 24))
	
	#define DGL_WHITE	0xFFFFFFFF
	#define DGL_BLACK	0xFF000000
	#define DGL_RED 	0xFF0000FF
	#define DGL_GREEN 	0xFF00FF00
	#define DGL_BLUE 	0xFFFF0000
	#define DGL_YELLOW	0xFF00FFFF
	#define DGL_CYAN	0xFFFFFF00
	#define DGL_PINK	0xFFFF00FF
#endif

#ifndef DGL_AALIAS_DEG
	#define DGL_AALIAS_DEG 2
#endif

#ifndef DGL_COORDINATE_CENTER_X
	#define DGL_COORDINATE_CENTER_X 0
#endif
#ifndef DGL_COORDINATE_CENTER_Y
	#define DGL_COORDINATE_CENTER_Y 0
#endif
// I feel like this transformation is more natural because it requires from user to enter center coordinates
// starting from bottom-left of the screen with standard x and y directions
// Additionaly, it allows us to specify center as (0,0) in above macros, while in the other case we wouldn't
// be able to specify bottom-left corner starting point with macros in the same way since we would need height
#define DGL_TRANSFORM_COORDINATES_X(x) ((x) + DGL_COORDINATE_CENTER_X)
#define DGL_TRANSFORM_COORDINATES_Y(y, canvas_height) ((canvas_height)-(y) - DGL_COORDINATE_CENTER_Y)

// #ifndef DGL_COORDINATE_CENTER_X
	// #define DGL_COORDINATE_CENTER_X 0
// #endif
// #ifndef DGL_COORDINATE_CENTER_Y
	// #define DGL_COORDINATE_CENTER_Y ???
// #endif
// This transformation requires user to enter center coordinates starting from top-left where x has standard direction
// and y grows downward
// This one also makes sense from the standpoint of "I will set center at the beginning and thus transform whole space to
// use standard coordinate system with (0,0) at the bottom-left, and after it I won't think anymore about old coords)
// But, it requires user to be aware of the old system at the beginning
// #define DGL_TRANSFORM_COORDINATES_X(x) ((x) + DGL_COORDINATE_CENTER_X)
// #define DGL_TRANSFORM_COORDINATES_Y(y, canvas_height) (-(y) + DGL_COORDINATE_CENTER_Y)

typedef struct { int x, y; 		   } dgl_Point2D;
typedef struct { dgl_Real x, y, z;    } dgl_Point3D;
typedef struct { dgl_Point2D v[3]; } dgl_Triangle2D;
typedef struct { dgl_Point3D v[3]; } dgl_Triangle3D;

inline void dgl_scale_point_2D(dgl_Point2D *p, int scale);
inline void dgl_scale_point_3D(dgl_Point3D *p, dgl_Real scale);
inline void dgl_translate_point_2D(dgl_Point2D *p, dgl_Point2D t);
inline void dgl_translate_point_3D(dgl_Point3D *p, dgl_Point3D t);
void dgl_rotate_point_2D(dgl_Point2D *p, dgl_Real angle);
void dgl_rotate_point_3D(dgl_Point3D *p, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z);

// TODO: Create functionality to transform to normal coordinate system from the one that starts at the top left
// TODO: This is just for testing out 3D rendering. Think more about representing models. Currently it is stupid.
// TODO: This model allows definition of colors for triangles (not interpolation based on vertices)
// It is assumed that model consists of triangles
// TODO: Right now, this type agregates information from various addresses in memory
//       It would be more efficient if it was actually contigious memory
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

dgl_Errno dgl_render_ppm(dgl_Canvas *canvas, char *filename);
dgl_Simple_Model *dgl_load_simple_model(char *filename, dgl_Bool hasQuads);
void dgl_delete_simple_model(dgl_Simple_Model *sm);
dgl_Simple_Model *dgl_cull_reduce_simple_model(dgl_Simple_Model *sm, int stride);

void dgl_clear(dgl_Canvas *canvas, uint32_t color);
void dgl_fill_rect(dgl_Canvas *canvas, int top_left_x, int top_left_y, size_t w, size_t h, uint32_t color);
void dgl_fill_circle(dgl_Canvas *canvas, int center_x, int center_y, size_t r, uint32_t color);
void dgl_draw_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t color);
void dgl_fill_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t color);
void dgl_draw_text(dgl_Canvas *canvas, const char *text, int x, int y, const dgl_Font *font, uint8_t scale, dgl_Bool vertical, uint32_t color);
void dgl_draw_line(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color);
void dgl_draw_line_bresenham(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color);
void dgl_draw_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, const dgl_Point3D focus, uint32_t color);
void dgl_fill_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, const dgl_Point3D focus, uint32_t color);

void dgl_sort3(int *a, int *b, int *c);
int dgl_clamp(int v, int left, int right);
uint32_t dgl_blend(uint32_t f, uint32_t b);

void dgl_scale_point_2D(dgl_Point2D *p, int scale);
void dgl_scale_point_3D(dgl_Point3D *p, dgl_Real scale);
void dgl_translate_point_2D(dgl_Point2D *p, dgl_Point2D t);
void dgl_translate_point_3D(dgl_Point3D *p, dgl_Point3D t);
void dgl_rotate_point_2D(dgl_Point2D *p, dgl_Real angle);
void dgl_rotate_point_3D(dgl_Point3D *p, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z);
	
void dgl_draw_simple_model(dgl_Canvas *canvas, const dgl_Simple_Model *sm, const dgl_Point3D focus);
void dgl_draw_simple_model_mesh(dgl_Canvas *canvas, const dgl_Simple_Model *sm, const dgl_Point3D focus);
void dgl_scale_simple_model(dgl_Simple_Model *sm, dgl_Real scale);
void dgl_translate_simple_model(dgl_Simple_Model *sm, dgl_Point3D translation);
void dgl_rotate_simple_model(dgl_Simple_Model *sm, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z);

#endif // DGL_H

#ifdef DGL_HEADER_IMPLEMENTATION

void dgl_scale_point_2D(dgl_Point2D *p, int scale){ p->x *= scale; p->y *= scale; }
void dgl_scale_point_3D(dgl_Point3D *p, dgl_Real scale){ p->x *= scale; p->y *= scale; p->z *= scale; }
void dgl_translate_point_2D(dgl_Point2D *p, dgl_Point2D t){ p->x += t.x; p->y += t.y; }
void dgl_translate_point_3D(dgl_Point3D *p, dgl_Point3D t){ p->x += t.x; p->y += t.y; p->z += t.z; }
// TODO: We should also exclude this header at some point
#include <math.h>
// TODO: Rotation like this is of course not the best way but will do for now
void dgl_rotate_point_2D(dgl_Point2D *p, dgl_Real angle){
	dgl_Point2D tp = (*p);
	p->x = cos(angle)*tp.x - sin(angle)*tp.y;
	p->y = sin(angle)*tp.x + cos(angle)*tp.y;
}
void dgl_rotate_point_3D(dgl_Point3D *p, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z){
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

// TODO: Blender uses coord system where z points upwards so we may need to take this into account
// TODO: We can also write our own functionality in the future (strtok is just dumb)
// We can load models that contain triangles and quads, but not arbitrary faces (no need for now)
#include <string.h>
dgl_Simple_Model *dgl_load_simple_model(char *filename, dgl_Bool hasQuads){
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
		else if(line[0] == 'f') sm->indices_length += (3 + hasQuads*3);
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
			if(hasQuads && f4 != NULL && f4[0] != '\n'){
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
	
	// FOR TESTING
	// Print vertices
	// printf("VERTICES\n");
	// for(int i = 0; i < sm->vertices_length; ++i){
	// 	printf("(%f, %f, %f)\n", sm->vertices[i].x, sm->vertices[i].y, sm->vertices[i].z);
	// }
	
	// Print indices
	// printf("INDICES\n");
	// for(int i = 0; i < sm->indices_length; ++i){
	// 	printf("%d ", sm->indices[i]);
	// }
	
	fclose(file);
	return sm;
}

void dgl_delete_simple_model(dgl_Simple_Model *sm){
	free(sm->vertices);
	free(sm->indices);
	free(sm->colors);
	free(sm);
}

// This is a very forceful way to reduce model complexity (by making it disconnected)
// The reason it works is that removal of triangles will roughly be uniform because of how
// locally close triangles are defined locally close in files
dgl_Simple_Model *dgl_cull_reduce_simple_model(dgl_Simple_Model *sm, int stride){
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
// TODO: Create function that will save simple model as .obj file

// TODO: Adapt this code, so it fits within specific mode of operation (like DGL_TARGET_WINDOWS)
dgl_Errno dgl_render_ppm(dgl_Canvas *canvas, char *filename){
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

void dgl_clear(dgl_Canvas *canvas, uint32_t color){
	for(size_t y = 0; y < canvas->height; ++y)
		for(size_t x = 0; x < canvas->width; ++x)
			DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
}

void dgl_draw_text(dgl_Canvas *canvas, const char *text, int x, int y, const dgl_Font *font, uint8_t scale, dgl_Bool vertical, uint32_t color){
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
void dgl_fill_rect(dgl_Canvas *canvas, int x0, int y0, size_t w, size_t h, uint32_t color){
	x0 = DGL_TRANSFORM_COORDINATES_X(x0);
	y0 = DGL_TRANSFORM_COORDINATES_Y(y0, canvas->height);
	
	for(int y = y0; y < y0 + (int)h; ++y)
		if(y >= 0 && y < (int)canvas->height)
			for(int x = x0; x < x0 + (int)w; ++x)
				if(x >= 0 && x < (int)canvas->width)
					DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
}

// TODO: Maybe it is possible to speed this up by iterating only on the portion of circle because of its symmetry
//		 (for example iterating over bounding box of quarter of the circle)
void dgl_fill_circle(dgl_Canvas *canvas, int center_x, int center_y, size_t r, uint32_t color){
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

void dgl_draw_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t color){
	// We don't need to transform coordinates because dgl_draw_line already does it

	dgl_draw_line_bresenham(canvas, t.v[0].x, t.v[0].y, t.v[1].x, t.v[1].y, color);
	dgl_draw_line_bresenham(canvas, t.v[0].x, t.v[0].y, t.v[2].x, t.v[2].y, color);
	dgl_draw_line_bresenham(canvas, t.v[1].x, t.v[1].y, t.v[2].x, t.v[2].y, color);
}

void dgl_fill_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, uint32_t color){
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
		if(y0 == 0) return;
		
		// Force order x0 <= x1 <= x2
		if(x0 > x1) DGL_SWAP(x0, x1, int);
		if(x1 > x2) DGL_SWAP(x1, x2, int);
		if(x0 > x1) DGL_SWAP(x0, x1, int);
		
		// Draw horizontal line
		for(int x = x0; x <= x2; ++x)
			if(!(x < 0) && !(x >= (int)canvas->width))
				DGL_SET_PIXEL(*canvas, x, y0, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y0)));
		
		return;
	}
	
	// Completely degenerate vertical line triangle
	if(x0 == x1 && x0 == x2){
		// Vertical line out of the screen
		if(x0 == 0) return;
		
		// Force order y0 <= y1 <= y2
		if(y0 > y1) DGL_SWAP(y0, y1, int);
		if(y1 > y2) DGL_SWAP(y1, y2, int);
		if(y0 > y1) DGL_SWAP(y0, y1, int);
		
		// Draw vertical line
		for(int y = y0; y <= y2; ++y)
			if(!(y < 0) && !(y >= (int)canvas->height))
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

	// TODO: Add triangle vertex interpolation (both for upper and lower part)

	if(dy10 != 0 && dy20 != 0){
		if(x1 > x0){
			DGL_SWAP(dy10, dy20, int);
			DGL_SWAP(dx10, dx20, int);
		}
	
		for(int y = y0; y < y1; ++y)
			if(!(y < 0) && !(y >= (int)canvas->height))
				for(int x = dx10*(y-y0)/dy10 + x0; x <= dx20*(y-y0)/dy20 + x0; ++x)
					if(!(x < 0) && !(x >= (int)canvas->width))
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
			if(!(y < 0) && !(y >= (int)canvas->height))
				for(int x = dx21*(y-y1)/dy21 + x1; x <= dx20*(y-y0)/dy20 + x0; ++x)
					if(!(x < 0) && !(x >= (int)canvas->width))
						DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
	}
}

// This is cheap but not great way to draw line since it struggles in cases of steep slopes
// Still, it is left here and can be used
void dgl_draw_line(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color){
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
			if(!(x < 0) && !(x >= (int)canvas->width)){
				int y = ((y1 - y0)*x - (y1 - y0)*x0 + y0*(x1 - x0))/(x1 - x0);
				if(!(y < 0) && !(y >= (int)canvas->height))
					DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
			}
		}
	}
	else{
		if(x0 < 0 || x0 >= (int)canvas->width) return;
		
		for(int y = y0; y <= y1; ++y)
			if(!(y < 0) && !(y >= (int)canvas->height))
				DGL_SET_PIXEL(*canvas, x0, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x0, y)));
	}
}

// TODO: It might be possible to not duplicate code in if and else partts because of symmetry
// TODO: Maybe add antialiasing capability in the future
void dgl_draw_line_bresenham(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color){
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
			if(x > 0 && x < (int)canvas->width && y > 0 && y < (int)canvas->height)
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
			if(x > 0 && x < (int)canvas->width && y > 0 && y < (int)canvas->height)
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
// TODO: Maybe create projection screen type or something similar that will hold focus point and direction the screen is 
void dgl_draw_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, const dgl_Point3D focus, uint32_t color){
    dgl_Triangle2D pt;
	pt.v[0].x = t.v[0].x;
	pt.v[0].y = t.v[0].y;
	pt.v[1].x = t.v[1].x;
	pt.v[1].y = t.v[1].y;
	pt.v[2].x = t.v[2].x;
	pt.v[2].y = t.v[2].y;
	dgl_draw_triangle_2D(canvas, pt, color);

}

void dgl_fill_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, const dgl_Point3D focus, uint32_t color){
	// TODO: Currently, only x and y shift of the focus has been incorporated, but not z (MAYBE NOT WORKING)
	
	// Projection
	// dgl_Triangle2D pt;
	
	// pt.v0.x = focus.z*(t.v0.x - focus.x)/(focus.z - t.v0.z) + focus.x;
	// pt.v0.y = focus.z*(t.v0.y - focus.y)/(focus.z - t.v0.z) + focus.y;
	
	// pt.v1.x = focus.z*(t.v1.x - focus.x)/(focus.z - t.v1.z) + focus.x;
	// pt.v1.y = focus.z*(t.v1.x - focus.x)/(focus.z - t.v1.z) + focus.y;
	
	// pt.v2.x = focus.z*(t.v2.x - focus.x)/(focus.z - t.v2.z) + focus.x;
	// pt.v2.y = focus.z*(t.v2.x - focus.x)/(focus.z - t.v2.z) + focus.y;
	
	/*
	
	pt.v0.x = (focus.z*(t.v0.x - focus.x) + focus.x*(focus.z - t.v0.z))/(focus.z - t.v0.z);
	pt.v0.y = (focus.z*(t.v0.y - focus.y) + focus.y*(focus.z - t.v0.z))/(focus.z - t.v0.z);
	
	pt.v1.x = focus.z*(t.v1.x - focus.x)/(focus.z - t.v1.z) + focus.x;
	pt.v1.y = focus.z*(t.v1.x - focus.x)/(focus.z - t.v1.z) + focus.y;
	
	pt.v2.x = focus.z*(t.v2.x - focus.x)/(focus.z - t.v2.z) + focus.x;
	pt.v2.y = focus.z*(t.v2.x - focus.x)/(focus.z - t.v2.z) + focus.y;
	*/
	
	// 2D Drawing
	// dgl_fill_triangle_2D(canvas, pt, color);
	
	// Orthogonal projection (seems to work)
	dgl_Triangle2D pt;
	pt.v[0].x = t.v[0].x;
	pt.v[0].y = t.v[0].y;
	pt.v[1].x = t.v[1].x;
	pt.v[1].y = t.v[1].y;
	pt.v[2].x = t.v[2].x;
	pt.v[2].y = t.v[2].y;
	dgl_fill_triangle_2D(canvas, pt, color);
}

// TODO: Create rendering of simple model mesh edges so that we can work with models
//       without worrying about which faces should be rendered

// Currently, there is no checking which triangles are further away
// Colors will be looped, so we don't need to provide as many colors as there are triangles
void dgl_draw_simple_model(dgl_Canvas *canvas, const dgl_Simple_Model *sm, const dgl_Point3D focus){
	// TODO: Add some simple differentiation between closer and furhter triangle of the model
	dgl_Triangle3D t;
	
	for(size_t i = 0; i < sm->indices_length/3; ++i){
		for(size_t j = 0; j < 3; ++j)
			t.v[j] = sm->vertices[sm->indices[i*3 + j]];
		
		dgl_fill_triangle_3D(canvas, t, focus, sm->colors[i % sm->colors_length]);
	}
}

void dgl_draw_simple_model_mesh(dgl_Canvas *canvas, const dgl_Simple_Model *sm, const dgl_Point3D focus){
	dgl_Triangle3D t;

	for(size_t i = 0; i < sm->indices_length/3; ++i){
		for(size_t j = 0; j < 3; ++j)
			t.v[j] = sm->vertices[sm->indices[i*3 + j]];
		
		dgl_draw_triangle_3D(canvas, t, focus, sm->colors[i % sm->colors_length]);
	}
}

void dgl_scale_simple_model(dgl_Simple_Model *sm, dgl_Real scale){
	for(size_t i = 0; i < sm->vertices_length; ++i)
		dgl_scale_point_3D(&(sm->vertices[i]), scale);
}

void dgl_translate_simple_model(dgl_Simple_Model *sm, dgl_Point3D translation){
	for(size_t i = 0; i < sm->vertices_length; ++i)
		dgl_translate_point_3D(&(sm->vertices[i]), translation);
}

void dgl_rotate_simple_model(dgl_Simple_Model *sm, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z){
	for(size_t i = 0; i < sm->vertices_length; ++i)
		dgl_rotate_point_3D(&(sm->vertices[i]), angle_x, angle_y, angle_z);
}

// TODO: It might be possible to speed this up further by expanding multiples of 255
// as series, measuring the error, and adjusting result by a constant factor so that
// we don't lose information
uint32_t dgl_blend(uint32_t f, uint32_t b){

	// ONLY WITH INTEGER ARITHMETIC
	uint8_t a1 = ((f >> 24) & 0xFF);
	uint8_t a2 = ((b >> 24) & 0xFF);
	
	uint8_t r1 = (f & 0xFF);
	uint8_t r2 = (b & 0xFF);
	
	uint8_t g1 = ((f >> 8) & 0xFF);
	uint8_t g2 = ((b >> 8) & 0xFF);
	
	uint8_t b1 = ((f >> 16) & 0xFF);
	uint8_t b2 = ((b >> 16) & 0xFF);
	
	// Attempt at further optimization: reduce division to shifting while keeping all the information (if possible)
	/*
	q = (1/256) = 2^(-8) = (>>8)
	1/255 = 1/(256-1) = (1/256)/(1 - (1/256)) = q*(q^0 + q^1 + q^2 + ...) ~ q*(q^0 + q^1) ~ q + q^2
	
	MAYBE ADD SOME CONSTANT TO APPROXIMATION?
	
	WITHOUT CONSTANT (not working, a lot of information lost this way)
	=>
	(1/255)*X = (q + q^2)*X = ((256 + 1)/(256^2))*X = (X*(256 + 1))/(256^2) = (257*X)>>16
	(1/65025)*X = (257*((257*X)>>16))>>16
	
	*/
	
	// These were used in dgl_Real arithmetic
	// uint32_t alpha 	= (uint32_t)((a1 + (1-a1)*a2)*255);
	// uint32_t red 	= (uint32_t)((a1*r1 + r2*a2*(1-a1))*255);
	// uint32_t green 	= (uint32_t)((a1*g1 + g2*a2*(1-a1))*255);
	// uint32_t blue 	= (uint32_t)((a1*b1 + b2*a2*(1-a1))*255);
	
	uint8_t red = (255*(a1*r1 + a2*r2) - a1*a2*r2)/65025;
	uint8_t green = (255*(a1*g1 + a2*g2) - a1*a2*g2)/65025;
	uint8_t blue = (255*(a1*b1 + a2*b2) - a1*a2*b2)/65025;
	uint8_t alpha = (255*(a1+a2) - a1*a2)/255;
	
	return (alpha << 24) + (blue << 16) + (green << 8) + red;
}

void dgl_sort3(int *a, int *b, int *c){
	if(*a > *b) DGL_SWAP(*a, *b, int);
	if(*b > *c) DGL_SWAP(*b, *c, int);
	if(*a > *b) DGL_SWAP(*a, *b, int);
}

// TODO: Maybe make it a macro
// TODO: Maybe make it branchless
int dgl_clamp(int v, int left, int right){
	if(v < left) return left;
	if(v > right) return right;
	return v;
}

static inline uint32_t DGL_RGBA_TO_BGRA(uint32_t v){
	uint8_t t = v & 0xFF;
	v &= 0xFFFFFF00;
	v |= ((v & 0x00FF0000) >> 16);
	v &= 0xFF00FFFF;
	v |= (t << 16);
	return v;
}

// TODO: See the point of adding static inline to functions (with some macro) that are part of the library
// TODO: Maybe separate header part from implementation by providing implementation only if the specific macro is defined
// TODO: Add ability to render models (simpler to complex)(from standard files like .obj)(or define own simple format)
// TODO: Add ability to triangulate and render surfaces
// TODO: Create UI elements like buttons
// TODO: Using PPM files as textures
// TODO: SIMD processing

#endif // DGL_HEADER_IMPLEMENTATION
