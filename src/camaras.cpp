#include "camaras.h"
#include <glm/gtc/matrix_transform.hpp>

DatosCamara calcularCamara(const Grua& grua, int modoCamara) {
    DatosCamara datos;

    float rad = glm::radians(grua.direccion);
    datos.frente = glm::vec3(sin(rad), 0.0f, cos(rad));

    if (modoCamara == 1) {
        datos.camPos = grua.posicion - datos.frente * 2.0f + glm::vec3(0.0f, 3.5f, 0.0f);
        datos.camTarget = grua.posicion + datos.frente * 12.0f + glm::vec3(0.0f, 3.0f, 0.0f);
    }
    else if (modoCamara == 2) {
        datos.camPos = grua.posicion - datos.frente * 24.0f + glm::vec3(0.0f, 8.0f, 0.0f);
        datos.camTarget = grua.posicion + glm::vec3(0.0f, 2.0f, 0.0f);
    }
    else {
        datos.camPos = glm::vec3(50.0f, 20.0f, 8.0f);
        datos.camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    datos.view = glm::lookAt(
        datos.camPos,
        datos.camTarget,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return datos;
}