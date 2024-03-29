//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.

#ifndef Manager_hpp
#define Manager_hpp

#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "FlatColorShader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "StandardModel.h"
#include "Satellite.h"
#include "OrbitLineModel.h"

class Manager
{
public:
  Manager(GLFWwindow* pWin);
  void start();
  void update(double deltaT);
  void draw();
  void end();
protected:
  Camera Cam;
	std::vector<std::unique_ptr<Satellite>> satellites;
	GLFWwindow* pWindow;
	std::vector<std::unique_ptr<StandardModel>> uModels;
	std::vector<std::unique_ptr<TriangleSphereModel>> planets;
	std::unique_ptr<TriangleSphereModel> instanceModel{};
	float timeScale = 1.0f;
	
	void addEarth();
	void addSatellite(double semiA, double lAscN, double incli, double argP, double ecc = 0.0f, double trueAnom = 0.0, bool orbitVis = true, bool fullLine = true);
	void addSatellite(OrbitEphemeris o, bool orbitVis = true, bool fullLine = true, Color satColor = Color(1.0f,.1f,.1f));
	void addEquatorLinePlane();
};

#endif /* Manager_hpp */
