//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.

#ifndef LinePlaneModel_hpp
#define LinePlaneModel_hpp

#include <stdio.h>
#include "StandardModel.h"
#include "vertexbuffer.h"

class LinePlaneModel : public StandardModel
{
public:
    LinePlaneModel( float DimX, float DimZ, int NumSegX, int NumSegZ );
    virtual ~LinePlaneModel() {}
    virtual void draw(const BaseCamera& Cam);
protected:
    VertexBuffer VB;
};

#endif /* LinePlaneModel_hpp */
