
// user header
#include "mimic_renderer.h"
#include "mimic_ui.h"
#include "Logger.h"

using namespace std;

static GLFWwindow *window = NULL;
//init GLSL Program
GLShader* obj_shader;
GLShader* bg_shader;
GLShader* light_shader;

mat4x4 g_obj_proj_mat =
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

mat4x4 g_obj_model_mat =
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

mat4x4 g_obj_view_mat =
{
   0.4f, 0.0f, 0.0f, 0.0f,
   0.0f, 0.4f, 0.0f, 0.0f,
   0.0f, 0.0f, 0.4f, 0.0f,
   0.0f, 0.0f, 0.0f, 1.0f
};
mat4x4 g_light_proj_mat =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
}; 

mat4x4 g_light_model_mat =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

mat4x4 g_light_view_mat =
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

#define PI 3.141592653

float Deg2Rad(float degree)
{
    return degree * PI / 180;
}

// 조명에 사용할 normal matrix를 위해 src_matrix에 대한 inverse_matrix를 생성한다
// 조명을 위해 사용할 때는 model matrix에 대해 inverse_matrix를 생성하고, 이를 normal vector와 곱하여
// 최종적으로는 normal_matrix를 사용하게 된다
// see more : http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
void CreateInverseMatrix(mat4x4 src_matrix, mat4x4 inverse_matrix)
{
	mat4x4_invert(inverse_matrix, src_matrix);
	mat4x4_transpose(inverse_matrix, inverse_matrix);
}
void TransformScale(mat4x4 src_matrix, float scale_x, float scale_y, float scale_z)
{
	if (scale_x > 1.0 || scale_y > 1.0 || scale_z > 1.0)
	{
		cout << "overflow scale value" << endl;
		return;
	}
	else
	{
        mat4x4_scale_aniso(src_matrix, src_matrix, scale_x, scale_y, scale_z);
	}
}

void RotateWithPos(mat4x4 src_matrix, float angle_x, float angle_y)
{
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

	mat4x4_mul(src_matrix, mRotate_X_Mat, mRotate_Y_Mat);
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
    glUseProgram(bg_shader->program);

    glUniform4f(bg_shader->vert_member.at("uGlobalColor"), 1.0f, 1.0f, 1.0f, 1.0f);
    glUniformMatrix4fv(bg_shader->vert_member.at("uProjection"), 1, GL_FALSE, (GLfloat *)g_obj_proj_mat);
    glEnableVertexAttribArray(bg_shader->vert_member.at("aPosition"));
    glEnableVertexAttribArray(bg_shader->vert_member.at("aTexCoord"));

    // Load the vertex position
    glVertexAttribPointer(bg_shader->vert_member.at("aPosition"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), g_bg_buf);

    // Load the texture coordinate
    glVertexAttribPointer(bg_shader->vert_member.at("aTexCoord"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), &g_bg_buf[3]);

    // Bind the base map
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, g_bg_info.tex_id);

    glUniform1i(bg_shader->frag_member.at("uTexture0"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, g_bg_buf_idx);
}
void MimicRender()
{
    InitRenderContext();
    InitUI();

    ImVec4 background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 obj_color = ImVec4(1.0f, 1.0f, 0.0f, 1.00f);
    ImVec4 light_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    static int x = 0, y = 0, frame_num = 0;
    static float angle = 90, cur_x_pos = 0, prev_x_pos = 0, cur_y_pos = 0, prev_y_pos = 0;
    static float angle_x = 0.0f, angle_y = 0.0f;
    static float scale_x = 0.4f, scale_y = 0.4f, scale_z = 0.4f;
    static float eye_x = 1.0f, eye_y = 1.0f, eye_z = 0.0f;
    static float center_x = 0.0f, center_y = 0.0f, center_z = 0.0f;
    static float up_x = 0.0f, up_y = 1.0f, up_z = 0.0f;
    static float ambient_value = 1.0f;
    int mouse_state = 0; // 0 release, 1 press

    obj_shader = new GLShader();
    bg_shader = new GLShader();

    bg_shader->LoadShader(GL_VERTEX_SHADER, "../src/gl_shader/texture.vshader");
    bg_shader->LoadShader(GL_FRAGMENT_SHADER, "../src/gl_shader/texture.fshader");
    bg_shader->LinkShaders();

    // bg_shader Vertex Shader Initialize
    bg_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aPosition");
    bg_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aTexCoord");

    bg_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uGlobalColor");
    bg_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uProjection");

    // bg_shader Fragment Shader Initialize
    bg_shader->SetGLUniformLocation(GL_FRAGMENT_SHADER, "uTexture0");

    obj_shader->LoadShader(GL_VERTEX_SHADER, "../src/gl_shader/object.vshader");
    obj_shader->LoadShader(GL_FRAGMENT_SHADER, "../src/gl_shader/object.fshader");
    obj_shader->LinkShaders();

    // obj_shader Vertex Shader Initialize
    obj_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aPosition");
    obj_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aNormal");
    obj_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aTexCoord");
    
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uProjection");
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uModel");
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uView");

    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uModelTransform");
    // obj_shader Fragment Shader Initialize
    obj_shader->SetGLUniformLocation(GL_FRAGMENT_SHADER, "uLightPosition");
    obj_shader->SetGLUniformLocation(GL_FRAGMENT_SHADER, "uLightColor");
    obj_shader->SetGLUniformLocation(GL_FRAGMENT_SHADER, "uObjectColor");
    obj_shader->SetGLUniformLocation(GL_FRAGMENT_SHADER, "uAmbientStrength");

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


        ImGuiIO& io = ImGui::GetIO();
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {

            ImGui::Begin("Renderer Settings"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::PushItemWidth(200.0f);
            ImGui::SliderFloat("Scale trans X ", &scale_x, 0.0f, 1.0f); ImGui::SameLine();            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Scale trans Y ", &scale_y, 0.0f, 1.0f); ImGui::SameLine();             // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Scale trans Z ", &scale_z, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f

            ImGui::SliderFloat("Eye trans X   ", &eye_x, -2.0f, 2.0f); ImGui::SameLine();            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("Eye trans Y   ", &eye_y, -2.0f, 2.0f); ImGui::SameLine();             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("Eye trans Z   ", &eye_z, -2.0f, 2.0f);             // Edit 1 float using a slider from 0.0f to 1.0f

            ImGui::SliderFloat("Center trans X", &center_x, -2.0f, 2.0f); ImGui::SameLine();            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("Center trans Y", &center_y, -2.0f, 2.0f); ImGui::SameLine();             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("Center trans Z", &center_z, -2.0f, 2.0f);             // Edit 1 float using a slider from 0.0f to 1.0f

            ImGui::SliderFloat("Up Vec trans X", &up_x, -2.0f, 2.0f); ImGui::SameLine();            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("Up Vec trans Y", &up_y, -2.0f, 2.0f); ImGui::SameLine();             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("Up Vec trans Z", &up_z, -2.0f, 2.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::PopItemWidth();
            ImGui::ColorEdit3("background color", (float*)&background_color); // Edit 3 floats representing a color
            ImGui::ColorEdit3("  object color  ", (float*)&obj_color); // Edit 3 floats representing a color
            ImGui::ColorEdit3("   light color  ", (float*)&light_color); // Edit 3 floats representing a color
            ImGui::SliderFloat(" Ambient value ", &ambient_value, 0.0f, 1.0f);

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

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h); 
        glClearColor(background_color.x * background_color.w, background_color.y * background_color.w, background_color.z * background_color.w, background_color.w);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_ONE, GL_ONE);
        
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

			RotateWithPos(g_obj_model_mat, angle_x, angle_y);
			TransformScale(g_obj_model_mat, scale_x, scale_y, scale_z);
            //linmath test

            vec3 m_eye = { eye_x ,eye_y, eye_z };
            vec3 m_center = { center_x ,center_y, center_z };
            vec3 m_up = { up_x ,up_y, up_z };

            mat4x4_perspective(g_obj_proj_mat, Deg2Rad(90),(float)display_w/(float)display_h, 0.1f, 100.0f);
            mat4x4_look_at(g_obj_view_mat, m_eye, m_center, m_up);

            GLfloat m_light_color[3] = { light_color.x , light_color.y , light_color.z };
            GLfloat m_object_color[3] = { obj_color.x , obj_color.y , obj_color.z };

            for( auto obj_info : v_models)
            {
				glUseProgram(obj_shader->program);

                glUniform1f(obj_shader->frag_member.at("uAmbientStrength"), (GLfloat)ambient_value);
                glUniform3fv(obj_shader->frag_member.at("uLightColor"), 1, m_light_color);
                glUniform3fv(obj_shader->frag_member.at("uObjectColor"), 1, m_object_color);

				glUniformMatrix4fv(obj_shader->vert_member.at("uProjection"), 1, GL_FALSE, (GLfloat *)g_obj_proj_mat);
				glUniformMatrix4fv(obj_shader->vert_member.at("uView"), 1, GL_FALSE, (GLfloat *)g_obj_view_mat);
				glUniformMatrix4fv(obj_shader->vert_member.at("uModel"), 1, GL_FALSE, (GLfloat *)g_obj_model_mat);

                glEnableVertexAttribArray(obj_shader->vert_member.at("aPosition"));
                glVertexAttribPointer(obj_shader->vert_member.at("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, &obj_info.positions[0]);

                glLineWidth(3);

                glDrawElements(GL_TRIANGLES, obj_info.v_faces.size(), GL_UNSIGNED_INT, &obj_info.v_faces[0]);
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