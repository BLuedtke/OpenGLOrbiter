//  Author/Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.

#include "Color.h"
#include <assert.h>
#include <algorithm>

Color::Color()
{
    this->R = 0.0f;
    this->G = 0.0f;
    this->B = 0.0f;
}

Color::Color(float r, float g, float b)
{
    this->R = r;
    this->G = g;
    this->B = b;
}

Color Color::operator*(const Color& c) const
{
    return Color(this->R * c.R,this->G * c.G, this->B * c.B);
}

Color Color::operator*(const float Factor) const
{
	return Color(this->R * Factor,this->G * Factor, this->B * Factor);
}

Color Color::operator+(const Color& c) const
{
    return Color(std::min(this->R + c.R,1.0f),std::min(this->G + c.G,1.0f), std::min(this->B + c.B,1.0f));
}

Color& Color::operator+=(const Color& c)
{
    this->R += c.R;
    this->G += c.G;
    this->B += c.B;
	return *this;
}

void Color::print() const{
    std::cout << "R: " << this->R << "; G: " << this->G << "; B: " << this->B << std::endl;
}
