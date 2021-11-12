#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "gl_manager.h"
#include "Logger.h"

enum _ObjDataType {
	kVertex = 0,
	kTexel,
	kNormal,
	kFace
}typedef ObjDataType;
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

float MyRound(float src, int depth)
{
	int mul_value = 1;
	int convert_src_to_int = 0;
	float tmp = 0.0f;

	for (int i = 1; i <= depth; i++)
	{
		mul_value *= 10;
	}

	convert_src_to_int = src * mul_value;

	for (int i = 0; i <= depth; i++)
	{
		int tmp_i = convert_src_to_int % 10;
		tmp += (float)tmp_i / mul_value;
		mul_value /= 10;
		convert_src_to_int /= 10;
	}

	return tmp;
}

int LoadObjectsFile(std::vector<GL3DObj> *dest_model, const char* path)
{
	std::ifstream obj_file(path, std::ios::in);
	std::string obj_src;
	bool is_first_obj = true;
	
	GL3DObj res_obj;

	if (obj_file.is_open())
	{
		std::string cur_line;

		while (getline(obj_file, cur_line))
		{
			std::stringstream line_stream(cur_line);
			std::string cur_token;

			getline(line_stream, cur_token, ' ');

			if (cur_token == "o")
			{
				//if (is_first_obj == false)
				//{
				//	dest_model->push_back(res_obj);
				//	res_obj.ClearObj();
				//}
				//else
				//{
				//	is_first_obj = false;
				//}
			}
			else if (cur_token == "v")
			{
				while (getline(line_stream, cur_token, ' '))
				{
					float get_value = atof(cur_token.c_str());

					res_obj.positions.push_back(get_value);
				}
			}
			else if (cur_token == "vt")
			{
				while (getline(line_stream, cur_token, ' '))
				{
					float get_value = atof(cur_token.c_str());

					res_obj.texels.push_back(get_value);
				}
			}
			else if (cur_token == "vn")
			{
				while (getline(line_stream, cur_token, ' '))
				{
					float get_value = atof(cur_token.c_str());

					res_obj.normals.push_back(get_value);
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
								res_obj.v_faces.push_back(atoi(cur_token.c_str()) - 1);
							}
						}
						else if (face_type == kTexel)
						{
							if (face_token != "")
							{
								res_obj.vt_faces.push_back(atoi(cur_token.c_str()) - 1);
							}
						}
						else if (face_type == kNormal)
						{
							if (face_token != "")
							{
								res_obj.vn_faces.push_back(atoi(cur_token.c_str()) - 1);
							}
						}
						face_type++;
					}
				}
			}
		}
		dest_model->push_back(res_obj);
		obj_file.close();
	}
	return 0;
}

int LoadObjectFile(GL3DObj* dest_model, const char* path)
{
	std::ifstream obj_file(path, std::ios::in);
	std::string obj_src;

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
				while (getline(line_stream, cur_token, ' '))
				{
					dest_model->positions.push_back(atof(cur_token.c_str()));
				}
			}
			else if (cur_token == "vt")
			{
				while (getline(line_stream, cur_token, ' '))
				{
					dest_model->texels.push_back(atof(cur_token.c_str()));
				}
			}
			else if (cur_token == "vn")
			{
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
								dest_model->v_faces.push_back(atoi(cur_token.c_str()) - 1);
							}
						}
						else if (face_type == kTexel)
						{
							if (face_token != "")
							{
								dest_model->vt_faces.push_back(atoi(cur_token.c_str()) - 1);
							}
						}
						else if (face_type == kNormal)
						{
							if (face_token != "")
							{
								dest_model->vn_faces.push_back(atoi(cur_token.c_str()) - 1);
							}
						}

						face_type++;
					}
				}
			}
		}
		obj_file.close();
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

bool GLShader::LoadShader(GLenum type, const char *shader_path)
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

		return false;
	}

	if (type == GL_VERTEX_SHADER)
	{
		v_shader = CreateShader(GL_VERTEX_SHADER, shader_src.c_str());
	}
	else if (type == GL_FRAGMENT_SHADER)
	{
		f_shader = CreateShader(GL_FRAGMENT_SHADER, shader_src.c_str());
	}
	return true;
}

void GLShader::LinkShaders()
{
	glAttachShader(program, v_shader);
	CheckError();
	glAttachShader(program, f_shader);
	CheckError();
	glLinkProgram(program);
	CheckError();
}

int GLShader::SetGLAttribLocation(GLenum type, std::string attri_name)
{
	GLint tmp_loc = glGetAttribLocation(program, attri_name.c_str());

	Log("key [%s] value[%d]",attri_name.c_str(), tmp_loc);

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

	Log("key [%s] value[%d]", uniform_name.c_str(), tmp_uniform);

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
