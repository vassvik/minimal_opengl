# Absolutely Minimal OpenGL Compute Shader Setup

Uses GLFW to create a window and handle window events. 

Uses [Glad](https://glad.dav1d.de/) to load OpenGL, specifically a 4.6 Core profile. 

GLFW 3.4 headers and a precompiled VS2022 dynamic library is bundled for Windows. 

To easily build for Windows just call `build.bat` from an x64 VS2022 command prompt. 

To build for Linux you'll likely know what to do, something to the effect of `gcc main.c -lglfw3` would do with the appropriate libraries installed. 

### NOTE: 

The application does not actually do or display anything, and does not actually run anything. The only thing it does is loading and compiling a single empty compute shader, and closing the window when Escape is pressed. 

The intention of this repository is to have an absolutely bare-bones minimal effort starting point to experiment with compute shaders in a standalone environment. 