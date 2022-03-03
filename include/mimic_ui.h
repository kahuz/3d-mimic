#pragma once

#ifndef __GL_MIMIC_UI_H__
#define __GL_MIMIC_UI_H__
// user header
#include "gl_manager.h"
#include "linmath.h"
#include <string>
#include <vector>

extern bool g_menu_load_model;
extern bool g_active_rotate;
extern GL3DObj g_model;
extern std::vector<GL3DObj> v_models;

#define DRAW_DEFAULT_UV_MIN		ImVec2(0.0f, 0.0f)
#define DRAW_DEFAULT_UV_MAX		ImVec2(1.0f, 1.0f)
#define DRAW_DEFAULT_TINT		ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define DRAW_DEFAULT_BORDER		ImVec4(1.0f, 1.0f, 1.0f, 0.0f)

#define UI_TOOLBAR_ICON_W 24
#define UI_TOOLBAR_ICON_H 24

struct _GLCamera{
    vec3 eye = {2.0f, 1.0f, 0.0f};
    vec3 center = {0.0f, 0.0f, 0.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
};

struct _GLLight{
    vec3 pos = {0.0f, 0.0f, 0.0f};
    vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    float ambient_value = 1.0f;
};

struct _ExternalSettings{
    _GLLight light;
    _GLCamera camera;
    vec2 angle = {0.0f, 0.0f};
    vec3 scale = {0.4f, 0.4f, 0.4f};
    vec3 move = {0.0f, 0.0f, 0.0f};

    vec4 bg_color = {0.45f , 0.55f, 0.60f, 1.0f};
    vec4 obj_color = {1.0f, 1.0f, 1.0f, 1.0f};
};

struct _IMGResourceInfo {
    std::string img_path;
    unsigned char* img_data;
    int img_width;
    int img_height;
    GLuint tex_id;
};

extern _ExternalSettings g_extern_settings;
extern _IMGResourceInfo g_bg_info;

void InitUI();
void GlobalMouseEvent();
void GlobalKeyboardEvent();
void DrawExternalSettingView();
void DrawToolBar();
void DrawFileMenuBar();
void DrawMenuBar();

#endif //#ifndef __GL_MIMIC_UI_H__