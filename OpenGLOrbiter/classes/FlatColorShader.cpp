//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.
    //  Copyright © 2016 Philipp Lensing. All rights reserved.

#include "FlatColorShader.h"

const char *CVertexShaderCode =
"#version 400\n"
"in vec4 VertexPos;"
"uniform mat4 ModelViewProjMat;"
"void main()"
"{"
"    gl_Position = ModelViewProjMat * VertexPos;"
"}";

const char *CFragmentShaderCode =
"#version 400\n"
"uniform vec3 Color;"
"out vec4 FragColor;"
"void main()"
"{"
"    FragColor = vec4(Color,0);"
"}";

FlatColorShader::FlatColorShader() : Col(0.2f,0.2f,1.0f)
{
    ShaderProgram = createShaderProgram(CVertexShaderCode, CFragmentShaderCode );
	
    ColorLoc = glGetUniformLocation(ShaderProgram, "Color");
    assert(ColorLoc>=0);
    ModelViewProjLoc  = glGetUniformLocation(ShaderProgram, "ModelViewProjMat");
    assert(ModelViewProjLoc>=0);
    
}
FlatColorShader::FlatColorShader(const Color & c) : Col(c)
{
	ShaderProgram = createShaderProgram(CVertexShaderCode, CFragmentShaderCode);
	ColorLoc = glGetUniformLocation(ShaderProgram, "Color");
	assert(ColorLoc >= 0);
	ModelViewProjLoc = glGetUniformLocation(ShaderProgram, "ModelViewProjMat");
	assert(ModelViewProjLoc >= 0);
}
void FlatColorShader::activate(const BaseCamera& Cam) const
{
	StandardShader::activate(Cam);
    glUniform3f(ColorLoc, Col.R, Col.G, Col.B);
    Matrix ModelView = Cam.getViewMatrix() * ModelTransform;
    Matrix ModelViewProj = Cam.getProjectionMatrix() * ModelView;
    glUniformMatrix4fv(ModelViewProjLoc, 1, GL_FALSE, ModelViewProj.m);
}
void FlatColorShader::color( const Color& c)
{
    Col = c;
}

