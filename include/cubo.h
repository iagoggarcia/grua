#ifndef CUBO_H
#define CUBO_H

#include <glad/glad.h>

// Dibuja el suelo reutilizando la geometría del cubo y aplicándole la textura correspondiente.
void crearSuelo(GLuint shaderProgram, GLuint texturaSuelo);

// Crea y configura la geometría base del cubo (VAO y VBO).
void crearCubo();

// Libera la memoria asociada a la geometría del cubo.
void liberarCubo();

// Exponemos el array de vértices del cubo y su tamaño total
// para poder reutilizarlos desde otros archivos si hace falta.
extern float vertices[];
extern const unsigned int verticesSize;

#endif