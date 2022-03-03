#pragma once

#ifndef __GL_MIMIC_RENDERER_H__
#define __GL_MIMIC_RENDERER_H__
// system header
#include <stdio.h>
// imgui header
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// user header
#include "gl_manager.h"
#include <map>
#include <string>

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <memory>

#include "Logger.h"
#include <cmath>
using namespace std;
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include <nfd.h>
#include "linmath.h"
// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

bool InitRenderContext(GLFWwindow* window);
void MimicRender();

#endif //#ifndef __GL_MIMIC_RENDERER_H__