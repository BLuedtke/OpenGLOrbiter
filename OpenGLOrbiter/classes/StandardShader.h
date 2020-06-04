//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.


#ifndef StandardShader_hpp
#define StandardShader_hpp

#include <stdio.h>
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
#include <iostream>
#include <assert.h>
#include "vector.h"
#include "color.h"
#include "camera.h"
#include "matrix.h"

class StandardShader
{
public:
    StandardShader();
    virtual ~StandardShader() {}
    virtual const Matrix& modelTransform() const { return ModelTransform; }
    virtual void modelTransform(const Matrix& m) { ModelTransform = m; }
    virtual void deactivate() const;
    virtual void activate(const BaseCamera& Cam) const;

protected:    
	GLuint createShaderProgram(std::string* VS_String, std::string* FS_String);
    GLuint ShaderProgram;
    Matrix ModelTransform;
    static const StandardShader* ShaderNowPiped;
};

#endif /* StandardShader_hpp */
