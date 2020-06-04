//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.
    //  Copyright © 2014 Philipp Lensing.

#ifndef Texture_hpp
#define Texture_hpp

#include <iostream>
#include <map>

#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#endif

class RGBImage;

class Texture
{
public:
    Texture();
    Texture(unsigned int width, unsigned int height, unsigned char* data);
	Texture(unsigned int width, unsigned int height, GLint InternalFormat, GLint Format, GLint ComponentSize, GLint MinFilter, GLint MagFilter, GLint AddressMode, bool GenMipMaps);
    Texture(const char* Filename );
    Texture(const RGBImage& img);
    ~Texture();
    bool load(const char* Filename);
    bool create(unsigned int width, unsigned int height, unsigned char* data);
	bool create(unsigned int width, unsigned int height, GLint InternalFormat, GLint Format, GLint ComponentSize, GLint MinFilter, GLint MagFilter, GLint AddressMode, bool GenMipMaps);
    bool create(const RGBImage& img);
    void activate(int slot=0) const;
    void deactivate() const;
    bool isValid() const;
	unsigned int width() const;
	unsigned int height() const;
	GLuint ID() const;
    const RGBImage* getRGBImage() const;
    static Texture* defaultTex();
    static Texture* defaultEmissiveTex();
	static Texture* defaultNormalTex();
    static const Texture* LoadShared(const char* Filename);
    static void ReleaseShared( const Texture* pTex );
    
private:
    void release();    
    RGBImage* createImage( unsigned char* Data, unsigned int width, unsigned int height );
    GLuint m_TextureID;
    RGBImage* m_pImage;
	unsigned int Width;
	unsigned int Height;
    mutable int CurrentTextureUnit;
    static Texture* pDefaultTex;
    static Texture* pEmissiveTex;
	static Texture* pDefaultNormalTex;
    
    struct TexEntry
    {
        int RefCount;
        const Texture* pTex;
    };
    typedef std::map<std::string, TexEntry> SharedTexMap;
    static SharedTexMap SharedTextures;
    
};

#endif /* Texture_hpp */
