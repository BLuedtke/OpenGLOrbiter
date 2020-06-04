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
#define mu 0.000001549

//For testing other µs
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
	;
}

// Call this every frame to update the satellite's position in orbit.
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

// Calculate the new position on orbit based on the current state + frameTime (deltaT).
void Satellite::calcOrbitPos(float deltaT, bool switchMU) {
	Matrix pqw = this->ephemeris.getOrCreatePQW();
	Vector P = pqw.right();
	Vector Q = pqw.backward();
	float semiMinorP = ephemeris.getOrCreateSemiMinorP();

	this->ephemeris.trueAnomaly = (float)calcAngleProgression(deltaT * speedUp);
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
		nextAnomaly = 0.00001;
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
		angle = calcHeronKahanFormula(a, b, c);
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
double Satellite::calcHeronKahanFormula(float a, float b, float c)
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
	int runner = 0;
	bool calc = true;
	float maxAngle = 0.0f;
	float startAngle = this->ephemeris.trueAnomaly;
	this->ephemeris.trueAnomaly = 0.0f;
	
	// timeCorr -> Speed up frametimes for calculating the lines/points, as otherwise a GPS Satellite orbit would have > 1 500 000 lines. That would be a bit excessive.
	float timeCorr = ((1.0f - this->ephemeris.eccentricity) * (this->ephemeris.semiMajorA*this->ephemeris.semiMajorA));

	//Prevent timeCorr from becoming 0 due to ephemeris.eccentricity being 0 (which occurs with a perfectly circular orbit).
	timeCorr = fmaxf(timeCorr, 1.0f);
	float stepper = (1.0f/60.0f)*timeCorr;
	cout << "Stepper " << stepper << endl;
	while (runner < 900000 && calc) {
		if (maxAngle >= (float)M_PI && this->ephemeris.trueAnomaly < 0.6f) {
			//Stop point generation after one orbit
			cout << "Stopping now " << endl;
			calc = false;
			
		}
		if (runner % 5000 == 0) {
			cout << "True Anomaly: " << this->ephemeris.trueAnomaly << endl;
			cout << "MaxAngle: " << maxAngle << endl;
		}
		this->calcOrbitPos(stepper);
		if (this->ephemeris.trueAnomaly > maxAngle) {
			maxAngle = this->ephemeris.trueAnomaly;
		}
		if (r.lengthSquared() > 0.000001f) {
			//Only add the point if it's not the origin (the first time this is called, r might be 0,0,0)
			resVec.push_back(r);
		}
		else {
			//cout << "CalcorbitVis r was indeed = origin." << endl;
		}
		runner++;
	}
	cout << "Runner: " << runner << endl;
	//cout << "Approximate time in minutes: " << (runner*stepper)/60.0f << endl;
	//cout << "Real ISS time in seconds: " << 92.9f * 60.0f << endl;
	//cout << "-----------" << endl;

	this->ephemeris.trueAnomaly = startAngle;
	return (resVec);
}
