//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.


#ifndef TriangleSphereModel_hpp
#define TriangleSphereModel_hpp

#include <stdio.h>
#include "StandardModel.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"

class TriangleSphereModel : public StandardModel
{
public:
    TriangleSphereModel( float Radius, int Stacks=36, int Slices=72 );
	  void recalcBuffers(float radius);
    virtual ~TriangleSphereModel() {}
    virtual void draw(const BaseCamera& Cam);
    bool instanced;
protected:
    VertexBuffer VB;
    IndexBuffer IB;
};


#endif /* TriangleSphereModel_hpp */
