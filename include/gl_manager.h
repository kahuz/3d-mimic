#ifndef __GL_MANAGER_H__
#define __GL_MANAGER_H__
// gl header

#include <GL/gl3w.h> // Initialize with gl3wInit()
#include <vector>
#include <map>
#include <string>

static std::map<int, const char*> glErrorMap = {
	//GL error Check
	{GL_NO_ERROR,						"GL_NO_ERROR"},
	{GL_INVALID_ENUM,					"GL_INVALID_ENUM"},
	{GL_INVALID_VALUE,					"GL_INVALID_VALUE"},
	{GL_OUT_OF_MEMORY,					"GL_INVALID_FRAMEBUFFER_OPERATION"},
	{GL_INVALID_FRAMEBUFFER_OPERATION,	"GL_OUT_OF_MEMORY"},
};

/*
typedef struct MTLInfo{
	std::string mtl_name;
	...
	...
} MTLInfo;
*/
typedef struct GL3DObj
{
	//std::string name;
	//MTLInfo mtl_info;
	
    std::vector<float> positions, texels, normals;
	std::vector<int> v_faces, vt_faces, vn_faces;
	//int vertices;
	//int positions;
	//int texels;
	//int normals;
	//int faces; // extend point, line, polygon
	bool smooth_shading;
	
} GL3DObj;

//general functions
int CheckError();
int LoadObjectFile(GL3DObj* dest_model, std::string path);

class GLShader
{
	public:
		GLuint program;
		GLuint v_shader;
		GLuint f_shader;

		std::map<std::string, GLint> vert_member;
		std::map<std::string, GLint> frag_member;

		std::vector<GL3DObj> gl_obj;
	private:

	public:
		GLShader();
		GLShader(const char* vertex_src, const char* fragment_src);

		~GLShader();

		void LinkShaders();
		GLuint LoadShader(GLenum type, const char *shader_path);

		int SetGLAttribLocation(GLenum type, std::string attri_name);
		int SetGLUniformLocation(GLenum type, std::string uniform_name);

	private:
		GLuint CreateShader(GLenum type, const char *shader_src);

};

class GLRenderer
{
	public:
//		GLShader shader[];

	private:

	public:
//		Render();

	private:
};

#endif // #ifndef __GL_MANAGER_H__
