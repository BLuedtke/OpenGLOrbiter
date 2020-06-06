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
	void update(double dtime);
	OrbitEphemeris ephemeris;
	float speedUp = 1.0f;
	void calcKeplerProblem(double timePassed);

private:
	Vector v;
	Vector r;
	double totalTime = 0.0;

	void calcOrbitPos(float deltaT, bool fixedStep = false);
	double calcHeronKahanFormula(double a, double b, double c);
	double calcHeronKahanFormula(Vector k, Vector i);
	double calcAngleProgression(float deltaT);

	double computeXfirstGuess(double t);
	double computeZ(double x);

	double xnNewtonIteration(double xn, double t, double tn, double xf);
	double computeNewDtDx(double xn);
	double computeNewDtDxWithMU(double xn);
	double computeTnByXn(double x, double z);
	
	double computeSmallF(double x);
	double computeSmallG(double x, double t);
	double computeSmallFDerivative(double x, double rLength);
	double computeSmallGDerivative(double x, double rLength);

	Vector computeRVec(double f, double g);
	Vector computeVVec(double fD, double gD);
	
	double computeCseries(double z, unsigned int maxSteps);
	double computeSseries(double z, unsigned int maxSteps);
};

#endif /* Satellite_hpp */
