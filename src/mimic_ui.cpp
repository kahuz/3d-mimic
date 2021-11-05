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

enum _EnableToolIdx {
    kMOVE = 0, //"../resource/icons/move_icon.png"
    kROTATE //"../resource/icons/rotate_icon.png"
}typedef EnableToolIdx;
vector<string> v_icon_path = { "../resource/icons/move_icon.png", "../resource/icons/rotate_icon.png"};
vector<IMGResourceInfo> v_icon_info;

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

void InitIconResource()
{
    for (int i = 0; i < v_icon_path.size(); i++)
    {
        IMGResourceInfo new_icon;

        int imgChannels;
        void* img_data = stbi_load(v_icon_path[i].c_str(), &new_icon.img_width, &new_icon.img_height, &imgChannels, 0);

        new_icon.tex_id = GetTextureId(img_data, new_icon.img_width, new_icon.img_height, imgChannels);

        v_icon_info.push_back(new_icon);
    }
}

void InitUI()
{
    InitIconResource();
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
    ImGui::MenuItem("(demo menu)", NULL, false, false);
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O"))
    {
        nfdchar_t* outPath = NULL;
        nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);

        if (result == NFD_OKAY)
        {
            LoadObjectFile(&g_model, outPath);
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
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}

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
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}