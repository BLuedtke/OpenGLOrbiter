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
	OrbitLineModel(std::vector<Vector> points, Matrix transform, bool fullLine = true);
	virtual ~OrbitLineModel() {}
	virtual void draw(const BaseCamera& Cam);
	void evaluatePoints(bool fullLine);
	std::vector<Vector> points;
protected:
	VertexBuffer VB;

};

#endif /* OrbitLineModel_hpp */
