#include "cube_mesh.h"

void CubeMesh::create() {
    float vertices[] = {
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };

    unsigned int indices[] = {
        // front
        0, 1, 2,
        0, 2, 3,
        // back
        5, 4, 7,
        5, 7, 6,
        // left
        4, 0, 3,
        4, 3, 7,
        // right
        1, 5, 6,
        1, 6, 2,
        // top
        3, 2, 6,
        3, 6, 7,
        // bottom
        4, 5, 1,
        4, 1, 0
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // upload vertices
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        &vertices[0],
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)(0)
    );
    glEnableVertexAttribArray(0);

    // upload indices
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        &indices[0],
        GL_STATIC_DRAW
    );
}

void CubeMesh::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

GLuint CubeMesh::getVAO() const {
	return VAO;
}