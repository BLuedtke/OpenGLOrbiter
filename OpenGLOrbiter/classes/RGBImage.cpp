//  Author: Bernhard Luedtke
//

#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>
#include "stdio.h"
#include "rgbimage.h"
#include "color.h"
#include "assert.h"


RGBImage::RGBImage( unsigned int Width, unsigned int Height)
{
    this->m_Width = Width;
    this->m_Height = Height;
    this->matrix.resize(this->m_Width, std::vector<Color>(this->m_Height, Color()));
    //Color* m_Image nicht verwendet, da im vgl. zu Realisierung mit Vektor deutlich höherer Aufwand
    //Wäre aber auch möglich, bsp
    //https://stackoverflow.com/questions/3904224/declaring-a-pointer-to-multidimensional-array-and-allocating-the-array/3904306#3904306
    //https://stackoverflow.com/questions/936687/how-do-i-declare-a-2d-array-in-c-using-new
}

RGBImage::~RGBImage()
{
    //Destruktor
}

void RGBImage::setPixelColor( unsigned int x, unsigned int y, const Color& c)
{
    if(x < width() && y < height()){
        this->matrix[x][y] = c;
    }
}

const Color& RGBImage::getPixelColor( unsigned int x, unsigned int y) const
{
    if(x < width() && y < height()){
        const Color &ref = this->matrix[x][y];
        return ref;
    }
    return Color(); //ist das so in Ordnung?
}

unsigned int RGBImage::width() const
{
    return this->m_Width;
}
unsigned int RGBImage::height() const
{
    return this->m_Height;
}

unsigned char RGBImage::convertColorChannel( float v)
{
    if(v < 0.0f){
        return 0;
    } else if( v > 1.0f){
        return 255;
    } else {
        return (unsigned char)(v * 255.999f);
    }
}

unsigned char file[14] = {
    'B','M',        //MS-Konvention
    0,0,0,0,        //Groesse in Bytes
    0,0,            //Muss 0 sein
    0,0,            // ""
    40+14,0,0,0     //Offset Beginn -> BitmapData;
};
unsigned char info[40] = {
    40,0,0,0,       // info-header-groesse
    0,0,0,0,        // breite in pixeln
    0,0,0,0,        // hoehe in pixeln; wenn Wert negativ: top-down; wenn positiv: bottom-up
    1,0,            // number color planes des Zielgeraets.
    24,0,           // bits per pixel (3 x 8)
    0,0,0,0,        // keine Kompression
    0,0,0,0,        // image bits size. Wenn Kompression = 0, dann darf hier null stehen
    0x13,0x0B,0,0,  // horizontale Aufloesung in pixel / m
    0x13,0x0B,0,0,  // vertikale Aufloesung (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
    0,0,0,0,        // Anzahl Farben in Pallete, wenn gleich 0, mithilfe von (bits per pixel) berechnet
    0,0,0,0,        // Anzahl wichtiger Farben, wenn gleich 0, sind alle Farben "wichtig"
};



bool RGBImage::saveToDisk(const char* Filename)
{
    FILE* picStream;
    int w = width();    //der Einfachheit halber hier lokal gespeichert
    int h = height();   // orig
    //int h = -height();   // test
    
    int padSize = (4 - 3 * width() % 4) % 4;                //Berechnung des benoetigten Paddings (Breite muss % 4 == 0 sein)
    int sizeData = width()*height()*3 + height() * padSize; //Groesse der Pixeldaten
    int sizeAll = sizeData + sizeof(file) + sizeof(info);   //Groesse Daten Pixel + Metadaten
    
    
    //Shifts, -> little Endian
    //FILEHEADER
    file[2] = (unsigned char)( sizeAll    );
    file[3] = (unsigned char)( sizeAll>> 8);
    file[4] = (unsigned char)( sizeAll>>16);
    file[5] = (unsigned char)( sizeAll>>24);
    //FILEHEADER BEARBEITUNG ENDE
    
    //BITMAPINFOHEADER
    //Breite
    info[4] = (unsigned char)(w    );
    info[5] = (unsigned char)(w>> 8);
    info[6] = (unsigned char)(w>>16);
    info[7] = (unsigned char)(w>>24);
    
    //hoehe
    //h = -h; //test
    info[8]  = (unsigned char)(h    );
    info[9]  = (unsigned char)(h>> 8);
    info[10] = (unsigned char)(h>>16);
    info[11] = (unsigned char)(h>>24);
    //h = -h; //test
    //Mit Test-Zeilen zwischen Bottom-Up und Top-Down wechseln
    
    //BildGroesse
    info[20] = (unsigned char)(sizeData    );
    info[21] = (unsigned char)(sizeData>> 8);
    info[22] = (unsigned char)(sizeData>>16);
    info[23] = (unsigned char)(sizeData>>24);
    //BEARBEITUNG BMPINFOHEADER ENDE
    fopen_s(&picStream,Filename, "w");
    
    if(picStream != NULL){
        fwrite((char*)file, sizeof(file), 1, picStream);   //fileHeader Schreiben
        fwrite((char*)info, sizeof(info), 1, picStream);   //BitmapInfoHeader Schreiben
        
        unsigned char pixel[3];
        unsigned char pad[3] = {0,0,0};     //Schwarzer Rand fuer das Padding
        
        for ( int y=h-1; y>=0; y-- )        //Hoehe/Zeilen -> umgekehrt, da BMP ueberkopf gespeichert (??)
        {
            for ( int x=0; x<w; x++ )       //Breite/Spalten
            {
                pixel[0] = convertColorChannel(this->matrix[x][y].B);       //Blau
                pixel[1] = convertColorChannel(this->matrix[x][y].G);       //Gruen
                pixel[2] = convertColorChannel(this->matrix[x][y].R);       //Rot
                fwrite((char*) pixel, sizeof(pixel), 1, picStream);         //Pixel schreiben
            }
            fwrite((char*)pad, sizeof(pad), padSize, picStream);            //Das noetige Padding in der Breite schreiben
        }
        fclose(picStream);          //Datei Schliessen
        
    } else {
        std::cerr << "Oeffnen der Datei gescheitert" << std::endl;
        return false;
    }
	return true;
}



//Hauptsaechliche Orientierung: https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries Antwort von Cullen Fluffy Jennings
//https://stackoverflow.com/questions/18838553/c-how-to-create-a-bitmap-file
//https://en.wikipedia.org/wiki/User:Evercat/Buddhabrot.c
//https://web.archive.org/web/20080912171714/http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html

/*
void RGBImage::bmpHeader(FILE* pFile){
    if(width() % 4 != 0){
        std::cerr << "Breite muss durch 4 teilbar sein!" << std::endl;
        exit(1);
    }
    
}*/
