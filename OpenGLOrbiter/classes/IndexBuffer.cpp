//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.
    //  Copyright © 2016 Philipp Lensing. All rights reserved.

#include "IndexBuffer.h"
#include <assert.h>

IndexBuffer::IndexBuffer() : BufferInitialized(false), WithinBeginAndEnd(false), IndexFormat(GL_UNSIGNED_INT), IndexCount(0)
{
    ;
}

IndexBuffer::~IndexBuffer()
{
    ;
}

void IndexBuffer::begin()
{
    if( BufferInitialized) {
        glDeleteBuffers(1, &IBO);
    }
    IndexCount = 0;
    Indices.clear();
    WithinBeginAndEnd = true;
}

void IndexBuffer::addIndex( unsigned int Index)
{
    if(!WithinBeginAndEnd) { std::cout << "call addIndex only between begin() and end() method!\n"; return; }
    Indices.push_back(Index);
    IndexCount = (unsigned int)Indices.size();
}

void IndexBuffer::end()
{
    if(Indices.size() == 0)
    {
        std::cout << "IndexBuffer::end(): no indices found. call addIndex() within begin() and end() method" << std::endl;
        return;
    }
 
    IndexCount = (unsigned int)Indices.size();
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    
    if(Indices.size() < 0xFFFF)
    {
        unsigned short* Data = new unsigned short[sizeof(unsigned short)*Indices.size()];
        assert(Data);
        for( unsigned int i=0; i<Indices.size(); ++i)
            Data[i] = Indices[i];
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size()*sizeof(unsigned short), Data, GL_STATIC_DRAW);
        delete [] Data;
        IndexFormat = GL_UNSIGNED_SHORT;
    } else {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size()*sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    WithinBeginAndEnd = false;    
}

void IndexBuffer::activate()
{
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
}

void IndexBuffer::deactivate()
{
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}