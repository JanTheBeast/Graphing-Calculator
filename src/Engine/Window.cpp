#include "Window.h"
#include <stdlib.h>

Window::Window()
{
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    m_WindowID = glfwCreateWindow(480, 480, "Hello World", NULL, NULL);
    if (!m_WindowID)
    {
        glfwTerminate();
        exit(-1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_WindowID);
    glfwSwapInterval(1);


}

Window::~Window()
{
    glfwTerminate();
}