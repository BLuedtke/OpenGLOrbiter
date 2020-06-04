//  Author: Bernhard Luedtke. 2020

#ifndef RGBImage_hpp
#define RGBImage_hpp

#include <iostream>
#include <vector>
class Color;

class RGBImage
{
public:
    RGBImage( unsigned int Width, unsigned Height);
    ~RGBImage();
    void setPixelColor( unsigned int x, unsigned int y, const Color& c);
    const Color& getPixelColor( unsigned int x, unsigned int y) const;
    bool saveToDisk( const char* Filename);
    unsigned int width() const;
    unsigned int height() const;
    
    static unsigned char convertColorChannel( float f);
protected:
    unsigned int m_Height;
    unsigned int m_Width;
    std::vector<std::vector<Color>> matrix;
};

#endif /* RGBImage_hpp */
