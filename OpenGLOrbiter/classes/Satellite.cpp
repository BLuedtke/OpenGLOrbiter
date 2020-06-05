//Author: Bernhard Luedtke

#include "Satellite.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define DEG_TO_RAD(x) ((x)*0.0174532925f)
#define RAD_TO_DEG(x) ((x)*57.2957795f)

#define sizeFactor 1.0f/6378.0f
// This mu was determined by trial-and-error, measured by approximating the flight time of the ISS orbit
// (adjusted so that with speedUp 1.0f, ISS orbit flighttime is about the same as in real life (roughly).
// In reality, there are more factors influencing this, and it does not scale down the same as the size.
// If you want to change it, change it in the OrbitalEphemeris.cpp as well.
#define mu 0.0000015399

using std::cout;
using std::endl;


int factorial(int n)
{
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}


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
	;
}

/* From 'Fundamentals of Astrodynamics', R. Bate et al., pg. 193
With the Kepler time-of-flight equations you can easily solve for the time-of-flight, t - to' if you are given a, e, Vo and v.
The inverse problem of finding v when you are given a, e, Vo and t -to is not so simple, as we shall see.
Small4,  in An  Account of the  Astronomical Discoveries of Kepler, relates:
	"This problem has, ever since the time of Kepler, con­tinued  to exercise  the ingenuity of the ablest geometers;
	but no solu­tion of it which is rigorously  accurate has been obtained.
	Nor is there much reason to hope that the difficulty will ever be overcome ... "
This problem classically involves the solution of Kepler's Equation and is often referred to as Kepler's problem.
*/

/*
	The problem referenced above (Kepler's Problem) is (sadly) exactly the problem we need to try and solve here.

	We have, hopefully:
	r0	Vector r (position) at t0;
	v0	Vector v (speed) at t0;
	t0	Time at start, assumed to be 0

	We want to find:
	r	Position Vector
	v	Speed Vector
	... at given time t
*/
void Satellite::testKeplerProblem(float timePassed)
{
	try
	{
		//Now try to calculate first x guess.
		float xn = computeXfirstGuess(timePassed);
		//cout << "First guess for x = " << xn << endl;
		float zn = computeZ(xn);
		float tn = computeTnByXn(xn, zn);
		float x = xnNewtonIteration(xn, timePassed, tn, zn);
		//cout << "Determined x: " << x << endl;
		//Evaluate f and g from equations (4.4-31) and (4.4-34);then compute r and r.length from equation (4.4-18)
		float f = computeSmallF(x);
		float g = computeSmallG(x, timePassed);
		//cout << "f: " << f << "; g: " << g << endl;
		//Now compute r by 4.4-18 (r.length() = rLength)
		Vector rN = computeRVec(f, g);
		//Evaluate fD and gD from equations 4.4-35 and 4.4-36
		//float fD = computeSmallFDerivative(x, rN.length());
		//float gD = computeSmallGDerivative(x, rN.length());
		// check for accuracy of f, g, fD, gD
		//float test = f * gD - fD * g;
		//cout << "4.4-20 Check, should be near 1: " << test << endl;
		// Now compute v from 4.4-19
		//Vector vN = computeVVec(fD, gD);
		//cout << "Vector v:   ";
		//vN.print();
		r = rN;
		//v = vN;
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

//4.4-7
float Satellite::computeZ(float x)
{
	float a = this->ephemeris.semiMajorA;
	if (a != 0.0f) {
		return powf(x, 2.0f) / a;
	}
	else
	{
		cout << "ERROR: computeZ a was 0 -> Division by 0!" << endl;
	}
	return 0.0f;
}

//4.4-10
float Satellite::computeCseries(float z, unsigned int maxSteps)
{
	float cz = 0.0f;
	if (z > 0.0001f) {
		cz = (1.0f - cosf(std::sqrtf(z))) / z;
	}
	else if (z < -0.0001f) {
		cz = (1.0f - coshf(std::sqrtf(-z))) / z;
	}
	else {
		for (unsigned int k = 0; k < maxSteps; k++) {
			float res = (powf((-z), (float)k) / factorial(2 * k + 2));
			if (isnan(res)) {
				break;
			}
			cz = cz + res;
		}
	}
	return cz;
}

//4.4-11
float Satellite::computeSseries(float z, unsigned int maxSteps)
{
	float sz = 0.0f;
	if (z > 0.0001f) {
		float sqZ = std::sqrtf(z);
		sz = (sqZ - sinf(sqZ)) / std::sqrtf(powf(z, 3.0f));
	}
	else if (z < -0.0001f) {
		float sqZ = std::sqrtf(-z);
		sz = (sinhf(sqZ) - sqZ) / std::sqrtf(powf((-z), 3.0f));
	}
	else {
		for (unsigned int k = 0; k < maxSteps; k++) {
			float res = (powf((-z), (float)k) / factorial(2 * k + 3));
			if (isnan(res)) {
				break;
			}
			sz = sz + res;
		}
	}
	return sz;
}

//4.4-14
float Satellite::computeTnByXn(float xn, float zn)
{
	// To be used with a trial Value for x
	float bigC = computeCseries(zn, 100);
	float bigS = computeSseries(zn, 100);
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();

	float sMuTn = 0.0f;

	float term1 = (r0.dot(v0) / (float)std::sqrt(mu)) * powf(xn, 2.0f) * bigC;
	float term2 = (1.0f - (r0.length() / this->ephemeris.semiMajorA)) * powf(xn, 3.0f) * bigS;
	float term3 = r0.length() * xn;

	sMuTn = term1 + term2 + term3;
	float tn = (float)(sMuTn / std::sqrt(mu));
	//cout << "tn: " << tn << endl;
	return tn;
}

//4.4-15 Newton iteration
float Satellite::xnNewtonIteration(float xn, float t, float tn, float zn)
{
	//Testing. Will be a loop with a break criteria when finished.
	float xNext = xn;
	float timeN = tn;
	float zNext = zn;

	//We may also need to check if anything becomes NAN during the iteration
	for (unsigned int i = 0; i < 200; i++) {
		//cout << "xNext: " << xNext << endl;
		//cout << "t - timeN = " << (t - timeN) << endl;
		float dtdx = computeNewDtDx(xNext);
		xNext = xNext + ((t - timeN) / dtdx);
		zNext = computeZ(xNext);
		timeN = computeTnByXn(xNext, zNext);
		if ((t - timeN) < 0.0001f) {
			break;
		}
	}
	return xNext;
}

//4.4-17 with MU removed
float Satellite::computeNewDtDx(float xn)
{
	return (float)(computeNewDtDxWithMU(xn) / std::sqrt(mu));
}

//4.4-17 basically = rLength (later)
float Satellite::computeNewDtDxWithMU(float xn)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	float zn = computeZ(xn);
	float bigC = computeCseries(zn, 100);
	float bigS = computeSseries(zn, 100);

	float term1 = powf(xn, 2.0f) * bigC;
	float term2 = (r0.dot(v0) / (float)std::sqrt(mu)) * xn * (1.0f - zn * bigS);
	float term3 = r0.length() * (1.0f - zn * bigC);

	return term1 + term2 + term3;;
}

//4.4-18
Vector Satellite::computeRVec(float f, float g)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	Vector rN = (r0*f + v0 * g);
	//cout << "rN: ";
	//rN.print();
	//cout << "rN Length: " << rN.length() << endl;
	return rN;
}

//4.4-19
Vector Satellite::computeVVec(float fD, float gD)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	return r0 * fD + v0 * gD;
}

//4.4-31
float Satellite::computeSmallF(float x)
{
	float z = computeZ(x);
	float bigC = computeCseries(z, 100);
	float a = this->ephemeris.semiMajorA;
	Vector r0 = this->ephemeris.getR0();
	return 1.0f - (powf(x, 2.0f) / r0.length()) * bigC;
}

//4.4-34
float Satellite::computeSmallG(float x, float t)
{
	float z = computeZ(x);
	float bigS = computeSseries(z, 100);
	return t - (powf(x, 3.0f) / std::sqrt(mu)) * bigS;
}

//4.4-35
float Satellite::computeSmallGDerivative(float x, float rLength)
{
	float z = computeZ(x);
	float bigC = computeCseries(z, 100);
	return 1.0f - (powf(x, 2.0f) / rLength) * bigC;
}

//4.4-36
float Satellite::computeSmallFDerivative(float x, float rLength)
{
	float z = computeZ(x);
	float bigS = computeSseries(z, 100);
	float r0Length = this->ephemeris.getR0().length();
	return ((float)std::sqrt(mu) / (r0Length*rLength)) * x * (z * bigS - 1.0f);
}

//4.5-10
float Satellite::computeXfirstGuess(float t)
{
	return ((float)std::sqrt(mu) * t) / this->ephemeris.semiMajorA;
}


// Call this every frame to update the satellite's position in orbit.
void Satellite::update(float deltaT)
{
	try
	{
		totalTime += (double)deltaT;
		testKeplerProblem(totalTime);
		//calcOrbitPos(deltaT);
		Matrix f = Matrix();
		f.translation(r);
		uTransform = f;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

// Calculate the new position on orbit based on the current state + frameTime (deltaT).
// Give fixedStep = true and a value for 'step' if the frametime should be ignored. -> Usually only used for calculation of Orbit-Visualization Points.
void Satellite::calcOrbitPos(float deltaT, bool fixedStep) {
	
	double posAngle = 0.0;
	if (fixedStep == true && deltaT > 0.0f) {
		// fixedStep -> directly interpret deltaT as the progression of true anomaly.
		this->ephemeris.trueAnomaly = this->ephemeris.trueAnomaly + deltaT;
		posAngle = this->ephemeris.trueAnomaly;
	} else {
		//"Normal" calculation for the new angle.
		this->ephemeris.trueAnomaly = (float)calcAngleProgression(deltaT * speedUp);
		posAngle = this->ephemeris.trueAnomaly;
	}

	float semiMinorP = ephemeris.getOrCreateSemiMinorP();
	float rScal = semiMinorP / (1 + ephemeris.eccentricity * (float)cos(posAngle));

	Matrix pqw = this->ephemeris.getOrCreatePQW();
	Vector P = pqw.right();
	Vector Q = pqw.backward();
	r = P * rScal * (float)cos(posAngle) + Q* rScal * (float)sin(posAngle);
	Vector temp = P * -1.0f*(float)sin(posAngle)+ Q * (ephemeris.eccentricity+ (float)cos(posAngle));
	v = temp * (float)std::sqrt(mu / semiMinorP);
	//cout << v.length() << endl;

	// If you want to check: the length of h (r cross v) should always be the same for one satellite at any point in orbit
	// Might be influenced by floating point inaccuracies
	// Vector h is the specific angular momentum 
	//Vector h = r.cross(v);
	//cout << h.length() << endl;
}

// Calculate the new true anomaly (angle that describes where the satellite is along it's orbit) based on current state + frameTime (deltaT)
double Satellite::calcAngleProgression(float deltaT)
{
	double nextAnomaly = 0.0;
	if (r.length() <= 0.0001f || deltaT <= 0.001f) {
		//Prevents numerical instability with very very short frametimes or invalid positions (near the origin).
		nextAnomaly = 0.00001 + (double)this->ephemeris.trueAnomaly;
	}
	else {
		// rCandidate = Candidate for new position based on naive progression. 
		// Since the speed vector v can't "curve" with the orbit, this can't be used as a new position directly.
		// It can however be used to approximate the 'new' true Anomaly, which can then be used to calculate the new position more accurately.
		// This technique is very far from perfect, might be replaced.
		Vector rCandidate = r + v * (deltaT);
		// Get the angle between current position (r) and proposed new naive position (rCandidate).
		nextAnomaly = calcHeronKahanFormula(rCandidate, r) + (double) this->ephemeris.trueAnomaly;
		while (nextAnomaly > (M_PI*2.0f)) {
			nextAnomaly = nextAnomaly - (M_PI*2.0);
		}
	}
	return nextAnomaly;
}

//https://scicomp.stackexchange.com/questions/27689/numerically-stable-way-of-computing-angles-between-vectors
//Returns the angle between the two vectors, given that |k| >= |i| -> The length of the vectors is used for angle calculation. Needs to be used carefully.
double Satellite::calcHeronKahanFormula(Vector k, Vector i)
{
	double angle = 0.0;
	try
	{
		float a = k.length();
		float b = i.length();
		float c = (k - i).length();
		angle = calcHeronKahanFormula((double)a, (double)b, (double)c);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return angle;
}

//https://scicomp.stackexchange.com/questions/27689/numerically-stable-way-of-computing-angles-between-vectors
// Calculates the angle between two Vectors. I advise using the overloaded methods accepting 2 Vectors.
// Much much better then using 'standard' cos(angle) = (V1 dot V2) / (|V1| * |V2|)
double Satellite::calcHeronKahanFormula(double a, double b, double c)
{
	double tMu = 0.000001;
	if (a >= (b - 0.0001f)) {
		if (b >= c && c >= 0.0f) {
			tMu = c - (a - b);
		}
		else if (c > b && b >= 0) {
			tMu = b - (a - c);
		}
		double term1 = ((a - b) + c)*tMu;
		double term2 = (a + (b + c))*((a - c) + b);
		return 2.0*atan(std::sqrt((term1) / (term2)));;
	}
	else {
		std::cerr << "Invalid TriangleB. a: " << a << "; b: " << b << endl;
		return 0.00001;	// Emergency replacement, basically.
	}
}

//Method for going through the orbit and calculating a collection of points along the orbit
// This can be used to represent the trajectory with lines (used for OrbitLineModel).
std::vector<Vector> Satellite::calcOrbitVis()
{
	std::vector<Vector> resVec;
	
	double startAngle = this->ephemeris.trueAnomaly;
	this->ephemeris.trueAnomaly = 0.0f;

	int runner = 0;
	bool calc = true;
	float stepper = 0.005f;
	
	while (runner < 900000 && calc) {
		if (this->ephemeris.trueAnomaly > 2.0*M_PI+0.0001) {
			//Stop point generation after one orbit
			break;
		}
		this->calcOrbitPos(stepper,true);
		if (r.lengthSquared() > 0.000001f) {
			//Only add the point if it's not the origin
			resVec.push_back(r);
		}
		runner++;
	}
	this->ephemeris.trueAnomaly = startAngle;


	//Just for testing:
	//cout << "Orbital Period according to Semi-Major Axis formula: " << ephemeris.getCircularOrbitalPeriod() << endl;

	return (resVec);
}