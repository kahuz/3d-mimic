#include "mimic_ui.h"

// system header
#include <string>
#include <memory>

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
vector<string> v_icon_path = { "../resource/icons/move_icon.png", "../resource/icons/rotate_icon.png"};
vector<IMGResourceInfo> v_icon_info;

string g_bg_path = "../resource/textures/bg_grid.png";
IMGResourceInfo g_bg_info;
// UI ���� ���� �Լ� ����
GLuint GetTextureId(const void* img_data, int width, int height, int img_channel)
{
    GLint prev_texture;
    GLuint tex_id;
    //���� �׸���(Rendering) �ִ� �ؽ�ó ���̵� �����´�.
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
    // ������ ����� icon �ؽ�ó�� �ƴ� ���� �ؽ��͸� �ٽ� �������ش�.
    glBindTexture(GL_TEXTURE_2D, prev_texture);

    return tex_id;
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
        IMGResourceInfo new_icon;

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
        nfdchar_t* outPath = NULL;
        nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);

        if (result == NFD_OKAY)
        {
            //LoadObjectFile(&g_model, outPath);
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