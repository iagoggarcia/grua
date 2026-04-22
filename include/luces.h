#ifndef LUCES_H
#define LUCES_H

#include <glad/glad.h>
#include "grua.h"

void configurarLucesGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco, bool luzEncendidaBrazo);
void configurarFocoFrontalGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco);
void configurarLuzBrazoGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaBrazo);

#endif