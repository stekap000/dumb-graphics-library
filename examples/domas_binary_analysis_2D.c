#define DGL_TARGET_WINDOWS
#define DGL_USE_ENGINE
#define DGL_USE_AUTOSCALE
#define DGL_IMPLEMENTATION
#include "dgl.h"

FILE *file;
#define READ_WINDOW 4096
unsigned char buffer[READ_WINDOW] = { 0 };
int succ_read = READ_WINDOW;

void init(){
	init_window_params(256, 256);
}

void start(){
	dgl_clear(&window.back_canvas, DGL_BLACK);
	
	file = fopen("../dgl.h", "rb");

	// Cool idea by Chrisopher Domas to visually represent binary data.
	// This is the simplest form where we take 2 adjacent bytes and see
	// them as X and Y coordinate of the point that we paint.
	// Here, we analyse "dgl.h" library and get characteristic pattern
	// for ascii files.
	while(succ_read == READ_WINDOW){
		succ_read = fread(buffer, 1, sizeof(buffer), file);
		
		for(int i = 0; i < succ_read; i += 2){
			if(i+1 < succ_read && i+2 < succ_read) {
				dgl_fill_rect(&window.back_canvas, buffer[i]*2, buffer[i+1]*2, 2, 2, DGL_RGBA(0,255,0,50));
			}
		}
	}

	fclose(file);
}

void update(float dt){}
void end(){}
