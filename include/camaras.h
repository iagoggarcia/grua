#ifndef CAMARAS_H
#define CAMARAS_H

#include <glm/glm.hpp>
#include "grua.h"

struct DatosCamara {
    glm::vec3 frente;
    glm::vec3 camPos;
    glm::vec3 camTarget;
    glm::mat4 view;
};

DatosCamara calcularCamara(const Grua& grua, int modoCamara);

#endif