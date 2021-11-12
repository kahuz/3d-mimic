
// user header
#include "mimic_renderer.h"
#include "mimic_ui.h"
#include "Logger.h"

using namespace std;

static GLFWwindow *window = NULL;
//init GLSL Program
GLShader* my_gl;
GLShader* bg_grid;

mat4x4 g_proj_mat =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
}; 

mat4x4 g_model_mat =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

mat4x4 g_view_mat =
{
   0.4f, 0.0f, 0.0f, 0.0f,
   0.0f, 0.4f, 0.0f, 0.0f,
   0.0f, 0.0f, 0.4f, 0.0f,
   0.0f, 0.0f, 0.0f, 1.0f
};

GLushort g_bg_buf_idx[] = { 0, 1, 2, 0, 2, 3 };

GLfloat g_bg_buf[] = {
    -1.0f,  1.0f, 0.0f,  // Position 0
    0.0f,  0.0f,        // TexCoord 0
    -1.0f, -1.0f, 0.0f,  // Position 1
    0.0f,  1.0f,        // TexCoord 1
    1.0f, -1.0f, 0.0f,  // Position 2
    1.0f,  1.0f,        // TexCoord 2
    1.0f,  1.0f, 0.0f,  // Position 3
    1.0f,  0.0f         // TexCoord 3
};

void TransformScale(float scale_x, float scale_y, float scale_z)
{
	if (scale_x > 1.0 || scale_y > 1.0 || scale_z > 1.0)
	{
		cout << "overflow scale value" << endl;
		return;
	}
	else
	{
		g_view_mat[0][0] = scale_x;
		g_view_mat[1][1] = scale_y;
		g_view_mat[2][2] = scale_z;
	}
}
// FIXME:: rename function
void RotateWithPos(ImVec2 pos)
{
    //cur_x_pos = io.MousePos.x;
    //cur_y_pos = io.MousePos.y;
    //
    //if (cur_x_pos != prev_x_pos)
    //{
    //    angle_x += std::fmod((cur_x_pos - prev_x_pos), 360.0f) / 20;
    //
    //    if (angle_x > 360.0f || angle_x < -360.0f)
    //    {
    //        angle_x = 0.0f;
    //    }
    //}
    //if (cur_y_pos != prev_y_pos)
    //{
    //    angle_y += std::fmod((cur_y_pos - prev_y_pos), 360.0f) / 20;
    //
    //    if (angle_y > 360.0f || angle_y < -360.0f)
    //    {
    //        angle_y = 0.0f;
    //    }
    //}
    //
    //float x_s = sinf(angle_y);
    //float x_c = cosf(angle_y);
    //
    //float y_s = sinf(angle_x);
    //float y_c = cosf(angle_x);
    //
    //mat4x4 mRotate_X_Mat =
    //{
    //    1.f, 0.f, 0.f, 0.f,
    //    0.f,   x_c,   x_s, 0.f,
    //    0.f,  -x_s,   x_c, 0.f,
    //    0.f, 0.f, 0.f, 1.f
    //};
    //
    //mat4x4 mRotate_Y_Mat =
    //{
    //    y_c, 0.f,  -y_s, 0.f,
    //     0.f, 1.f, 0.f, 0.f,
    //       y_s, 0.f,   y_c, 0.f,
    //     0.f, 0.f, 0.f, 1.f
    //};
    //
    //mat4x4_mul(g_model_mat, mRotate_X_Mat, mRotate_Y_Mat);
}

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
    window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
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

void DrawBackGround()
{
    glUseProgram(bg_grid->program);

    glUniform4f(bg_grid->vert_member.at("uGlobalColor"), 1.0f, 1.0f, 1.0f, 1.0f);
    glUniformMatrix4fv(bg_grid->vert_member.at("uProjection"), 1, GL_FALSE, (GLfloat *)g_proj_mat);
    glEnableVertexAttribArray(bg_grid->vert_member.at("aPosition"));
    glEnableVertexAttribArray(bg_grid->vert_member.at("aTexCoord"));

    // Load the vertex position
    glVertexAttribPointer(bg_grid->vert_member.at("aPosition"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), g_bg_buf);

    // Load the texture coordinate
    glVertexAttribPointer(bg_grid->vert_member.at("aTexCoord"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), &g_bg_buf[3]);

    // Bind the base map
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, g_bg_info.tex_id);

    glUniform1i(bg_grid->frag_member.at("uTexture0"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, g_bg_buf_idx);
}
void MimicRender()
{
    InitRenderContext();
    InitUI();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    my_gl = new GLShader();
    bg_grid = new GLShader();

    bg_grid->LoadShader(GL_VERTEX_SHADER, "../src/gl_shader/texture.vshader");
    bg_grid->LoadShader(GL_FRAGMENT_SHADER, "../src/gl_shader/texture.fshader");
    bg_grid->LinkShaders();
    //set vertex Attribute vPosition
    bg_grid->SetGLAttribLocation(GL_VERTEX_SHADER, "aTexCoord");
    bg_grid->SetGLUniformLocation(GL_VERTEX_SHADER, "uGlobalColor");
    bg_grid->SetGLUniformLocation(GL_VERTEX_SHADER, "uProjection");
    bg_grid->SetGLAttribLocation(GL_VERTEX_SHADER, "aPosition");

    bg_grid->SetGLUniformLocation(GL_FRAGMENT_SHADER, "uTexture0");

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

        DrawToolBar();
        DrawMenuBar();

        static int x = 0, y = 0, frame_num = 0;
        static float angle = 90, cur_x_pos = 0, prev_x_pos = 0, cur_y_pos = 0, prev_y_pos = 0;
        static float angle_x = 0.0f, angle_y = 0.0f;
		static float scale_x = 0.4f, scale_y = 0.4f, scale_z = 0.4f;
        int mouse_state = 0; // 0 release, 1 press

        ImGuiIO& io = ImGui::GetIO();
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("scale trans X", &scale_x, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("scale trans Y", &scale_y, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("scale trans Z", &scale_z, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;

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
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        
        glViewport(0, 0, display_w, display_h);
        if (g_menu_load_model)
        {
            frame_num++;

            if (frame_num % 33 == 0)
            {
                angle++;
            }
            
            if (mouse_state == 1 && g_active_rotate)
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

            mat4x4 mRotate_X_Mat =
            {
                1.f, 0.f, 0.f, 0.f,
                0.f,   x_c,   x_s, 0.f,
                0.f,  -x_s,   x_c, 0.f,
                0.f, 0.f, 0.f, 1.f
            };

            mat4x4 mRotate_Y_Mat =
            {
                y_c, 0.f,  -y_s, 0.f,
                 0.f, 1.f, 0.f, 0.f,
                   y_s, 0.f,   y_c, 0.f,
                 0.f, 0.f, 0.f, 1.f
            };

            mat4x4_mul(g_model_mat, mRotate_X_Mat, mRotate_Y_Mat);
			TransformScale(scale_x, scale_y, scale_z);
            //linmath test


            for( auto obj_info : v_models)
            {
				glUseProgram(my_gl->program);

				glUniformMatrix4fv(my_gl->vert_member.at("uProjection"), 1, GL_FALSE, (GLfloat *)g_proj_mat);
				glUniformMatrix4fv(my_gl->vert_member.at("uView"), 1, GL_FALSE, (GLfloat *)g_view_mat);
				glUniformMatrix4fv(my_gl->vert_member.at("uModel"), 1, GL_FALSE, (GLfloat *)g_model_mat);

                glEnableVertexAttribArray(my_gl->vert_member.at("vPosition"));
                glVertexAttribPointer(my_gl->vert_member.at("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, &obj_info.positions[0]);

                glLineWidth(3);

                glDrawElements(GL_LINE_STRIP, obj_info.v_faces.size(), GL_UNSIGNED_INT, &obj_info.v_faces[0]);
            }
            prev_x_pos = cur_x_pos;
            prev_y_pos = cur_y_pos;

        }
       // DrawBackGround();
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