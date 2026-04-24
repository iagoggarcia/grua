#ifndef CAMARAS_H
#define CAMARAS_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "grua.h"

// Estructura que agrupa los datos necesarios para definir una cámara: dirección frontal, posición, punto al que mira y matriz de vista.
struct DatosCamara {
    glm::vec3 frente;
    glm::vec3 camPos;
    glm::vec3 camTarget;
    glm::mat4 view;
};

// Calcula los parámetros de la cámara según el modo seleccionado y el estado actual de la grúa.
DatosCamara calcularCamara(const Grua& grua, int modoCamara);

// Envía al shader los datos de la cámara necesarios para renderizar: matriz de vista, matriz de proyección y posición de la cámara.
void configurarCamaraShader(const DatosCamara& camara, GLuint shaderProgram, const glm::mat4& projection);

#endif