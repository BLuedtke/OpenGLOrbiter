// Author: Bernhard Luedtke
// Created on 2020-06-02
#include "OrbitEphemeris.h"
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef muS
#define muS 0.0000015399
#endif // !muS
#ifndef mu
// This uses the unit km³/s², NOT m³/s²!!
#define mu (double)398600
#endif // !mu
#ifndef sizeFactor
#define sizeFactor 1.0f/6378.0f
#endif // !sizeFactor



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

OrbitEphemeris::OrbitEphemeris(double semiA, double ecc, double incli,
	double longAscNode, double argP, double trueAnomal) :
	semiMajorA(semiA), eccentricity(ecc), inclination(incli),
	longitudeAsc(longAscNode), argPeriaps(argP), trueAnomaly(trueAnomal)
{
	;
}

double OrbitEphemeris::getOrCreateSemLatRect()
{
	if (this->semiLatRect == 0.0 || (this->semiLatRect > -0.0000000001 && this->semiLatRect < 0.0000000001)) {
		this->semiLatRect = semiMajorA*(1.0 - pow(eccentricity,2.0));
	}
	return semiLatRect;
}

double OrbitEphemeris::getEllipseOrbitalPeriod()
{
	return 2.0 * M_PI * std::sqrt(std::pow(semiMajorA, 3.0) / mu);
}

void OrbitEphemeris::calcR0V0()
{
	try
	{
		double semiLatRect = this->getOrCreateSemLatRect();
		float rScal = (float)(semiLatRect / (1.0 + eccentricity * 1.0));
		Matrix pqw = this->getPQWMatrix();
		Vector P = pqw.right();
		Vector Q = pqw.backward();
		//This is simplified by assuming r0 and v0 are always @ perigee. 
		// For more complicated setups, this'll need improvement.
		r0 = P * rScal * 1.0f;
		Vector temp = Q * ((float)this->eccentricity + 1.0f);
		v0 = temp * (float)std::sqrt(mu / semiLatRect);
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
		std::cout << "V0: ";
		v0.print();
		std::cout << v0.length();
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

void OrbitEphemeris::updateR0V0(Vector nextR0, Vector nextV0)
{
	this->r0 = nextR0;
	this->v0 = nextV0;
	doR0V0exist = true;
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

Matrix OrbitEphemeris::getPQWMatrix()
{
	if (pqwExists == true) {
		return pqw;
	}
	else {
		calcPQWMatrix();
		return pqw;
	}
}

double OrbitEphemeris::getAlphaValue()
{
	if (alphaExists) {
		return alpha;
	}
	if (!doR0V0exist) {
		this->calcR0V0();
	}
	double upper = (2.0 * mu / (float)r0.length() - v0.lengthSquared());
	return (upper/mu);
}
