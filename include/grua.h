#ifndef GRUA_H
#define GRUA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

//declaramos una estructura para la creación de piezas, y en ella introducimos campos para indicar su posición, poder aplicar la rotacion y escalarlas
struct Pieza {
    glm::vec3 posicion;
    glm::vec3 rotacion;
    glm::vec3 escala;
};

//luego construimos la grua a partir de esas piezas, las cuales vamos a mover como una unidad aplicando la misma matriz model
//como la grua va a moverse en el plano, es necesario añadir campos como dirección, velocidad y tuvimos que meter tambien 
//una velocidad de rotacion de las ruedas 
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
void dibujarGrua(const Grua& grua, GLuint shaderProgram, GLuint texturaGrua);
void dibujarEscena(const Grua& grua, GLuint shaderProgram, GLuint texturaSuelo, GLuint texturaArbusto, GLuint texturaGrua, GLuint texturaFondo, glm::vec3 camPos);
void actualizarGrua(Grua& grua, float deltaTime);

#endif