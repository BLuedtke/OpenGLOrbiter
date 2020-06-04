#include "vector.h"
#include <assert.h>
#include <math.h>
#include <string>
#include <emmintrin.h>

Vector::Vector(float x, float y, float z)
{
	X = x;
	Y = y;
	Z = z;
}

Vector::Vector()
{
	X = 0.0f;
	Y = 0.0f;
	Z = 0.0f;
}

float Vector::dot(const Vector& v) const
{
	return X * v.X + Y * v.Y + Z * v.Z;
}

Vector Vector::cross(const Vector& v) const
{
	float a = (Y*v.Z) - (Z*v.Y);
	float b = (Z*v.X) - (X*v.Z);
	float c = (X*v.Y) - (Y*v.X);
	return Vector(a, b, c);
}


Vector Vector::operator+(const Vector& v) const
{
	float a = X + v.X;
	float b = Y + v.Y;
	float c = Z + v.Z;
	return Vector(a, b, c);
}

Vector Vector::operator-(const Vector& v) const
{
	float a = X - v.X;
	float b = Y - v.Y;
	float c = Z - v.Z;
	return Vector(a, b, c);
}

Vector Vector::operator*(float c) const
{
	float a = X * c;
	float b = Y * c;
	float d = Z * c;
	return Vector(a, b, d);
}

Vector Vector::operator-() const // Vektor invertieren
{
	return Vector(-X, -Y, -Z);
}

Vector& Vector::operator+=(const Vector& v)
{
	this->X = this->X + v.X;
	this->Y = this->Y + v.Y;
	this->Z = this->Z + v.Z;
	return *this;
}

Vector& Vector::normalize()
{
	float l = this->length();
	float kehrwert = 1 / l;
	X = X * kehrwert;
	Y = Y * kehrwert;
	Z = Z * kehrwert;
	return  *this;
}

bool Vector::operator==(const Vector & v) const
{
	if (X == v.X && Y == v.Y && Z == v.Z) {
		return true;
	}
	return false;
}

/*
//F�hrt zu einem seltsamen Bild
float sqrt14(float n)
{

	//__m128 cache1 = reinterpret_cast<__m128&>(n);
	//__m128 mmsqrt = _mm_rsqrt_ss(cache1);
	//cache1 = _mm_mul_ss(cache1, mmsqrt);
	//return reinterpret_cast<float&>(cache1);

	//oder (deutlich langsamer als sqrtf):
	__m128 mmsqrt = _mm_sqrt_ss(reinterpret_cast<__m128&>(n));
	return reinterpret_cast<float&>(mmsqrt);
}/**/

float Vector::length() const
{
	return sqrtf(this->lengthSquared());
	//return sqrt14(this->lengthSquared());
}


float Vector::lengthSquared() const
{
	return this->dot(*this);
}

Vector Vector::reflection(const Vector& normal) const
{
	float s = -2.0f * (this->dot(normal));
	return *this + normal * s;
}

std::string Vector::toString() const
{
	return "X: " + std::to_string(this->X) + "; Y: " + std::to_string(this->Y) + "; Z: " + std::to_string(this->Z);
}

void Vector::print() const {
	std::cout << "X: " << this->X << "; Y: " << this->Y << "; Z: " << this->Z << std::endl;
}

bool Vector::triangleIntersection(const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const
{
	Vector n = ((b - a).cross(c - a)).normalize();
	//n.normalize();
	float ndL = n.dot(d);
	//Strahl und Ebene Parallel?
	if (fabs(ndL) <= 1e-20f) {
		return false;
	}
	float dl = n.dot(a);
	s = (dl - n.dot(*this)) / ndL;
	// s zu klein?
	if (s < 1e-20f) {
		return false;
	}
	Vector p = *this + d * s;
	Vector pMa = p - a;
	return triangleArea(a, b, c) + 1e-5f >= (triangleAreaX(a, b, pMa) + triangleAreaX(a, c, pMa) + triangleArea(b, c, p));
	//return triangleArea(a,b,c) + 1e-5f >= (triangleArea(a,b,p) + triangleArea(a,c,p) + triangleArea(b,c,p));   
}


bool Vector::triangleIntersectionInformed(const Vector & d, const Vector & a, const Vector & b, const Vector & c, float & s, const Vector tN) const
{
	Vector n = tN;
	//Vector n = ((b - a).cross(c - a)).normalize();
	//n.normalize();
	float ndL = n.dot(d);
	//Strahl und Ebene Parallel?
	if (fabs(ndL) <= 1e-20f) {
		return false;
	}
	float dl = n.dot(a);
	s = (dl - n.dot(*this)) / ndL;
	// s zu klein?
	if (s < 1e-20f) {
		return false;
	}
	Vector p = *this + d * s;
	Vector pMa = p - a;
	return triangleArea(a, b, c) + 1e-5f >= (triangleAreaX(a, b, pMa) + triangleAreaX(a, c, pMa) + triangleArea(b, c, p));
}


float Vector::triangleAreaX(const Vector & u, const Vector & v, const Vector & wMu) const
{
	return 0.5f * ((v - u).cross(wMu)).length();
}

float Vector::triangleArea(const Vector& u, const Vector& v, const Vector& w) const
{
	return 0.5f * ((v - u).cross(w - u)).length();
}
