//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 03.11.14.
    //  Copyright © 2014 Philipp Lensing. All rights reserved.


#ifndef Matrix_hpp
#define Matrix_hpp

#include <iostream>
#include "vector.h"

class Matrix
{
public:
    union
    {
        struct {
            float m00, m10,m20,m30;
            float m01, m11,m21,m31;
            float m02, m12,m22,m32;
            float m03, m13,m23,m33;
        };
        struct { float m[16]; };
    };
    Matrix();
    Matrix( float _00, float _01, float _02, float _03,
            float _10, float _11, float _12, float _13,
            float _20, float _21, float _22, float _23,
            float _30, float _31, float _32, float _33 );

    
    operator float*();
    operator const float* const();
    
    Matrix operator*(const Matrix& M) const;
    Matrix& operator*=(const Matrix& M);
    Vector operator*(const Vector& v) const;
    
    bool operator==(const Matrix& M);
    bool operator!=(const Matrix& M);
    
    Vector left() const;
    Vector right() const;
    Vector up() const;
    Vector down() const;
    Vector forward() const;
    Vector backward() const;
    Vector translation() const;
    
    void up( const Vector& v);
    void forward( const Vector& v);
    void right( const Vector& v);
    
    Matrix& multiply(const Matrix& M );
    Matrix& translation(float X, float Y, float Z );
    Matrix& translation(const Vector& XYZ );
    Matrix& rotationX(float Angle );
    Matrix& rotationY(float Angle );
    Matrix& rotationZ(float Angle );
    Matrix& rotationYawPitchRoll( float Yaw, float Pitch, float Roll );
    Matrix& rotationYawPitchRoll(const Vector& Angles );
    Matrix& rotationAxis(const Vector& Axis, float Angle);
    Matrix& scale(float ScaleX, float ScaleY, float ScaleZ );
    Matrix& scale(const Vector& Scalings );
    Matrix& scale(float Scaling );
    Matrix& identity();
    Matrix& transpose();
    Matrix& invert();
    Matrix& lookAt(const Vector& Target, const Vector& Up, const Vector& Position );
    Matrix& perspective(float Fovy, float AspectRatio, float NearPlane, float FarPlane );
    Matrix& orthographic(float Width, float Height, float Near, float Far );
    Vector transformVec4x4( const Vector& v) const;
    Vector transformVec3x3( const Vector& v) const;
    float determinat();
};


#endif /* Matrix_hpp */
