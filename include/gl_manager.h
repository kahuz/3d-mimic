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
	//MTLInfo mtl_info;
	
    std::vector<GLfloat> positions, texels, normals;
	std::vector<unsigned int> v_faces, vt_faces, vn_faces;

	bool smooth_shading = false;

	void GL3DObj::ClearObj()
	{
		positions.clear();
		texels.clear();
		normals.clear();
		v_faces.clear();
		vt_faces.clear();
		vn_faces.clear();

		positions.shrink_to_fit();
		texels.shrink_to_fit();
		normals.shrink_to_fit();
		v_faces.shrink_to_fit();
		vt_faces.shrink_to_fit();
		vn_faces.shrink_to_fit();

		smooth_shading = false;
	}
	
} GL3DObj;
//general functions
int CheckError();
int LoadObjectFile(GL3DObj* dest_model, const char* path);
int LoadObjectsFile(std::vector<GL3DObj> *dest_model, const char* path);

class GLShader
{
	public:
		GLuint program;
		GLuint v_shader;
		GLuint f_shader;

		std::map<std::string, GLint> vert_member;
		std::map<std::string, GLint> frag_member;

	private:

	public:
		GLShader();
		GLShader(const char* vertex_src, const char* fragment_src);

		~GLShader();

		void LinkShaders();
		bool LoadShader(GLenum type, const char *shader_path);

		int SetGLAttribLocation(GLenum type, const char* attri_name);
		int SetGLUniformLocation(GLenum type, const char* uniform_name);

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
