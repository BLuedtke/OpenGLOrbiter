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
#define sizeF 1.0f/6378.0f

#define DEG_TO_RAD(x) ((x)*0.0174532925)
#define RAD_TO_DEG(x) ((x)*57.2957795)

using std::vector;
using std::unique_ptr;
using std::endl;
using std::cout;



Manager::Manager(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin)
{
	//800x quicker than reality
	timeScale = 800.f;
	//-> Using this to slow things down might cause numerical instability!

	addEarth();
	
	
	//Sample with all parameters. This height corresponds to GEO.
	//addSatellite(42164.0, 0.0, 0.0, 0.0, 0.0, 0.0, true, true);
	
	//addSatellite(20550.0, 300., 50., 280., 0.0, 0);
	addSatellite(10550.0, 300., 50., 280., 0.0, 0);
	
	/*
	// THIS IS THE GPS 'CONSTELLATION' -> with realistic orbital elements.
	//TODO Switch the parameters to doubles.
	addSatellite(26550.0f, 302.8080f, 56.01f, 279.2863f, 0.0186085f, 0);
	addSatellite(26550.0f, 302.6010f, 56.06f, 35.3566f, 0.0113652f, 0);
	addSatellite(26550.0f, 301.5764f, 56.01f, 65.4874f, 0.0078611f, 0);
	addSatellite(26550.0f, 297.6432f, 55.32f, 52.6418f, 0.0090654f, 0);
	addSatellite(26550.0f, 295.5525f, 54.82f, 10.4744f, 0.0046989f, 0);
	addSatellite(26550.0f, 239.0614f, 54.86f, 6.1746f, 0.0094815f, 0);
	addSatellite(26550.0f, 239.5030f, 53.77f, 198.5504f, 0.0047049f, 0);
	addSatellite(26550.0f, 238.2884f, 54.61f, 223.4894f, 0.0140270f, 0);
	addSatellite(26550.0f, 234.0640f, 53.65f, 37.1639f, 0.0095254f, 0);
	addSatellite(26550.0f, 184.8549f, 55.46f, 60.1978f, 0.0039199f, 0);
	addSatellite(26550.0f, 182.1325f, 55.03f, 250.6634f, 0.0116744f, 0);
	addSatellite(26550.0f, 176.1806f, 54.06f, 235.4258f, 0.0144104f, 0);
	addSatellite(26550.0f, 171.5119f, 53.18f, 49.6816f, 0.0121179f, 0);
	addSatellite(26550.0f, 177.0815f, 55.09f, 222.6391f, 0.0041555f, 0);
	addSatellite(26550.0f, 179.3429f, 55.09f, 223.8194f, 0.0008908f, 0);
	addSatellite(26550.0f, 176.5922f, 54.58f, 105.8600f, 0.0012930f, 0);
	addSatellite(26550.0f, 117.4385f, 55.09f, 211.3608f, 0.0057282f, 0);
	addSatellite(26550.0f, 109.8874f, 53.52f, 154.9692f, 0.0050379f, 0);
	addSatellite(26550.0f, 113.0046f, 53.32f, 292.6875f, 0.0073969f, 0);
	addSatellite(26550.0f, 115.7546f, 54.58f, 44.0539f, 0.0057386f, 0);
	addSatellite(26550.0f, 117.5668f, 55.33f, 36.8613f, 0.0027203f, 0);
	addSatellite(26550.0f, 53.7124f, 54.61f, 283.9817f, 0.019f, 0);
	addSatellite(26550.0f, 53.5235f, 54.96f, 266.9234f, 0.0199562f, 0);
	addSatellite(26550.0f, 57.9073f, 56.16f, 44.0277f, 0.0097490f, 0);
	addSatellite(26550.0f, 57.4396f, 56.13f, 296.4550f, 0.0021052f, 0);
	addSatellite(26550.0f,   3.4232f, 56.27f, 93.3527f,	0.0092063f, 0);
	addSatellite(26550.0f,   0.7710f, 56.37f, 266.0448f, 0.0134264f, 0);
	addSatellite(26550.0f,   1.4579f, 56.47f, 114.9660f, 0.0012895f, 0);
	addSatellite(26550.0f, 357.4799f, 56.04f, 30.9207f, 0.0081731f, 0);
	addSatellite(26550.0f, 356.6780f, 55.09f, 353.3877f, 0.0053773f, 0);
	/**/

	// Random Satellite
	//addSatellite(28164.0f, 0.0f, 19.0f, 0.0f, 0.6f);
	//addSatellite(9164.0f, 0.0f, 90.0f, 0.0f, 0.2f);

	/**/
	//addEquatorLinePlane();
}

//addSat Params: semi Major Axis, longitude of ascending node, inclination, argument of periapsis, eccentricity, true Anomaly, orbitVisualisation, fullLine
//Passing fullLine=false is experimental and may lead to numerical instability. More robust orbit visualization techniques are being thought of.
void Manager::addSatellite(double semiA, double lAscN, double incli, double argP, double ecc, double trueAnom, bool orbitVis, bool fullLine )
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
		unique_ptr<FlatColorShader> uCShader = std::make_unique<FlatColorShader>(Color(0, 0.4f, 0));
		unique_ptr<StandardModel> uModel = std::make_unique<OrbitLineModel>(resOrbit,fullLine);
		uModel->setShader(std::move(uCShader));
		uModels.push_back(std::move(uModel));
	}
	this->satellites.push_back(std::move(sat));
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
	//In Debug mode, loading the large earth texture takes VERY long -> load simpler texture
#ifdef _DEBUG
	uShader->diffuseTexture(Texture::LoadShared("earth.jpg"));
#else
	uShader->diffuseTexture(Texture::LoadShared("earth5.jpg"));
#endif // DEBUG

	uShader->ambientColor(Color(0.5f, 0.5f, 0.5f));
	uModel->transform(baseTransform);
	uModel->setShader(std::move(uShader));
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

void Manager::update(double deltaT)
{
	deltaT *= timeScale;
	for (unsigned int i = 0; i < satellites.size(); i++)
	{
		satellites[i]->update(deltaT);
	}
	//For earth rotation. 
	const double coeff = (deltaT / 86400.0)*DEG_TO_RAD(360);
	for (unsigned int k = 0; k < planets.size(); k++) {
		Matrix t = planets[k]->transform();
		Matrix r = Matrix().rotationY(coeff);
		planets[k]->transform(t * r);
	}
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