//Author: Bernhard Luedtke
#ifndef Satellite_hpp
#define Satellite_hpp

#include "TriangleSphereModel.h"
#include "OrbitLineModel.h"
#include "OrbitEphemeris.h"

class Satellite : public TriangleSphereModel {
	
public:
	Satellite(float Radius, int Stacks = 18, int Slices = 36);
	Satellite(float Radius, OrbitEphemeris eph, int Stacks = 18, int Slices = 36);
	Satellite();
	~Satellite();
	std::vector<Vector> calcOrbitVis();
	void update(float dtime);
	OrbitEphemeris ephemeris;
	float speedUp = 1.0f;
	void testKeplerProblem(float timePassed);

private:
	Vector v;
	Vector r;
	double totalTime = 0.0;

	void calcOrbitPos(float deltaT, bool fixedStep = false);
	double calcHeronKahanFormula(double a, double b, double c);
	double calcHeronKahanFormula(Vector k, Vector i);
	double calcAngleProgression(float deltaT);

	float computeXfirstGuess(float t);
	float computeZ(float x);

	float xnNewtonIteration(float xn, float t, float tn, float xf);
	float computeNewDtDx(float xn);
	float computeNewDtDxWithMU(float xn);
	float computeTnByXn(float x, float z);
	
	float computeSmallF(float x);
	float computeSmallG(float x, float t);
	float computeSmallFDerivative(float x, float rLength);
	float computeSmallGDerivative(float x, float rLength);

	Vector computeRVec(float f, float g);
	Vector computeVVec(float fD, float gD);
	
	float computeCseries(float z, unsigned int maxSteps);
	float computeSseries(float z, unsigned int maxSteps);
};

#endif /* Satellite_hpp */
