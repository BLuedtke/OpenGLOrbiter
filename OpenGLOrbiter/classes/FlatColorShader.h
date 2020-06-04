//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.
    //  Copyright © 2016 Philipp Lensing. All rights reserved.

#ifndef FlatColorShader_hpp
#define FlatColorShader_hpp

#include <stdio.h>
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#include <GL/glew.h>
#include <glfw/glfw3.h>
#endif
#include <iostream>
#include <assert.h>
#include "color.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "StandardShader.h"


class FlatColorShader : public StandardShader
{
public:
    FlatColorShader();
    FlatColorShader(const Color& c);
    void color( const Color& c);
    const Color& color() const { return Col; }
    virtual void activate(const BaseCamera& Cam) const;
private:
    Color Col;
    GLuint ShaderProgram;
    GLint ColorLoc;
    GLint ModelViewProjLoc;
};

#endif /* FlatColorShader_hpp */
