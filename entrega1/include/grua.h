#ifndef GRUA_H
#define GRUA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Pieza {
    glm::vec3 posicion;
    glm::vec3 rotacion;
    glm::vec3 escala;
};

struct Grua {
    Pieza base;
    Pieza cabina;
    Pieza articulacion;
    Pieza brazo;

    glm::vec3 posicion;
    float direccion;
    float velocidad;
};

void inicializarGrua(Grua& grua);
void dibujarGrua(const Grua& grua, GLuint shaderProgram);
static void dibujarRueda(const glm::vec3& posicionLocal, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b);

#endif