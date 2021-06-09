#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

class Window
{
private:
    const int windowWidth = 800, windowHeight = 600;
public:
    Window();
    void initialiseWindow();
    void gameLoop(GLFWwindow* window);
    int createWindow();
    ~Window();
};

Window::Window(/* args */)
{
}

void gameLoop(GLFWwindow* window)
{
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwSwapInterval(1);
    }
}

int createWindow()
{
    if(!glfwInit())
    {
        cout << "Cannot initiate GLFW! Exiting..." << endl;
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Not another Minecraft clone", NULL, NULL);

    if(!window)
    {
        cout << "Cannot create window! Exiting..." << endl;
        glfwTerminate();
        return 1;
    }

    int widthBuffer, heightBuffer;
    glfwGetFramebufferSize(window, &widthBuffer, &heightBuffer);

    glfwMakeContextCurrent(window);

    glEnable(GL_DEPTH_TEST);

    glewExperimental = GLFW_TRUE;

    GLenum error = glewInit();
    if(error != GLEW_OK)
    {
        cout << "Cannot initiate GLEW! Exiting..." << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, widthBuffer, heightBuffer);

    gameLoop(window);
}

void initialiseWindow()
{
    createWindow();
}

Window::~Window()
{
}
