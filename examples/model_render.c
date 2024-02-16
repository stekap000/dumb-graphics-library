#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_IMPLEMENTATION
#include "dgl.h"

dgl_Simple_Model *loaded_sm;
dgl_Point3D loaded_sm_translation;
dgl_Point3D loaded_sm_inverse_translation;
float loaded_sm_angle_x;
float loaded_sm_angle_y;
float loaded_sm_angle_z;

// Here only because drawing function requires it as parameter, but still doesn't use it.
dgl_Mat proj_mat = {0};

void init(){
	int width = 800;
	int height = 800;
	init_window_params(width, height);
	// Any negative value basically means there is no upper limit to FPS imposed by the program.
	init_fps(-1);

	loaded_sm_translation.x = width/2.0f;
	loaded_sm_translation.y = height/2.0f - 70;
	loaded_sm_translation.z = -50;

	loaded_sm_inverse_translation.x = -width/2.0f;
	loaded_sm_inverse_translation.y = -height/2.0f + 70;
	loaded_sm_inverse_translation.z = 50;

	loaded_sm_angle_x = 1;
}

void start(){
	dgl_clear(&window.back_canvas, DGL_BLACK);

	loaded_sm = dgl_load_simple_model("models/hand.obj", true);
	loaded_sm->colors[0] = DGL_RGBA(0, 255, 0, 50);

	dgl_scale_simple_model(loaded_sm, 100);
	dgl_rotate_simple_model(loaded_sm, -3.1415/2, 0, 0);
	dgl_translate_simple_model(loaded_sm, loaded_sm_translation);
}

void update(float dt){
	dgl_translate_simple_model(loaded_sm, loaded_sm_inverse_translation);
	dgl_rotate_simple_model(loaded_sm, loaded_sm_angle_x*dt, loaded_sm_angle_x*dt, loaded_sm_angle_x*dt);
	dgl_translate_simple_model(loaded_sm, loaded_sm_translation);		

	dgl_draw_simple_model_mesh(&window.canvas, loaded_sm, proj_mat);
}
	
void end(){}
