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

private:
	Vector v;
	Vector r;
	void calcOrbitPos(float deltaT, bool switchMU = false);
	double calcHeronKahanFormula(float a, float b, float c);
	float calcAngleProgression(float deltaT);

};

#endif /* Satellite_hpp */
