/*
*   Coded by: Peter S - https://github.com/peterspbr
*   This source code is distributed under the MIT license. Please take a look at the LICENSE.MD file.
*/

// Standard libraries
#include <iostream>
#include <string.h>
#include <vector>

// GL libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Header files
#include "../include/Mesh.h"
#include "../include/Tex.h"
#include "../include/Shader.h"

using namespace std;

// Vectors
vector<Mesh*> meshList;
vector<Tex*> texList;
vector<Shader*> shaderList;

typedef struct
{
    int w, a, s, d, q, e;
} buttonKeys; buttonKeys keys;

// Function callbacks
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mod);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void setupGravity(float mass, float gravityForce, float gravityAccel, bool isEnabled);

// Integer variables
GLuint uniformModel, uniformProjection, uniformCamera, uniformTexture;
const int windowWidth = 800, windowHeight = 600;
const int xChunkSize = 10, zChunkSize = 10;
int frames;

// Double precision variables
double lastTime = 0.0;
float cameraPosition_X = -10, cameraPosition_Y = -2, cameraPosition_Z = -10; // Initial camera position
float cameraRotation_X, cameraRotation_Y;
float mouseSensibility_X = 0.1f, mouseSensibility_Y = 0.1f;

// Boolean variables
bool cursorLocked = true;

// Shaders
// Vertex Shader
const char* vShader = "                                                     \n\
#version 330 core                                                           \n\
                                                                            \n\
layout (location = 0) in vec3 pos;                                          \n\
layout (location = 1) in vec3 mTexCord;                                     \n\
                                                                            \n\
out vec2 TexCord;                                                           \n\
                                                                            \n\
uniform mat4 model;                                                         \n\
uniform mat4 view;                                                          \n\
uniform mat4 perspective;                                                   \n\
                                                                            \n\
void main() {                                                               \n\
    gl_Position = perspective * view * model * vec4(pos, 1.0);              \n\
    TexCord = vec2(mTexCord.x, mTexCord.y);                                 \n\
}";

// Fragment shader
const char* fShader = "                                                     \n\
#version 330 core                                                           \n\
                                                                            \n\
out vec4 color;                                                             \n\
                                                                            \n\
in vec2 TexCord;                                                            \n\
                                                                            \n\
uniform sampler2D textmod1;                                                 \n\
                                                                            \n\
void main(){                                                                \n\
    color = texture(textmod1, TexCord);                                     \n\
}";

// Create object
void createCube()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

         1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,

        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f
    };

    Mesh *obj1 = new Mesh();
	obj1->createMesh(vertices, sizeof(vertices));
	meshList.push_back(obj1);

    Tex *tex1 = new Tex();
    tex1->createTexture();
    texList.push_back(tex1);
}

// Create the shader
void createShader()
{
    Shader *shader1 = new Shader();
    shader1->getFromString(vShader, fShader);
    shaderList.push_back(shader1);
}

// Generate the terrain
void generateTerrain()
{
    for(int z = 0; z < zChunkSize; z++)
        {
            for(int x = 0; x < xChunkSize; x++)
            {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(x * 2, 0.0f, z * 2));
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                meshList[0]->renderMesh();
            }
        }
}

// Main function
int main()
{
    // Initialization process
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

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    createCube();
    createShader();

    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if(keys.w == 1){cameraPosition_Z += 0.4;}
        if(keys.a == 1){cameraPosition_X += 0.4;}
        if(keys.s == 1){cameraPosition_Z -= 0.4;}
        if(keys.d == 1){cameraPosition_X -= 0.4;}
        if(keys.q == 1){cameraPosition_Y += 0.4;}
        if(keys.e == 1){cameraPosition_Y -= 0.4;}

        // Get framerate values
        double time = glfwGetTime();
        double delta = time - lastTime;
        frames++;

        if(delta >= 1.0)
        {
            double fps = double(frames) / delta;
            cout << "Fps: " << fps << endl;
            
            frames = 0;
            lastTime = time;
        }

        setupGravity(1.0f, 0, 9.81f, false);

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0]->useShader();
        uniformModel = shaderList[0]->getModelLocation();
        uniformProjection = shaderList[0]->getProjectionLocation();
        uniformCamera = shaderList[0]->getViewLocation();
        uniformTexture = shaderList[0]->getTexLocation();

        texList[0]->renderTexture();
        
        glm::mat4 view(1.0f);
        glm::mat4 persp(1.0f);

        persp = glm::perspective(glm::radians(60.0f), float(widthBuffer) / float(heightBuffer), 0.01f, 3000.0f);

        view = glm::rotate(view, glm::radians(cameraRotation_X), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(cameraRotation_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(cameraPosition_X, cameraPosition_Y, cameraPosition_Z));

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(persp));
        glUniformMatrix4fv(uniformCamera, 1, GL_FALSE, glm::value_ptr(view));

        if(cursorLocked)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        generateTerrain();

        glfwSwapBuffers(window);
        glfwSwapInterval(1);
    }

    return 0;
}

// Handle keys
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mod)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_W:
                keys.w = 1;
                break;
            case GLFW_KEY_A:
                keys.a = 1;
                break;
            case GLFW_KEY_S:
                keys.s = 1;
                break;
            case GLFW_KEY_D:
                keys.d = 1;
                break;
            case GLFW_KEY_Q:
                keys.q = 1;
                break;
            case GLFW_KEY_E:
                keys.e = 1;
                break;
            case GLFW_KEY_ESCAPE:
                cursorLocked = false;
                break;
        }
    }

    else if(action == GLFW_RELEASE || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_W:
                keys.w = 0;
                break;
            case GLFW_KEY_A:
                keys.a = 0;
                break;
            case GLFW_KEY_S:
                keys.s = 0;
                break;
            case GLFW_KEY_D:
                keys.d = 0;
                break;
            case GLFW_KEY_Q:
                keys.q = 0;
                break;
            case GLFW_KEY_E:
                keys.e = 0;
                break;
        }
    }
}

// Handle mouse input
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    cameraRotation_Y = xpos * mouseSensibility_Y;
    cameraRotation_X = ypos * mouseSensibility_X;
}

// Setup gravity
void setupGravity(float mass, float gravityForce, float gravityAccel, bool isEnabled)
{
    gravityForce = mass * gravityAccel;
    
    if(isEnabled)
    {
        cameraPosition_Y += gravityForce;
    }
}
