//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.
    //  Copyright © 2016 Philipp Lensing. All rights reserved.


#include "Camera.h"
#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera(GLFWwindow* pWin) : m_Position(0.0f,5.0f,5.0f), m_Target(0.0f,0.0f,0.0f), m_Up(0.0f,1.0f,0.0f), m_LastMouseX(-1), m_LastMouseY(-1), m_Panning(0,0,0), m_Zoom(0,0,0), m_Rotation(0,0,0), WindowWidth(640), WindowHeight(480), pWindow(pWin)
{
    if(pWindow)
        glfwGetWindowSize(pWindow, &WindowWidth, &WindowHeight);
    
    m_ViewMatrix.identity();
    m_ProjMatrix.perspective((float)M_PI*65.0f/180.0f, (float)WindowWidth/(float)WindowHeight, 0.045f, 1000.0f);
}

Vector Camera::position() const
{
    return m_Position + m_Panning + m_Zoom + m_Rotation;
}
Vector Camera::target() const
{
    return m_Target + m_Panning;
}
Vector Camera::up() const
{
    return m_Up;
}

void Camera::setPosition( const Vector& Pos)
{
    m_Position = Pos;
    m_Panning = m_Rotation = m_Zoom = Vector(0,0,0);


}
void Camera::setTarget( const Vector& Target)
{
    m_Target = Target;
    m_Panning = Vector(0,0,0);
}
void Camera::setUp( const Vector& Up)
{
    m_Up = Up;
}

void Camera::mouseInput( int x, int y, int Button, int State)
{

    if(State == GLFW_PRESS)
    {
        if(m_LastMouseX==-1) m_LastMouseX = x;
        if(m_LastMouseY==-1) m_LastMouseY = y;
        
        if( Button == GLFW_MOUSE_BUTTON_LEFT )
        {
            rotate((float)x, (float)y );
        }
        else if( Button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            pan( (float)(m_LastMouseX-x)*0.01f, (float)(m_LastMouseY-y)*0.01f );
        }
        else if( Button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            zoom( (float)(m_LastMouseY-y)*0.01f );
            
        }
    }
    else
    {
        m_Position += m_Panning + m_Zoom + m_Rotation;
        m_Target += m_Panning;
        m_Panning = Vector(0,0,0);
        m_Zoom = Vector(0,0,0);
        m_Rotation = Vector(0,0,0);
        m_LastMouseX = -1;
        m_LastMouseY = -1;

    }
}

void Camera::pan( float dx, float dy)
{
    // calculate panning-plane
    
    Vector aDir = m_Target-m_Position;
    aDir.normalize();
    Vector aRight = aDir.cross(m_Up);
    aRight.normalize();
    Vector aUp = aDir.cross(aRight);
    m_Panning = aRight * dx + aUp * dy;
}

void Camera::zoom( float dz)
{
    Vector aDir = m_Target-m_Position;
    float Dist = aDir.length();
    aDir.normalize();
  
    if( Dist-dz <= 1.0f)
    {
        m_Zoom = aDir * (Dist-1.0f);
        return;
    }
    
     m_Zoom = aDir * dz;
}

void Camera::rotate( float x, float y )
{
    Vector po = getVSpherePos((float) m_LastMouseX, (float)m_LastMouseY);
    Vector pn = getVSpherePos(x, y);
    
    if((po-pn).lengthSquared() < 0.0001f ){
        return;
    }
    
    float cosangle = po.dot(pn);
    if(cosangle>1.0f) {
    	cosangle=1.0f;
    }
    if(cosangle<-1.0f) {
    	cosangle=-1.0f;
    }
    
    const float angle = acosf(cosangle);
    Vector RotAxis = pn.cross(po);
    RotAxis.normalize();
    
    Vector Diff(0,0,(m_Position-m_Target).length());
    Vector RotDiff = rotateAxisAngle(Diff, RotAxis, angle);
    
    Vector cdir = m_Target-m_Position;
    cdir.normalize();
    Vector cup = m_Up;
    Vector cright = cdir.cross(cup);
    cright.normalize();
    cup = cright.cross(cdir);

    Vector RotDiffW;
    RotDiffW.X = cright.X * RotDiff.X + cup.X * RotDiff.Y +  -cdir.X * RotDiff.Z;
    RotDiffW.Y = cright.Y * RotDiff.X + cup.Y * RotDiff.Y +  -cdir.Y * RotDiff.Z;
    RotDiffW.Z = cright.Z * RotDiff.X + cup.Z * RotDiff.Y +  -cdir.Z * RotDiff.Z;
    m_Rotation = RotDiffW - (m_Position-m_Target);
}

Vector Camera::rotateAxisAngle( Vector v, Vector n, float a)
{
    float co = cos(a);
    float si = sin(a);
    Vector o = Vector();
    Vector mx( n.X*n.X*(1.0f-co)+co, n.X*n.Y*(1.0f-co)-n.Z*si,n.X*n.Z*(1.0f-co)+n.Y*si );
    Vector my( n.X*n.Y*(1.0f-co)+n.Z*si, n.Y*n.Y*(1.0f-co)+co, n.Y*n.Z*(1.0f-co)-n.X*si );
    Vector mz( n.X*n.Z*(1.0f-co)-n.Y*si, n.Z*n.Y*(1.0f-co)+n.X*si, n.Z*n.Z*(1.0f-co)+co);
    o.X = mx.dot(v);
    o.Y = my.dot(v);
    o.Z = mz.dot(v);
    return o;
}

const Matrix& Camera::getViewMatrix() const
{
    return m_ViewMatrix;
}

const Matrix& Camera::getProjectionMatrix() const
{
    return m_ProjMatrix;
}

Vector Camera::getVSpherePos(float x, float y)
{
    Vector p( 1.0f*x/(float)WindowWidth*2.0f - 1.0f, 1.0f*y/(float)WindowHeight*2.0f -1.0f, 0);
    p.Y = -p.Y;
    float sqrLen = p.lengthSquared();

    if( sqrLen <= 1.0f) {
        p.Z = sqrt( 1- sqrLen);
    }
    else{
        p.normalize();
    }
    return p;
    
}

void Camera::updateMouseInput()
{
    double xpos, ypos;
    glfwGetCursorPos(pWindow, &xpos, &ypos);
    if( glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        mouseInput((int)xpos, (int)ypos, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    else if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        mouseInput((int)xpos, (int)ypos, GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);
    else if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        mouseInput((int)xpos, (int)ypos, GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS);
    else
        mouseInput((int)xpos, (int)ypos, GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE);
}

void Camera::update()
{
    updateMouseInput();
    Vector Pos = position(); //m_Position + m_Panning + m_Zoom + m_Rotation;
    Vector Target = target(); //m_Target + m_Panning;
    m_ViewMatrix.lookAt(Target, m_Up, Pos);
}