//Author: Bernhard Luedtke

#include "Satellite.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define DEG_TO_RAD(x) ((x)*0.0174532925f)
#define RAD_TO_DEG(x) ((x)*57.2957795f)

#define G 8.6852f
#define sizeFactor 1.0f/6378.0f
// This mu was determined by trial-and-error, measured by approximating the flight time of the ISS orbit
// (adjusted so that with speedUp 1.0f, ISS orbit flighttime is about the same as in real life (roughly).
#define mu 0.000001549
//Testing other µs
#define mu2 0.005f

using std::cout;
using std::endl;

Satellite::Satellite(float Radius, int Stacks, int Slices) : TriangleSphereModel(Radius,Stacks,Slices)
{
	Matrix standard = Matrix();
	standard.translation(0.0f, 0.0f, 0.0f);
	transform(standard);
}

Satellite::Satellite(float Radius, OrbitEphemeris eph, int Stacks, int Slices) : TriangleSphereModel(Radius, Stacks, Slices)
{
	this->ephemeris = eph;
	Matrix standard = Matrix();
	standard.translation(0.0f, 0.0f, 0.0f);
	transform(standard);
}

Satellite::Satellite() : TriangleSphereModel(1.0f, 18, 36)
{
	Matrix standard = Matrix();
	standard.translation(0.0f, 0.0f, 0.0f);
	transform(standard);
}

Satellite::~Satellite()
{
	//No dynamically (new) allocated memory in this class so far
}


void Satellite::update(float deltaT)
{
	try
	{
		calcOrbitPos(deltaT);
		Matrix f = Matrix();
		f.translation(r);
		uTransform = f;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}


void Satellite::calcOrbitPos(float deltaT, bool switchMU) {
	Matrix pqw = this->ephemeris.getOrCreatePQW();
	Vector P = pqw.right();
	Vector Q = pqw.backward();
	float semiMinorP = ephemeris.getOrCreateSemiMinorP();
	this->ephemeris.trueAnomaly = calcAngleProgression(deltaT * speedUp);
	float posAngle = this->ephemeris.trueAnomaly;
	float rScal = semiMinorP / (1 + ephemeris.eccentricity * cosf(posAngle));
	r = P * rScal * cosf(posAngle) + Q* rScal * sinf(posAngle);
	Vector temp = P * -1.0f*sinf(posAngle)+ Q * (ephemeris.eccentricity+cosf(posAngle));
	if (switchMU) {
		//Testing
		v = temp * (float)std::sqrt(mu2 / semiMinorP);
	}
	else {
		v = temp * (float)std::sqrt(mu / semiMinorP);
	}
	//cout << v.length() << endl;
	// If you want to check: the length of h (r cross v) should always be the same for one satellite at any point in orbit
	// Might be influenced by floating point inaccuracies
	
	//Vector h = r.cross(v);
	//cout << h.length() << endl;
}

float Satellite::calcAngleProgression(float deltaT)
{
	float sAnomaly = 0.0f;
	if (r.length() <= 0.0001f || deltaT <= 0.001f) {
		sAnomaly = 0.00001f + this->ephemeris.trueAnomaly;
	}
	else {
		Vector rCandidate = r + v * (deltaT);
		float a = rCandidate.length();
		float b = r.length();
		float c = (rCandidate - r).length();
		double angle = calcHeronKahanFormula(a, b, c);
		sAnomaly = (float)(angle + this->ephemeris.trueAnomaly);
		while (sAnomaly > M_PI*2.0f) {
			sAnomaly = sAnomaly - ((float)M_PI*2.0f);
		}
	}
	if (sAnomaly <= this->ephemeris.trueAnomaly + 0.0000000001 && this->ephemeris.trueAnomaly - sAnomaly <= 0.1f) {
		cout << "Angle not large enough" << endl;
	}
	//posAngle = sAnomaly;
	return sAnomaly;
}

//https://scicomp.stackexchange.com/questions/27689/numerically-stable-way-of-computing-angles-between-vectors
double Satellite::calcHeronKahanFormula(float a, float b, float c)
{
	double tMu = 0.000001;
	if (a >= (b - 0.0001f)) {
		if (b >= c && c >= 0) {
			tMu = c - (a - b);
		}
		else if (c > b && b >= 0) {
			tMu = b - (a - c);
		}
	}
	else {
		std::cerr << "Invalid TriangleB. a: " << a << "; b: " << b << endl;
	}
	double term1 = ((a - b) + c)*tMu;
	double term2 = (a + (b + c))*((a - c) + b);
	return 2.0*atan(std::sqrt((term1) / (term2)));;
}

//Method for going through the orbit and calculating a collection of points along the orbit
// This can be used to represent the trajectory with lines (see code in Manager.cpp and OrbitLineModel).
std::vector<Vector> Satellite::calcOrbitVis()
{
	std::cout << "calcOrbitVis" << std::endl;
	std::vector<Vector> resVec;
	int runner = 0;
	bool calc = true;
	float maxAngle = 0.0f;
	float startAngle = this->ephemeris.trueAnomaly;
	this->ephemeris.trueAnomaly = 0.0f;

	float timeCorr = ((1.0f - this->ephemeris.eccentricity) * (this->ephemeris.semiMajorA*this->ephemeris.semiMajorA));
	timeCorr = fmaxf(timeCorr, 1.0f);
	float stepper = (1.0f/60.0f)*timeCorr;

	while (runner < 9000000 && calc) {
		if (maxAngle > (float)M_PI && this->ephemeris.trueAnomaly < 0.1f) {
			calc = false;
		}
		this->calcOrbitPos(stepper);
		if (this->ephemeris.trueAnomaly > maxAngle) {
			maxAngle = this->ephemeris.trueAnomaly;
		}
		if (r.lengthSquared() > 0.000001f) {
			resVec.push_back(r);
		}
		runner++;
	}
	cout << "Runner: " << runner << endl;
	cout << "Approximate time in minutes: " << (runner*stepper)/60.0f << endl;
	//cout << "Real ISS time in seconds: " << 92.9f * 60.0f << endl;
	cout << "-----------" << endl;
	//this->ephemeris.trueAnomaly = startAngle;
	return (resVec);
}
