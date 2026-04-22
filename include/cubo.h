#ifndef CUBO_H
#define CUBO_H

#include <glad/glad.h>

void crearSuelo(GLuint shaderProgram, GLuint texturaSuelo);

void crearCubo();

void liberarCubo();

extern float vertices[];
extern const unsigned int verticesSize;

#endif