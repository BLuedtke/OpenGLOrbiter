// Author: Bernhard Luedtke
// Created on 2020-06-02
#include "OrbitEphemeris.h"


OrbitEphemeris::OrbitEphemeris()
{
	//Default Konstruktor
	semiMajorA = 2.0f;
	eccentricity = 0.0f;
	inclination = 0.0f;
	longitudeAsc = 0.0f;
	argPeriaps = 0.0f;
	trueAnomaly = 0.0;
}

OrbitEphemeris::OrbitEphemeris(float semiA, float ecc, float incli, 
								float longAscNode, float argP, float trueAnomal) :
	semiMajorA(semiA), eccentricity(ecc), inclination(incli),
	longitudeAsc(longAscNode), argPeriaps(argP), trueAnomaly(trueAnomal)
{
}

float OrbitEphemeris::getOrCreateSemiMinorP()
{
	if (this->semiMinorP != 0.0f || (this->semiMinorP > 0.0f - 0.00001f && this->semiMinorP < 0.0f + 0.000001f)) {
		this->semiMinorP = semiMajorA*(1.0f - eccentricity * eccentricity);
	}
	return semiMinorP;
}

void OrbitEphemeris::calcPQWMatrix()
{
	Matrix a = Matrix().rotationY(longitudeAsc);
	Matrix b = Matrix().rotationX(inclination);
	pqw = Matrix();
	//pqw = pqw * b * a;
	//Vector t = pqw.right().cross(pqw.forward());
	//Vector axis = pqw.forward().normalize();
	//axis.print();
	Vector axis = Vector(0, 1, 0);
	//Matrix c = Matrix();
	Matrix c = Matrix().rotationAxis(axis, argPeriaps);
	pqw = pqw * a * b * c;
	pqwExists = true;
}

Matrix OrbitEphemeris::getOrCreatePQW()
{
	if (pqwExists == true) {
		return pqw;
	}
	else {
		calcPQWMatrix();
		return pqw;
	}
}
