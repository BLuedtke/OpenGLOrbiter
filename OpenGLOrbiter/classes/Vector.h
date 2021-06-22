//	Author: Bernhard Luedtke

#ifndef Vector_hpp
#define Vector_hpp

#include <iostream>

class Vector
{
public:
	float X;
	float Y;
	float Z;

	Vector(float x, float y, float z);
	Vector();
	Vector(Vector&&) noexcept = default;
	Vector(const Vector&) noexcept = default;

	float dot(const Vector& v) const;
	Vector cross(const Vector& v) const;
	Vector& normalize();

	Vector operator+(const Vector& v) const;
	Vector operator-(const Vector& v) const;
	Vector& operator+=(const Vector& v);
	Vector operator*(float c) const;
	Vector operator-() const;
	Vector& operator=(Vector&&) noexcept = default;
	Vector& operator=(const Vector&) noexcept = default;
	bool operator==(const Vector& v) const;

	float length() const;
	float lengthSquared() const;
	
	Vector reflection(const Vector& normal) const;
	
	bool triangleIntersection(const Vector& d, const Vector& a, const Vector& b,
		const Vector& c, float& s) const;
	bool triangleIntersectionInformed(const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s, const Vector tN) const;
	float triangleArea(const Vector& u, const Vector& v, const Vector& w) const;
	float triangleAreaX(const Vector& u, const Vector& v, const Vector& wMu) const;
	
	void print() const;
	std::string toString() const;
};


#endif /* Vector_hpp */
