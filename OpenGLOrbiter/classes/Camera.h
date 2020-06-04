//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.



#ifndef Camera_hpp
#define Camera_hpp

#include <iostream>
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#else
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#endif
#endif

#include "vector.h"
#include "matrix.h"

class BaseCamera
{
public:
    virtual void update() = 0;
    virtual const Matrix& getViewMatrix() const  = 0;
    virtual const Matrix& getProjectionMatrix() const  = 0;
    virtual Vector position() const  = 0;
    virtual ~BaseCamera() {};
};

class Camera : public BaseCamera
{
public:
    Camera(GLFWwindow* pWin);
    virtual ~Camera() {};
    
    virtual Vector position() const;
    Vector target() const;
    Vector up() const;
    
    void setPosition( const Vector& Pos);
    void setTarget( const Vector& Target);
    void setUp( const Vector& Up);

    void mouseInput(int x, int y, int Button, int State);
    
    virtual void update();
    virtual const Matrix& getViewMatrix() const;
    virtual const Matrix& getProjectionMatrix() const;
private:
    void updateMouseInput();
    void pan( float dx, float dy);
    void zoom( float dz);
    void rotate( float x, float y );
    Vector getVSpherePos( float x, float y);
    Vector rotateAxisAngle( Vector v, Vector n, float a);
    
    Matrix m_ViewMatrix;
    Matrix m_ProjMatrix;
    Vector m_Position;
    Vector m_Target;
    Vector m_Up;
    Vector m_Panning;
    Vector m_Zoom;
    Vector m_Rotation;
    int m_LastMouseX;
    int m_LastMouseY;
    int WindowWidth;
    int WindowHeight;
    GLFWwindow* pWindow;
};


#endif /* Camera_hpp */
