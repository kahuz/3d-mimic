#pragma once

#ifndef __GL_MIMIC_UI_H__
#define __GL_MIMIC_UI_H__
// user header
#include "gl_manager.h"

extern bool g_menu_load_model;
// FIXME :: remove under line !!!
extern GL3DObj g_model;

void DrawFileMenuBar();
void DrawMenuBar();

#endif //#ifndef __GL_MIMIC_UI_H__