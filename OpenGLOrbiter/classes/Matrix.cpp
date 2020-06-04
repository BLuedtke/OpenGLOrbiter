//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 03.11.14.


#include "Matrix.h"
#include "math.h"
#include <assert.h>

#define WEAK_EPSILON 1e-4f

//Matrix Default constructor initialises the identity (matrix).
Matrix::Matrix()
{
	m00 = 1;	m01 = 0;	m02 = 0;	m03 = 0;
	m10 = 0;	m11 = 1;	m12 = 0;	m13 = 0;
	m20 = 0;	m21 = 0;	m22 = 1;	m23 = 0;
	m30 = 0;	m31 = 0;	m32 = 0;	m33 = 1;
}

Matrix::Matrix( float _00, float _01, float _02, float _03,
                float _10, float _11, float _12, float _13,
                float _20, float _21, float _22, float _23,
                float _30, float _31, float _32, float _33 ) :
    m00(_00), m01(_01), m02(_02), m03(_03),
    m10(_10), m11(_11), m12(_12), m13(_13),
    m20(_20), m21(_21), m22(_22), m23(_23),
    m30(_30), m31(_31), m32(_32), m33(_33)
{
}

Matrix::operator float*()
{
    return m;
}
Matrix::operator const float* const()
{
    return m;
}

Matrix Matrix::operator*(const Matrix& M) const
{
    Matrix Out = *this;
    Out.multiply(M);
    return Out;
}
Matrix& Matrix::operator*=(const Matrix& M)
{
    multiply(M);
    return *this;
}

Vector Matrix::operator*(const Vector& v) const
{
    return transformVec4x4(v);
}

bool Matrix::operator==(const Matrix& M)
{
    const float Epsilon = WEAK_EPSILON;
    return fabs(m00-M.m00)<=Epsilon && fabs(m01-M.m01)<=Epsilon && fabs(m02-M.m02)<=Epsilon && fabs(m03-M.m03)<=Epsilon &&
    fabs(m10-M.m10)<=Epsilon && fabs(m11-M.m11)<=Epsilon && fabs(m12-M.m12)<=Epsilon && fabs(m13-M.m13)<=Epsilon &&
    fabs(m20-M.m20)<=Epsilon && fabs(m21-M.m21)<=Epsilon && fabs(m22-M.m22)<=Epsilon && fabs(m23-M.m23)<=Epsilon &&
    fabs(m30-M.m30)<=Epsilon && fabs(m31-M.m31)<=Epsilon && fabs(m32-M.m32)<=Epsilon && fabs(m33-M.m33)<=Epsilon;
    
    return false;
}

Vector Matrix::transformVec4x4( const Vector& v) const
{
    float X = m00*v.X + m01*v.Y + m02*v.Z + m03;
    float Y = m10*v.X + m11*v.Y + m12*v.Z + m13;
    float Z = m20*v.X + m21*v.Y + m22*v.Z + m23;
    float W = m30*v.X + m31*v.Y + m32*v.Z + m33;
    return Vector( X/W, Y/W, Z/W);
    
}
Vector Matrix::transformVec3x3( const Vector& v) const
{
    float X = m00*v.X + m01*v.Y + m02*v.Z;
    float Y = m10*v.X + m11*v.Y + m12*v.Z;
    float Z = m20*v.X + m21*v.Y + m22*v.Z;
    return Vector( X, Y, Z);
}


bool Matrix::operator!=(const Matrix& M)
{
    return !(*this==M);
}

Vector Matrix::left() const
{
    return Vector(-m00, -m10, -m20);
}

Vector Matrix::right() const
{
    return Vector(m00, m10, m20);
}

Vector Matrix::up() const
{
    return Vector(m01, m11, m21);
}

Vector Matrix::down() const
{
    return Vector(-m01, -m11, -m21);
}

Vector Matrix::forward() const
{
    return Vector(m02, m12, m22);
}

Vector Matrix::backward() const
{
    return Vector(-m02, -m12, -m22);
}

Vector Matrix::translation() const
{
    return Vector(m03, m13, m23);
}

void Matrix::up( const Vector& v)
{
    m01 = v.X;
    m11 = v.Y;
    m21 = v.Z;
}

void Matrix::forward( const Vector& v)
{
    m02 = v.X;
    m12 = v.Y;
    m22 = v.Z;
}

void Matrix::right( const Vector& v)
{
    m00 = v.X;
    m10 = v.Y;
    m20 = v.Z;
}

Matrix& Matrix::multiply(const Matrix& M )
{
    const Matrix& A = *this;
    
    Matrix Tmp(
        A.m00 * M.m00 + A.m01 * M.m10  + A.m02 * M.m20 + A.m03 * M.m30,
        A.m00 * M.m01 + A.m01 * M.m11  + A.m02 * M.m21 + A.m03 * M.m31,
        A.m00 * M.m02 + A.m01 * M.m12  + A.m02 * M.m22 + A.m03 * M.m32,
        A.m00 * M.m03 + A.m01 * M.m13  + A.m02 * M.m23 + A.m03 * M.m33,

        A.m10 * M.m00 + A.m11 * M.m10  + A.m12 * M.m20 + A.m13 * M.m30,
        A.m10 * M.m01 + A.m11 * M.m11  + A.m12 * M.m21 + A.m13 * M.m31,
        A.m10 * M.m02 + A.m11 * M.m12  + A.m12 * M.m22 + A.m13 * M.m32,
        A.m10 * M.m03 + A.m11 * M.m13  + A.m12 * M.m23 + A.m13 * M.m33,

        A.m20 * M.m00 + A.m21 * M.m10  + A.m22 * M.m20 + A.m23 * M.m30,
        A.m20 * M.m01 + A.m21 * M.m11  + A.m22 * M.m21 + A.m23 * M.m31,
        A.m20 * M.m02 + A.m21 * M.m12  + A.m22 * M.m22 + A.m23 * M.m32,
        A.m20 * M.m03 + A.m21 * M.m13  + A.m22 * M.m23 + A.m23 * M.m33,

        A.m30 * M.m00 + A.m31 * M.m10  + A.m32 * M.m20 + A.m33 * M.m30,
        A.m30 * M.m01 + A.m31 * M.m11  + A.m32 * M.m21 + A.m33 * M.m31,
        A.m30 * M.m02 + A.m31 * M.m12  + A.m32 * M.m22 + A.m33 * M.m32,
        A.m30 * M.m03 + A.m31 * M.m13  + A.m32 * M.m23 + A.m33 * M.m33 );
    *this = Tmp;
    return *this;
}
Matrix& Matrix::translation(float X, float Y, float Z )
{
    m00= 1;	m01= 0;	m02= 0;	m03= X;
    m10= 0;	m11= 1;	m12= 0;	m13= Y;
    m20= 0;	m21= 0;	m22= 1;	m23= Z;
    m30= 0;	m31= 0;	m32= 0;	m33= 1;
    return *this;
}
Matrix& Matrix::translation(const Vector& XYZ )
{
    return translation(XYZ.X, XYZ.Y, XYZ.Z);
}
Matrix& Matrix::rotationX(float Angle )
{
    m00= 1;	m01= 0;	m02= 0;	m03= 0;
    m10= 0;					m13= 0;
    m20= 0;					m23= 0;
    m30= 0;	m31= 0;	m32= 0;	m33= 1;
    
    m11 = m22 = cos(Angle);
    m21 = sin(Angle);
    m12 = -m21;
    
    return *this;
}
Matrix& Matrix::rotationY(float Angle )
{
            m01= 0;         m03= 0;
    m10= 0;	m11= 1;	m12= 0;	m13= 0;
            m21= 0;         m23= 0;
    m30= 0;	m31= 0;	m32= 0;	m33= 1;
    
    m00 = m22 = cos(Angle);
    m02 = sin(Angle);
    m20 = -m02;
    
    return *this;
}
Matrix& Matrix::rotationZ(float Angle )
{
                    m02= 0;	m03= 0;
                    m12= 0;	m13= 0;
    m20= 0;	m21= 0;	m22= 1;	m23= 0;
    m30= 0;	m31= 0;	m32= 0;	m33= 1;
    
    m00 = m11 = cos(Angle);
    m10= sin(Angle);
    m01= -m10;
    
    return *this;
}
Matrix& Matrix::rotationYawPitchRoll( float Yaw, float Pitch, float Roll )
{
    float cosx = cos(Pitch);
    float cosy = cos(Yaw);
    float cosz = cos(Roll);
    
    float sinx = sin(Pitch);
    float siny = sin(Yaw);
    float sinz = sin(Roll);
    
    m00 = cosz*cosy + sinz*sinx*siny;
    m10 = sinz*cosx;
    m20 = -cosz*siny + sinz*sinx*cosy;
    m30 = 0;

    m01 = -sinz*cosy + cosz*sinx*siny;
    m11 = cosz*cosx;
    m21 = sinz*siny + cosz*sinx*cosy;
    m31 = 0;
    
    m02 = cosx*siny;
    m12 = -sinx;
    m22 = cosx*cosy;
    m32 = 0;
    
    m03 = m13 = m23 = 0;
    m33 = 1;
    
    return *this;
}
Matrix& Matrix::rotationYawPitchRoll(const Vector& Angles )
{
    rotationYawPitchRoll(Angles.X, Angles.Y, Angles.Z);
    return *this;
}
Matrix& Matrix::rotationAxis(const Vector& Axis, float Angle)
{
	
    const float Si = sinf(Angle);
    const float Co = cosf(Angle);
    const float OMCo = 1 - Co;
    Vector Ax = Axis;
    Ax.normalize();
    /*
	std::cout << "Ax: ";
	Ax.print();
	std::cout << "Angle: " << Angle << std::endl;
	std::cout << "Si: " << Si << "; Co: " << Co << "; OMCo: " << OMCo << std::endl;
    /**/
	m00= (Ax.X * Ax.X) * OMCo + Co;
    m01= (Ax.X * Ax.Y) * OMCo - (Ax.Z * Si);
    m02= (Ax.X * Ax.Z) * OMCo + (Ax.Y * Si);
    m03= 0;
    
    m10= (Ax.Y * Ax.X) * OMCo + (Ax.Z * Si);
    m11= (Ax.Y * Ax.Y) * OMCo + Co;
    m12= (Ax.Y * Ax.Z) * OMCo - (Ax.X * Si);
    m13= 0;
    
    m20= (Ax.Z * Ax.X) * OMCo - (Ax.Y * Si);
    m21= (Ax.Z * Ax.Y) * OMCo + (Ax.X * Si);
    m22= (Ax.Z * Ax.Z) * OMCo + Co;
    m23= 0;
    
    m30= 0;
    m31= 0;
    m32= 0;
    m33= 1;
    
    return *this;
}
Matrix& Matrix::scale(float ScaleX, float ScaleY, float ScaleZ )
{
    m00= ScaleX;	m01= 0;			m02= 0;			m03= 0;
    m10= 0;			m11= ScaleY;	m12= 0;			m13= 0;
    m20= 0;			m21= 0;			m22= ScaleZ;	m23= 0;
    m30= 0;			m31= 0;			m32= 0;			m33= 1;
    
    return *this;
}
Matrix& Matrix::scale(const Vector& Scalings )
{
    scale( Scalings.X, Scalings.Y, Scalings.Z);
    return *this;
}
Matrix& Matrix::scale(float Scaling )
{
    scale(Scaling, Scaling, Scaling);
    return *this;
}
Matrix& Matrix::identity()
{
    m00= 1;	m01= 0;	m02= 0;	m03= 0;
    m10= 0;	m11= 1;	m12= 0;	m13= 0;
    m20= 0;	m21= 0;	m22= 1;	m23= 0;
    m30= 0;	m31= 0;	m32= 0;	m33= 1;
    return *this;
}
Matrix& Matrix::transpose()
{
    Matrix Tmp(
      m00, m10, m20, m30,
      m01, m11, m21, m31,
      m02, m12, m22, m32,
      m03, m13, m23, m33 );
    *this = Tmp;
    return *this;
}
Matrix& Matrix::invert()
{
    const float num5 = m00;
    const float num4 = m01;
    const float num3 = m02;
    const float num2 = m03;
    const float num9 = m10;
    const float num8 = m11;
    const float num7 = m12;
    const float num6 = m13;
    const float num17 = m20;
    const float num16 = m21;
    const float num15 = m22;
    const float num14 = m23;
    const float num13 = m30;
    const float num12 = m31;
    const float num11 = m32;
    const float num10 = m33;
    const float num23 = (num15 * num10) - (num14 * num11);
    const float num22 = (num16 * num10) - (num14 * num12);
    const float num21 = (num16 * num11) - (num15 * num12);
    const float num20 = (num17 * num10) - (num14 * num13);
    const float num19 = (num17 * num11) - (num15 * num13);
    const float num18 = (num17 * num12) - (num16 * num13);
    const float num39 = ((num8 * num23) - (num7 * num22)) + (num6 * num21);
    const float num38 = -(((num9 * num23) - (num7 * num20)) + (num6 * num19));
    const float num37 = ((num9 * num22) - (num8 * num20)) + (num6 * num18);
    const float num36 = -(((num9 * num21) - (num8 * num19)) + (num7 * num18));
    const float num = (float)1 / ((((num5 * num39) + (num4 * num38)) + (num3 * num37)) + (num2 * num36));
    m00 = num39 * num;
    m10 = num38 * num;
    m20 = num37 * num;
    m30 = num36 * num;
    m01 = -(((num4 * num23) - (num3 * num22)) + (num2 * num21)) * num;
    m11 = (((num5 * num23) - (num3 * num20)) + (num2 * num19)) * num;
    m21 = -(((num5 * num22) - (num4 * num20)) + (num2 * num18)) * num;
    m31 = (((num5 * num21) - (num4 * num19)) + (num3 * num18)) * num;
    const float num35 = (num7 * num10) - (num6 * num11);
    const float num34 = (num8 * num10) - (num6 * num12);
    const float num33 = (num8 * num11) - (num7 * num12);
    const float num32 = (num9 * num10) - (num6 * num13);
    const float num31 = (num9 * num11) - (num7 * num13);
    const float num30 = (num9 * num12) - (num8 * num13);
    m02 = (((num4 * num35) - (num3 * num34)) + (num2 * num33)) * num;
    m12 = -(((num5 * num35) - (num3 * num32)) + (num2 * num31)) * num;
    m22 = (((num5 * num34) - (num4 * num32)) + (num2 * num30)) * num;
    m32 = -(((num5 * num33) - (num4 * num31)) + (num3 * num30)) * num;
    const float num29 = (num7 * num14) - (num6 * num15);
    const float num28 = (num8 * num14) - (num6 * num16);
    const float num27 = (num8 * num15) - (num7 * num16);
    const float num26 = (num9 * num14) - (num6 * num17);
    const float num25 = (num9 * num15) - (num7 * num17);
    const float num24 = (num9 * num16) - (num8 * num17);
    m03 = -(((num4 * num29) - (num3 * num28)) + (num2 * num27)) * num;
    m13 = (((num5 * num29) - (num3 * num26)) + (num2 * num25)) * num;
    m23 = -(((num5 * num28) - (num4 * num26)) + (num2 * num24)) * num;
    m33 = (((num5 * num27) - (num4 * num25)) + (num3 * num24)) * num;
    return *this;
}
Matrix& Matrix::lookAt(const Vector& Target, const Vector& Up, const Vector& Position )
{
    Vector f = Target-Position;
    f.normalize();
    Vector u = Up;
    u.normalize();
    Vector r = f.cross(u);
    r.normalize();
    u = r.cross(f);
    m00 = r.X;   m01 = r.Y;   m02 = r.Z;   m03 = -(r.dot(Position));
    m10 = u.X;   m11 = u.Y;   m12 = u.Z;   m13 = -(u.dot(Position));
    m20 = -f.X;  m21 = -f.Y;  m22 = -f.Z;  m23 = (f.dot(Position));
    m30 = 0;     m31 = 0;     m32 = 0;     m33 = 1;
    return *this;
}
Matrix& Matrix::perspective(float Fovy, float AspectRatio, float NearPlane, float FarPlane )
{
    assert(NearPlane<FarPlane);
    
    const float f = 1.0f/tan(Fovy*0.5f);
    const float NearMinusFar = NearPlane-FarPlane;
    
    m01 = m02 = m03 = 0;
    m10 = m12 = m13 = 0;
    m20 = m21 = 0;
    m30 = m31 = m33 = 0;
    m32 = -1;
    
    m00 = f / AspectRatio;
    m11 = f;
    m22 = (FarPlane+NearPlane)/NearMinusFar;
    m23 = 2.0f*FarPlane*NearPlane/NearMinusFar;
    return *this;
}
Matrix& Matrix::orthographic(float Width, float Height, float Near, float Far )
{
    float FMN = 1.0f/(Far-Near);
    m00 = 2.0f/Width;   m01 = 0.0f;         m02 = 0.0f;      m03 = 0.0f;
    m10 = 0.0f;         m11 = 2.0f/Height;  m12 = 0.0f;      m13 = 0.0f;
    m20 = 0.0f;         m21 = 0.0f;         m22 = -2.0f*FMN; m23 = -(Far+Near)*FMN;
    m30 = 0.0f;         m31 = 0.0f;         m32 = 0.0f;      m33 = 1.0f;
    return *this;
}
float Matrix::determinat()
{
    return	m00 * (m11 * m22 - m12 * m21) -
    m01 * (m10 * m22 - m12 * m20) +
    m02 * (m10 * m21 - m11 * m20);
}

