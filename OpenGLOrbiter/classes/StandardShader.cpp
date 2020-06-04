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


GLuint StandardShader::createShaderProgram( const char* VShadercode, const char* FShadercode )
{
    ModelTransform.identity();
    const unsigned int LogSize = 64*1024;
    char ShaderLog[LogSize];
    GLsizei written=0;
    GLint sucsStat = 0;
   
    GLuint VShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint FShader = glCreateShader(GL_FRAGMENT_SHADER);
    
	GLenum glError = glGetError();
    if(glError != 0){
        std::cout << "Shader glError. Make sure OpenGL context creation was successful first." << std::endl;
        exit(0);
    }
    
    glShaderSource(VShader, 1, &VShadercode, NULL);
    glShaderSource(FShader, 1, &FShadercode, NULL);
    
    glCompileShader(VShader);
    glGetShaderiv(VShader, GL_COMPILE_STATUS, &sucsStat);
    if(sucsStat==GL_FALSE){
        glGetShaderInfoLog(VShader, LogSize, &written, ShaderLog);
    }
    
    
    glCompileShader(FShader);
    glGetShaderiv(FShader, GL_COMPILE_STATUS, &sucsStat);
    if(sucsStat==GL_FALSE){
        GLsizei Written=0;
        glGetShaderInfoLog(FShader, LogSize-written, &Written, &ShaderLog[written]);
        written+=Written;
    }
    
    if(written > 0){
        std::cerr << ShaderLog;
        exit(0);
    }
    
    ShaderProgram = glCreateProgram();
    assert(ShaderProgram);
    
    glAttachShader(ShaderProgram, VShader);
    glDeleteShader(VShader);
    glAttachShader(ShaderProgram, FShader);
    glDeleteShader(FShader);
    glLinkProgram(ShaderProgram);
    
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &sucsStat);
    if(sucsStat==GL_FALSE)
        glGetShaderInfoLog(ShaderProgram, LogSize-written, NULL, &ShaderLog[written]);

    if(written > 0){
        std::cerr << ShaderLog;
        exit(0);
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

