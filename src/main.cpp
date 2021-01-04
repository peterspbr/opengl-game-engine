#define windowWidth 800
#define windowHeight 600

#include <iostream>
#include <string.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/Mesh.h"
#include "../include/Tex.h"
#include "../include/Shader.h"

using namespace std;

vector<Mesh*> meshList;
vector<Tex*> texList;
vector<Shader*> shaderList;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mod);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void setupGravity(float mass, float gravityForce, float gravityAccel, bool isEnabled);

GLuint uniformModel, uniformProjection, uniformCamera, uniformTexture;

const int xChunkSize = 10, zChunkSize = 10;

int cameraPosition_X = -10, cameraPosition_Y = -2, cameraPosition_Z = -10; // Initial camera position
// Collision
int collisionPointForward = cameraPosition_Z * 2;
int collisionPointDownward = -cameraPosition_Y * 2;
int frames;

double lastTime = 0.0;
float cameraRotation_X, cameraRotation_Y;
float mouseSensibility_X = 0.1f, mouseSensibility_Y = 0.1f;

bool cursorLocked = true;

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

void createShader()
{
    Shader *shader1 = new Shader();
    shader1->getFromString(vShader, fShader);
    shaderList.push_back(shader1);
}

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

int main()
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

    if(glewInit() != GLEW_OK)
    {
        cout << "Cannot initiate GLEW! Exiting..." << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, widthBuffer, heightBuffer);

    createCube();
    createShader();

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, mouse_callback);

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

        //cout << "Current camera position: " << "X: " << cameraPosition_X << " Y: " << cameraPosition_Y << " Z: " << cameraPosition_Z << endl;

        if(cursorLocked)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        glm::mat4 view(1.0f);
        glm::mat4 persp(1.0f);

        persp = glm::perspective(glm::radians(60.0f), float(widthBuffer) / float(heightBuffer), 0.01f, 3000.0f);

        view = glm::rotate(view, glm::radians(cameraRotation_X), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(cameraRotation_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(cameraPosition_X, cameraPosition_Y, cameraPosition_Z));

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(persp));
        glUniformMatrix4fv(uniformCamera, 1, GL_FALSE, glm::value_ptr(view));

        generateTerrain();

        glfwSwapBuffers(window);
        glfwSwapInterval(1);
    }

    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mod)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_W:
                cameraPosition_Z += 1.0f;
                break;
            case GLFW_KEY_A:
                cameraPosition_X += 1.0f;
                break;
            case GLFW_KEY_S:
                cameraPosition_Z -= 1.0f;
                break;
            case GLFW_KEY_D:
                cameraPosition_X -= 1.0f;
                break;
            case GLFW_KEY_ESCAPE:
                cursorLocked = false;
                break;
        }
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    cameraRotation_Y = xpos * mouseSensibility_X;
    cameraRotation_X = ypos * mouseSensibility_Y;
}

void setupGravity(float mass, float gravityForce, float gravityAccel, bool isEnabled)
{
    gravityForce = mass * gravityAccel;
    
    if(isEnabled)
    {
        cameraPosition_Y += gravityForce;
    }
}
