
// user header
#include "mimic_renderer.h"
#include "mimic_ui.h"
#include "Logger.h"

using namespace std;

static GLFWwindow *window = NULL;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool InitRenderContext()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    
    if (!glfwInit())
    {
        return false;
    }
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    window = glfwCreateWindow(640, 640, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
    {
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;

    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return false;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void MimicRender()
{
    
    InitRenderContext();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //init GLSL Program
    GLShader* my_gl = new GLShader();

    my_gl->LoadShader(GL_VERTEX_SHADER, "../src/gl_shader/object.vshader");
    my_gl->LoadShader(GL_FRAGMENT_SHADER, "../src/gl_shader/object.fshader");
    my_gl->LinkShaders();
    //set vertex Attribute vPosition
    my_gl->SetGLAttribLocation(GL_VERTEX_SHADER, "vPosition");
    my_gl->SetGLUniformLocation(GL_VERTEX_SHADER, "uProjection");
    my_gl->SetGLUniformLocation(GL_VERTEX_SHADER, "uModel");
    my_gl->SetGLUniformLocation(GL_VERTEX_SHADER, "uView");

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

        DrawMenuBar();

        static int x = 0, y = 0, frame_num = 0;
        static float angle = 90, cur_x_pos = 0, prev_x_pos = 0, cur_y_pos = 0, prev_y_pos = 0;
        static float angle_x = 0.0f, angle_y = 0.0f;
        static bool active_rotate = false;
        static std::string rotate_btn_str = "Disable Rotate";
        int mouse_state = 0; // 0 release, 1 press

        ImGuiIO& io = ImGui::GetIO();
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;

            if (ImGui::Button(rotate_btn_str.c_str()))
            {
                active_rotate = !active_rotate;

                if (active_rotate)
                {
                    rotate_btn_str = "Enable Rotate";
                }
                else
                {
                    rotate_btn_str = "Disable Rotate";
                }
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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

            std::string mouse_state_str = mouse_state ? "Clicked" : "Released";
            ImGui::Text("Mouse state %s", mouse_state_str.c_str());
            ImGui::Text("angle x %f", angle_x);
            ImGui::Text("angle y %f", angle_y);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        if (g_menu_load_model)
        {
            frame_num++;

            if (frame_num % 33 == 0)
            {
                angle++;
            }
            GLfloat mProjeMat[16] =
            {
                0.3f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.3f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.3f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            GLfloat mViewMat[16] =
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            GLfloat mModelMat[16] =
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            if (mouse_state == 1 && active_rotate)
            {
                cur_x_pos = io.MousePos.x;
                cur_y_pos = io.MousePos.y;

                if (cur_x_pos != prev_x_pos)
                {
                    angle_x += std::fmod((cur_x_pos - prev_x_pos), 360.0f) / 20;

                    if (angle_x > 360.0f || angle_x < -360.0f)
                    {
                        angle_x = 0.0f;
                    }
                }
                if (cur_y_pos != prev_y_pos)
                {
                    angle_y += std::fmod((cur_y_pos - prev_y_pos), 360.0f) / 20;

                    if (angle_y > 360.0f || angle_y < -360.0f)
                    {
                        angle_y = 0.0f;
                    }
                }
            }

            float x_s = sinf(angle_y);
            float x_c = cosf(angle_y);

            float y_s = sinf(angle_x);
            float y_c = cosf(angle_x);

            GLfloat mRotate_X_Mat[16] =
            {
                1.f, 0.f, 0.f, 0.f,
                0.f,   x_c,   x_s, 0.f,
                0.f,  -x_s,   x_c, 0.f,
                0.f, 0.f, 0.f, 1.f
            };

            GLfloat mRotate_Y_Mat[16] = {
                   y_c, 0.f,  -y_s, 0.f,
                 0.f, 1.f, 0.f, 0.f,
                   y_s, 0.f,   y_c, 0.f,
                 0.f, 0.f, 0.f, 1.f
            };

            //linmath test
            glViewport(0, 0, display_w, display_h);

            glUseProgram(my_gl->program);

            glUniformMatrix4fv(my_gl->vert_member.at("uProjection"), 1, GL_FALSE, mProjeMat);
            //glUniformMatrix4fv(my_gl->vert_member.at("uView"), 1, GL_FALSE, mViewMat);
            glUniformMatrix4fv(my_gl->vert_member.at("uView"), 1, GL_FALSE, mRotate_X_Mat);

            //glUniformMatrix4fv(my_gl->vert_member.at("uModel"), 1, GL_FALSE, mModelMat);
            glUniformMatrix4fv(my_gl->vert_member.at("uModel"), 1, GL_FALSE, mRotate_Y_Mat);

            glEnableVertexAttribArray(my_gl->vert_member.at("vPosition"));
            glVertexAttribPointer(my_gl->vert_member.at("vPosition"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), &g_model.positions[0]);

            glLineWidth(3);

            glDrawElements(GL_LINES, g_model.v_faces.size(), GL_UNSIGNED_INT, &g_model.v_faces[0]);

            prev_x_pos = cur_x_pos;
            prev_y_pos = cur_y_pos;
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();


}