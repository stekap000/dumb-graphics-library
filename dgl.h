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

// TODO: Depth buffer for solid colors.
// TODO: Depth buffer that also works with alpha blending.

#ifndef DGL_DECLARATION
#define DGL_DECLARATION
#define DGLAPI static

#include <stdlib.h>
#include <stdio.h>	// TODO: Maybe remove in the future, or let user choose with macro. This is only because we have functions that work with files
#include <string.h> // TODO: Maybe remove (currently it is here because of memcpy)
#include <stdint.h>
#include <errno.h>  // TODO: Remove in the future. We can do without this
#include <math.h>

#include <assert.h>
#define DGL_NOT_IMPLEMENTED(msg) (assert(!msg))

// TODO: Add inclusion for C++

#define DGL_PI 3.14159265358
#define DGL_MAX_DISTANCE -1000000000

#define DEFER_RETURN(v) do { DEFER_RESULT=v; goto DEFER; } while(0)

#ifdef DGL_TARGET_WINDOWS
    #define WIN32_LEAN_AND_MEAN

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

#define DGL_OFFSET_OF(s, f) ((size_t)&((((s)*)0)->(f)))
#define DGL_SWAP(a, b, T) do { T t = (a); (a) = (b); (b) = (t); } while(0)
#define DGL_LERP(a, b, t) ((a)+((b)-(a))*(t))
#define DGL_LERP_INVERSE(a, b, value) ((value) > (b)) ? 1 : (((value) - (a))/((b)-(a)));
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

#define DGL_GET_PIXEL(canvas, x, y) (canvas).pixels[(y)*(canvas).stride + (x)]
#define DGL_SET_PIXEL(canvas, x, y, v) (canvas).pixels[(y)*(canvas).stride + (x)] = (v)

#define DGL_GET_Z_INDEX(canvas, x, y) (canvas).z_indices[(y)*(canvas).stride + (x)]
#define DGL_SET_Z_INDEX(canvas, x, y, v) (canvas).z_indices[(y)*(canvas).stride + (x)] = (v)

#define DGL_DEFAULT_FONT_WIDTH		6
#define DGL_DEFAULT_FONT_HEIGHT		7

typedef enum { false, true } dgl_Bool;
typedef float dgl_Real; // This is because we can use it to easily change precision
typedef int dgl_Errno;

typedef dgl_Real dgl_Mat4[16];
typedef dgl_Real dgl_Vec4[4];

typedef struct {
	dgl_Real *cells;
	int height;
	int width;
} dgl_Mat;

typedef uint32_t dgl_Color;

typedef union {
 	struct { int x, y; };
 	int v[2];
} dgl_Point2D;

typedef union {
 	struct { dgl_Real x, y, z; };
 	dgl_Real v[3];
} dgl_V3, dgl_Point3D;
 
typedef struct { dgl_Point2D v[3];	} dgl_Triangle2D;
typedef struct { dgl_Point3D v[3];	} dgl_Triangle3D;

// TODO: This model allows definition of colors for triangles (not interpolation based on vertices)
// It is assumed that model consists of triangles
// TODO: Right now, this type agregates information from various addresses in memory
//       It would be more efficient if it was actually continuous memory
typedef struct{
	dgl_Point3D *vertices;
	int *indices;
	dgl_Color *colors;
	size_t vertices_length;
	size_t indices_length;
	size_t colors_length;
} dgl_Simple_Model;

// This is useful since it allows us to operate only on one portion of pixels (for example use dgl_clear just on one portion)
typedef struct{
	dgl_Color *pixels;
	dgl_Real *z_indices;
	int width;
	int height;
	int stride;
} dgl_Canvas;

// This is a general font type
typedef struct{
	size_t width, height;	// This must match with glyph mesh dimensions
	char *glyphs;			// This allows us to iterate through 3D matrix character by character
} dgl_Font;

DGLAPI dgl_Canvas dgl_create_canvas(int width, int height);
DGLAPI void dgl_clear(dgl_Canvas *canvas, dgl_Color color);
DGLAPI dgl_Color dgl_read_pixel(dgl_Canvas *canvas, int x, int y);
DGLAPI void dgl_fill_pixel(dgl_Canvas *canvas, int x, int y, dgl_Color color);
DGLAPI float dgl_read_z_index(dgl_Canvas *canvas, int x, int y);
DGLAPI void dgl_fill_z_index(dgl_Canvas *canvas, int x, int y, float z_index);
DGLAPI void dgl_draw_text(dgl_Canvas *canvas, const char *text, int x, int y, const dgl_Font *font, uint8_t scale, dgl_Bool vertical, dgl_Color color);
DGLAPI void dgl_fill_rect(dgl_Canvas *canvas, int top_left_x, int top_left_y, size_t w, size_t h, dgl_Color color);
DGLAPI void dgl_draw_rect(dgl_Canvas *canvas, int top_left_x, int top_left_y, size_t w, size_t h, dgl_Color color);
DGLAPI void dgl_fill_circle(dgl_Canvas *canvas, int center_x, int center_y, size_t r, dgl_Color color);
DGLAPI void dgl_draw_circle(dgl_Canvas *canvas, int center_x, int center_y, size_t r, dgl_Color color);
DGLAPI void dgl_draw_line(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, dgl_Color color);
DGLAPI void dgl_draw_vertical_line(dgl_Canvas *canvas, int x, dgl_Color color);
DGLAPI void dgl_draw_horizontal_line(dgl_Canvas *canvas, int y, dgl_Color color);
DGLAPI void dgl_draw_line_bresenham(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, dgl_Color color);
DGLAPI void dgl_fill_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, dgl_Color color);
DGLAPI void dgl_fill_triangle_bary_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, float z_index, dgl_Color c1, dgl_Color c2, dgl_Color c3);
DGLAPI void dgl_draw_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, dgl_Color color);
DGLAPI void dgl_fill_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, dgl_Mat proj_mat, dgl_Color color);
DGLAPI void dgl_draw_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, dgl_Mat proj_mat, dgl_Color color);

DGLAPI dgl_Errno         dgl_render_ppm(dgl_Canvas *canvas, char *filename);
DGLAPI dgl_Simple_Model *dgl_load_simple_model(char *filename, dgl_Bool has_quads);
DGLAPI int               dgl_save_simple_model(dgl_Simple_Model *sm, char *filename);
DGLAPI void              dgl_delete_simple_model(dgl_Simple_Model *sm);
DGLAPI dgl_Simple_Model *dgl_cull_reduce_simple_model(dgl_Simple_Model *sm, int stride);
DGLAPI void dgl_draw_simple_model(dgl_Canvas *canvas, const dgl_Simple_Model *sm, dgl_Mat proj_mat);
DGLAPI void dgl_draw_simple_model_mesh(dgl_Canvas *canvas, const dgl_Simple_Model *sm, dgl_Mat proj_mat);
DGLAPI void dgl_scale_simple_model(dgl_Simple_Model *sm, dgl_Real scale);
DGLAPI void dgl_translate_simple_model(dgl_Simple_Model *sm, dgl_Point3D translation);
DGLAPI void dgl_rotate_simple_model(dgl_Simple_Model *sm, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z);

DGLAPI inline void dgl_scale_point_2D(dgl_Point2D *p, int scale);
DGLAPI inline void dgl_scale_point_3D(dgl_Point3D *p, dgl_Real scale);
DGLAPI inline void dgl_translate_point_2D(dgl_Point2D *p, dgl_Point2D t);
DGLAPI inline void dgl_translate_point_3D(dgl_Point3D *p, dgl_Point3D t);
DGLAPI void        dgl_rotate_point_2D(dgl_Point2D *p, dgl_Real angle);
DGLAPI void        dgl_rotate_point_3D(dgl_Point3D *p, dgl_Real angle_x, dgl_Real angle_y, dgl_Real angle_z);

DGLAPI inline dgl_V3	dgl_v3_add(dgl_V3 v1, dgl_V3 v2);
DGLAPI inline dgl_V3	dgl_v3_sub(dgl_V3 v1, dgl_V3 v2);
DGLAPI inline dgl_V3	dgl_v3_scale(dgl_V3 v, dgl_Real s);
DGLAPI inline dgl_Real	dgl_v3_dot(dgl_V3 v1, dgl_V3 v2);
DGLAPI inline dgl_Real	dgl_v3_lensq(dgl_V3 v);
DGLAPI inline dgl_Real	dgl_v3_len(dgl_V3 v);
DGLAPI inline dgl_V3	dgl_v3_unit(dgl_V3 v);
DGLAPI inline void		dgl_v3_add_mut(dgl_V3 *v1, dgl_V3 v2);
DGLAPI inline void		dgl_v3_sub_mut(dgl_V3 *v1, dgl_V3 v2);
DGLAPI inline void		dgl_v3_scale_mut(dgl_V3 *v, dgl_Real s);
DGLAPI inline void		dgl_v3_unit_mut(dgl_V3 *v);

DGLAPI void		 dgl_sort3(int *a, int *b, int *c);
DGLAPI int		 dgl_min3(int a, int b, int c);
DGLAPI int		 dgl_max3(int a, int b, int c);
DGLAPI float	 dgl_min3f(float a, float b, float c);
DGLAPI float	 dgl_max3f(float a, float b, float c);
DGLAPI int		 dgl_clamp(int v, int left, int right);
DGLAPI dgl_Color dgl_blend(dgl_Color f, dgl_Color b);
DGLAPI dgl_Color dgl_bary_color(float s, float t, dgl_Color c1, dgl_Color c2, dgl_Color c3);

// TODO: Maybe do projection this way after implementing matrices, maybe do it dumber
DGLAPI void dgl_init_proj_params(float fov_deg, float aspect_ratio, dgl_Real *l, dgl_Real *r, dgl_Real *t, dgl_Real *b, dgl_Real n, dgl_Real f);
DGLAPI dgl_Mat dgl_mat_get_proj(dgl_Real l, dgl_Real r, dgl_Real t, dgl_Real b, dgl_Real n, dgl_Real f);

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
	['-'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	
	},
	[':'] = {
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 0, 0},
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

// This is default font provided with library
dgl_Font dgl_default_font = {
	.width = DGL_DEFAULT_FONT_WIDTH,
	.height = DGL_DEFAULT_FONT_HEIGHT,
	.glyphs = &dgl_Default_Glyphs[0][0][0]
};

#endif // DGL_H

#ifdef DGL_IMPLEMENTATION

dgl_Canvas dgl_create_canvas(int width, int height) {
	assert(width > 0 && height > 0);
	
	dgl_Canvas canvas = {0};
	canvas.pixels = calloc(width * height, sizeof(dgl_Color));
	
	canvas.z_indices = calloc(width * height, sizeof(dgl_Real));
	for(int i = 0; i < width * height; ++i) canvas.z_indices[i] = DGL_MAX_DISTANCE;
	
	canvas.width = width;
	canvas.height = height;
	canvas.stride = width;
	return canvas;
}

DGLAPI void dgl_clear(dgl_Canvas *canvas, dgl_Color color){
	for(int y = 0; y < canvas->height; ++y)
		for(int x = 0; x < canvas->width; ++x)
			DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
}

DGLAPI void dgl_clear_z_index(dgl_Canvas *canvas, dgl_Real z_index) {
	for(int y = 0; y < canvas->height; ++y)
		for(int x = 0; x < canvas->width; ++x)
			DGL_SET_Z_INDEX(*canvas, x, y, z_index);
}

DGLAPI dgl_Color dgl_read_pixel(dgl_Canvas *canvas, int x, int y) {
	x = DGL_TRANSFORM_COORDINATES_X(x);
	y = DGL_TRANSFORM_COORDINATES_Y(y, canvas->height);
	if(x >= 0 && x < canvas->width && y >= 0 && y < canvas->height)
		return DGL_GET_PIXEL(*canvas, x, y);
	return DGL_BLACK;
}

DGLAPI void dgl_fill_pixel(dgl_Canvas *canvas, int x, int y, dgl_Color color) {
	x = DGL_TRANSFORM_COORDINATES_X(x);
	y = DGL_TRANSFORM_COORDINATES_Y(y, canvas->height);
	if(x >= 0 && x < canvas->width && y >= 0 && y < canvas->height) {
		dgl_Color c = dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y));
		DGL_SET_PIXEL(*canvas, x, y, c);
	}
}

DGLAPI float dgl_read_z_index(dgl_Canvas *canvas, int x, int y) {
	x = DGL_TRANSFORM_COORDINATES_X(x);
	y = DGL_TRANSFORM_COORDINATES_Y(y, canvas->height);
	if(x >= 0 && x < canvas->width && y >= 0 && y < canvas->height)
		return DGL_GET_Z_INDEX(*canvas, x, y);
	return DGL_MAX_DISTANCE;
}

DGLAPI void dgl_fill_z_index(dgl_Canvas *canvas, int x, int y, float z_index) {
	x = DGL_TRANSFORM_COORDINATES_X(x);
	y = DGL_TRANSFORM_COORDINATES_Y(y, canvas->height);
	if(x >= 0 && x < canvas->width && y >= 0 && y < canvas->height)
		DGL_SET_Z_INDEX(*canvas, x, y, z_index);
}

DGLAPI void dgl_draw_text(dgl_Canvas *canvas, const char *text, int x, int y, const dgl_Font *font, uint8_t scale, dgl_Bool vertical, dgl_Color color){
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
DGLAPI void dgl_fill_rect(dgl_Canvas *canvas, int x0, int y0, size_t w, size_t h, dgl_Color color){
	x0 = DGL_TRANSFORM_COORDINATES_X(x0);
	y0 = DGL_TRANSFORM_COORDINATES_Y(y0, canvas->height);
	
	for(int y = y0; y < y0 + (int)h; ++y)
		if(y >= 0 && y < (int)canvas->height)
			for(int x = x0; x < x0 + (int)w; ++x)
				if(x >= 0 && x < (int)canvas->width) 
					DGL_SET_PIXEL(*canvas, x, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x, y)));
}

DGLAPI void dgl_draw_rect(dgl_Canvas *canvas, int x0, int y0, size_t w, size_t h, dgl_Color color){
	x0 = DGL_TRANSFORM_COORDINATES_X(x0);
	y0 = DGL_TRANSFORM_COORDINATES_Y(y0, canvas->height);

	dgl_draw_line_bresenham(canvas, x0, y0, x0 + w, y0, color);
	dgl_draw_line_bresenham(canvas, x0, y0, x0, y0 + h, color);
	dgl_draw_line_bresenham(canvas, x0 + w, y0 + h, x0, y0 + h, color);
	dgl_draw_line_bresenham(canvas, x0 + w, y0 + h, x0 + w, y0, color);
}

// TODO: Maybe it is possible to speed this up by iterating only on the portion of circle because of its symmetry
//		 (for example iterating over bounding box of quarter of the circle)
DGLAPI void dgl_fill_circle(dgl_Canvas *canvas, int center_x, int center_y, size_t r, dgl_Color color){
	center_x = DGL_TRANSFORM_COORDINATES_X(center_x);
	center_y = DGL_TRANSFORM_COORDINATES_Y(center_y, canvas->height);
	
	dgl_Color orig_color = color;
	
	int top_left_y = center_y - r;
	int top_left_x = center_x - r;
	for(size_t y = 0; y < 2*r; ++y)
		if(top_left_y + (int)y >= 0 && top_left_y + (int)y < (int)canvas->height)
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

// TODO: Include blending
// TODO: Make it more efficient
DGLAPI void dgl_draw_circle(dgl_Canvas *canvas, int center_x, int center_y, size_t r, dgl_Color color) {
	center_x = DGL_TRANSFORM_COORDINATES_X(center_x);
	center_y = DGL_TRANSFORM_COORDINATES_Y(center_y, canvas->height);

	int top_left_y = center_y - r;
	int top_left_x = center_x - r;

	for(size_t y = 0; y < 2*r; ++y) {
		if(top_left_y + (int)y >= 0 && top_left_y + (int)y < (int)canvas->height){
			for(size_t x = 0; x < 2*r; ++x) {
				if(!(top_left_x + (int)x < 0) && !(top_left_x + (int)x >= (int)canvas->width)){
					int pxl = top_left_x + x - center_x;
					int pyl = top_left_y + y - center_y;
					unsigned int left_side = pxl*pxl + pyl*pyl;
					
					if(left_side >= (r-1)*(r-1) && left_side <= r*r){
						DGL_SET_PIXEL(*canvas, top_left_x + x, top_left_y + y, dgl_blend(color, DGL_GET_PIXEL(*canvas, top_left_x + x, top_left_y + y)));
					}
				}
			}
		}
	}
}

// This is cheap but not great way to draw line since it struggles in cases of steep slopes
// Still, it is left here and can be used
DGLAPI void dgl_draw_line(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, dgl_Color color){
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

		if(y0 > y1) {
			DGL_SWAP(y0, y1, int);
		}
		
		for(int y = y0; y <= y1; ++y)
			if(y >= 0 && y < (int)canvas->height)
				DGL_SET_PIXEL(*canvas, x0, y, dgl_blend(color, DGL_GET_PIXEL(*canvas, x0, y)));
	}
}

DGLAPI void dgl_draw_vertical_line(dgl_Canvas *canvas, int x, dgl_Color color) {
	DGL_NOT_IMPLEMENTED("");
}

DGLAPI void dgl_draw_horizontal_line(dgl_Canvas *canvas, int y, dgl_Color color) {
	DGL_NOT_IMPLEMENTED("");
}

// TODO: It might be possible to not duplicate code in if and else parts because of symmetry
// TODO: Maybe add antialiasing capability
DGLAPI void dgl_draw_line_bresenham(dgl_Canvas *canvas, int x0, int y0, int x1, int y1, dgl_Color color){
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

// TODO: Find out the reason for weird shaded triangles apearing when rendering model with filled triangles
DGLAPI void dgl_fill_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, dgl_Color color){
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

// TODO: Move vertex color information into triangle
// TODO: Feels like things can be optimized here, but it's not primary now
// TODO: See how to adapt this code to SIMD
// @Test It is roughly two times slower that other triangle filling function when using single color
DGLAPI void dgl_fill_triangle_bary_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, float z_index, dgl_Color c1, dgl_Color c2, dgl_Color c3){
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

						if(0 <= l1 && l1 <= 1 && 0 <= l2 && l2 <= 1 && (l1+l2) <= 1) {
							if(DGL_GET_Z_INDEX(*canvas, x, y) < z_index) {
								DGL_SET_Z_INDEX(*canvas, x, y, z_index);
								DGL_SET_PIXEL(*canvas, x, y, dgl_blend(dgl_bary_color(l1, l2, c1, c2, c3), DGL_GET_PIXEL(*canvas, x, y)));
							}
						}
				}
}

DGLAPI void dgl_draw_triangle_2D(dgl_Canvas *canvas, const dgl_Triangle2D t, dgl_Color color){
	// We don't need to transform coordinates because dgl_draw_line already does it

	dgl_draw_line_bresenham(canvas, t.v[0].x, t.v[0].y, t.v[1].x, t.v[1].y, color);
	dgl_draw_line_bresenham(canvas, t.v[0].x, t.v[0].y, t.v[2].x, t.v[2].y, color);
	dgl_draw_line_bresenham(canvas, t.v[1].x, t.v[1].y, t.v[2].x, t.v[2].y, color);
}

DGLAPI void dgl_fill_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, dgl_Mat proj_mat, dgl_Color color){
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

	// Doesn't work in specific case when two triangles are connected
	// but the portion of one is behind portion of the other.
	//float z_index = dgl_min3f(t.v[0].z, t.v[1].z, t.v[2].z);

	// TODO: This only covers nicely behaved meshes
	float z_index = (t.v[0].z + t.v[1].z + t.v[2].z);
	dgl_fill_triangle_bary_2D(canvas, pt, z_index, DGL_RED, DGL_GREEN, DGL_BLUE);
}

// TODO: For now, we are just doing orthogonal projection because it requires minimal calculation. Move to perspective projection
// TODO: Maybe create projection screen type or something similar that will hold focus point and direction the screen is facing
DGLAPI void dgl_draw_triangle_3D(dgl_Canvas *canvas, const dgl_Triangle3D t, dgl_Mat proj_mat, dgl_Color color){
    dgl_Triangle2D pt;
	//dgl_Mat v = dgl_mat_alloc(4, 1);
	
	// Perspective projection (division by 'w' constructs NDC space (we don't need that?))
	// We should also add clipping check after vertices have been transformed?
	/*
	v.cells[0] = t.v[0].x;
	v.cells[1] = t.v[0].y;
	v.cells[2] = t.v[0].z;
	v.cells[3] = 1;
	
	v = dgl_mat_mul_mut(proj_mat, v); 
	pt.v[0].x = v.cells[0];
	pt.v[0].y = v.cells[1];

	v.cells[0] = t.v[1].x;
	v.cells[1] = t.v[1].y;
	v.cells[2] = t.v[1].z;
	v.cells[3] = 1;
	
	v = dgl_mat_mul_mut(proj_mat, v); 
	pt.v[1].x = v.cells[0];
	pt.v[1].y = v.cells[1];

	v.cells[0] = t.v[2].x;
	v.cells[1] = t.v[2].y;
	v.cells[2] = t.v[2].z;
	v.cells[3] = 1;
	
	v = dgl_mat_mul_mut(proj_mat, v); 
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

// TODO: Adapt this code, so it fits within specific mode of operation (like DGL_TARGET_WINDOWS)
DGLAPI dgl_Errno dgl_render_ppm(dgl_Canvas *canvas, char *filename){
	int DEFER_RESULT = 0;
	
	FILE *file = fopen(filename, "wb");
	if(file == NULL) return -1;
	
	fprintf(file, "P6\n%d %d\n255\n", (unsigned int)canvas->width, (unsigned int)canvas->height);
	if(ferror(file)) DEFER_RETURN(errno);
	
	for(int i = 0; i < canvas->width*canvas->height; ++i){
		fwrite(&canvas->pixels[i], sizeof(char), 3, file);
		if(ferror(file)) DEFER_RETURN(errno);
	}
	
DEFER:
	fclose(file);
	return DEFER_RESULT;
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
	sm->colors = (dgl_Color*)malloc(sizeof(dgl_Color));
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

DGLAPI inline dgl_V3 dgl_v3_add(dgl_V3 v1, dgl_V3 v2){
	return (dgl_V3){
		.x = v1.x + v2.x,
		.y = v1.y + v2.y,
		.z = v1.z + v2.z
	};
}

DGLAPI inline dgl_V3 dgl_v3_sub(dgl_V3 v1, dgl_V3 v2){
	return (dgl_V3){
		.x = v1.x - v2.x,
		.y = v1.y - v2.y,
		.z = v1.z - v2.z
	};
}

DGLAPI inline dgl_V3 dgl_v3_scale(dgl_V3 v, dgl_Real s){
	return (dgl_V3){
		.x = s * v.x,
		.y = s * v.y,
		.z = s * v.z
	};
}

DGLAPI inline dgl_Real dgl_v3_dot(dgl_V3 v1, dgl_V3 v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

DGLAPI inline dgl_Real dgl_v3_lensq(dgl_V3 v){
	return dgl_v3_dot(v, v);
}

DGLAPI inline dgl_Real dgl_v3_len(dgl_V3 v){
	return sqrt(dgl_v3_lensq(v));
}

DGLAPI inline dgl_V3 dgl_v3_unit(dgl_V3 v){
	return dgl_v3_scale(v, 1.0/dgl_v3_len(v));
}

DGLAPI inline void dgl_v3_add_mut(dgl_V3 *v1, dgl_V3 v2){
	v1->x += v2.x;
	v1->y += v2.y;
	v1->z += v2.z;
}

DGLAPI inline void dgl_v3_sub_mut(dgl_V3 *v1, dgl_V3 v2){
	v1->x -= v2.x;
	v1->y -= v2.y;
	v1->z -= v2.z;
}

DGLAPI inline void dgl_v3_scale_mut(dgl_V3 *v, dgl_Real s){
	v->x *= s;
	v->y *= s;
	v->z *= s;
}

DGLAPI inline void dgl_v3_unit_mut(dgl_V3 *v){
	dgl_v3_scale_mut(v, 1.0/dgl_v3_len(*v));
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

DGLAPI float dgl_min3f(float a, float b, float c) {
	if(a < b){
		if(a < c) return a;
		else return c;
	}
	else{
		if(b < c) return b;
		else return c;
	}
}

DGLAPI float dgl_max3f(float a, float b, float c) {
	if(a > b){
		if(a > c) return a;
		else return c;
	}
	else{
		if(b > c) return b;
		else return c;
	}
}

DGLAPI int dgl_clamp(int v, int left, int right){
	if(v < left) return left;
	if(v > right) return right;
	return v;
}

DGLAPI dgl_Color dgl_blend(dgl_Color f, dgl_Color b){
	// These were used in dgl_Real arithmetic
	// dgl_Color alpha 	= (dgl_Color)((a1    +    a2*(1-a1))*255);
	// dgl_Color red 	= (dgl_Color)((a1*r1 + r2*a2*(1-a1))*255);
	// dgl_Color green 	= (dgl_Color)((a1*g1 + g2*a2*(1-a1))*255);
	// dgl_Color blue 	= (dgl_Color)((a1*b1 + b2*a2*(1-a1))*255);

	// Conversion to integer arithmetic below
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

DGLAPI dgl_Color dgl_bary_color(float s, float t, dgl_Color c1, dgl_Color c2, dgl_Color c3){
	dgl_Color a = DGL_SCALE_RGB(s, c1);
	dgl_Color b = DGL_SCALE_RGB(t, c2);
	dgl_Color c = DGL_SCALE_RGB(1-s-t, c3);
	return DGL_SUM_RGB(DGL_SUM_RGB(a, b), c);
}

DGLAPI dgl_Mat dgl_mat_alloc(int height, int width){
	dgl_Mat m = {0};
	m.cells = calloc(height*width, sizeof(dgl_Real));
	m.height = height;
	m.width = width;
	return m;
}

DGLAPI void dgl_mat_free(dgl_Mat m){
	free(m.cells);
}

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

dgl_Window window = {0};
unsigned int fps = 60;
dgl_Point2D cursor_pos = {0};
dgl_Bool running = true;

void init();
void start();
void update(float dt);
void end();

void init_window_params(int width, int height){
	window.width = width;
	window.height = height;
}

void init_fps(unsigned int v){
	fps = v;
}

void write_debug_string(char *str){
	FILE *dbf = fopen("debug.txt", "a");
	fprintf(dbf, "%s\n", str);
	fclose(dbf);
}

#ifdef DGL_TARGET_WINDOWS
#include <windows.h>

typedef struct{
	HBITMAP hbm;
	dgl_Color* data;
	BITMAPINFO bmi;
} _Dib_Buffer;

BITMAPINFO _get_bitmap_info(int width, int height) {
	BITMAPINFOHEADER bmi_h = {0};
	bmi_h.biSize = sizeof(BITMAPINFOHEADER);
	bmi_h.biWidth = width;
	bmi_h.biHeight = -height;
	bmi_h.biPlanes = 1;
	bmi_h.biBitCount = 32;
	bmi_h.biCompression = BI_RGB;
	
	BITMAPINFO bmi = {0};
	bmi.bmiHeader = bmi_h;

	return bmi;
}

_Dib_Buffer _create_dib_buffer(int width, int height){
	BITMAPINFO bmi = _get_bitmap_info(width, height);
	_Dib_Buffer gb;
	gb.bmi = bmi;
	gb.hbm = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&gb.data, (HANDLE)NULL, (DWORD)NULL);
	return gb;
}

HBITMAP _bitmap_handle = NULL;
_Dib_Buffer _front_buffer = {0};

void _windows_init(){
	init();

	if(window.width <= 0) window.width = DEFAULT_WINDOW_WIDTH;
	if(window.height <= 0) window.height = DEFAULT_WINDOW_HEIGHT;

	window.canvas = dgl_create_canvas(window.width, window.height);
	window.back_canvas = dgl_create_canvas(window.width, window.height);
}

void _windows_start(){
	start();
}

int _cursor_correction_x = 0;
int _cursor_correction_y = 0;
void _windows_update(HWND window_handle, float dt){
	GetCursorPos((POINT *)&cursor_pos);
	ScreenToClient(window_handle, (POINT *)&cursor_pos);

#ifndef DGL_USE_AUTOSCALE
	cursor_pos.x = cursor_pos.x - DGL_COORDINATE_CENTER_X;
	cursor_pos.y = DGL_TRANSFORM_COORDINATES_Y(cursor_pos.y, window.canvas.height);
#else
	// Allow small error in order to avoid float arithmetic.
	_cursor_correction_x = (cursor_pos.x * window.canvas.width)/window.width;
	_cursor_correction_y = (cursor_pos.y * window.canvas.height)/window.height;
	cursor_pos.x = _cursor_correction_x - DGL_COORDINATE_CENTER_X;
	cursor_pos.y = DGL_TRANSFORM_COORDINATES_Y(_cursor_correction_y, window.canvas.height);
#endif

	memcpy(window.canvas.pixels, window.back_canvas.pixels, sizeof(dgl_Color)*window.canvas.width*window.canvas.height);

	update(dt);

	memcpy(_front_buffer.data, window.canvas.pixels, sizeof(dgl_Color)*window.canvas.width*window.canvas.height);
}

void _windows_end(){
	end();
}

LRESULT CALLBACK _window_procedure(HWND, UINT, WPARAM, LPARAM);
int APIENTRY WinMain(HINSTANCE instance_handle, HINSTANCE prev_instance_handle, LPSTR cmd_line, int cmd_show){
	LARGE_INTEGER performance_frequency;
	QueryPerformanceFrequency(&performance_frequency);

	_windows_init();

	MSG msg = {0};
	HWND hwnd = {0};
	WNDCLASSEX wc = {0};
	
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.lpszClassName	= L"Window";
    wc.hInstance		= instance_handle;
    wc.lpszMenuName		= NULL;
	wc.hbrBackground    = CreateSolidBrush(COLOR_WINDOW);
	wc.hCursor          = LoadCursor(0, IDC_ARROW);
    wc.lpfnWndProc		= _window_procedure;

	RegisterClassEx(&wc);

	RECT rect = {0};
	rect.left = 0;
	rect.top = 0;
	rect.right = window.width;
	rect.bottom = window.height;
	AdjustWindowRectEx(&rect, WS_TILEDWINDOW, 0, WS_EX_OVERLAPPEDWINDOW);

	hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
						  wc.lpszClassName,
						  L"Engine",
						  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						  100, 100,
						  rect.right - rect.left, rect.bottom - rect.top,
						  0, 0, instance_handle, 0);

	if(hwnd != 0){
		ShowWindow(hwnd, cmd_show);
	
		LARGE_INTEGER ticks_start;
		LARGE_INTEGER ticks_end;
	
		QueryPerformanceCounter(&ticks_start);
		QueryPerformanceCounter(&ticks_end);

		float time_acc = 0;
		float time_slice = 0;

		time_slice = 1.0/fps;
		time_slice -= 0.01*time_slice;	// Experimental adjustment

		while(running) {
			while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(msg.message != WM_QUIT) {
				ticks_start = ticks_end;
				QueryPerformanceCounter(&ticks_end);

				time_acc += (float)(ticks_end.QuadPart - ticks_start.QuadPart)/(float)performance_frequency.QuadPart;
			
				// Update things after accumulated time
				if(time_acc > time_slice){
					_windows_update(hwnd, time_acc);
					InvalidateRect(hwnd, NULL, FALSE);
				
					time_acc = 0;
				}
			}
		}
	}
	
	return (int)msg.wParam;
}

PAINTSTRUCT	 _ps;
HDC			 _hdc, _hdc_temp;
RECT         _rect;
LRESULT CALLBACK _window_procedure(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param){
    switch(msg) {
	case WM_CREATE:
		{
			_windows_start();
			
			_front_buffer = _create_dib_buffer(window.canvas.width, window.canvas.height);
			_bitmap_handle = _front_buffer.hbm;

		} break;
	case WM_PAINT:
		{
			GetClientRect(window_handle, &_rect);
			window.width = _rect.right - _rect.left;
			window.height = _rect.bottom - _rect.top;

			_hdc = BeginPaint(window_handle, &_ps);

#ifndef DGL_USE_AUTOSCALE
			_hdc_temp = CreateCompatibleDC(_hdc);
			SelectObject(_hdc_temp, _bitmap_handle);
			BitBlt(_hdc, 0, 0, window.canvas.width, window.canvas.height, _hdc_temp, 0, 0, SRCCOPY);
			DeleteDC(_hdc_temp);
#else // TODO: Maybe write own scaler in the future, instead of using windows defined one.
			StretchDIBits(_hdc,
			 			  0, 0, window.width, window.height,
			 			  0, 0, window.canvas.width, window.canvas.height,
			 			  window.canvas.pixels, &_front_buffer.bmi,
			 			  DIB_RGB_COLORS, SRCCOPY);
#endif
				   
			EndPaint(window_handle, &_ps);		
		} break;
	case WM_CLOSE:
		{
			running = false;
		} break;
	case WM_DESTROY:
		{
			_windows_end();
			running = false;
			PostQuitMessage(0);
		} break;
    }

    return DefWindowProc(window_handle, msg, w_param, l_param);
}

#endif // DGL_USE_ENGINE::DGL_TARGET_WINDOWS

#endif // DGL_USE_ENGINE

