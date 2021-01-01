#define windowWidth 800
#define windowHeight 600

#include <iostream>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mod);
void mouse_callback(GLFWwindow *window, double xpos, double zpos);

GLuint VAO, VBO, shader;
GLuint uniformModel, camera, projection;

int cameraPosition_X = 0, cameraPosition_Y = 0, cameraPosition_Z = -5;

float cameraRotation_X, cameraRotation_Y;
float mouseSensibility_X = 0.1f, mouseSensibility_Y = 0.1f;

bool cursorLocked = true;

const char* vShader = "                                                     \n\
#version 330 core                                                           \n\
                                                                            \n\
layout (location = 0) in vec3 pos;                                          \n\
                                                                            \n\
uniform mat4 model;                                                         \n\
uniform mat4 view;                                                          \n\
uniform mat4 perspective;                                                   \n\
                                                                            \n\
void main() {                                                               \n\
    gl_Position = perspective * view * model * vec4(pos, 1.0);              \n\
}";

const char* fShader = "                                                     \n\
#version 330 core                                                           \n\
                                                                            \n\
out vec4 color;                                                             \n\
                                                                            \n\
void main(){                                                                \n\
    color = vec4(1.0f, 1.0f, 1.0f);                                         \n\
}";

void createTriangle()
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

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void addShader(GLuint ID, const char* shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLenght[1];
    codeLenght[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLenght);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = {0};

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

    if(!result)
    {
        glGetShaderInfoLog(theShader, 1024, NULL, eLog);
        cout << "Cannot compile the shader: " << eLog << endl;
        return;
    }

    glAttachShader(ID, theShader);
}

void compileShaders()
{
	shader = glCreateProgram(); // Creates the shader program.

	// Check for any errors.
	if(!shader)
	{
		cout << "Failed to create shader" << endl;
		return;
	}
	
	// Add the shader to VAO and VBO.
	addShader(shader, vShader, GL_VERTEX_SHADER);
	addShader(shader, fShader, GL_FRAGMENT_SHADER);

    // Check for errors.
	GLint result = 0;
	GLchar eLog[1024] = {0};

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cout << "Failed to link program: " << eLog << endl;
		return;
	}

	// Validate the shader program and check for any errors.
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cout << "Failed to validate program: " << eLog << endl;
	}

    uniformModel =   glGetUniformLocation(shader, "model");
    camera       =   glGetUniformLocation(shader, "view");
    projection   =   glGetUniformLocation(shader, "perspective");
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

    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Hello, triangle", NULL, NULL);

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

    createTriangle();
    compileShaders();

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, mouse_callback);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        cout << "Current camera position: " << "X: " << cameraPosition_X << " Y: " << cameraPosition_Y << " Z: " << cameraPosition_Z << endl;

        if(cursorLocked)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        glm::mat4 model(1.0f);
        glm::mat4 view(1.0);
        glm::mat4 persp(1.0f);

        persp = glm::perspective(glm::radians(60.0f), float(windowWidth) / float(windowHeight), 0.01f, 3000.0f);

        view = glm::rotate(view, glm::radians(cameraRotation_X), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(cameraRotation_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(cameraPosition_X, cameraPosition_Y, cameraPosition_Z));

		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(camera, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(persp));

        glBindVertexArray(VAO);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode view.
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
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
            case GLFW_KEY_Q:
                cameraPosition_Y += 1.0f;
                break;
            case GLFW_KEY_E:
                cameraPosition_Y -= 1.0f;
                break;
            case GLFW_KEY_ESCAPE:
                cursorLocked = false;
                break;
        }
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double zpos)
{
    cameraRotation_Y = xpos * mouseSensibility_Y;
    cameraRotation_X = zpos * mouseSensibility_X;
}