//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing.
    //  Copyright © 2016 Philipp Lensing.
#include "GL\glew.h"
#include "GLFW\glfw3.h"

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
    
    const int WindowWidth = 1800;
    const int WindowHeight = 1350;
    
    GLFWwindow* window = glfwCreateWindow (WindowWidth, WindowHeight, "OpenGLOrbiterV0-0-1", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: can not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);
	
	//Next 2 lines probably unnecessary for Apple Users
	glewExperimental = GL_TRUE;
	glewInit();

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
            App.update((float)delta);
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
