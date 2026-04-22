#ifndef CUBO_H
#define CUBO_H

#include <glad/glad.h>

void crearSuelo(GLuint shaderProgram, unsigned int VAO_cubo, GLuint texturaSuelo);

void crearCubo(GLuint& VAO_cubo, GLuint& VBO_cubo);

extern float vertices[];
extern const unsigned int verticesSize;

#endif