//Author: Bernhard Luedtke

#include "Satellite.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#define DEG_TO_RAD(x) ((x)*0.0174532925)
#define RAD_TO_DEG(x) ((x)*57.2957795)

// If you want to change it, change it in the OrbitalEphemeris.cpp as well. ///TODO: move to constants
// This uses the unit km³/s², NOT m³/s²!!
constexpr double mu = 398600.0;
constexpr double sqMU = 631.34776470658387846982160428348675796819064249894744823745763911;
constexpr double sizeFactor = 1.0 / 6378.0;

using std::cout;
using std::endl;

using time_point = std::chrono::time_point<std::chrono::steady_clock>;
double timeInMilliSeconds(time_point start, time_point end) {
	using std::chrono::duration_cast;
	auto nano = duration_cast<std::chrono::nanoseconds>(end - start).count();
	auto mikro = duration_cast<std::chrono::microseconds>(end - start).count();
	auto milli = duration_cast<std::chrono::milliseconds>(end - start).count();
	double t = (double)milli + ((double)mikro / 1000.0) + ((double)nano / 1e+6);
	return t;
}


constexpr long long factorial(int n)
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
		const auto diffTime = timePassed - t0;
		const auto xn = computeXfirstGuess(diffTime);

		const auto zn = computeZ(xn);
		const auto tn = computeTnByXn(xn, zn); //NAN if eccentricity = 1
		//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		const auto x = xnNewtonIteration(xn, diffTime, tn, zn);
		//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		//std::cout << "Time difference = " << timeInMilliSeconds(begin, end) << std::endl;
		
		//Evaluate f and g from equations (4.4-31) and (4.4-32)
		
		const auto f = computeSmallF(x);
		const auto g = computeSmallG(x, diffTime);
		
		//Now compute r by 4.4-18
		const Vector rN = computeRVec(f, g);
		
		//TESTING alternate 4.4-13
		//const auto z = computeZ(x);
		//Vector r0 = this->ephemeris.getR0();
		//Vector v0 = this->ephemeris.getV0();
		//double rTestLength = pow(x, 2.0) * computeCseries(z) + ((r0.dot(v0)) / (sqMU)) * x * (1.0 - z * computeSseries(z)) + r0.length() * (1.0 - z * computeCseries(z));
		//cout << "; xn: " << xn  << "; x: " << x << "; z: " << z << "; rN Length: " << rN.length() << "; rTest Length: " << rTestLength << endl;
		//cout << "rN Length: " << rN.length() << "; rTest Length: " << rTestLength << endl;
		//cout << "rN Length: " << std::setprecision(10) << rN.length() << "; tD " << std::setprecision(6) << diffTime << endl;
		//Evaluate fD and gD from equations 4.4-35 and 4.4-36
		const auto fD = computeSmallFDerivative(x, rN.length());
		const auto gD = computeSmallGDerivative(x, rN.length());
		
		//check for accuracy of f, g, fD, gD
		const auto test = f * gD - fD * g;
		if (abs(test) > 1.0 + 1e-10) {
			//The larger the difference to 1, the higher the error of the determined position
			cout << "should be near 1: " << abs(test) << endl;
			cout << "tDelta: " << diffTime << "; xn: " << xn << "; tn: " << tn << "; x: " << x << "; " << endl;
			//-> TODO handling
		}
		
		// Now compute v from 4.4-19
		const Vector vN = computeVVec(fD, gD);
		if (t0 > 0.0) {
			this->ephemeris.updateR0V0(rN, vN);
		}
		//All calculations are performed in "real" scale (1 unit = 1km), but the coordinate system is not to scale (1 unit = 6378.0km), so scale the vectors down
		r = rN * sizeFactor;
		v = vN * sizeFactor;
		//std::cout << "Position: " << r.toString() << std::endl;
		//std::cout << "z: " << z << "; r: " << r.toString() << "; v: " << v.toString() << std::endl;
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

void Satellite::calcKeplerProblem_experimental(double timePassed, double t0)
{
	if (this->ephemeris.semiMajorA == 0.0) {
		std::cerr << "Semi Major Axis is 0, abort kepler calc" << std::endl;
		return;
	}
	const auto r0Length = ephemeris.getR0().length();
	//Can now use raw r0 v0 from here on out
	//Compute first guesses and then use Newton Iteration to find x.
	const auto diffTime = timePassed - t0;
	const auto xn = (sqMU * diffTime) / ephemeris.semiMajorA;
	
	const auto zn = pow(xn, 2.0) / ephemeris.semiMajorA; //4.4-7
	const auto tn = ((ephemeris._getR0_raw().dot(ephemeris._getV0_raw()) / sqMU) * pow(xn, 2.0) * computeCseries(zn) + (1.0 - (r0Length / ephemeris.semiMajorA)) * pow(xn, 3.0) * computeSseries(zn) + r0Length * xn) / sqMU;
	const auto x = xnNewtonIteration_DEV(xn, diffTime, tn, r0Length);

	const auto z = pow(x, 2.0) / ephemeris.semiMajorA;		//4.4-7
	const auto bigC = computeCseries(z);

	//Evaluate f and g from equations (4.4-31) and (4.4-32)
	const auto f = (1.0 - (pow(x, 2.0) / r0Length) * bigC);
	const auto g = (pow(x, 2.0) * (ephemeris._getR0_raw().dot(ephemeris._getV0_raw()) / sqMU) * bigC + r0Length * x * (1.0 - z * bigC)) / sqMU;

	//Now compute r by 4.4-18
	const auto rN = (ephemeris._getR0_raw() * static_cast<float>(f) + ephemeris._getV0_raw() * static_cast<float>(g));
	const auto rNLength = rN.length();

	//Evaluate fD and gD from equations 4.4-35 and 4.4-36
	const auto fD = (sqMU / (static_cast<double>(r0Length) * rNLength)) * x * (z * computeSseries(z) - 1.0);
	const auto gD = 1.0 - (pow(x, 2.0) / rNLength) * bigC;

	//check for accuracy of f, g, fD, gD
	const auto test = f * gD - fD * g;
	if (abs(test) > 1.0 + 1e-10) {
		//The larger the difference to 1, the higher the error of the determined position
		cout << "should be near 1: " << abs(test) << endl;
		cout << "tDelta: " << diffTime << "; xn: " << xn << "; tn: " << tn << "; x: " << x << "; " << endl;
		//-> TODO handling
	}

	// Now compute v from 4.4-19
	const auto vN = (ephemeris._getR0_raw() * static_cast<float>(fD) + ephemeris._getV0_raw() * static_cast<float>(gD));

	if (t0 > 0.0) {
		this->ephemeris.updateR0V0(rN, vN);
	}
	//All calculations are performed in "real" scale (1 unit = 1km), but the coordinate system is not to scale (1 unit = 6378.0km), so scale the vectors down
	r = rN * static_cast<float>(sizeFactor);
	v = vN * static_cast<float>(sizeFactor);
	//std::cout << "Position: " << r.toString() << std::endl;
	//std::cout << "z: " << z << "; r: " << r.toString() << "; v: " << v.toString() << std::endl;
}

Vector Satellite::getV() const
{
	return this->v;
}

Vector Satellite::getR() const
{
	return this->r;
}

OrbitEphemeris Satellite::getEphemeris() const
{
	return this->ephemeris;
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
	//dangerous test
	
	//cout << z << "\n";
	if (std::abs(z) > 0.0) {
		//Easy Calculation if z != 0 (albeit different depending on sign)
		if (z > 0.0) {
			cz = (1.0 - cos(std::sqrt(z))) / z;
		} else if (z < 0.0) {
			cz = (1.0 - cosh(std::sqrt(-z))) / z;
		} else {
			std::cout << "computeCseries should NOT land here!" << std::endl;
		}
	}
	else {
		// This is a form of stumpff's formulas which can be used if z is exactly zero to avoid div by zero.
		//std::cout << "bigC Alternative Calc" << std::endl;
		// Limited to k 0 - 9, because with k=10 we'd have an overflow in the factorial function
		for (unsigned int k = 0; k < 10; k++) {
			double res = (pow((-z), (double)k) / factorial(2 * k + 2));
			
			if (std::abs(res) != 0.0 && !isnan(cz + res)) {
				cz += res;
			} else {
				break;
			}
		}
	}
	return cz;
}

//4.4-11
double Satellite::computeSseries(double z)
{
	using std::sqrt; using std::abs;
	//Easy Calculation if z != 0 (albeit different depending on sign)
	if (abs(z) > 0.0) {
		double sqZ = sqrt(abs(z));
		if (z > 0.0) {
			return (sqZ - sin(sqZ)) / sqrt(pow(z, 3.0));
		} else if (z < 0.0) {
			return (sinh(sqZ) - sqZ) / sqrt(pow((-z), 3.0));
		} else {
			std::cout << "computeSseries should NOT land here!" << std::endl;
		}
	}
	else {
		// This is a form of stumpff's formulas.
		// Limited to k 0 - 8, because with k=9 we'd have an overflow in the factorial function
		double sz = 0.0;
		for (unsigned int k = 0; k < 9; ++k) {
			double res = (pow((-z), static_cast<double>(k)) / factorial(2 * k + 3));
			if (abs(res) != 0.0 && !isnan(sz+res)) {
				sz += res;
			} else {
				break;
			}
		}
		return sz;
	}
}

//4.4-14
// To be used with a trial value for x during the newton iteration
double Satellite::computeTnByXn(double xn, double zn)
{
	//const double bigC = computeCseries(zn); //V2
	//const double bigS = computeSseries(zn); //V2
	//const Vector r0 = this->ephemeris.getR0(); //V3
	//const Vector v0 = this->ephemeris.getV0(); //V3

	const double term1 = (ephemeris.getR0().dot(ephemeris.getV0()) / sqMU) * pow(xn, 2.0) * computeCseries(zn);
	const double term2 = (1.0 - (ephemeris.getR0().length() / this->ephemeris.semiMajorA)) * pow(xn, 3.0) * computeSseries(zn);
	const double term3 = ephemeris.getR0().length() * xn;

	//const double sMuTn = (term1 + term2 + term3) / sqMU;//V2
	//std::cout << "tnComp. zN: " << zn << " bigC: " << bigC << "; bigS: " << bigS << "; tN: " << ret << std::endl;
	return (term1 + term2 + term3) / sqMU;
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
		//Yes, we need to be that precise - else we get a pretty substantial error-build-up.
		if ((t - timeN) < 0.0000000000001) {
			//cout << "Approximated time: " << timeN << endl;
			//std::cout << "Break at " << i << std::endl;
			break;
		}
	}
	return xNext;
}

double Satellite::xnNewtonIteration_DEV(double xn, double t, double tn, double r0Length)
{
	double xNext = xn;
	double timeN = tn;
	//double zNext = 0.0;
	//TODO: Implement NaN checks for calculated values during iteration
	for (unsigned int i = 0; i < 200; i++) {
		xNext = xNext + ((t - timeN) / computeNewDtDx_DEV(xNext, r0Length));
		//const auto zNext = pow(xNext, 2.0) / this->ephemeris.semiMajorA;
		const auto xNextPow2 = pow(xNext, 2.0);
		const double term1 = (ephemeris.getR0().dot(ephemeris.getV0()) / sqMU) * xNextPow2 * computeCseries(xNextPow2 / this->ephemeris.semiMajorA);
		const double term2 = (1.0 - (r0Length / ephemeris.semiMajorA)) * pow(xNext, 3.0) * computeSseries(xNextPow2 / this->ephemeris.semiMajorA);
		timeN = (term1 + term2 + r0Length * xNext) / sqMU;
		
		//Yes, we need to be that precise
		if ((t - timeN) < 0.0000000000001) {
			break;
		}
	}
	return xNext;
}

//4.4-17 with additional division by sqMU -> Required for Newton iteration process only
double Satellite::computeNewDtDx(double xn)
{
	//const Vector r0 = this->ephemeris.getR0();//V3
	//const Vector v0 = this->ephemeris.getV0();//V3
	const auto xnP2 = pow(xn, 2.0);
	const double zn = xnP2 / this->ephemeris.semiMajorA;//V5,V6,V11
	const double bigC = computeCseries(zn); //V2,V6
	//const double bigS = computeSseries(zn); //V2

	const double term1 = xnP2 * bigC;
	const double term2 = (ephemeris.getR0().dot(ephemeris.getV0()) / sqMU) * xn * (1.0 - zn * computeSseries(zn));
	const double term3 = ephemeris.getR0().length() * (1.0 - zn * bigC);

	return (term1 + term2 + term3) / sqMU;
}

double Satellite::computeNewDtDx_DEV(double xn, double r0Length)
{
	const auto xnP2 = pow(xn, 2.0);
	const auto zn = xnP2 / this->ephemeris.semiMajorA;
	const auto bigC = computeCseries(zn);
	//const double term1 = xnP2 * bigC;
	const double term2 = (ephemeris.getR0().dot(ephemeris.getV0()) / sqMU) * xn * (1.0 - zn * computeSseries(zn));
	//const double term3 = r0Length * (1.0 - zn * bigC);
	return (xnP2 * bigC + term2 + r0Length * (1.0 - zn * bigC)) / sqMU;
}

//4.4-18
Vector Satellite::computeRVec(double f, double g)
{
	//const Vector r0 = this->ephemeris.getR0();//V3
	//const Vector v0 = this->ephemeris.getV0();//V3
	
	//const Vector rN = (r0 * static_cast<float>(f) + v0 * static_cast<float>(g));//V2
	return (ephemeris.getR0() * static_cast<float>(f) + ephemeris.getV0() * static_cast<float>(g));
}

//4.4-19
Vector Satellite::computeVVec(double fD, double gD)
{
	//const Vector r0 = this->ephemeris.getR0();//V3
	//const Vector v0 = this->ephemeris.getV0();//V3
	
	return (ephemeris.getR0() * static_cast<float>(fD) + ephemeris.getV0() * static_cast<float>(gD));
}

//4.4-31
double Satellite::computeSmallF(double x)
{
	//const double bigC = computeCseries(computeZ(x));//V2
	return (1.0 - (pow(x, 2.0) / this->ephemeris.getR0().length()) * computeCseries(computeZ(x)));
}

//4.4-32
//Testing showed that using equation 4.4-34 produced high error when the time delta t since the last calculation is larger than a few seconds
// 4.4-32 (which is being used here) does produce correct results even if t is larger than a few seconds
double Satellite::computeSmallG(double x, double t)
{
	//const Vector r0 = this->ephemeris.getR0();//V3
	//const Vector v0 = this->ephemeris.getV0();//V3
	const double z = computeZ(x);//V5,V6
	//const double bigS = computeSseries(z); //V2
	//const double bigC = computeCseries(z); //V2
	//const double gSmall = (pow(x, 2.0) * (r0.dot(v0) / sqMU) * computeCseries(z) + r0.length()*x*(1.0 - z * computeSseries(z))) / sqMU; //V2
	return (pow(x, 2.0) * (ephemeris.getR0().dot(ephemeris.getV0()) / sqMU) * computeCseries(z) + ephemeris.getR0().length() * x * (1.0 - z * computeSseries(z))) / sqMU;
}

//4.4-35
double Satellite::computeSmallGDerivative(double x, double rLength)
{
	//const auto bigC = computeCseries(computeZ(x));//V2
	return 1.0 - (pow(x, 2.0) / rLength) * computeCseries(computeZ(x));
}

//4.4-36
double Satellite::computeSmallFDerivative(double x, double rLength)
{
	const auto z = computeZ(x);//V5,V6
	//const auto bigS = computeSseries(z);//V2
	//const auto r0Length = this->ephemeris.getR0().length();//V4
	return (sqMU / (ephemeris.getR0().length() * rLength)) * x * (z * computeSseries(z) - 1.0);
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
	//TODO Rewrite Update Process to switch to consistent physic timesteps + interpolation instead of calculating at every frame
	if (totalTime > ephemeris.getEllipseOrbitalPeriod()) {
		orbits++;
		float currentR0Length = this->ephemeris.getR0().length();
		//cout << orbits << " Orbits. RLength: " << currentR0Length << "; R-Llastorbit: " << lastOrbitR0Length << "; Diff: " << currentR0Length - lastOrbitR0Length << endl;
		//cout << "V0 Length: " << this->ephemeris.getV0().length() << endl;
		this->lastOrbitR0Length = currentR0Length;
		//Debugging Rounding Errors stuff with a satellite with eccentricity 0 -> Will absolutely fail with other sats
		this->ephemeris.semiMajorA = currentR0Length;
		//Still debugging -> Force recalculation of expected orbital period with adjusted SemiMajorAxis, to collect data on the rounding error per orbit
		//double newPeriod = ephemeris.getEllipseOrbitalPeriod(true);
		//cout << "next expected orbital period: " << newPeriod << endl;
		//This is done to keep the value for totalTime in a reasonable range. Should replace this with a simpler/more intuitive overflow check
		totalTime = totalTime - ephemeris.getEllipseOrbitalPeriod();
	}
	//cout << deltaT << endl;
		
	double t0 = totalTime;
	totalTime += deltaT;
	//Prevent Time Delta from getting too low - extremely naive fix. This will likely cause stuttering and incorrect flight times on some configurations. However, if a pc is limited
	// to 60fps or 75fps (such is the case for @BLuedtke), it'll work.
	if (deltaT < 0.013 && savedTime < 0.013) {
		savedTime += deltaT;
		//std::cout << "SavedTime" << std::endl;
	}
	else {
		//calcKeplerProblem(totalTime, totalTime-(savedTime+deltaT));
		calcKeplerProblem_experimental(totalTime, totalTime - (savedTime + deltaT));
		savedTime = 0;
	}
		
	Matrix f = Matrix();
	f.translation(r);
	uTransform = f;
	/*
	try
	{
	}
	catch (const std::exception& e)
	{
		std::cout << "Sat Update Exception occured" << std::endl;
		std::cout << e.what() << std::endl;
	}/**/
}


//Method for going through the orbit and calculating a collection of points along the orbit
// This can be used to represent the trajectory with lines (i.e. used for OrbitLineModel).
std::vector<Vector> Satellite::calcOrbitVis()
{
	std::vector<Vector> resVec;
	double startAngle = this->ephemeris.trueAnomaly;
	//The large timesteps of this method cause some rounding errors. Therefore, save the first values for r0 and v0 and update them later.
	Vector r0s = this->ephemeris.getR0();
	Vector v0s = this->ephemeris.getV0();
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
		cout << "calcOrbitVis Changing max Steps to " << maxSteps << "\n";
	}
	
	while (runner < maxSteps) {
		this->totalTime += stepper;

		t1 = std::chrono::steady_clock::now();
		//this->calcKeplerProblem(this->totalTime, this->totalTime-stepper);
		this->calcKeplerProblem_experimental(this->totalTime, this->totalTime-stepper);
		t2 = std::chrono::steady_clock::now();
		
		totalTimeMilli += timeInMilliSeconds(t1, t2);
		
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
	this->ephemeris.updateR0V0(r0s, v0s);
	//cout << "t calcKepler: " << totalTimeMilli << "\n";
	cout << "Avg: " << (totalTimeMilli / static_cast<double>(runner)) << "\n";
	//cout << "Runner: " << runner << "; MaxSteps: " << maxSteps << "; timestep size (s): " << stepper << endl;
	//Just for testing:
	//cout << "Orbital Period according to Semi-Major Axis formula: " << ephemeris.getEllipseOrbitalPeriod() << endl;
	//cout << "Points for vis: " << resVec.size() << endl;
	//cout << "Finished Calculating Orbit Line" << endl;
	//cout << "Starting Position: " << this->ephemeris.getR0().toString() << std::endl;
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
