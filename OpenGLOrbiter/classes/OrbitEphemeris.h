// Author: Bernhard Luedtke
// Created on 2020-06-02
#ifndef OrbitEphemeris_hpp
#define OrbitEphemeris_hpp
#include "Matrix.h"
class OrbitEphemeris {
public:
	OrbitEphemeris();
	OrbitEphemeris(double semiA, double ecc, double incli, double longAscNode, double argP, double trueAnomal);
	double semiMajorA;	// a
	double eccentricity; // e
	double inclination;  // i
	double longitudeAsc; // Omega
	double argPeriaps;	// w (small Omega)
	double trueAnomaly;  // T
	
	double semiMinorP = 0.0f;
	double getOrCreateSemLatRect();

	double getEllipseOrbitalPeriod();
	void calcR0V0();
	Vector getR0();
	Vector getV0();
	bool doR0V0exist = false;
	
	Matrix pqw;
	bool pqwExists = false;
	void calcPQWMatrix();
	Matrix getOrCreatePQW();
private:
	Vector r0;
	Vector v0;
};

#endif /* OrbitEphemeris_hpp */
