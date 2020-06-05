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
	if (this->semiMinorP == 0.0f || (this->semiMinorP > 0.0f - 0.00001f && this->semiMinorP < 0.0f + 0.000001f)) {
		this->semiMinorP = semiMajorA*(1.0f - pow(eccentricity,2.0));
	}
	return semiMinorP;
}

float OrbitEphemeris::getCircularOrbitalPeriod()
{
	return 2.0f * (float)M_PI * std::sqrtf(std::powf(semiMajorA, 3.0f) / (float)mu);
}

void OrbitEphemeris::calcR0V0()
{
	try
	{

		float semiMinorP = this->getOrCreateSemiMinorP();
		if (eccentricity >= 1.0f) {
			//semiMinorP
		}
		float rScal = semiMinorP / (1 + eccentricity * 1.0f);
		float h = std::sqrt(mu*semiMinorP);
		std::cout << "1.7-9 h: " << h << std::endl;
		Matrix pqw = this->getOrCreatePQW();
		Vector P = pqw.right();
		Vector Q = pqw.backward();
		r0 = P * rScal * 1.0f + Q * rScal * 0.0f;
		Vector temp = P * -1.0f * 0.0f + Q * (this->eccentricity + 1.0f);
		v0 = temp * (float)std::sqrt(mu / semiMinorP);
		Vector h0 = r0.cross(v0);
		std::cout << "r0xV0 h: " << h0.length() << std::endl;
		float a = (-mu) / (powf(v0.length(), 2.0f) - 2 * mu / r0.length());
		std::cout << "Given a: " << semiMajorA << "; r0v0 a: " << a << std::endl;
		doR0V0exist = true;
	}
	catch (const std::exception& e)
	{
		doR0V0exist = false;
		std::cout << e.what() << std::endl;
	}
	
}

Vector OrbitEphemeris::getR0()
{
	if (doR0V0exist) {
		return r0;
	}
	else
	{
		calcR0V0();
		return r0;
	}
}

Vector OrbitEphemeris::getV0()
{
	if (doR0V0exist) {
		return v0;
	}
	else
	{
		calcR0V0();
		return v0;
	}
}


void OrbitEphemeris::calcPQWMatrix()
{
	try
	{
		Matrix a = Matrix().rotationY(longitudeAsc);
		Matrix b = Matrix().rotationX(inclination);
		Vector axis = Vector(0, 1, 0);
		Matrix c = Matrix().rotationAxis(axis, argPeriaps);
		pqw = Matrix();
		pqw = pqw * a * b * c;
		pqwExists = true;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
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
