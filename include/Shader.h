#ifndef SHADER_H
#define SHADER_h

#include <iostream>
#include <string.h>

#include <GL/glew.h>

using namespace std;

class Shader
{
private:
    GLuint shaderID;
    GLuint uniformModel, uniformTexture, uniformCamera, uniformProjection;
    void addShader(GLuint ID, const char* shaderCode, GLenum shaderType);
    void compileShaders(const char* vertexShader, const char* fragmentShader);
public:
    Shader();
    void getFromString(const char* vertexShader, const char* fragmentShader);
    void clearShader();
    void useShader();
    GLuint getModelLocation();
    GLuint getProjectionLocation();
    GLuint getViewLocation();
    GLuint getTexLocation();
    ~Shader();
};

Shader::Shader()
{
    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
    uniformCamera = 0;
}

void Shader::getFromString(const char* vertexShader, const char* fragmentShader)
{
    Shader::compileShaders(vertexShader, fragmentShader);
}

void Shader::addShader(GLuint ID, const char* shaderCode, GLenum shaderType)
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

void Shader::compileShaders(const char* vertexShader, const char* fragmentShader)
{
	shaderID = glCreateProgram(); // Creates the shader program.

	// Check for any errors.
	if(!shaderID)
	{
		cout << "Failed to create shader" << endl;
		return;
	}
	
	// Add the shader to VAO and VBO.
	addShader(shaderID, vertexShader, GL_VERTEX_SHADER);
	addShader(shaderID, fragmentShader, GL_FRAGMENT_SHADER);

    // Check for errors.
	GLint result = 0;
	GLchar eLog[1024] = {0};

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);

	if(!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		cout << "Failed to link program: " << eLog << endl;
		return;
	}

	// Validate the shader program and check for any errors.
	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);

	if(!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		cout << "Failed to validate program: " << eLog << endl;
	}
}

void Shader::useShader()
{
    glUseProgram(shaderID);
}

GLuint Shader::getModelLocation()
{
    return uniformModel = glGetUniformLocation(shaderID, "model");
}

GLuint Shader::getProjectionLocation()
{
    return uniformProjection = glGetUniformLocation(shaderID, "perspective");
}

GLuint Shader::getViewLocation()
{
    return uniformCamera = glGetUniformLocation(shaderID, "view");
}

GLuint Shader::getTexLocation()
{
    return uniformTexture = glGetUniformLocation(shaderID, "textmod1");
}

void Shader::clearShader()
{
    if(shaderID != 0)
    {
        glDeleteShader(shaderID);
        shaderID = 0;
    }

    uniformModel = 0;
    uniformProjection = 0;
    uniformCamera = 0;
    uniformTexture = 0;
}

Shader::~Shader()
{
    clearShader();
}

#endif