#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>
#include <time.h>
#include <math.h>

#define DGL_TARGET_WINDOWS
#define DGL_AALIAS_DEG 2
#define DGL_COORDINATE_CENTER_X 0
#define DGL_COORDINATE_CENTER_Y 0
#define DGL_HEADER_IMPLEMENTATION
#include "dgl.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
uint32_t back_pixels[WINDOW_WIDTH * WINDOW_HEIGHT];

dgl_Canvas canvas = {
	.pixels = pixels,
	.width = WINDOW_WIDTH,
	.height = WINDOW_HEIGHT,
	.stride = WINDOW_WIDTH
};

dgl_Canvas back_canvas = {
	.pixels = back_pixels,
	.width = WINDOW_WIDTH,
	.height = WINDOW_HEIGHT,
	.stride = WINDOW_WIDTH
};

// Both WINAPI and CALLBACK are typedefs for "__stdcall" calling convention
// The idea is to use CALLBACK to distinguish callback functions

// SelectObject selects object into given context
// GetObject copies specified number of object bytes from object pointed by handle to given buffer

typedef struct{
	HBITMAP hbm;
	uint32_t* data;
}dgl_Dib_Buffer;

dgl_Dib_Buffer dgl_create_dib_buffer(int width, int height){
	BITMAPINFOHEADER bmi_h = {};
	bmi_h.biSize = sizeof(BITMAPINFOHEADER);
	bmi_h.biWidth = width;
	bmi_h.biHeight = -height;	// windows stores bitmaps from bottom to top and we don't want that, so we invert it
	bmi_h.biPlanes = 1;
	bmi_h.biBitCount = 32;
	bmi_h.biCompression = BI_RGB;
	
	BITMAPINFO bmi = {};
	bmi.bmiHeader = bmi_h;
	
	dgl_Dib_Buffer gb;
	// We don't need first parameter if we use DIB_RGB_COLORS
	// DIB is internal windows representation for Bitmap that is device independent (Device Independent Bitmap)
	// WINDOWS CONVENTION FOR PIXEL CHANNELS ORDER FOR "DIB" IS BGR (0x00RRGGBB)
	// This means that if we modify buffer with our library to RED color, it will be seen as BLUE in DIB format
	gb.hbm = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&gb.data, (HANDLE)NULL, (DWORD)NULL);

	return gb;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HBITMAP hBitmap = NULL;
dgl_Dib_Buffer front_buffer = {};

LARGE_INTEGER performance_frequency;
LARGE_INTEGER ticks_start;
LARGE_INTEGER ticks_end;
LARGE_INTEGER var_fps_ticks_start;
LARGE_INTEGER var_fps_ticks_end;

int fps = 120;
float time_slice = 0;
float time_acc = 0;
float var_fps;
float deltaTime = 0;
char temp1[30];
char temp2[20];
POINT cursorPos = {};

dgl_Point3D focus = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 300};

dgl_Simple_Model *loaded_sm;
dgl_Point3D loaded_sm_translation = { WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 70, -50 };
dgl_Point3D loaded_sm_inverse_translation = { -WINDOW_WIDTH/2, -WINDOW_HEIGHT/2 + 70, 50 };
float loaded_sm_angle_x = 1;
float loaded_sm_angle_y = 2;
float loaded_sm_angle_z = 3;

// Initialize back pixels which represent static background
void start(){
	loaded_sm = dgl_load_simple_model("models/hand.obj", true);
	//loaded_sm = dgl_cull_reduce_simple_model(loaded_sm, 4);
	loaded_sm->colors[0] = DGL_RGBA(0, 255, 0, 50);

	dgl_scale_simple_model(loaded_sm, 120);
	dgl_rotate_simple_model(loaded_sm, -3.1415/2, 0, 0);
	dgl_translate_simple_model(loaded_sm, loaded_sm_translation);
	
	dgl_clear(&back_canvas, DGL_BLACK);

	//dgl_draw_simple_model_mesh(&back_canvas, loaded_sm, focus);
	
}

void update(HWND hwnd, float dt){
	// To measure drawing time so that we can optimize library
	QueryPerformanceCounter(&var_fps_ticks_start);
	
	// Update data
	{
  		dgl_translate_simple_model(loaded_sm, loaded_sm_inverse_translation);
  		dgl_rotate_simple_model(loaded_sm, loaded_sm_angle_x*dt, loaded_sm_angle_y*dt, loaded_sm_angle_z*dt);
  		dgl_translate_simple_model(loaded_sm, loaded_sm_translation);		

		GetCursorPos(&cursorPos);
		ScreenToClient(hwnd, &cursorPos);
	}
	
	// Draw
	{
		// Instead of using dgl_clear, we clear screen by setting it to static background that we initialized in start()
		// dgl_clear(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, DGL_BLACK);
		memcpy(canvas.pixels, back_canvas.pixels, sizeof(canvas.pixels[0])*canvas.width*canvas.height);
	    
		//dgl_draw_simple_model(&canvas, &tetrahedron, focus);
		dgl_draw_simple_model_mesh(&canvas, loaded_sm, focus);
		
		// Targeted FPS
		dgl_draw_text(&canvas, strcat(itoa(fps, temp1, 10), " fps, ideal"), 5, WINDOW_HEIGHT - 5, &dgl_Default_Font, 2, false, DGL_GREEN);
		
		// Actual FPS
		dgl_draw_text(&canvas, strcat(itoa((int)(1.0/dt), temp1, 10), " fps, raw"), 5, WINDOW_HEIGHT - 30, &dgl_Default_Font, 2, false, DGL_CYAN);
		
		// Cursor position
		dgl_draw_text(&canvas, strcat(strcat(itoa(cursorPos.x, temp1, 10), ","), itoa(cursorPos.y, temp2, 10)), WINDOW_WIDTH - 120, WINDOW_HEIGHT - 5, &dgl_Default_Font, 2, false, DGL_RED);
	}
	
	QueryPerformanceCounter(&var_fps_ticks_end);
	
	// Variable FPS
	var_fps = ((float)(var_fps_ticks_end.QuadPart - var_fps_ticks_start.QuadPart)/(float)performance_frequency.QuadPart);
	dgl_draw_text(&canvas, strcat(itoa((int)(1.0/var_fps), temp1, 10), " fps, var"), 5, WINDOW_HEIGHT - 55, &dgl_Default_Font, 2, false, DGL_YELLOW);
	
	memcpy(front_buffer.data, canvas.pixels, sizeof(canvas.pixels[0])*canvas.width*canvas.height);
}

void end(){
	dgl_delete_simple_model(loaded_sm);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow){
	// The value stays the same so it is enough to load it once
	QueryPerformanceFrequency(&performance_frequency);
	
	hPrevInstance = hPrevInstance;	// JUST TO KEEP COMPILER QUIET
	pCmdLine = pCmdLine;			// JUST TO KEEP COMPILER QUIET

	MSG msg;
	HWND hwnd;
	WNDCLASSW wc;
	
	wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.lpszClassName = L"Window";
    wc.hInstance     = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName  = NULL;
    wc.lpfnWndProc   = WndProc;	// Here we register window procedure that should be called
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassW(&wc);
	
	hwnd = CreateWindowW(wc.lpszClassName, L"Engine",
						 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);	// This sends WM_PAINT message (WM_PAINT is sent by the system)
	
	time_slice = 1.0/fps;
	time_slice -= 0.01*time_slice;	// THIS IS EXPERIMENTAL ADJUSTMENT (so that we get precisely defined FPS)
	QueryPerformanceCounter(&ticks_start);
	QueryPerformanceCounter(&ticks_end);
	// TODO: Add start and end functions (before and after game loop)(what is efficient way of sharing info between these functions?)
	//		 Or maybe put these functions in corresponding switch cases in window proc
	// GAME LOOP
	while(msg.message != WM_QUIT){
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
			time_acc += (float)(ticks_end.QuadPart - ticks_start.QuadPart)/(float)performance_frequency.QuadPart;
			
			// Update things after time has been accumulated and prepare for next accumulation
			if(time_acc > time_slice){
				update(hwnd, time_acc);
				InvalidateRect(hwnd, NULL, FALSE);
				
				time_acc = 0;
			}
		}
		
		ticks_start = ticks_end;
		QueryPerformanceCounter(&ticks_end);
	}
	
	return (int)msg.wParam;
}

PAINTSTRUCT ps;
HDC 			hdc;
HDC             hdc_temp;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	// All messages that we don't handle are sent to "DefWindowProcW" default window procedure for processing
    switch(msg) {
	case WM_CREATE:
		// Fill back pixels
		start();
		memcpy(canvas.pixels, back_canvas.pixels, sizeof(canvas.pixels[0])*canvas.width*canvas.height);
		
		front_buffer = dgl_create_dib_buffer(WINDOW_WIDTH, WINDOW_HEIGHT);
		memcpy(front_buffer.data, canvas.pixels, sizeof(canvas.pixels[0])*canvas.width*canvas.height);
		hBitmap = front_buffer.hbm;
		
		break;
	case WM_PAINT:
		; // Empty statement. This is because prior to C99, C doesn't allow declaration to follow label

		hdc = BeginPaint(hwnd, &ps);
		
		hdc_temp = CreateCompatibleDC(hdc);
		SelectObject(hdc_temp, hBitmap);
		BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_temp, 0, 0, SRCCOPY);
		DeleteDC(hdc_temp);
		
		EndPaint(hwnd, &ps);
		
		break;
	case WM_DESTROY:	// Used to terminate window
		DeleteObject(front_buffer.hbm);
		DeleteObject(hBitmap);
		PostQuitMessage(0);	// Generate WM_QUIT message and send it to queue (used to terminate application)

		end();
		
		break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}


