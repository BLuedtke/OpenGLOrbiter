//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.


#include "PhongShader.h"

const char *VertexShaderCode =
"#version 400\n"
"layout(location=0) in vec4 VertexPos;"
"layout(location=1) in vec4 VertexNormal;"
"layout(location=2) in vec2 VertexTexcoord;"
"out vec3 Position;"
"out vec3 Normal;"
"out vec2 Texcoord;"
"uniform mat4 ModelMat;"
"uniform mat4 ModelViewProjMat;"
"void main()"
"{"
"    Position = (ModelMat * VertexPos).xyz;"
"    Normal =  (ModelMat * VertexNormal).xyz;"
"    Texcoord = VertexTexcoord;"
"    gl_Position = ModelViewProjMat * VertexPos;"
"}";


const char *FragmentShaderCode =
"#version 400\n"
"uniform vec3 EyePos;"
"uniform vec3 LightPos;"
"uniform vec3 LightColor;"
"uniform vec3 DiffuseColor;"
"uniform vec3 SpecularColor;"
"uniform vec3 AmbientColor;"
"uniform float SpecularExp;"
"uniform sampler2D DiffuseTexture;"
"in vec3 Position;"
"in vec3 Normal;"
"in vec2 Texcoord;"
"out vec4 FragColor;"
"float sat( in float a)"
"{"
"    return clamp(a, 0.0, 1.0);"
"}"
"void main()"
"{"
"    vec3 N = normalize(Normal);"
"    vec3 L = normalize(LightPos-Position);"
"    vec3 E = normalize(EyePos-Position);"
"    vec3 R = reflect(-L,N);"
"    vec3 DiffTex = texture( DiffuseTexture, Texcoord).rgb;"
"    vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));"
"    vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);"
"    FragColor = vec4((DiffuseComponent + AmbientColor)*DiffTex + SpecularComponent ,0);"
"}";

PhongShader::PhongShader() :
	DiffuseColor(0.8f, 0.8f, 0.8f),
	SpecularColor(0.5f, 0.5f, 0.5f),
	AmbientColor(0.2f, 0.2f, 0.2f),
	SpecularExp(20.0f),
	LightPos(5.0f, 5.0f, 5.0f),
	LightColor(1, 1, 1),
	DiffuseTexture(Texture::defaultTex()),
	UpdateState(0xFFFFFFFF)
{
	ShaderProgram = createShaderProgram(VertexShaderCode, FragmentShaderCode);
	assignLocations();
}
void PhongShader::assignLocations()
{
	DiffuseColorLoc = glGetUniformLocation(ShaderProgram, "DiffuseColor");
	AmbientColorLoc = glGetUniformLocation(ShaderProgram, "AmbientColor");
	SpecularColorLoc = glGetUniformLocation(ShaderProgram, "SpecularColor");
	SpecularExpLoc = glGetUniformLocation(ShaderProgram, "SpecularExp");
	DiffuseTexLoc = glGetUniformLocation(ShaderProgram, "DiffuseTexture");
	LightPosLoc = glGetUniformLocation(ShaderProgram, "LightPos");
	LightColorLoc = glGetUniformLocation(ShaderProgram, "LightColor");
	EyePosLoc = glGetUniformLocation(ShaderProgram, "EyePos");
	ModelMatLoc = glGetUniformLocation(ShaderProgram, "ModelMat");
	ModelViewProjLoc = glGetUniformLocation(ShaderProgram, "ModelViewProjMat");
}
void PhongShader::activate(const BaseCamera& Cam) const
{
	StandardShader::activate(Cam);

	// update uniforms if necessary
	if (UpdateState&DIFF_COLOR_CHANGED)
		glUniform3f(DiffuseColorLoc, DiffuseColor.R, DiffuseColor.G, DiffuseColor.B);
	if (UpdateState&AMB_COLOR_CHANGED)
		glUniform3f(AmbientColorLoc, AmbientColor.R, AmbientColor.G, AmbientColor.B);
	if (UpdateState&SPEC_COLOR_CHANGED)
		glUniform3f(SpecularColorLoc, SpecularColor.R, SpecularColor.G, SpecularColor.B);
	if (UpdateState&SPEC_EXP_CHANGED)
		glUniform1f(SpecularExpLoc, SpecularExp);

	DiffuseTexture->activate(0);
	if (UpdateState&DIFF_TEX_CHANGED && DiffuseTexture)
		glUniform1i(DiffuseTexLoc, 0);

	if (UpdateState&LIGHT_COLOR_CHANGED)
		glUniform3f(LightColorLoc, LightColor.R, LightColor.G, LightColor.B);
	if (UpdateState&LIGHT_POS_CHANGED)
		glUniform3f(LightPosLoc, LightPos.X, LightPos.Y, LightPos.Z);

	// always update matrices
	Matrix ModelViewProj = Cam.getProjectionMatrix() * Cam.getViewMatrix() * modelTransform();
	glUniformMatrix4fv(ModelMatLoc, 1, GL_FALSE, modelTransform().m);
	glUniformMatrix4fv(ModelViewProjLoc, 1, GL_FALSE, ModelViewProj.m);

	Vector EyePos = Cam.position();
	glUniform3f(EyePosLoc, EyePos.X, EyePos.Y, EyePos.Z);

	UpdateState = 0x0;
}
void PhongShader::diffuseColor(const Color& c)
{
	DiffuseColor = c;
	UpdateState |= DIFF_COLOR_CHANGED;
}
void PhongShader::ambientColor(const Color& c)
{
	AmbientColor = c;
	UpdateState |= AMB_COLOR_CHANGED;
}
void PhongShader::specularColor(const Color& c)
{
	SpecularColor = c;
	UpdateState |= SPEC_COLOR_CHANGED;
}
void PhongShader::specularExp(float exp)
{
	SpecularExp = exp;
	UpdateState |= SPEC_EXP_CHANGED;
}
void PhongShader::lightPos(const Vector& pos)
{
	LightPos = pos;
	UpdateState |= LIGHT_POS_CHANGED;
}
void PhongShader::lightColor(const Color& c)
{
	LightColor = c;
	UpdateState |= LIGHT_COLOR_CHANGED;
}

void PhongShader::diffuseTexture(const Texture* pTex)
{
	DiffuseTexture = pTex;
	if (!DiffuseTexture)
		DiffuseTexture = Texture::defaultTex();

	UpdateState |= DIFF_TEX_CHANGED;
}


