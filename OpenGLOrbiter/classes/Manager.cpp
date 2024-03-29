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

#include "PhongShaderInstanced.h"
#include "lineplanemodel.h"
#include "trianglespheremodel.h"
#include "Satellite.h"
#include "FlatColorShader.h"

//Debug/Time measurement
#include <iostream>
#include <chrono>
#include <omp.h>

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif
#define sizeF 1.0f/6378.0f
#define karmanline 6478.1
#define DEG_TO_RAD(x) ((x)*0.0174532925)
#define RAD_TO_DEG(x) ((x)*57.2957795)

using std::vector;
using std::unique_ptr;
using std::endl;
using std::cout;



Manager::Manager(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin)
{
	//speedup, higher timescale = faster
	//-> Using this to slow things down might cause numerical instability!
	timeScale = 10.f;
	cout << "Timescale: " << timeScale << "\n";

	addEarth();
	double t = 0.01;
	/*
	for (auto i = 0; i < 1000; ++i) {
		addSatellite(60975.0, t, t, t, 0.00, 0.00, true, true);
		t += t;
		if (t > 1000000000.0) {
			t = 0.01 * i;
		}
		//cout << "t: " << t << "\n";
	}/**/
	/**/
	addSatellite(60975.0, 0.00, 0.00, 0.00, 0.00, 0.00, true, true);
	addSatellite(60675.0, 0.00, 0.2, 2.00, 0.01, 0.00, true, true);
	addSatellite(60375.0, 0.00, 0.6, 4.0, 0.02, 0.01, true, true);
	addSatellite(60375.0, 0.00, 1.8, 8.0, 0.03, 0.01, true, true);
	addSatellite(60375.0, 0.00, 5.4, 16.0, 0.04, 0.01, true, true);
	addSatellite(60375.0, 0.00, 16.2, 32.0, 0.05, 0.01, true, true);
	addSatellite(60375.0, 0.00, 48.6, 64.0, 0.06, 0.01, true, true);
	addSatellite(60375.0, 0.00, 145.8, 128.0, 0.07, 0.01, true, true);
	/**/
	/*
	addSatellite(22164.0, 0.01, 0.01, 0.01, 0.01, 0.01, true, true);
	addSatellite(20550.0, 300., 0., 0., 0., 0);
	addSatellite(7500, 300., 50., 280., 0.0, 0);
	addSatellite(7500, 100., 20., 210., 0.1, 0);
	addSatellite(12500, 200., 10., 250., 0.3, 0);
	addSatellite(21164.0f, 0.0f, 39.0f, 0.0f, 0.6f);
	/**/
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
	std::cout << "Satellites added.\n";
	addEquatorLinePlane();
	std::cout << "Plane added.\n";
	instanceModel = std::make_unique<TriangleSphereModel>(0.03, 12, 24);
	auto instanceShader = std::make_unique<PhongShaderInstanced>();
	instanceModel->setShader(std::move(instanceShader));
	instanceModel->transform(Matrix());
	instanceModel->instanced = true;
	std::cout << "Instancer added.\n";
}

//addSat Params: semi Major Axis, longitude of ascending node, inclination, argument of periapsis, eccentricity, true Anomaly, orbitVisualisation, fullLine
//Passing fullLine=false is experimental and may lead to numerical instability. More robust orbit visualization techniques are being thought of.
void Manager::addSatellite(double semiA, double lAscN, double incli, double argP, double ecc, double trueAnom, bool orbitVis, bool fullLine )
{
	addSatellite(OrbitEphemeris(semiA, ecc, DEG_TO_RAD(incli), DEG_TO_RAD(lAscN), DEG_TO_RAD(argP), DEG_TO_RAD(trueAnom)),orbitVis,fullLine);
}


void Manager::addSatellite(OrbitEphemeris o, bool orbitVis, bool fullLine, Color satColor)
{
	unique_ptr<PhongShader> uShader = std::make_unique<PhongShader>();
	//auto uShader = std::make_unique<PhongShaderInstanced>();
	uShader->diffuseColor(satColor);
	std::unique_ptr<Satellite> sat = std::make_unique<Satellite>(0.03f,o);
	sat->setShader(std::move(uShader));
	if (orbitVis == true) {
		std::vector<Vector> resOrbit = sat->calcOrbitVis();
		unique_ptr<FlatColorShader> uCShader = std::make_unique<FlatColorShader>(Color(0.9f, 0.2f, 0));
		unique_ptr<StandardModel> uModel = std::make_unique<OrbitLineModel>(resOrbit,fullLine);
		uModel->setShader(std::move(uCShader));
		uModels.push_back(std::move(uModel));
	}
	this->satellites.push_back(std::move(sat));
}

void Manager::addEquatorLinePlane()
{
	Matrix baseTransform = Matrix();
	unique_ptr<StandardModel>   uModel   = std::make_unique<LinePlaneModel>(20, 20, 45, 45);
	unique_ptr<FlatColorShader> uCShader = std::make_unique<FlatColorShader>();
	uCShader->color(Color(0.4f, 0.4f, 0.4f));
	uModel->setShader(std::move(uCShader));
	uModel->transform(baseTransform);
	uModels.push_back(std::move(uModel));
}

void Manager::addEarth()
{
	unique_ptr<TriangleSphereModel> uModel = std::make_unique<TriangleSphereModel>(1.0f);
	unique_ptr<PhongShader> uShader = std::make_unique<PhongShader>();
	
	uShader->diffuseTexture(Texture::LoadShared("earth5.bmp"));
	uShader->ambientColor(Color(0.5f, 0.5f, 0.5f));

	Matrix baseTransform = Matrix();
	baseTransform.translation(0.0f, 0.0f, 0.0f);
	uModel->transform(baseTransform);

	uModel->setShader(std::move(uShader));
	planets.push_back(std::move(uModel));
}

void Manager::start()
{
	// enable depth-testing
    glEnable(GL_DEPTH_TEST); 
	// depth-testing interprets a smaller value as "closer"
    glDepthFunc(GL_LESS); 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);	
}

void Manager::update(double deltaT)
{
	deltaT *= timeScale;
	int limit = satellites.size();
	//#pragma omp parallel for 
	for (int i = 0; i < limit; ++i)
	{
		satellites.at(i)->update(deltaT);
	}
	//For earth rotation. 
	const double coeff = (deltaT / 86400.0)* DEG_TO_RAD(360.0);
	for (unsigned int k = 0; k < planets.size(); k++) {
		Matrix t = planets[k]->transform();
		Matrix r = Matrix().rotationY(coeff);
		planets[k]->transform(t * r);
	}
	Cam.update();
}

void Manager::draw()
{
	//std::cout << "DrawCall\n";
  // 1. clear screen
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//2. Draw models
	for (unsigned int i = 0; i < uModels.size(); i++) {
		uModels.at(i)->draw(Cam);
	}
	//2.b) Draw Satellites
	std::vector<Vector> satPositions;
	for (unsigned int i = 0; i < satellites.size(); i++)
	{
		//satPositions.push_back(satellites.at(i)->getR());
		//cout << satPositions[i].X << "; " << satPositions[i].Y << "; " << satPositions[i].Z << "\n";
		satellites.at(i)->draw(Cam);
	}
	//reinterpret_cast<PhongShaderInstanced*>(instanceModel->uShader.get())->setInstancePositions(std::move(satPositions));
	//instanceModel->draw(Cam);
	
	//2.c) Draw Planet(s)
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
	//No pointers to release/delete as we are working with smart pointers.
}