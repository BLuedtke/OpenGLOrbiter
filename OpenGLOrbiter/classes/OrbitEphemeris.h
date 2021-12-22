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
	
	double getEllipseOrbitalPeriod(bool forceReCalc=false);
	Vector _getR0_raw() const noexcept { return r0;  } ;
	Vector _getV0_raw() const noexcept { return v0;  };
	Vector getR0();
	Vector getV0();
	void updateR0V0(Vector nextR0, Vector nextV0);

private:
	
	double orbitalperiod = 0.0;
	Vector r0;
	Vector v0;
	Matrix pqw;
	Matrix calcPQWMatrix();
	bool doR0V0exist = false;
	void calcR0V0();
};

#endif /* OrbitEphemeris_hpp */
