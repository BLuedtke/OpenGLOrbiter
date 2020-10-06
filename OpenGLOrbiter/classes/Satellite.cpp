//Author: Bernhard Luedtke

#include "Satellite.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>

#define DEG_TO_RAD(x) ((x)*0.0174532925)
#define RAD_TO_DEG(x) ((x)*57.2957795)

#define sizeFactor (1.0f/6378.0f)

// If you want to change it, change it in the OrbitalEphemeris.cpp as well.
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
	t0	Time of last determination of r0 and v0

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
		
		//Evaluate f and g from equations (4.4-31) and (4.4-32)
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
			//The larger the difference to 1, the higher the error of the determined position
			cout << "should be near 1: " << abs(test) << endl;
		}
		
		// Now compute v from 4.4-19
		Vector vN = computeVVec(fD, gD);
		if (t0 > 0.0) {
			this->ephemeris.updateR0V0(rN, vN);
		}
		//All calculations are performed in "real" scale (1 unit = 1km), but the coordinate system is not to scale (1 unit = 6378.0km), so scale the vectors down
		r = rN * (sizeFactor);
		v = vN * (sizeFactor);
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

//For Debugging
void Satellite::saveCurrentR0Length()
{
	this->r0LengthSaved = this->ephemeris.getR0().length();
}

//4.4-7
//The Semi Major Axis in the OrbitalEphemeris object has to be != 0 for this to work
double Satellite::computeZ(double x)
{
	if (this->ephemeris.semiMajorA != 0.0) {
		return pow(x, 2.0) / this->ephemeris.semiMajorA;
	}
	else
	{
		cout << "ERROR in computeZ (Satellite CPP): Semi Major Axis was 0 -> Division by 0." << endl;
	}
	return 0.0;
}

//4.4-10
double Satellite::computeCseries(double z)
{
	double cz = 0.0;
	//Easy Calculation if z != 0 (albeit different depending on sign)
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
	//Easy Calculation if z != 0 (albeit different depending on sign)
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
// To be used with a trial value for x during the newton iteration
double Satellite::computeTnByXn(double xn, double zn)
{
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
	//TODO: Implement NaN checks for calculated values during iteration
	for (unsigned int i = 0; i < 200; i++) {
		double dtdx = computeNewDtDx(xNext);
		xNext = xNext + ((t - timeN) / dtdx);
		zNext = computeZ(xNext);
		timeN = computeTnByXn(xNext, zNext);
		if ((t - timeN) < 0.0000001) {
			//cout << "Approximated time: " << timeN << endl;
			break;
		}
	}
	return xNext;
}

//4.4-17 with additional division by sqMU -> Required for Newton iteration process only
double Satellite::computeNewDtDx(double xn)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	double zn = computeZ(xn);
	double bigC = computeCseries(zn);
	double bigS = computeSseries(zn);

	double term1 = pow(xn, 2.0) * bigC;
	double term2 = (r0.dot(v0) / sqMU) * xn * (1.0 - zn * bigS);
	double term3 = r0.length() * (1.0 - zn * bigC);

	return (term1 + term2 + term3)/sqMU;
}

//4.4-18
Vector Satellite::computeRVec(double f, double g)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	//TODO Remove the (float) once switched to vectors that use doubles
	Vector rN = (r0 * (float)f + v0 * (float)g);
	return rN;
}

//4.4-19
Vector Satellite::computeVVec(double fD, double gD)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	//TODO Remove the (float) once switched to vectors that use doubles
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

//4.4-32
//Testing showed that using equation 4.4-34 produced high error when the time delta t since the last calculation is larger than a few seconds
// 4.4-32 (which is being used here) does produce correct results even if t is larger than a few seconds
double Satellite::computeSmallG(double x, double t)
{
	Vector r0 = this->ephemeris.getR0();
	Vector v0 = this->ephemeris.getV0();
	double z = computeZ(x);
	double bigS = computeSseries(z);
	double bigC = computeCseries(z);
	double gSmall = (pow(x, 2.0) * (r0.dot(v0) / sqMU) * bigC + r0.length()*x*(1.0 - z * bigS)) / sqMU;
	return gSmall;
}

//4.4-35
double Satellite::computeSmallGDerivative(double x, double rLength)
{
	double z = computeZ(x);
	double bigC = computeCseries(z);
	return 1.0 - (pow(x, 2.0) / rLength) * bigC;
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
//First guess to start the newton iteration process with, recommended by the book.
double Satellite::computeXfirstGuess(double t)
{
	return (sqMU * t) / this->ephemeris.semiMajorA;
}


// Call this every frame to update the satellite's position in orbit.
void Satellite::update(double deltaT)
{
	try
	{
		//TODO Rewrite Update Process to switch to consistent physic timesteps + interpolation instead of calculating at every frame
		if (totalTime > ephemeris.getEllipseOrbitalPeriod()) {
			orbits++;
			float currentR0Length = this->ephemeris.getR0().length();
			cout << orbits << " Orbits completed. R Length: " << currentR0Length << "; R-L last orbit: " << lastOrbitR0Length << "; Diff: " << lastOrbitR0Length - currentR0Length << endl;
			this->lastOrbitR0Length = currentR0Length;
			//Debugging Rounding Errors stuff with a satellite with eccentricity 0 -> Will absolutely fail with other sats
			this->ephemeris.semiMajorA = currentR0Length;
			//Still debugging -> Force recalculation of expected orbital period with adjusted SemiMajorAxis, to collect data on the rounding error per orbit
			double newPeriod = ephemeris.getEllipseOrbitalPeriod(true);
			cout << "next expected orbital period: " << newPeriod << endl;
			//This is done to keep the value for totalTime in a reasonable range. Should replace this with a simpler/more intuitive overflow check
			totalTime = totalTime - ephemeris.getEllipseOrbitalPeriod();
		}
		double t0 = totalTime;
		totalTime += deltaT;
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
// This can be used to represent the trajectory with lines (i.e. used for OrbitLineModel).
std::vector<Vector> Satellite::calcOrbitVis()
{
	std::vector<Vector> resVec;
	double startAngle = this->ephemeris.trueAnomaly;
	this->ephemeris.trueAnomaly = 0.0f;

	//Debugging/Time Measurement
	time_point t1, t2;
	double totalTimeMilli = 0.0;
	
	int runner = 0;
	double stepper = 120;
	int maxSteps = 10000;
	//Required for orbits with periods >~333 hours
	if (stepper * 10000 < this->ephemeris.getEllipseOrbitalPeriod()) {
		maxSteps = (int)(this->ephemeris.getEllipseOrbitalPeriod() / 120)+1;
		cout << "calcOrbitVis Changing max Steps to " << maxSteps << endl;
	}
	
	while (runner < maxSteps) {
		this->totalTime += stepper;
		//t1 = std::chrono::high_resolution_clock::now();
		this->calcKeplerProblem(this->totalTime, this->totalTime-stepper);
		//t2 = std::chrono::high_resolution_clock::now();
		//totalTimeMilli += timeInMilliSeconds(t1, t2);
		if (r.lengthSquared() > 0.000001f) {
			resVec.push_back(r);
		}
		runner++;
		if (this->totalTime-stepper > this->ephemeris.getEllipseOrbitalPeriod()) {
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
