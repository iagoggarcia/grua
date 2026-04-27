#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "grua.h"
#include "input.h"

struct RecursosRender {
    GLuint shaderProgram;
    GLuint texturaGrua;
    GLuint texturaSuelo;
    GLuint texturaArbusto;
    GLuint texturaFondo;
};

GLFWwindow* inicializarVentana(unsigned int ancho, unsigned int alto, const char* titulo);
void openGlInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
glm::mat4 obtenerProjection();
void cargarRecursos(RecursosRender& recursos);
void renderizarFrame(const Grua& grua, const EstadoEntrada& estado, const RecursosRender& recursos);
void liberar(const RecursosRender& recursos);

#endif
