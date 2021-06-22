//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.
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
#include <stdio.h>
#include "Manager.h"
#include "FreeImage.h"

void PrintOpenGLVersion();


int main () {
    FreeImage_Initialise();
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit ()) {
		std::cout << (stderr, "ERROR: could not start GLFW3\n") << std::endl;
        return 1;
    }
    
#ifdef __APPLE__
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
	//TODO: Add recognition of user screen size and adjust the window size accordingly.
    const int WindowWidth = (int) (1800);
    const int WindowHeight = (int) (1150);
    
    GLFWwindow* window = glfwCreateWindow (WindowWidth, WindowHeight, "OpenGLOrbiterV0-0-1", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: can not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);
	

#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif // !__APPLE__

	PrintOpenGLVersion();
    {
		double lastTime = 0.0;
        Manager App(window);
        App.start();
        while (!glfwWindowShouldClose (window)) {
            // once per frame
			double now = glfwGetTime();
			double delta = now - lastTime;
			lastTime = now;
            glfwPollEvents();
            App.update(delta);
            App.draw();
            glfwSwapBuffers (window);
        }
        App.end();
    }
    
    glfwTerminate();
    return 0;
}


void PrintOpenGLVersion()
{
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
}
