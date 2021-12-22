//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.


#ifndef PhongShaderInstanced_hpp
#define PhongShaderInstanced_hpp

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
#include <vector>
#include <memory>
#include <assert.h>
#include "color.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "texture.h"
#include "PhongShader.h"

class PhongShaderInstanced : public PhongShader
{
public:
	PhongShaderInstanced();
	virtual ~PhongShaderInstanced() {};
	virtual void activate(const BaseCamera& Cam) const;
	void setInstancePositions(std::vector<Vector> && __instance_Positions);
protected:
	void assignLocations();
	std::vector<Vector> instancePositions{};
	GLint VOffsetsLoc;
	enum UPDATESTATES
	{
		POSITIONS_CHANGED = 1 << 7
	};
};

#endif /* PhongShaderInstanced_hpp */
