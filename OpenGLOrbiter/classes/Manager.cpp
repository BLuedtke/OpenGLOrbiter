//  Modified by: Bernhard Luedtke
//  Author of the original Class (heavily changed since then):
    //  Created by Philipp Lensing on 19.09.16.

#include "Manager.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#else
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#endif
#endif

#include "lineplanemodel.h"
#include "trianglespheremodel.h"
#include "Satellite.h"
#include "FlatColorShader.h"

//Debug/Time measurement
#include <iostream>
#include <chrono>

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif
#define sizeF 1.0f/6371.0f

#define DEG_TO_RAD(x) ((x)*0.0174532925f)
#define RAD_TO_DEG(x) ((x)*57.2957795f)

using std::vector;
using std::unique_ptr;
using std::endl;
using std::cout;



Manager::Manager(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin)
{
	addEarth();
	/*
	// THIS IS THE GPS 'CONSTELLATION'
	addSatellite(26550.0f * sizeF, 302.8080, 56.01f, 279.2863, 0.0186085f, 0);
	addSatellite(26550.0f * sizeF, 302.6010, 56.06f, 35.3566, 0.0113652f, 0);
	addSatellite(26550.0f * sizeF, 301.5764, 56.01f, 65.4874, 0.0078611f, 0);
	addSatellite(26550.0f * sizeF, 297.6432, 55.32f, 52.6418, 0.0090654f, 0);
	addSatellite(26550.0f * sizeF, 295.5525, 54.82f, 10.4744, 0.0046989f, 0);
	addSatellite(26550.0f * sizeF, 239.0614, 54.86f, 6.1746, 0.0094815f, 0);
	addSatellite(26550.0f * sizeF, 239.5030, 53.77f, 198.5504, 0.0047049f, 0);
	addSatellite(26550.0f * sizeF, 238.2884, 54.61f, 223.4894, 0.0140270f, 0);
	addSatellite(26550.0f * sizeF, 234.0640, 53.65f, 37.1639, 0.0095254f, 0);
	addSatellite(26550.0f * sizeF, 184.8549, 55.46f, 60.1978, 0.0039199f, 0);
	addSatellite(26550.0f * sizeF, 182.1325, 55.03f, 250.6634, 0.0116744f, 0);
	addSatellite(26550.0f * sizeF, 176.1806, 54.06f, 235.4258, 0.0144104f, 0);
	addSatellite(26550.0f * sizeF, 171.5119, 53.18f, 49.6816, 0.0121179f, 0);
	addSatellite(26550.0f * sizeF, 177.0815, 55.09f, 222.6391, 0.0041555f, 0);
	addSatellite(26550.0f * sizeF, 179.3429, 55.09f, 223.8194, 0.0008908f, 0);
	addSatellite(26550.0f * sizeF, 176.5922, 54.58f, 105.8600, 0.0012930f, 0);
	addSatellite(26550.0f * sizeF, 117.4385, 55.09f, 211.3608, 0.0057282f, 0);
	addSatellite(26550.0f * sizeF, 109.8874, 53.52f, 154.9692, 0.0050379f, 0);
	addSatellite(26550.0f * sizeF, 113.0046, 53.32f, 292.6875, 0.0073969f, 0);
	addSatellite(26550.0f * sizeF, 115.7546, 54.58f, 44.0539, 0.0057386f, 0);
	addSatellite(26550.0f * sizeF, 117.5668, 55.33f, 36.8613, 0.0027203f, 0);
	addSatellite(26550.0f * sizeF, 53.7124, 54.61f, 283.9817, 0.019f, 0);
	addSatellite(26550.0f * sizeF, 53.5235, 54.96f, 266.9234, 0.0199562f, 0);
	addSatellite(26550.0f * sizeF, 57.9073, 56.16f, 44.0277, 0.0097490f, 0);
	addSatellite(26550.0f * sizeF, 57.4396, 56.13f, 296.4550, 0.0021052f, 0);
	addSatellite(26550.0f * sizeF,   3.4232, 56.27f, 93.3527,	0.0092063f, 0);
	addSatellite(26550.0f * sizeF,   0.7710, 56.37f, 266.0448,	0.0134264f, 0);
	addSatellite(26550.0f * sizeF,   1.4579, 56.47f, 114.9660,	0.0012895f, 0);
	addSatellite(26550.0f * sizeF, 357.4799, 56.04f, 30.9207, 0.0081731f, 0);
	addSatellite(26550.0f * sizeF, 356.6780, 55.09f, 353.3877, 0.0053773f, 0);
	/**/


	// THIS IS A ISS-LIKE SATELLITE

	addSatellite(6796.0f * sizeF, 0.0f, 51.6f, 0.0f, 0.0f);

	/**/
	addEquatorLinePlane();

	//THIS SPEEDS UP EVERYTHING BY THE FACTOR
	speedUpSats(1.0f);
	//-> Using this to slow things down might cause numerical instability!
}

//Passing fullLine=false is experimental and lead to numerical instability. More robust orbit visualization techniques are being thought of.
void Manager::addSatellite(float semiA, float lAscN, float incli, float argP, float ecc, float trueAnom, bool orbitVis, bool fullLine )
{
	OrbitEphemeris o = OrbitEphemeris();
	o.semiMajorA = semiA;
	o.eccentricity = ecc;
	o.inclination = DEG_TO_RAD(incli);
	o.longitudeAsc = DEG_TO_RAD(lAscN);
	o.argPeriaps = DEG_TO_RAD(argP);
	o.trueAnomaly = DEG_TO_RAD(trueAnom);
	addSatellite(o,orbitVis,fullLine);
}

void Manager::addSatellite(OrbitEphemeris o, bool orbitVis, bool fullLine, Color satColor)
{
	unique_ptr<PhongShader> uShader = std::make_unique<PhongShader>();
	uShader->diffuseColor(satColor);
	std::unique_ptr<Satellite> sat = std::make_unique<Satellite>(0.03f,o);
	sat->setShader(std::move(uShader));
	if (orbitVis == true) {
		std::vector<Vector> resOrbit = sat->calcOrbitVis();
		unique_ptr<FlatColorShader> uCShader = std::make_unique<FlatColorShader>(Color(0, 1, 0));
		unique_ptr<StandardModel> uModel = std::make_unique<OrbitLineModel>(resOrbit,fullLine);
		uModel->setShader(std::move(uCShader));
		uModels.push_back(std::move(uModel));
	}
	this->satellites.push_back(std::move(sat));
}

void Manager::speedUpSats(float speedUp)
{
	for (unsigned int i = 0; i < this->satellites.size(); i++)
	{
		satellites[i]->speedUp = speedUp;
	}
}

void Manager::addEquatorLinePlane()
{
	Matrix baseTransform = Matrix();
	std::unique_ptr<StandardModel> uModel = std::make_unique<LinePlaneModel>(10, 10, 5, 5);
	unique_ptr<FlatColorShader>uCShader = std::make_unique<FlatColorShader>();
	uCShader->color(Color(0.1f, 0.5f, 0.1f));
	uModel->setShader(std::move(uCShader));
	uModel->transform(baseTransform);
	uModels.push_back(std::move(uModel));
}

void Manager::addEarth()
{
	Matrix baseTransform = Matrix();
	baseTransform.translation(0.0f, 0.0f, 0.0f);

	unique_ptr<TriangleSphereModel> uModel = std::make_unique<TriangleSphereModel>(1.0f);
	std::unique_ptr<PhongShader> uShader = std::make_unique<PhongShader>();
	uShader->diffuseTexture(Texture::LoadShared("earth.jpg"));
	uShader->ambientColor(Color(0.5f, 0.5f, 0.5f));

	uModel->transform(baseTransform);
	uModel->setShader(std::move(uShader));
	//This uses the planets std::vector for preparing to let earth rotate -> will be implemented sometime soon (tm)
	planets.push_back(std::move(uModel));
}

void Manager::start()
{
	//Might need Depth-Testing later.
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);	
}

void Manager::update(float deltaT)
{
	for (unsigned int i = 0; i < satellites.size(); i++)
	{
		satellites[i]->update(deltaT);
	}
	//TODO add planet updates

	Cam.update();
}

void Manager::draw()
{
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//2. Draw models
	for (unsigned int i = 0; i < uModels.size(); i++) {
		uModels.at(i)->draw(Cam);
	}
	//2.b) Draw Satellites
	for (unsigned int i = 0; i < satellites.size(); i++)
	{
		satellites[i]->draw(Cam);
	}
	for (unsigned int i = 0; i < planets.size(); i++)
	{
		planets[i]->draw(Cam);
	}
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}
void Manager::end()
{
	cout << "Ending." << endl;
}