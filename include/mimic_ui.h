#pragma once

#ifndef __GL_MIMIC_UI_H__
#define __GL_MIMIC_UI_H__
// user header
#include "gl_manager.h"
#include <string>

extern bool g_menu_load_model;
extern bool g_active_rotate;
extern GL3DObj g_model;

#define DRAW_DEFAULT_UV_MIN		ImVec2(0.0f, 0.0f)
#define DRAW_DEFAULT_UV_MAX		ImVec2(1.0f, 1.0f)
#define DRAW_DEFAULT_TINT		ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define DRAW_DEFAULT_BORDER		ImVec4(1.0f, 1.0f, 1.0f, 0.0f)

#define UI_TOOLBAR_ICON_W 24
#define UI_TOOLBAR_ICON_H 24

struct _IMGResourceInfo {
    std::string img_path;
    int img_width;
    int img_height;
    GLuint tex_id;
}typedef IMGResourceInfo;

void InitUI();
void DrawToolBar();
void DrawFileMenuBar();
void DrawMenuBar();

#endif //#ifndef __GL_MIMIC_UI_H__