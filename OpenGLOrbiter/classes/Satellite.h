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

	void update(double dtime);
	void calcKeplerProblem(double timePassed, double t0);

	Vector getV() const;
	Vector getR() const;
	OrbitEphemeris getEphemeris() const;

	std::vector<Vector> calcOrbitVis();

private:
	Vector v;
	Vector r;
	OrbitEphemeris ephemeris;

	double totalTime = 0.0;
	//stupid hack
	double savedTime = 0.0;

	//counts completed orbits
	unsigned int orbits = 0;
	float lastOrbitV0Length = 0.f;
	float lastOrbitR0Length = 0.f;

	double computeXfirstGuess(double t);
	double computeZ(double x);

	double xnNewtonIteration(double xn, double t, double tn, double xf);
	double computeNewDtDx(double xn);
	double computeTnByXn(double x, double z);
	
	double computeSmallF(double x);
	double computeSmallG(double x, double t);
	double computeSmallFDerivative(double x, double rLength);
	double computeSmallGDerivative(double x, double rLength);

	Vector computeRVec(double f, double g);
	Vector computeVVec(double fD, double gD);
	
	double computeCseries(double z);
	double computeSseries(double z);
};

#endif /* Satellite_hpp */
