#ifndef LUCES_H
#define LUCES_H

#include <glad/glad.h>
#include "grua.h"

// Configura en el shader todas las luces asociadas a la grúa.
void configurarLucesGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco, bool luzEncendidaBrazo);

// Configura el foco frontal de la grúa, teniendo en cuenta
// su posición, dirección y si está encendido o apagado.
void configurarFocoFrontalGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco);

// Configura la luz situada en el brazo de la grúa,
// usando también su estado de encendido o apagado.
void configurarLuzBrazoGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaBrazo);

#endif