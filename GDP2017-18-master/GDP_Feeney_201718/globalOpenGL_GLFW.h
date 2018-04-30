#ifndef _globalOpenGL_GLFW_HG_
#define _globalOpenGL_GLFW_HG_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// These are here as they are set within the GLFW call back. 
// (The thinking is that anything that sets these call backs would 
//  HAVE to include this file, anyway.)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void window_size_callback(GLFWwindow* window, int width, int height);

#include "Error/COpenGLError.h"


#endif
