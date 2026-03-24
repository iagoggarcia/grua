#ifndef GRUA_H
#define GRUA_H

#include <glad.h>
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

#endif