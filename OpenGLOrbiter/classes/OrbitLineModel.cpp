//Author: Bernhard Luedtke

#include "OrbitLineModel.h"

OrbitLineModel::OrbitLineModel(std::vector<Vector> points, bool fullLine)
{
	this->points = points;
	this->evaluatePoints(fullLine);
	Matrix standard = Matrix();
	standard.translation(0.0f, 0.0f, 0.0f);
	transform(standard);
}

OrbitLineModel::OrbitLineModel(std::vector<Vector> points, Matrix transform, bool fullLine)
{
	this->points = points;
	this->evaluatePoints(fullLine);
	this->transform(transform);
}


void OrbitLineModel::draw(const BaseCamera & Cam)
{
	StandardModel::draw(Cam);
	VB.activate();

	//glDrawArrays(GL_LINE_STRIP, 0, VB.vertexCount());
	glDrawArrays(GL_LINES, 0, VB.vertexCount());

	VB.deactivate();
}

void OrbitLineModel::evaluatePoints(bool fullLine)
{
	try
	{
		VB.begin();
		VB.addColor(Color(0.0f, 1.0f, 0.0f));

		if (points.size() > 0) {
			for (unsigned int i = 1; i < points.size(); i++) {
				if (fullLine) {
					Vector p0 = points[i - 1];
					VB.addVertex(p0.X, p0.Y, p0.Z);
				}
				Vector p1 = points[i];				
				VB.addVertex(p1.X, p1.Y, p1.Z);
			}
		}
		else {
			std::cout << "NO POINTS TO DRAW THE ORBIT" << std::endl;
		}
		VB.end();
	}
	catch (const std::exception&)
	{
		std::cerr << "MIST" << std::endl;
	}
}
