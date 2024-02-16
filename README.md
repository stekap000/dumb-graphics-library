## Dumb graphics library
### General information
* Library executes code on CPU without parallelization (SIMD support might get added at some point).
* At any point, library contains functions or types that still serve no purpose, or are there just for experimentation.
* Library is header only (```#include "dgl.h"```) with a list of flags and constants that change the behaviour or add additional parts to the program. Use ```#define <flag_name>``` before include statement.\
List of flags and constants:
  * **DGL_IMPLEMENTATION** - Use provided implementations for functions and not just their declarations.
  * **DGL_TARGET_WINDOWS** - Use if you intend to target windows. Reason for this is that windows stores color components as **0xAARRGGBB** and not **0xAABBGGRR** in its internal representation (DIB). This flag will change behaviour of things like **DGL_RGB(r, g, b)**.
  * **DGL_USE_ENGINE** - Use if you want a simple engine that will allow you to preview the results.
  * **DGL_USE_AUTOSCALE** - Use if you want rendered content to scale automatically when you scale window.
  * **DGL_AALIAS_DEG \<number>** - Use this to set the level of anti-aliasing. Number parameter is the dimension of the grid used for supersampling. If number is 2, then grid is 2x2, meaning we take 4 samples. Samples are taken at the center of grid cells. Not all drawing functions have anti-aliasing implemented.
  * **DGL_COORDINATE_CENTER_X \<number>** - Set the X coordinate of the coordinate system center. Number represents X coordinate starting from lower-left corner.
  * **DGL_COORDINATE_CENTER_Y \<number>** - Set the Y coordinate of the coordinate system center. Number represents Y coordinate starting from lower-left corner.

### Compilation and linking

Turn off **unused_parameter**, **unused-function** and **incompatible-pointer-types** when compiling (or equivalent names for different compilers). (optional)

If you are targeting specific system, then specify subsystem depending on compiler. For example, if you are using MSVC, you don't need to specify anything on Windows since it will recognize WinMain entry point, and if you are using GCC and compiling for Windows, you would need to specify ```-Wl,-subsystem,windows``` among other parameters.

If you are using provided prototype engine then you need to link additional libraries. At this point, simple engine exists only for Windows, so you will need to link **gdi32.lib**, **user32.lib** and **kernel32.lib** if it is not linked automatically.

### Simple prototype engine usage

Use this engine to quickly view results of your experimentation.

To use engine, first you need to set **DGL_TARGET_WINDOWS** and **DGL_USE_ENGINE** flags before ```#include "dgl.h"```. In the future there might be some other **DGL_TARGET_XXX**.

Secondly, you need to provide implementation for four functions in the same file (this file doesn't have standard main entrypoint). If you don't provide implementations, you will get linking error. Functions are:
* **init()** - Called once before main window initialization. Use this to set window dimensions with **init_window_params(int width, int height)**, targeted fps with **init_fps(int fps)** and to initialize any custom parameters.
* **start()** - Called once on the first render. Useful if you want to set some static background.
* **update(float dt)** - Called every iteration of the game loop. Main logic goes here.
* **end()** - Called once before application closes. Use this to save some information to file or wherever. You can also use it to release used resources, but there is no need for this, since the OS will do it for you anyway, and you would just be wasting CPU time with that.

When using engine, you have access to ```window``` global variable that has ```canvas``` and ```back_canvas``` to draw to with library functions. ```back_canvas``` content is always layered first (like a background). ```canvas``` content is layered over ```back_canvas``` content. If the background does not change in your case, then it is enough to set ```back_canvas``` once in **start()** function.

With this kind of engine layout, shared state between four main functions will be fully global. With this in mind, this simple visual engine is not serious, but it is good for quick prototypes.

### Examples

Examples are provided in examples folder and the code contained there is written as if it is meant to be run from precisely that folder (for example, when model is loaded from file, file path is relative to that directory).

Examples show some usage cases, but do not cover all functionalities.