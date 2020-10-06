//Author: Bernhard Luedtke

#ifndef OrbitLineModel_hpp
#define OrbitLineModel_hpp

#include <stdio.h>
#include "StandardModel.h"
#include "vertexbuffer.h"

class OrbitLineModel : public StandardModel
{
public:
	OrbitLineModel(std::vector<Vector> points, bool fullLine = true);
	OrbitLineModel(std::vector<Vector> points, Color c, bool fullLine = true);
	OrbitLineModel(std::vector<Vector> points, Matrix transform, bool fullLine = true);
	virtual ~OrbitLineModel() {}
	virtual void draw(const BaseCamera& Cam);
	
	std::vector<Vector> points;
protected:
	VertexBuffer VB;
	void evaluatePoints(bool fullLine,Color c = Color(0.0f,0.6f,0.0f));
};

#endif /* OrbitLineModel_hpp */
