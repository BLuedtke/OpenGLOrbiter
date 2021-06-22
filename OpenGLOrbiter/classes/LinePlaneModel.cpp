//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.

#include "LinePlaneModel.h"

LinePlaneModel::LinePlaneModel( float DimX, float DimZ, int NumSegX, int NumSegZ )
{
    VB.begin();
    
    float StepX = DimX / static_cast<float>(NumSegX);
    float StepZ = DimZ / static_cast<float>(NumSegZ);
    
    float BeginX = -DimZ/2.0f;
    float BeginZ = -DimX/2.0f;
    
    for( int i=0; i<=NumSegX; ++i )
    {
        VB.addVertex( BeginZ + i*StepX, 0, BeginX );
        VB.addVertex( BeginZ + i*StepX, 0, -BeginX );
    }
    for( int i=0; i<=NumSegZ; ++i )
    {
        VB.addVertex( BeginZ, 0, BeginX + i*StepZ );
        VB.addVertex( -BeginZ, 0, BeginX + i*StepZ );
    }
    
    VB.end();
    
}

void LinePlaneModel::draw(const BaseCamera& Cam)
{
    StandardModel::draw(Cam);

    VB.activate();
    
    glDrawArrays(GL_LINES, 0, VB.vertexCount());
    
    VB.deactivate();
}