//Author: Bernhard Luedtke

#include "Satellite.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>

#define DEG_TO_RAD(x) ((x)*0.0174532925f)
#define RAD_TO_DEG(x) ((x)*57.2957795f)

#define sizeFactor (1.0f/6378.0f)
// This mu was determined by trial-and-error, measured by approximating the flight time of the ISS orbit
// (adjusted so that with speedUp 1.0f, ISS orbit flighttime is about the same as in real life (roughly).
// In reality, there are more factors influencing this, and it does not scale down the same as the size.
// If you want to change it, change it in the OrbitalEphemeris.cpp as well.
#ifndef muS
#define muS 0.0000015399
#endif // !muS
#ifndef mu
// This uses the unit km�/s�, NOT m�/s�!!
#define mu (double)398600
#endif // !mu

#ifndef sqMU
#define sqMU std::sqrt(mu)
#endif // !sqMU


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
	"This problem has, ever since the time of Kepler, con�tinued  to exercise  the ingenuity of the ablest geometers;
	but no solu�tion of it which is rigorously  accurate has been obtained.
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
void Satellite::calcKeplerProblem(double timePassed, double t0)
{
	try
	{
		//Compute first guesses and then use Newton Iteration to find x.
		double diffTime = timePassed - t0;
		double xn = computeXfirstGuess(diffTime);
		
		double zn = computeZ(xn);
		double tn = computeTnByXn(xn, zn); //NAN if eccentricity = 1
		double x = xnNewtonIteration(xn, diffTime, tn, zn);
		//cout << "x: " << x << endl;
		//Evaluate f and g from equations (4.4-31) and (4.4-32);then compute r and r.length from equation (4.4-18)
		double f = computeSmallF(x);
		double g = computeSmallG(x, diffTime);
		//Now compute r by 4.4-18
		Vector rN = computeRVec(f, g);
		
		//Evaluate fD and gD from equations 4.4-35 and 4.4-36
		double fD = computeSmallFDerivative(x, rN.length());
		double gD = computeSmallGDerivative(x, rN.length());
		
		//check for accuracy of f, g, fD, gD
		double test = f * gD - fD * g;
		if (abs(test) > 1.0000000001) {
			cout << "should be near 1: " << abs(test) << endl;
		}
		
		// Now compute v from 4.4-19
		Vector vN = computeVVec(fD, gD);
		if (t0 > 0.0) {
			this->ephemeris.updateR0V0(rN, vN);
		}
		r = rN * (sizeFactor);
		v = vN * (sizeFactor);
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

//4.4-7
double Satellite::computeZ(double x)
{
	double a = this->ephemeris.semiMajorA;
	if (a != 0.0) {
		return pow(x, 2.0) / a;
	}
	else
	{
		cout << "ERROR: computeZ a was 0 -> Division by 0!" << endl;
	}
	return 0.0;
}

//4.4-10
double Satellite::computeCseries(double z)
{
	double cz = 0.0;
	if (z > 0.0000000000001) {
		cz = (1.0 - cos(std::sqrt(z))) / z;
	}
	else if (z < -0.000000000001) {
		cz = (1.0 - cosh(std::sqrt(-z))) / z;
	}
	else {
		// This is a form of stumpff's formulas. Convergence is usually very quick.
		for (unsigned int k = 0; k < 20; k++) {
			double res = (pow((-z), (double)k) / factorial(2 * k + 2));
			double save = cz;
			cz = cz + res;
			if (isnan(cz)) {
				cz = save;
				break;
			}
		}
	}
	return cz;
}

//4.4-11
double Satellite::computeSseries(double z)
{
	double sz = 0.0;
	if (z > 0.000000000001) {
		double sqZ = std::sqrt(z);
		sz = (sqZ - sin(sqZ)) / std::sqrt(pow(z, 3.0));
	}
	else if (z < -0.0000000000001) {
		double sqZ = std::sqrt(-z);
		sz = (sinh(sqZ) - sqZ) / std::sqrt(pow((-z), 3.0));
	}
	else {
		// This is a form of stumpff's formulas. Convergence is usually very quick.
		for (unsigned int k = 0; k < 20; k++) {
			double res = (pow((-z), (double)k) / factorial(2 * k + 3));
			double save = sz;
			sz = sz + res;
			if (isnan(sz)) {
				sz = save;
				break;
			}
		}
	}
	return sz;
}

//4.4-14
double Satellite::computeTnByXn(double xn, double zn)
{
	// To be used with a trial Value for x
	double bigC = computeCseries(zn);
	double bigS = computeSseries(zn);
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();

	double term1 = (r0.dot(v0) / sqMU) * pow(xn, 2.0) * bigC;
	double term2 = (1.0 - (r0.length() / this->ephemeris.semiMajorA)) * pow(xn, 3.0) * bigS;
	double term3 = r0.length() * xn;

	double sMuTn = term1 + term2 + term3;
	return (sMuTn / sqMU);
}

//4.4-15 Newton iteration
double Satellite::xnNewtonIteration(double xn, double t, double tn, double zn)
{
	double xNext = xn;
	double timeN = tn;
	double zNext = zn;
	//We may also need to check if anything becomes NAN during the iteration
	for (unsigned int i = 0; i < 200; i++) {
		double dtdx = computeNewDtDx(xNext);
		xNext = xNext + ((t - timeN) / dtdx);
		zNext = computeZ(xNext);
		timeN = computeTnByXn(xNext, zNext);
		if ((t - timeN) < 0.000001) {
			//cout << "Approximated time: " << timeN << endl;
			break;
		}
	}
	return xNext;
}

//4.4-17 with MU removed
double Satellite::computeNewDtDx(double xn)
{
	return (computeNewDtDxWithMU(xn) / sqMU);
}

//4.4-17 basically = rLength
double Satellite::computeNewDtDxWithMU(double xn)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	double zn = computeZ(xn);
	double bigC = computeCseries(zn);
	double bigS = computeSseries(zn);

	double term1 = pow(xn, 2.0) * bigC;
	double term2 = (r0.dot(v0) / sqMU) * xn * (1.0 - zn * bigS);
	double term3 = r0.length() * (1.0 - zn * bigC);

	return (term1 + term2 + term3);
}

//4.4-18
Vector Satellite::computeRVec(double f, double g)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	Vector rN = (r0 * (float)f + v0 * (float)g); // The Vector class doesnt use doubles (yet)
	return rN;
}

//4.4-19
Vector Satellite::computeVVec(double fD, double gD)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	return r0 * (float)fD + v0 * (float)gD;
}

//4.4-31
double Satellite::computeSmallF(double x)
{
	double z = computeZ(x);
	double bigC = computeCseries(z);
	double r0Length = this->ephemeris.getR0().length();
	return (1.0 - (pow(x, 2.0) / r0Length) * bigC);
}

//4.4-34 / (4.4-32)
//Testing showed that using equation 4.4-34 produced high error for trueAnomaly < Pi (so the first half of the orbit), if t0 stays 0 and is not updated.
// 4.4-32 (which is being used here) does produce correct results even if t0 (and r0, v0) are not updated. 
// However, updating t0, r0 and v0 is absolutely trivial, and allows usage of 4.4-34, which should be less expensive in terms of operations needed.
double Satellite::computeSmallG(double x, double t)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	double z = computeZ(x);
	double bigS = computeSseries(z);
	//double bigC = computeCseries(z);
	double gSmall = t - pow(x, 3.0) / sqMU * bigS;
	//double proper = (pow(x, 2.0) * (r0.dot(v0) / sqMU) * bigC + r0.length()*x*(1.0 - z * bigS)) / sqMU;
	//cout << "4.4-32: " << proper << "; 4.4-34: " << gSmall << endl;
	return gSmall;
	//return proper;
}

//4.4-35
double Satellite::computeSmallGDerivative(double x, double rLength)
{
	double z = computeZ(x);
	double bigC = computeCseries(z);
	return 1.0f - (pow(x, 2.0) / rLength) * bigC;
}

//4.4-36
double Satellite::computeSmallFDerivative(double x, double rLength)
{
	double z = computeZ(x);
	double bigS = computeSseries(z);
	double r0Length = this->ephemeris.getR0().length();
	return (sqMU / (r0Length*rLength)) * x * (z * bigS - 1.0);
}

//4.5-10
double Satellite::computeXfirstGuess(double t)
{
	return (sqMU * t) / this->ephemeris.semiMajorA;
}


// Call this every frame to update the satellite's position in orbit.
void Satellite::update(double deltaT)
{
	try
	{
		if (totalTime > ephemeris.getEllipseOrbitalPeriod()) {
			//This is done first to keep the difference between new t and t0 stable
			totalTime = totalTime - ephemeris.getEllipseOrbitalPeriod();
		}
		double t0 = totalTime;
		totalTime += (deltaT*(double)speedUp);
		calcKeplerProblem(totalTime, t0);
		Matrix f = Matrix();
		f.translation(r);
		uTransform = f;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}


using time_point = std::chrono::time_point<std::chrono::steady_clock>;
double timeInMilliSeconds(time_point start, time_point end) {
	using std::chrono::duration_cast;
	auto nano = duration_cast<std::chrono::nanoseconds>(end - start).count();
	auto mikro = duration_cast<std::chrono::microseconds>(end - start).count();
	auto milli = duration_cast<std::chrono::milliseconds>(end - start).count();
	double t = (double)milli + ((double)mikro / 1000.0) + ((double)nano / 1e+6);
	return t;
}
//Method for going through the orbit and calculating a collection of points along the orbit
// This can be used to represent the trajectory with lines (used for OrbitLineModel).
std::vector<Vector> Satellite::calcOrbitVis()
{
	std::vector<Vector> resVec;
	double startAngle = this->ephemeris.trueAnomaly;
	this->ephemeris.trueAnomaly = 0.0f;

	//This is only valid for an ellipse -> Everything probably needs to be slightly rewritten for parabolas.
	double orbPeriod = this->ephemeris.getEllipseOrbitalPeriod();

	int runner = 0;
	double stepper = 120;
	time_point t1;
	time_point t2;
	double totalTimeMilli = 0.0;
	while (runner < 10000) {
		this->totalTime += stepper;
		//t1 = std::chrono::high_resolution_clock::now();
		this->calcKeplerProblem(this->totalTime, this->totalTime-stepper);
		//t2 = std::chrono::high_resolution_clock::now();
		//totalTimeMilli += timeInMilliSeconds(t1, t2);
		if (r.lengthSquared() > 0.000001f) {
			resVec.push_back(r);
		}
		runner++;
		if (this->totalTime-stepper > orbPeriod) {
			//Stop point generation after one orbit
			break;
		}
	}
	this->ephemeris.trueAnomaly = startAngle;
	this->totalTime = 0.0;
	//cout << "Total time taken for all calcKepler: " << totalTimeMilli << endl;
	//cout << "Avg: " << (totalTimeMilli / (double)runner) << endl;
	//Just for testing:
	//cout << "Orbital Period according to Semi-Major Axis formula: " << ephemeris.getEllipseOrbitalPeriod() << endl;
	//cout << "Points for vis: " << resVec.size() << endl;
	return (resVec);
}

//Stumpff function for testing purposes mostly
/*
double stumpff(double x, unsigned int order) {
	double total = 0.0;
	for (unsigned int i = 0; i < 5; i++) {
		double upper = pow(-1.0, i) * pow(x, i);
		double lower = factorial(order + 2 * i);
		double res = upper / lower;
		if (isnan(res)) {
			break;
		}
		double s = total;
		total += res;
		if (isnan(total)) {
			total = s;
			break;
		}
	}
	return total;
}*/
