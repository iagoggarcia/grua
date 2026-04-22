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
    float giroruedas;
};

void inicializarGrua(Grua& grua);
void dibujarGrua(const Grua& grua, GLuint shaderProgram);
void dibujarEscena(const Grua& grua, GLuint shaderProgram, GLuint texturaSuelo, GLuint texturaArbusto, GLuint texturaFondo, glm::vec3 camPos);
void actualizarGrua(Grua& grua, float deltaTime);

#endif