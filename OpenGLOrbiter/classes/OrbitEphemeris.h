// Author: Bernhard Luedtke
// Created on 2020-06-02
#ifndef OrbitEphemeris_hpp
#define OrbitEphemeris_hpp
#include "Matrix.h"
class OrbitEphemeris {
public:
	OrbitEphemeris();
	OrbitEphemeris(float semiA,float ecc, float incli,float longAscNode,float argP, float trueAnomal);
	float semiMajorA;	// a
	float eccentricity; // e
	float inclination;  // i
	float longitudeAsc; // Omega
	float argPeriaps;	// w (small Omega)
	double trueAnomaly;  // T
	
	float semiMinorP = 0.0f;
	float getOrCreateSemiMinorP();

	float getCircularOrbitalPeriod();
	void calcR0V0();
	Vector r0;
	Vector v0;

	Matrix pqw;
	bool pqwExists = false;
	void calcPQWMatrix();
	Matrix getOrCreatePQW();
};

#endif /* OrbitEphemeris_hpp */
