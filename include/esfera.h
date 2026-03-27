#pragma warning(disable:4996)
#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <math.h> 
#include <iostream>

extern const unsigned int verticesEsferaSize;
extern float vertices_esfera[];

void crearEsfera(GLuint& VAO_esfera, GLuint& VBO_esfera);
