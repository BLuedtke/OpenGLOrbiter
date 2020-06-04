//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.


#include "StandardShader.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#else
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#endif
#endif

const StandardShader* StandardShader::ShaderNowPiped = NULL;

StandardShader::StandardShader()
{
	ModelTransform.identity();
}


GLuint StandardShader::createShaderProgram(std::string* VS_String, std::string* FS_String)
{
	ModelTransform.identity();
	const unsigned int LogSize = 8 * 1024;	//urspr 64*1024 -> Überschreitet Stack-Größe!!
	char ShaderLog[LogSize];
	GLsizei WrittenToLog = 0;
	GLint Success = 0;


	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);

	GLenum Error = glGetError();
	if (Error != 0)
	{
		std::cout << "Unable to create shader objects. Please ensure that the Shader is used for the first time AFTER successful creation of an OpenGL context!";
		throw std::exception();
	}

	GLchar const *VScode = VS_String->c_str();
	GLint const vs_length = (GLint) VS_String->size();
	GLchar const *FScode = FS_String->c_str();
	GLint const fs_length = (GLint) FS_String->size();

	glShaderSource(VS, 1, &VScode, &vs_length);
	glShaderSource(FS, 1, &FScode, &fs_length);

	glCompileShader(VS);
	glGetShaderiv(VS, GL_COMPILE_STATUS, &Success);
	if (Success == GL_FALSE)
	{
		std::cout << "VS: " << ShaderLog << std::endl;
		WrittenToLog += 3;
		GLsizei Written = 0;
		glGetShaderInfoLog(VS, LogSize - WrittenToLog, &Written, &ShaderLog[WrittenToLog]);
		WrittenToLog += Written;
	}

	glCompileShader(FS);
	glGetShaderiv(FS, GL_COMPILE_STATUS, &Success);
	if (Success == GL_FALSE)
	{
		std::cout << "FS: " << &ShaderLog[WrittenToLog] << std::endl;
		WrittenToLog += 3;
		GLsizei Written = 0;
		glGetShaderInfoLog(FS, LogSize - WrittenToLog, &Written, &ShaderLog[WrittenToLog]);
		WrittenToLog += Written;
	}

	if (WrittenToLog > 0)
	{		// compilation failed        
		std::cout << ShaderLog;
		throw std::exception();
	}

	ShaderProgram = glCreateProgram();
	assert(ShaderProgram);

	glAttachShader(ShaderProgram, VS);
	glDeleteShader(VS);
	glAttachShader(ShaderProgram, FS);
	glDeleteShader(FS);
	glLinkProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == GL_FALSE)
	{
		GLsizei Written = 0;
		glGetProgramInfoLog(ShaderProgram, LogSize - WrittenToLog, &Written, &ShaderLog[WrittenToLog]);
		WrittenToLog += Written;
	}

	if (WrittenToLog > 0)
	{
		// compilation failed
		std::cout << ShaderLog;
		throw std::exception();
	}

	return ShaderProgram;
}


void StandardShader::deactivate() const
{
    glUseProgram(0);
}

void StandardShader::activate(const BaseCamera& Cam) const
{
    if(ShaderNowPiped != this){
        glUseProgram(ShaderProgram);
    }
    ShaderNowPiped = this;
}

