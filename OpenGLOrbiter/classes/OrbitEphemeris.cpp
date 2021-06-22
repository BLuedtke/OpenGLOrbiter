// Author: Bernhard Luedtke
// Created on 2020-06-02
#include "OrbitEphemeris.h"
#define _USE_MATH_DEFINES
#include <math.h>

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
	semiMajorA = 400.0;
	eccentricity = 0.0;
	inclination = 0.0;
	longitudeAsc = 0.0;
	argPeriaps = 0.0;
	trueAnomaly = 0.0;
}

OrbitEphemeris::OrbitEphemeris(double semiA, double ecc, double incli,
	double longAscNode, double argP, double trueAnomal) :
	semiMajorA(semiA), eccentricity(ecc), inclination(incli),
	longitudeAsc(longAscNode), argPeriaps(argP), trueAnomaly(trueAnomal)
{
	;
}

//This is only valid for an ellipse -> Everything probably needs to be slightly rewritten for parabolas.
// Pass force=true to force the value to be calculated anew (using cached value otherwise)
double OrbitEphemeris::getEllipseOrbitalPeriod(bool forceReCalc)
{
	if (orbitalperiod < 0.001 || forceReCalc) {
		orbitalperiod = 2.0 * M_PI * std::sqrt(std::pow(semiMajorA, 3.0) / mu);
	}
	return orbitalperiod;
}

//This is simplified by assuming that the satellite is at its perigeee at start.
// For more complicated setups, this'll need improvement. Can be worked around by rotating manually.
void OrbitEphemeris::calcR0V0()
{
	try
	{
		//Construct P and Q (see 'Fundamentals of Astrodynamics', R. Bate et al.)
		Matrix pqw = this->calcPQWMatrix();
		Vector P = pqw.right();
		Vector Q = pqw.backward();
		double semiLatRect = semiMajorA * (1.0 - pow(eccentricity, 2.0));
		float rScal = static_cast<float>(semiLatRect / (1.0 + eccentricity * 1.0));
		r0 = P * rScal;
		//r0 = P * rScal * 1.0f;	//Unsure about origin of the *1.0f there TODO Re-check in the book

		Vector temp = Q * static_cast<float>(this->eccentricity + 1.0);
		v0 = temp * static_cast<float>std::sqrt(mu / semiLatRect);
		doR0V0exist = true;
	}
	catch (const std::exception& e)
	{
		doR0V0exist = false;
		std::cout << e.what() << std::endl;
	}
}
//Provides the currently saved value for R0, calculates it anew if no value is present
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

//Provides the currently saved value for V0, calculates it anew if no value is present
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

//Set new Values for r0 and v0
void OrbitEphemeris::updateR0V0(Vector nextR0, Vector nextV0)
{
	this->r0 = nextR0;
	this->v0 = nextV0;
	doR0V0exist = true;
}

//Calculates the matrix that can be used to infer the plane that the orbit lies in
Matrix OrbitEphemeris::calcPQWMatrix()
{
	try
	{
		Matrix a = Matrix().rotationY(longitudeAsc);
		Matrix b = Matrix().rotationX(inclination);
		Matrix c = Matrix().rotationAxis(Vector(0, 1, 0), argPeriaps);
		pqw = Matrix();
		pqw = pqw * a * b * c;
		return pqw;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return Matrix();
	}
}
