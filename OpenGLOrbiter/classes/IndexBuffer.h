//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.
    //  Copyright © 2016 Philipp Lensing. All rights reserved.

#ifndef IndexBuffer_hpp
#define IndexBuffer_hpp

#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#endif
#include <iostream>
#include <vector>
#include <stdio.h>

class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer();
    void begin();
    void addIndex(unsigned int Index);
    void end();
    
    void activate();
    void deactivate();
    
    GLenum indexFormat() { return IndexFormat; }
    unsigned int indexCount() { return IndexCount; }
    
private:
    std::vector<unsigned int> Indices;
    GLuint IBO;
    bool BufferInitialized;
    bool WithinBeginAndEnd;
    GLenum IndexFormat;
    unsigned int IndexCount;
};

#endif /* IndexBuffer_hpp */
