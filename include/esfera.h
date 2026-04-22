#pragma warning(disable:4996)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <math.h> 
#include <iostream>

extern const unsigned int verticesEsferaSize;
extern float vertices_esfera[];

void crearEsfera();
void crearFondo(GLuint texturaFondo, glm::vec3 posicionCamara, GLuint shaderProgram);
void liberarEsfera();
