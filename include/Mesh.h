#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

class Mesh
{
private:
    GLuint VAO, VBO;
public:
    Mesh();
    void createMesh(GLfloat *vertices, unsigned int numOfVertices);
    void renderMesh();
    void clearMesh();
    ~Mesh();
};

Mesh::Mesh()
{
    VAO = 0;
    VBO = 0;
}

void Mesh::createMesh(GLfloat *vertices, unsigned int numOfVertices)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numOfVertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::renderMesh()
{
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, GL_UNSIGNED_INT, 1);
}

void Mesh::clearMesh()
{
    if(VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if(VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

Mesh::~Mesh()
{
    clearMesh();
}

#endif