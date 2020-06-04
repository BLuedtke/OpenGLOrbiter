//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.


#include "Texture.h"
#include "rgbimage.h"
#include "color.h"
#include <assert.h>
#include <stdint.h>
#include <exception>
#include <algorithm>
#include "FreeImage.h"

Texture* Texture::pDefaultTex = NULL;
Texture* Texture::pEmissiveTex = NULL;
Texture* Texture::pDefaultNormalTex = NULL;
Texture::SharedTexMap Texture::SharedTextures;

Texture* Texture::defaultTex()
{
    if(pDefaultTex)
        return pDefaultTex;
    
    unsigned char* data = new unsigned char[4*4*4];
    assert(data);
    std::memset(data, 255, 4*4*4);
    pDefaultTex = new Texture(4, 4, data);
    delete [] data;
    
    return pDefaultTex;
}

Texture * Texture::defaultEmissiveTex()
{
	if (pEmissiveTex)
		return pEmissiveTex;

	unsigned char* data = new unsigned char[4 * 4 * 4];
	assert(data);
	std::memset(data, 1, 4 * 4 * 4);
	pEmissiveTex = new Texture(4, 4, data);
	delete[] data;

	return pEmissiveTex;
}

Texture* Texture::defaultNormalTex()
{
	if (pDefaultNormalTex)
		return pDefaultNormalTex;

	unsigned char data[4 * 4 * 4] = {
		127,127,255,255, 127,127,255,255, 127,127,255,255, 127,127,255,255,
		127,127,255,255, 127,127,255,255, 127,127,255,255, 127,127,255,255,
		127,127,255,255, 127,127,255,255, 127,127,255,255, 127,127,255,255,
		127,127,255,255, 127,127,255,255, 127,127,255,255, 127,127,255,255,
	};

	pDefaultNormalTex = new Texture(4, 4, data);

	return pDefaultNormalTex;
}

const Texture* Texture::LoadShared(const char* Filename)
{	
    std::string path = Filename;
    
    std::transform(path.begin(), path.end(), path.begin(), ::tolower);
    
    SharedTexMap::iterator it = SharedTextures.find( path);
    
    if(it != SharedTextures.end())
    {
        it->second.RefCount++;
        return it->second.pTex;
    }
    
    Texture* pTex = new Texture();
    if(!pTex->load(Filename) )
    {
        delete pTex;
        std::cout << "WARNING: Texture " << Filename << " not loaded (not found).\n";
        return NULL;
    }
    
    TexEntry TE;
    TE.pTex = pTex;
    TE.RefCount = 1;
    SharedTextures[path] = TE;
    
    return pTex;
}

void Texture::ReleaseShared( const Texture* pTex )
{
    for( SharedTexMap::iterator it=SharedTextures.begin(); it!=SharedTextures.end(); ++it)
    {
        if(it->second.pTex == pTex)
        {
            it->second.RefCount--;
            if(it->second.RefCount<=0)
            {
                delete it->second.pTex;
                SharedTextures.erase(it);
            }
            return;
        }
    }
}



Texture::Texture() : m_TextureID(0), m_pImage(NULL), CurrentTextureUnit(0), Width(0), Height(0)
{
    
}



Texture::Texture(unsigned int width, unsigned int height, unsigned char* data): m_TextureID(0), m_pImage(NULL), CurrentTextureUnit(0), Width(0), Height(0)
{
    bool Result = create(width, height, data);
    if(!Result)
        throw std::exception();
	Width = width;
	Height = height;
}

Texture::Texture(unsigned int width, unsigned int height, GLint InternalFormat, GLint Format, GLint ComponentSize, GLint MinFilter, GLint MagFilter, GLint AddressMode, bool GenMipMaps)
	: m_TextureID(0), m_pImage(NULL), CurrentTextureUnit(0), Width(0), Height(0)
{
	bool Result = create(width, height, InternalFormat, Format, ComponentSize, MinFilter, MagFilter, AddressMode, GenMipMaps);
	if (!Result)
		throw std::exception();
	Width = width;
	Height = height;
}

Texture::Texture(const char* Filename ): m_TextureID(0), m_pImage(NULL), CurrentTextureUnit(0), Width(0), Height(0)
{
    bool Result = load(Filename);
    if(!Result)
        throw std::exception();

	Width = getRGBImage()->width();
	Height = getRGBImage()->height();
}

Texture::Texture(const RGBImage& img) : m_TextureID(0), m_pImage(NULL), CurrentTextureUnit(0), Width(0), Height(0)
{
    bool Result = create(img);
    if(!Result)
        throw std::exception();

	Width = getRGBImage()->width();
	Height = getRGBImage()->height();
}

Texture::~Texture()
{
    release();
}

void Texture::release()
{
    if(isValid())
    {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = -1;
    }
    if(m_pImage)
        delete m_pImage;
    m_pImage = NULL;
}

bool Texture::isValid() const
{
    return m_TextureID > 0;
}

unsigned int Texture::width() const
{
	return Width;
}
unsigned int Texture::height() const
{
	return Height;
}

GLuint Texture::ID() const
{
	return m_TextureID;
}

bool Texture::load( const char* Filename)
{
    release();
    FREE_IMAGE_FORMAT ImageFormat = FreeImage_GetFileType(Filename, 0);
    if(ImageFormat == FIF_UNKNOWN)
        ImageFormat = FreeImage_GetFIFFromFilename(Filename);
    
    if(ImageFormat == FIF_UNKNOWN)
    {
        std::cout << "Warning: Unkown texture format: " << Filename << std::endl;
        return false;
    }
    
    FIBITMAP* pBitmap = FreeImage_Load( ImageFormat, Filename);
    
    if(pBitmap==NULL)
    {
        std::cout << "Warning: Unable to open texture image " << Filename << std::endl;
        return false;
    }
    
    FREE_IMAGE_TYPE Type = FreeImage_GetImageType(pBitmap);
    assert(Type==FIT_BITMAP);
    
    unsigned int Width = FreeImage_GetWidth(pBitmap);
    unsigned int Height = FreeImage_GetHeight(pBitmap);
    unsigned int bpp = FreeImage_GetBPP(pBitmap);
    assert(bpp==32||bpp==16||bpp==24);
    
	//TODO rewrite this portion to get rid of new entirely.
    unsigned char* data = new unsigned char[Width* Height*4];
    unsigned char* dataPtr = data-1;
    
    if(data==NULL)
    {
        FreeImage_Unload(pBitmap);
        return false;
    }

    
    RGBQUAD c;
    for( unsigned int i=0; i<Height; ++i)
        for( unsigned int j=0; j<Width; ++j)
        {
            FreeImage_GetPixelColor(pBitmap, j, Height-i-1, &c);
            *(++dataPtr) = c.rgbRed;
            *(++dataPtr) = c.rgbGreen;
            *(++dataPtr) = c.rgbBlue;
            if(bpp==32)
                *(++dataPtr) = c.rgbReserved;
            else
                *(++dataPtr) = 255;
        }
    
    FreeImage_Unload(pBitmap);

    if( m_pImage )
        delete m_pImage;
    
    m_pImage = createImage(data, Width, Height);
    
    glGenTextures(1, &m_TextureID);
    
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)8.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glBindTexture(GL_TEXTURE_2D, 0);
	Width = m_pImage->width();
	Height = m_pImage->height();
    
    delete [] data;
    return true;
}

bool Texture::create( unsigned int width, unsigned int height, unsigned char* data)
{
    release();
    
	if (data)
		m_pImage = createImage(data, width, height);
	else
		m_pImage = NULL;
    
    glGenTextures(1, &m_TextureID);
    
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)16.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glBindTexture(GL_TEXTURE_2D, 0);
	Width = width;
	Height = height;
    
    return true;
}

bool Texture::create(unsigned int width, unsigned int height, GLint InternalFormat, GLint Format, GLint ComponentSize, GLint MinFilter, GLint MagFilter, GLint AddressMode, bool GenMipMaps)
{
	release();

	glGenTextures(1, &m_TextureID);

	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, Format, ComponentSize, NULL);
	if(GenMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, AddressMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, AddressMode);
	glBindTexture(GL_TEXTURE_2D, 0);

	Width = width;
	Height = height;

	if (glGetError() != 0)
		return false;

	return true;
}

bool Texture::create(const RGBImage& img)
{
    if( img.width()<= 0 || img.height() <=0)
        return false;
    
    release();
    
    const unsigned int w = img.width();
    const unsigned int h = img.height();
    unsigned char* data = new unsigned char[w*h*4];
    
    unsigned int k=0;
    for( unsigned int i=0; i<h; i++)
        for (unsigned int j = 0; j<w; j++)
        {
            Color c = img.getPixelColor(j, i);
            data[k++] = RGBImage::convertColorChannel(c.R);
            data[k++] = RGBImage::convertColorChannel(c.G);
            data[k++] = RGBImage::convertColorChannel(c.B);
            data[k++] = RGBImage::convertColorChannel(1.0f);
        }
    
    bool success = create(w, h, data);
    delete [] data;

	Width = w;
	Height = h;
    
    return success;
}


RGBImage* Texture::createImage( unsigned char* Data, unsigned int width, unsigned int height )
{
    // create CPU accessible image
    RGBImage* pImage = new RGBImage(width, height);
    assert(pImage);
    for( unsigned int i=0; i<height; i++)
		for (unsigned int j = 0; j<width; j++)
        {
            Color c( (float)*(Data)/255.0f, (float)*(Data+1)/255.0f, (float)*(Data+2)/255.0f);
            pImage->setPixelColor(j, i, c);
            Data+=4;
        }
    return pImage;
}

void Texture::activate(int slot) const
{
    if(m_TextureID==0 || slot < 0 || slot > 7 )
        return;
    
    CurrentTextureUnit = slot;

    glActiveTexture(GL_TEXTURE0 + CurrentTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::deactivate() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    if(CurrentTextureUnit>0)
        glActiveTexture(GL_TEXTURE0 + CurrentTextureUnit-1);
    CurrentTextureUnit=0;
    
}

const RGBImage* Texture::getRGBImage() const
{
    return m_pImage;
}
