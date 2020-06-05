// Author: Bernhard Luedtke
// Created on 2020-06-02
#include "OrbitEphemeris.h"
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef mu
#define mu 0.0000015399
#endif // !mu


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
	;
}

float OrbitEphemeris::getOrCreateSemiMinorP()
{
	if (this->semiMinorP != 0.0f || (this->semiMinorP > 0.0f - 0.00001f && this->semiMinorP < 0.0f + 0.000001f)) {
		this->semiMinorP = semiMajorA*(1.0f - eccentricity * eccentricity);
	}
	return semiMinorP;
}

float OrbitEphemeris::getCircularOrbitalPeriod()
{
	return 2.0f * (float)M_PI * std::sqrtf(std::powf(semiMajorA, 3.0f) / (float)mu);
}

void OrbitEphemeris::calcR0V0()
{
	float semiMinorP = this->getOrCreateSemiMinorP();
	float rScal = semiMinorP / (1 + eccentricity * cosf(0.0f));

	Matrix pqw = this->getOrCreatePQW();
	Vector P = pqw.right();
	Vector Q = pqw.backward();
	r0 = P * rScal * cosf(0.0f) + Q * rScal * sinf(0.0f);
	Vector temp = P * -1.0f * sinf(0.0f) + Q * (this->eccentricity + cosf(0.0f));
	v0 = temp * (float)std::sqrt(mu / semiMinorP);
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
