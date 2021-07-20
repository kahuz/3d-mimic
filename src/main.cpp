// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

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

using namespace std;
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h> // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h> // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h> // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h> // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE      // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h> // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE        // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h> // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char **)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    const char vShaderStr[] =
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";

    const char fShaderStr[] =
        "precision mediump float;\n"
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
        "}                                            \n";
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#define MY_GL_CODE

#ifdef MY_GL_CODE
    //init GLSL Program
    GLShader *my_gl = new GLShader(vShaderStr, fShaderStr);

    //set vertex Attribute vPosition
    my_gl->SetGLAttribLocation(GL_VERTEX_SHADER, "vPosition");

//	GLint tmp_loc = glGetAttribLocation(my_gl->program, "vPosition");

    
	GL3DObj model = { 0 };
    std::cout << "Hello World!\n";
	string path = "D:/github/3d-mimic/resource/cube.obj";
	std::ifstream obj_file(path, std::ios::in);
	string obj_src;

	if (obj_file.is_open())
	{
		string cur_line;

		while (getline(obj_file, cur_line))
		{

			string type = cur_line.substr(0, 2);

			// 6
			if (type.compare("v ") == 0)
				model.positions++;
			else if (type.compare("vt") == 0)
				model.texels++;
			else if (type.compare("vn") == 0)
				model.normals++;
			else if (type.compare("f ") == 0)
				model.faces++;

			obj_src += "\n" + cur_line;
		}
		obj_file.close();
	}

    model.obj_positions = (float **)malloc(sizeof(float *) * model.positions);
    for(int i = 0; i < model.positions; i++)
    {
        model.obj_positions[i] = (float *)malloc(sizeof(float) * 3);
    }

    model.obj_texels = (float **)malloc(sizeof(float *) * model.texels);
    for(int i = 0; i < model.texels; i++)
    {
        model.obj_texels[i] = (float *)malloc(sizeof(float) * 2);
    }

    model.obj_normals = (float **)malloc(sizeof(float *) * model.normals);
    for(int i = 0; i < model.normals; i++)
    {
        model.obj_normals[i] = (float *)malloc(sizeof(float) * 3);
    }

    model.obj_faces = (float **)malloc(sizeof(float *) * model.faces);
    for(int i = 0; i < model.faces; i++)
    {
        model.obj_faces[i] = (float *)malloc(sizeof(float) * 12);
    }


    // Counters
    int p = 0;
    int t = 0;
    int n = 0;
    int fn = 0;

    obj_file.open(path);
	if (obj_file.is_open())
	{
		string cur_line;

		while (getline(obj_file, cur_line))
		{

			string type = cur_line.substr(0, 2);

			// 6
			if (type.compare("v ") == 0)
            {
                // 1
                // Copy line for parsing
                char *l = new char[cur_line.size() + 1];
                memcpy(l, cur_line.c_str(), cur_line.size() + 1);

                // 2
                // Extract tokens
                std::strtok(l, " ");
                for (int i = 0; i < 3; i++)
                    model.obj_positions[p][i] = atof(std::strtok(NULL, " "));

                // 3
                // Wrap up
                delete[] l;
                p++;
            }
            else if (type.compare("vt") == 0)
            {
                char *l = new char[cur_line.size() + 1];
                memcpy(l, cur_line.c_str(), cur_line.size() + 1);

                std::strtok(l, " ");
                for (int i = 0; i < 2; i++)
                    model.obj_texels[t][i] = atof(std::strtok(NULL, " "));

                delete[] l;
                t++;
            }
            else if (type.compare("vn") == 0)
            {
                char *l = new char[cur_line.size() + 1];
                memcpy(l, cur_line.c_str(), cur_line.size() + 1);

                std::strtok(l, " ");
                for (int i = 0; i < 3; i++)
                    model.obj_normals[n][i] = std::atof(std::strtok(NULL, " "));

                delete[] l;
                n++;
            }
            else if (type.compare("f ") == 0)
            {
                char *l = new char[cur_line.size() + 1];
                memcpy(l, cur_line.c_str(), cur_line.size() + 1);

                std::strtok(l, " ");
                for (int i = 0; i < 12; i++)
                    model.obj_faces[fn][i] = atof(std::strtok(NULL, " /"));

                delete[] l;
                fn++;
            }
        }
        obj_file.close();
	}

    GLfloat *out_obj = new GLfloat[model.faces * 6 * 3];
    //vector<GLfloat> out_obj;
    //out_obj.resize(model.faces * 6 * 3);

    for(int i=0; i< model.faces; i++)
    {
        // 3
        int vA = model.obj_faces[i][0] - 1;
        int vB = model.obj_faces[i][3] - 1;
        int vC = model.obj_faces[i][6] - 1;
        int vD = model.obj_faces[i][9] - 1;

        out_obj[(i*18) + 0] = model.obj_positions[vA][0];
        out_obj[(i*18) + 1] = model.obj_positions[vA][1];
        out_obj[(i*18) + 2] = model.obj_positions[vA][2];
        out_obj[(i*18) + 3] = model.obj_positions[vB][0];
        out_obj[(i*18) + 4] = model.obj_positions[vB][1];
        out_obj[(i*18) + 5] = model.obj_positions[vB][2];
        out_obj[(i*18) + 6] = model.obj_positions[vC][0];
        out_obj[(i*18) + 7] = model.obj_positions[vC][1];
        out_obj[(i*18) + 8] = model.obj_positions[vC][2];
        out_obj[(i*18) + 9] = model.obj_positions[vA][0];
        out_obj[(i*18) + 10] = model.obj_positions[vA][1];
        out_obj[(i*18) + 11] = model.obj_positions[vA][2];
        out_obj[(i*18) + 12] = model.obj_positions[vB][0];
        out_obj[(i*18) + 13] = model.obj_positions[vB][1];
        out_obj[(i*18) + 14] = model.obj_positions[vB][2];
        out_obj[(i*18) + 15] = model.obj_positions[vD][0];
        out_obj[(i*18) + 16] = model.obj_positions[vD][1];
        out_obj[(i*18) + 17] = model.obj_positions[vD][2];
    }
#endif
    static float f = 0.0f;
    static int counter = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);


#ifdef MY_GL_CODE

        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        

        GLfloat vVertices[] = {
            0.0f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f};

	    glUseProgram ( my_gl->program );

        // you must use []. not .at()
        // see more https://gpgstudy.com/forum/viewtopic.php?t=25224
        // char * 대해 at 동작이 가능하도록 수정할 것
        //glEnableVertexAttribArray(my_gl->vert_member["vPosition"]);
        //glVertexAttribPointer(my_gl->vert_member["vPosition"], 3, GL_FLOAT, GL_FALSE, 0, vVertices);
        glEnableVertexAttribArray(my_gl->vert_member.at("vPosition"));
        glVertexAttribPointer(my_gl->vert_member.at("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, out_obj);


        glDrawArrays(GL_LINES, 0, 59);

#endif MY_GL_CODE

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }              
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
