#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

dgl_Simple_Model *loaded_sm;
dgl_Point3D loaded_sm_translation = {0};
dgl_Point3D loaded_sm_inverse_translation = {0};
float loaded_sm_angle_x;
float loaded_sm_angle_y;
float loaded_sm_angle_z;

dgl_Mat proj_mat = {0};

void init(){
	int width = 800;
	int height = 600;
	init_window_params(width, height);
	init_fps(-1);

	loaded_sm_angle_x = 1;
	loaded_sm_angle_y = 2;
	loaded_sm_angle_z = 3;
}

void start(){
	dgl_clear(&window.back_canvas, DGL_BLACK);

	// Load model and resize it
	loaded_sm = dgl_load_simple_model("models/icosahedron.obj", true);
	loaded_sm->colors[0] = DGL_RGBA(0, 255, 0, 50);
	dgl_scale_simple_model(loaded_sm, 150);
}

// Just for itoa(...)
char temp1[30];

void update(float dt){
	dgl_Point3D delta = {0};
	int cx = cursor_pos.x;
	int cy = cursor_pos.y;

	// Translate model by cursor movement that happened since previous frame
	delta.x = cx - loaded_sm_translation.x;
	delta.y = cy - loaded_sm_translation.y;
	dgl_translate_simple_model(loaded_sm, delta);

	// Update current translations to represent new cursor position
	loaded_sm_translation.x = cx;
	loaded_sm_translation.y = cy;
	loaded_sm_inverse_translation.x = -cx;
	loaded_sm_inverse_translation.y = -cy;

	// Rotate model around its center
	dgl_translate_simple_model(loaded_sm, loaded_sm_inverse_translation);
	dgl_rotate_simple_model(loaded_sm, loaded_sm_angle_x*dt, loaded_sm_angle_y*dt, loaded_sm_angle_z*dt);
	dgl_translate_simple_model(loaded_sm, loaded_sm_translation);

	// Change color based on coordinates
	loaded_sm->colors[0] = DGL_RGBA(cursor_pos.x % 256,
									cursor_pos.y % 256,
									(cursor_pos.x + cursor_pos.y) % 256,
									50);

	// Draw model
	dgl_draw_simple_model_mesh(&window.canvas, loaded_sm, proj_mat);

	// Draw cursor coordinates
	dgl_draw_text(&window.canvas, itoa(cx, temp1, 10), window.canvas.width - 120, window.canvas.height - 10, &dgl_default_font, 2, false, DGL_GREEN);
	dgl_draw_text(&window.canvas, itoa(cy, temp1, 10), window.canvas.width - 60, window.canvas.height - 10, &dgl_default_font, 2, false, DGL_GREEN);
}
	
void end(){}
