#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "gl_manager.h"
#include "Logger.h"

int CheckError()
{
	int err_type = 0;
	err_type = glGetError();

	if(err_type != GL_NO_ERROR)
	{
		Err("%s", glErrorMap.at(err_type));
		return true;
	}
	return false;
}

enum _ObjDataType {
	kVertex = 0,
	kTexel,
	kNormal,
	kFace
}typedef ObjDataType;

int LoadObjectFile(GL3DObj *dest_model, std::string path)
{
	std::ifstream obj_file(path, std::ios::in);
	std::string obj_src;
	int vertex_indices = 0;
	int face_devide = 0;

	bool exist_v = false, exist_vn = false, exist_vt = false;

	if (obj_file.is_open())
	{
		std::string cur_line;

		while (getline(obj_file, cur_line))
		{
			std::stringstream line_stream(cur_line);
			std::string cur_token;

			getline(line_stream, cur_token, ' ');

			if (cur_token == "v")
			{
				if (exist_v == false)
				{
					exist_v = true;
				}

				while (getline(line_stream, cur_token, ' '))
				{
					dest_model->positions.push_back(atof(cur_token.c_str()));
				}
			}
			else if (cur_token == "vt")
			{
				if (exist_vt == false)
				{
					exist_vt = true;
				}

				while (getline(line_stream, cur_token, ' '))
				{
					dest_model->texels.push_back(atof(cur_token.c_str()));
				}
			}
			else if (cur_token == "vn")
			{
				if (exist_vn == false)
				{
					exist_vn = true;
				}

				while (getline(line_stream, cur_token, ' '))
				{
					dest_model->normals.push_back(atof(cur_token.c_str()));
				}
			}
			else if (cur_token == "f")
			{
				while (getline(line_stream, cur_token, ' '))
				{
					std::stringstream face_stream(cur_token);
					std::string face_token;

					int face_type = 0;

					while (getline(face_stream, face_token, '/'))
					{
						if (face_type == kVertex)
						{
							if (face_token != "")
							{
								dest_model->v_faces.push_back(atof(cur_token.c_str()));
							}
						}
						else if (face_type == kTexel)
						{
							if (face_token != "")
							{
								dest_model->vt_faces.push_back(atof(cur_token.c_str()));
							}
						}
						else if (face_type == kNormal)
						{
							if (face_token != "")
							{
								dest_model->vn_faces.push_back(atof(cur_token.c_str()));
							}
						}


						face_type++;
					}
				}
				//dest_model->faces++;
			}
		}
		obj_file.close();
	}

	if (exist_v)
	{
		face_devide++;
	}
	if (exist_vt)
	{
		face_devide++;
	}
	if (exist_vn)
	{
		face_devide++;
	}

	return 0;
}
GLShader::GLShader()
{
	program = glCreateProgram();
}

GLShader::GLShader(const char* vertex_src, const char* fragment_src)
{
	program = glCreateProgram();
    v_shader = CreateShader(GL_VERTEX_SHADER, vertex_src);
    //v_shader = LoadShader(GL_VERTEX_SHADER, vertex_src);
	printf("vertex src\n %s",vertex_src);
	Log("vertex src\n %s",vertex_src);
	f_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_src);
	//f_shader = LoadShader(GL_FRAGMENT_SHADER, fragment_src);
	Log("fragment src\n %s",fragment_src);

	glAttachShader(program, v_shader);
	glAttachShader(program, f_shader);
	glLinkProgram(program);
}

GLShader::~GLShader()
{
	glDetachShader(program, v_shader);
	glDetachShader(program, f_shader);
	glDeleteProgram(program);
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);
}

GLuint GLShader::CreateShader(GLenum type, const char *shader_src)
{
    GLint status = 0;
    GLuint shader_id = glCreateShader(type);

    glShaderSource(shader_id, 1, &shader_src, NULL);
    
	glCompileShader(shader_id);
    
	//check compile result
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    
	// if status value is GL_FALSE, that is error state
	if (status == GL_FALSE) 
	{
        GLint l;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &l);
        char *msg = (char*)malloc(l);
        glGetShaderInfoLog(shader_id, l, NULL, msg);
        Err("compiling shader: %s", msg);
        free(msg);
        glDeleteShader(shader_id);
        shader_id = 0;
    }

    return shader_id;
}

GLuint GLShader::LoadShader(GLenum type, const char *shader_path)
{
	GLuint shader_id = -1;
	std::string shader_src;

	std::ifstream shader_stream(shader_path, std::ios::in);
	
	if(shader_stream.is_open())
	{
		std::string cur_line = "";

		while(getline(shader_stream, cur_line))
		{
			shader_src += "\n" + cur_line;
		}
		shader_stream.close();
	}else
	{
		Err("Don't open %s", shader_path);
		getchar();
		return 0;
	}

	shader_id = CreateShader(type, shader_src.c_str());

	return shader_id;
}



void GLShader::LinkShaders()
{
	glAttachShader(program, v_shader);
	glAttachShader(program, f_shader);
	glLinkProgram(program);
}
int GLShader::SetGLAttribLocation(GLenum type, std::string attri_name)
{
	GLint tmp_loc = glGetAttribLocation(program, attri_name.c_str());

	Log("key [%s] value[%d]",attri_name, tmp_loc);
	if(type == GL_VERTEX_SHADER)
	{
		vert_member.insert(std::pair<std::string , GLint>(attri_name, tmp_loc));
	}
	else if(type == GL_FRAGMENT_SHADER)
	{
		frag_member.insert(std::pair<std::string , GLint>(attri_name, tmp_loc));
	}

	return 0;
}

int GLShader::SetGLUniformLocation(GLenum type, std::string uniform_name)
{
	GLint tmp_uniform = glGetUniformLocation(program, uniform_name.c_str());

	Log("key [%s] value[%d]", uniform_name, tmp_uniform);
	if(type == GL_VERTEX_SHADER)
	{
		vert_member.insert(std::pair<std::string, GLint>(uniform_name, tmp_uniform));
	}
	else if(type == GL_FRAGMENT_SHADER)
	{
		frag_member.insert(std::pair<std::string, GLint>(uniform_name, tmp_uniform));
	}

	return 0;
}
