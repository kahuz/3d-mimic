
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
GLuint light_cube_vbo, light_cube_vao;

float light_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

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
    return (float)(degree * PI / 180);
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
        static float prev_scale_x = 0.2;
        static float prev_scale_y = 0.2;
        static float prev_scale_z = 0.2;
        
        if( prev_scale_x != scale_x || prev_scale_y != scale_y || prev_scale_z != scale_z )
        {
            mat4x4_scale_aniso(src_matrix, src_matrix, scale_x, scale_y, scale_z);
	    }
        
        prev_scale_x = scale_x;
        prev_scale_y = scale_y;
        prev_scale_z = scale_z;
    }
}

void TurnAroundObject(mat4x4 src_matrix)
{
    float min_max_value = 5;
    static int angle = -90;

    if(angle > 90)
    {
        angle = -90;
    }
    
    float x = min_max_value * sin(angle * PI / 180);
    float z = min_max_value * sin((angle+90) * PI / 180);
    
    mat4x4_translate(src_matrix, x, 0.0f, z);

    angle++;
}
void MoveWithPos(mat4x4 src_matrix, float move_x, float move_y, float move_z)
{
    static float prev_move_x = 0.0f;
    static float prev_move_y = 0.0f;
    static float prev_move_z = 0.0f;

    if( prev_move_x != move_x || prev_move_y != move_y || prev_move_z != move_z )
    {
        mat4x4_translate(src_matrix, move_x, move_y, move_z);
    }
    
    prev_move_x = move_x;
    prev_move_y = move_y;
    prev_move_z = move_z;
}

void RotateWithPos(mat4x4 src_matrix, vec3 angle)
{
    static float prev_angle_x = 0.0f;
    static float prev_angle_y = 0.0f;
    static float prev_angle_z = 0.0f;
    
    if(prev_angle_x != angle[0])
    {
        mat4x4_rotate_X(src_matrix, src_matrix, angle[0]);
    }

    if(prev_angle_y != angle[1])
    {
        mat4x4_rotate_Y(src_matrix, src_matrix, angle[1]);
    }

    if(prev_angle_z != angle[2])
    {
        mat4x4_rotate_Z(src_matrix, src_matrix, angle[2]);
    }
    
    prev_angle_x = angle[0];
    prev_angle_y = angle[1];
    prev_angle_z = angle[2];
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

void InitGLShader()
{   
    obj_shader = new GLShader();
    bg_shader = new GLShader();

    bg_shader->LoadShader(GL_VERTEX_SHADER, "D:\\github\\3d-mimic\\src\\gl_shader\\texture.vshader");
    bg_shader->LoadShader(GL_FRAGMENT_SHADER, "D:\\github\\3d-mimic\\src\\gl_shader\\texture.fshader");
    bg_shader->LinkShaders();

    // bg_shader Vertex Shader Initialize
    bg_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aPosition");
    bg_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aTexCoord");

    bg_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uGlobalColor");
    bg_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uProjection");

    // bg_shader Fragment Shader Initialize
    bg_shader->SetGLUniformLocation(GL_FRAGMENT_SHADER, "uTexture0");

    obj_shader->LoadShader(GL_VERTEX_SHADER, "D:\\github\\3d-mimic\\src\\gl_shader\\object.vshader");
    obj_shader->LoadShader(GL_FRAGMENT_SHADER, "D:\\github\\3d-mimic\\src\\gl_shader\\object.fshader");
    obj_shader->LinkShaders();

    // obj_shader Vertex Shader Initialize
    obj_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aPosition");
    obj_shader->SetGLAttribLocation(GL_VERTEX_SHADER, "aNormal");
    
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uProjection");
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uModel");
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uView");
    
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uLightColor");
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uObjectColor");
    obj_shader->SetGLUniformLocation(GL_VERTEX_SHADER, "uAmbientStrength");

    obj_shader->SetGLUniformLocation(GL_FRAGMENT_SHADER, "uLightPos");

};    

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
    InitGLShader();

    static int frame_num = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawToolBar();
        DrawMenuBar();
        DrawExternalSettingView();
        
        GlobalMouseEvent();
        GlobalKeyboardEvent();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        
        glfwGetFramebufferSize(window, &display_w, &display_h); 

        glClearColor(g_extern_settings.bg_color[0] * g_extern_settings.bg_color[3], g_extern_settings.bg_color[2] * g_extern_settings.bg_color[3], 
                    g_extern_settings.bg_color[2] * g_extern_settings.bg_color[3], g_extern_settings.bg_color[3]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(g_extern_settings.option.active_depth)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
        
        if(g_extern_settings.option.active_blend)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
        }
        else
        {
            glDisable(GL_BLEND);
        }

        if (g_menu_load_model)
        {
            frame_num++;

            MoveWithPos(g_obj_model_mat, g_extern_settings.move[0], g_extern_settings.move[1],g_extern_settings.move[2]);
            RotateWithPos(g_obj_model_mat, g_extern_settings.angle);
            TransformScale(g_obj_model_mat, g_extern_settings.scale[0], g_extern_settings.scale[1], g_extern_settings.scale[2]);

            mat4x4_perspective(g_obj_proj_mat, Deg2Rad(g_extern_settings.camera.zoom), (float)display_w / (float)display_h, 0.1f, 1000.0f);
            mat4x4_look_at(g_obj_view_mat, g_extern_settings.camera.eye, g_extern_settings.camera.center, g_extern_settings.camera.up);
            vec3 tmp_light_pos = {10.2f, 10.0f, 10.0f};
            // My Object
			glUseProgram(obj_shader->program);
            for( auto obj_info : v_models)
            {

				glUniform3fv(obj_shader->frag_member.at("uLightPos"), 1, (GLfloat *)g_extern_settings.light.pos);

				glUniform4fv(obj_shader->vert_member.at("uObjectColor"), 1, (GLfloat *)g_extern_settings.obj_color);
				glUniform4fv(obj_shader->vert_member.at("uLightColor"), 1, (GLfloat *)g_extern_settings.light.color);
                glUniform1f(obj_shader->vert_member.at("uAmbientStrength"), g_extern_settings.light.ambient_value);

				glUniformMatrix4fv(obj_shader->vert_member.at("uProjection"), 1, GL_FALSE, (GLfloat *)g_obj_proj_mat);
				glUniformMatrix4fv(obj_shader->vert_member.at("uView"), 1, GL_FALSE, (GLfloat *)g_obj_view_mat);
				glUniformMatrix4fv(obj_shader->vert_member.at("uModel"), 1, GL_FALSE, (GLfloat *)g_obj_model_mat);

                glEnableVertexAttribArray(obj_shader->vert_member.at("aPosition"));
                glEnableVertexAttribArray(obj_shader->vert_member.at("aNormal"));

                glVertexAttribPointer(obj_shader->vert_member.at("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, &obj_info.positions[0]);
                glVertexAttribPointer(obj_shader->vert_member.at("aNormal"), 3, GL_FLOAT, GL_FALSE, 0, &obj_info.normals[0]);
 
                glDrawElements(GL_TRIANGLES, (GLsizei)obj_info.v_faces.size(), GL_UNSIGNED_INT, &obj_info.v_faces[0]);

            }
        }
        //DrawBackGround();
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