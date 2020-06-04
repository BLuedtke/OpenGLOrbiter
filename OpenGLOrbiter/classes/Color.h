//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.

#ifndef Color_hpp
#define Color_hpp

#include <iostream>

class Color
{
public:
    float R;
    float G;
    float B;
    Color();
    Color(float r, float g, float b);
    Color operator*(const Color& c) const;
    Color operator*(const float Factor) const;
    Color operator+(const Color& c) const;
    Color& operator+=(const Color& c);
    void print() const;
};


#endif /* Color_hpp */