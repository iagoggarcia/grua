#ifndef TEXTURAS_H
#define TEXTURAS_H

#include <glad/glad.h>

// Carga una imagen desde disco y crea a partir de ella una textura 2D de OpenGL.
GLuint cargarTextura(const char* ruta);

#endif