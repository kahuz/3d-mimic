#include "mimic_ui.h"

// system header
#include <string>
#include <memory>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// imgui header
#include "imgui.h"
// user header
#include "Logger.h"
#include "nfd.h"

using namespace std;

bool g_menu_load_model = false;
bool g_active_rotate = false;
// FIXME :: remove under line !!!
GL3DObj g_model;
vector<GL3DObj> v_models;

enum _EnableToolIdx {
    kMOVE = 0, //"../resource/icons/move_icon.png"
    kROTATE //"../resource/icons/rotate_icon.png"
}typedef EnableToolIdx;
vector<string> v_icon_path = { "../../resource/icons/move_icon.png", "../../resource/icons/rotate_icon.png"};
vector<_IMGResourceInfo> v_icon_info;

string g_bg_path = "../../resource/textures/bg_grid.png";

_ExternalSettings g_extern_settings;
_IMGResourceInfo g_bg_info;

// UI 관련 전역 함수 모음

GLuint GetTextureId(const void* img_data, int width, int height, int img_channel)
{
    GLint prev_texture;
    GLuint tex_id;
    //현재 그리고(Rendering) 있는 텍스처 아이디를 가져온다.
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_texture);

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    switch (img_channel)
    {
    case 3: // RGB
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
        break;

    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
        break;
    default:
        break;
    }
    // Restore state
    // 위에서 연결된 icon 텍스처가 아닌 원래 텍스터를 다시 연결해준다.
    glBindTexture(GL_TEXTURE_2D, prev_texture);

    return tex_id;
}

void OpenFileDialog()
{
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);

    if (result == NFD_OKAY)
    {
        // LoadObjectFile(&g_model, outPath);
        LoadObjectsFile(&v_models, outPath);

        g_menu_load_model = true;
        free(outPath);
    }
    else if (result == NFD_CANCEL)
    {
    }
    else
    {
        Log("Error: %s", NFD_GetError());
    }
}

void InitBackGround()
{
    int imgChannels;
    void* tmp_img_data;
    tmp_img_data = stbi_load(g_bg_path.c_str(), &g_bg_info.img_width, &g_bg_info.img_height, &imgChannels, 0);

    g_bg_info.img_data = (unsigned char*)malloc(sizeof(unsigned char) * g_bg_info.img_width * g_bg_info.img_height * imgChannels);
    
    memcpy((void *)g_bg_info.img_data, tmp_img_data, g_bg_info.img_width * g_bg_info.img_height * imgChannels);

    g_bg_info.tex_id = GetTextureId(g_bg_info.img_data, g_bg_info.img_width, g_bg_info.img_height, imgChannels);
}

void InitIconResource()
{
    for (int i = 0; i < v_icon_path.size(); i++)
    {
        _IMGResourceInfo new_icon;

        int imgChannels;
        void *img_data = stbi_load(v_icon_path[i].c_str(), &new_icon.img_width, &new_icon.img_height, &imgChannels, 0);

        new_icon.tex_id = GetTextureId(img_data, new_icon.img_width, new_icon.img_height, imgChannels);

        v_icon_info.push_back(new_icon);
    }
}

void InitUI()
{
    InitIconResource();
    InitBackGround();
}

void GetAngleWithPos(vec2 angle, ImVec2 cur_pos, ImVec2 prev_pos)
{
    if (cur_pos.x != prev_pos.x)
    {
        angle[0] += std::fmod((cur_pos.x - prev_pos.x), 360.0f) / 20;

        if (angle[0] > 360.0f || angle[0] < -360.0f)
        {
            angle[0] = 0.0f;
        }
    }
    if (cur_pos.y != prev_pos.y)
    {
        angle[1] += std::fmod((cur_pos.y - prev_pos.y), 360.0f) / 20;

        if (angle[1] > 360.0f || angle[1] < -360.0f)
        {
            angle[1] = 0.0f;
        }
    }
}

void GlobalMouseEvent()
{
    static ImVec2 prev_pos = {0.0f, 0.0f};
    static int mouse_state = 0; // 0 release, 1 press

    ImGuiIO &io = ImGui::GetIO();

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        if (ImGui::IsMouseDown(i))
        {
            mouse_state = 1;
        }
    }

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        if (ImGui::IsMouseReleased(i))
        {
            mouse_state = 0;
        }
    }

    if(io.MouseWheel < 0.0)
    {
        if(g_extern_settings.camera.zoom > 3)
        {
            g_extern_settings.camera.zoom -= 3;
        }
    }
    else if ( io.MouseWheel > 0.0)
    {
        if(g_extern_settings.camera.zoom < 87)
        {
            g_extern_settings.camera.zoom += 3;
        }
    }

    if (mouse_state == 1 && g_active_rotate)
    {
        GetAngleWithPos(g_extern_settings.angle ,io.MousePos, prev_pos);
    }

    prev_pos = io.MousePos;
}

void GlobalKeyboardEvent()
{
    auto &io = ImGui::GetIO();

    if(io.KeyCtrl)
    {
        if(ImGui::IsKeyPressed(ImGuiKey_O))
        {
            OpenFileDialog();
        }
    }

}

void DrawExternalSettingView()
{
    ImGui::Begin("Renderer Settings");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::PushItemWidth(200.0f);
    ImGui::Checkbox("GL Depth", &g_extern_settings.option.active_depth);
    ImGui::SameLine();
    ImGui::Checkbox("GL Blend", &g_extern_settings.option.active_blend);

    ImGui::SliderFloat("Move trans  X ", &g_extern_settings.move[0], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine(); 
    ImGui::SliderFloat("Move trans  Y ", &g_extern_settings.move[1], -10.0f, 10.0f);  // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine();                                                           
    ImGui::SliderFloat("Move trans  Z ", &g_extern_settings.move[2], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f

    ImGui::SliderFloat("Angle trans X ", &g_extern_settings.angle[0], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine(); 
    ImGui::SliderFloat("Angle trans Y ", &g_extern_settings.angle[1], -10.0f, 10.0f);  // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine(); 
    ImGui::SliderFloat("Angle trans Z ", &g_extern_settings.angle[2], -10.0f, 10.0f);  // Edit float using a slider from -10.0f to 10.0f

    ImGui::SliderFloat("Scale trans X ", &g_extern_settings.scale[0], 0.0f, 1.0f); // Edit float using a slider from 0.0f to 1.0f
    ImGui::SameLine(); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("Scale trans Y ", &g_extern_settings.scale[1], 0.0f, 1.0f); // Edit float using a slider from 0.0f to 1.0f
    ImGui::SameLine();                                                            
    ImGui::SliderFloat("Scale trans Z ", &g_extern_settings.scale[2], 0.0f, 1.0f); // Edit float using a slider from 0.0f to 1.0f

    ImGui::SliderFloat("Eye trans X   ", &g_extern_settings.camera.eye[0], -100.0f, 100.0f); // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine(); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("Eye trans Y   ", &g_extern_settings.camera.eye[1], -100.0f, 100.0f); // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine();                                                                 
    ImGui::SliderFloat("Eye trans Z   ", &g_extern_settings.camera.eye[2], -100.0f, 100.0f); // Edit float using a slider from -10.0f to 10.0f

    ImGui::SliderFloat("Center trans X", &g_extern_settings.camera.center[0], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine(); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("Center trans Y", &g_extern_settings.camera.center[1], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine();                                                                     
    ImGui::SliderFloat("Center trans Z", &g_extern_settings.camera.center[2], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f

    ImGui::SliderFloat("Up Vec trans X", &g_extern_settings.camera.up[0], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine(); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("Up Vec trans Y", &g_extern_settings.camera.up[1], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f
    ImGui::SameLine();                                                                 
    ImGui::SliderFloat("Up Vec trans Z", &g_extern_settings.camera.up[2], -10.0f, 10.0f); // Edit float using a slider from -10.0f to 10.0f
    
    ImGui::SliderFloat("Light Pos X", &g_extern_settings.light.pos[0], -1000.0f, 1000.0f); // Edit float using a slider from -1.0f to 1.0f
    ImGui::SameLine(); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("Light Pos Y", &g_extern_settings.light.pos[1], -1000.0f, 1000.0f); // Edit float using a slider from -1.0f to 1.0f
    ImGui::SameLine();                                                                 
    ImGui::SliderFloat("Light Pos Z", &g_extern_settings.light.pos[2], -1000.0f, 1000.0f); // Edit float using a slider from -1.0f to 1.0f

    ImGui::PopItemWidth();
    ImGui::ColorEdit3("background color", (float *)&g_extern_settings.bg_color);  // Edit 3 floats representing a color
    ImGui::ColorEdit3("  object color  ", (float *)&g_extern_settings.obj_color);         // Edit 3 floats representing a color
    ImGui::ColorEdit3("   light color  ", (float *)&g_extern_settings.light.color); // Edit 3 floats representing a color
    ImGui::SliderFloat(" Ambient value ", &g_extern_settings.light.ambient_value, 0.0f, 1.0f);
    ImGui::SliderFloat(" Specular value ", &g_extern_settings.light.specular_value, 0.0f, 1.0f);



    ImGui::End();
}

void DrawToolBar()
{
    ImGui::Begin("Toolbar");

    ImVec2 img_size(UI_TOOLBAR_ICON_W, UI_TOOLBAR_ICON_H);

    if(ImGui::ImageButton((ImTextureID)(intptr_t)v_icon_info[kMOVE].tex_id, img_size))
    {

    }
    if (ImGui::ImageButton((ImTextureID)(intptr_t)v_icon_info[kROTATE].tex_id, img_size))
    {
        g_active_rotate = !g_active_rotate;
    }

    ImGui::End();
}

void DrawFileMenuBar()
{
    if (ImGui::MenuItem("Open", "Ctrl+O"))
    {
        OpenFileDialog();
    }
}

void DrawMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            DrawFileMenuBar();

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}