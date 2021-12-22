//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.

#include "TriangleSphereModel.h"
#define _USE_MATH_DEFINES
#include <math.h>

TriangleSphereModel::TriangleSphereModel( float Radius, int Stacks, int Slices )
{
    VB.begin();
    for( int i=0; i<Stacks; ++i)
        for( int j=0; j<Slices; ++j)
        {
            float phi = (float)(j)*(float)(M_PI)*2.0f/(float)(Slices-1);
            float theta = (float)(i)*(float)(M_PI)/(float)(Stacks-1);
            float x = Radius*sin( phi )*sin(theta);
            float z = Radius*cos( phi )*sin(theta);
            float y = Radius*cos( theta );
            VB.addNormal( Vector(x,y,z).normalize());
            VB.addTexcoord0( phi/((float)(M_PI)*2.0f), theta/ (float)(M_PI) );
            VB.addTexcoord1( (float)(Slices*phi)/((float)(M_PI)*2.0f), (float)(Stacks*(theta/ (float)(M_PI))) );
            VB.addVertex(x, y, z);
        }
    VB.end();
    
    IB.begin();
    for( int i=0; i<Stacks-1; ++i)
        for( int j=0; j<Slices-1; ++j)
        {
            IB.addIndex( i*Slices + j+1 );
            IB.addIndex( i*Slices + j );
            IB.addIndex( (i+1)*Slices + j );

            IB.addIndex( (i+1)*Slices + j );
            IB.addIndex( (i+1)*Slices + j+1 );
            IB.addIndex( i*Slices + j+1 );
        }
    
    IB.end();
}

void TriangleSphereModel::recalcBuffers(float radius)
{
}


void TriangleSphereModel::draw(const BaseCamera& Cam)
{
    StandardModel::draw(Cam);
    
    VB.activate();
    IB.activate();
    if (instanced) {
      //glVertexAttribPointer()
      //glDrawArraysInstanced(GL_TRIANGLES, )
      //GLint instAttrib = glGetAttribLocation(uShader->ShaderProgram, "VOffsets[]");
      //std::cout << "Instattrib: " << instAttrib << "\n";
      //glEnableVertexAttribArray(instAttrib);
      //glVertexAttribPointer(instAttrib, 1000, , GL_FALSE, sizeof(float), 0);
      //glVertexAttribDivisor(instAttrib, 1);
      glDrawElementsInstanced(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), (void*)0, 50);
      //glDrawArraysInstanced(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 1000);
    }
    else {
      glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
    }
    IB.deactivate();
    VB.deactivate();
}
