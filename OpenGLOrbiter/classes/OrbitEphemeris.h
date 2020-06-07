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
	
	double getOrCreateSemLatRect();
	double getEllipseOrbitalPeriod();

	Vector getR0();
	Vector getV0();
	void updateR0V0(Vector nextR0, Vector nextV0);
	Matrix getPQWMatrix();
	double getAlphaValue();
private:
	double semiLatRect = 0.0;
	double alpha = 0.0;
	Vector r0;
	Vector v0;
	Matrix pqw;
	bool doR0V0exist = false;
	bool pqwExists = false;
	bool alphaExists = false;
	void calcPQWMatrix();
	void calcR0V0();
};

#endif /* OrbitEphemeris_hpp */
