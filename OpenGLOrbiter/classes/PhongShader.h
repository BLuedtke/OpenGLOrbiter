//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.


#ifndef PhongShader_hpp
#define PhongShader_hpp

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
#include <memory>
#include <string>
#include "color.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "StandardShader.h"
#include "texture.h"

class PhongShader : public StandardShader
{
public:
	PhongShader();
	virtual ~PhongShader() {};
	// setter
	void diffuseColor(const Color& c);
	void ambientColor(const Color& c);
	void specularColor(const Color& c);
	void specularExp(float exp);
	void diffuseTexture(const Texture* pTex);
	void lightPos(const Vector& pos);
	void lightColor(const Color& c);
	//getter
	const Color& diffuseColor() const { return DiffuseColor; }
	const Color& ambientColor() const { return AmbientColor; }
	const Color& specularColor() const { return SpecularColor; }
	float specularExp() const { return SpecularExp; }
	const Texture* diffuseTexture() const { return DiffuseTexture; }
	const Vector& lightPos() const { return LightPos; }
	const Color& lightColor() const { return LightColor; }

	virtual void activate(const BaseCamera& Cam) const;

protected:
	virtual void assignLocations();

	Color DiffuseColor;
	Color SpecularColor;
	Color AmbientColor;
	float SpecularExp;
	Vector LightPos;
	Color LightColor;
	const Texture* DiffuseTexture;
	std::unique_ptr<std::string> cvCode{};
	std::unique_ptr<std::string> cfCode{};

	GLint DiffuseColorLoc;
	GLint SpecularColorLoc;
	GLint AmbientColorLoc;
	GLint SpecularExpLoc;
	GLint LightPosLoc;
	GLint LightColorLoc;
	GLint ModelMatLoc;
	GLint ModelViewProjLoc;
	GLint EyePosLoc;
	GLint DiffuseTexLoc;

	mutable unsigned int UpdateState;

	enum UPDATESTATES
	{
		DIFF_COLOR_CHANGED = 1 << 0,
		AMB_COLOR_CHANGED = 1 << 1,
		SPEC_COLOR_CHANGED = 1 << 2,
		SPEC_EXP_CHANGED = 1 << 3,
		LIGHT_POS_CHANGED = 1 << 4,
		LIGHT_COLOR_CHANGED = 1 << 5,
		DIFF_TEX_CHANGED = 1 << 6
	};

};

#endif /* PhongShader_hpp */
