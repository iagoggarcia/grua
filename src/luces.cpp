#include "luces.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void configurarFocoFrontalGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco)
{
    // Indicamos al shader si el foco frontal está encendido o apagado.
    glUniform1i(glGetUniformLocation(shaderProgram, "luz1Encendida"), luzEncendidaFoco ? 1 : 0);

    // Si está encendido, le damos un color amarillento intenso
    // si no, su color es negro y no aporta iluminación
    if (luzEncendidaFoco)
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor1"), 2.2f, 2.2f, 1.8f);
    else
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor1"), 0.0f, 0.0f, 0.0f);

    // Convertimos la dirección global de la grúa a radianes
    float rad = glm::radians(grua.direccion);

    // Calculamos los vectores locales de orientación de la grúa:
    // frente, derecha y arriba.
    glm::vec3 frente  = glm::normalize(glm::vec3(std::sin(rad), 0.0f, std::cos(rad)));
    glm::vec3 derecha = glm::normalize(glm::vec3(std::cos(rad), 0.0f, -std::sin(rad)));
    glm::vec3 arriba  = glm::vec3(0.0f, 1.0f, 0.0f);

    // Posición local del foco respecto a la cabina
    glm::vec3 focoLocal(
        0.0f,
        grua.cabina.posicion.y - 0.10f,
        grua.cabina.posicion.z + grua.cabina.escala.z / 2.0f + 0.16f
    );

    // Convertimos esa posición local a coordenadas globales del mundo usando la posición y orientación actuales de la grúa
    glm::vec3 lightPos = grua.posicion
        + derecha * focoLocal.x
        + arriba  * focoLocal.y
        + frente  * focoLocal.z;

    // El foco apunta hacia delante con una ligera inclinación hacia abajo
    glm::vec3 lightDir = glm::normalize(frente + glm::vec3(0.0f, -0.18f, 0.0f));

    // Enviamos al shader la posición y dirección del foco.
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos1"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightDir1"), lightDir.x, lightDir.y, lightDir.z);

    // Definimos la apertura interna y externa del cono de luz.
    glUniform1f(glGetUniformLocation(shaderProgram, "cutOff1"), glm::cos(glm::radians(12.5f)));
    glUniform1f(glGetUniformLocation(shaderProgram, "outerCutOff1"), glm::cos(glm::radians(18.0f)));
}

void configurarLuzBrazoGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaBrazo)
{
    // Indicamos al shader si la luz del brazo está encendida o apagada.
    glUniform1i(glGetUniformLocation(shaderProgram, "luz2Encendida"), luzEncendidaBrazo ? 1 : 0);

    // Si está encendida, le damos un color claro; si no, queda anulada.
    if (luzEncendidaBrazo)
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor2"), 1.7f, 1.7f, 1.5f);
    else
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor2"), 0.0f, 0.0f, 0.0f);

    // Construimos la transformación global de la grúa.
    glm::mat4 modeloGrua = glm::mat4(1.0f);
    modeloGrua = glm::translate(modeloGrua, grua.posicion);
    modeloGrua = glm::rotate(modeloGrua, glm::radians(grua.direccion), glm::vec3(0.0f, 1.0f, 0.0f));

    // Construimos la transformación de la articulación sin aplicar su escala, para no deformar la referencia usada por el brazo.
    glm::mat4 modeloArt = modeloGrua;
    modeloArt = modeloArt * glm::translate(glm::mat4(1.0f), grua.articulacion.posicion);
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Construimos la transformación del brazo sin aplicar aún su escala final.
    glm::mat4 modeloBrazo = modeloArt;
    modeloBrazo = glm::translate(modeloBrazo, grua.brazo.posicion);
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Definimos un punto en la punta del brazo en coordenadas locales.
    glm::vec4 puntaLocal(0.0f, 0.0f, grua.brazo.escala.z, 1.0f);

    // Transformamos ese punto a coordenadas globales para obtener
    // la posición real de la luz en el mundo.
    glm::vec3 lightPos = glm::vec3(modeloBrazo * puntaLocal);

    // La luz del brazo apunta verticalmente hacia el suelo.
    glm::vec3 lightDir = glm::vec3(0.0f, -1.0f, 0.0f);

    // Enviamos posición y dirección al shader.
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos2"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightDir2"), lightDir.x, lightDir.y, lightDir.z);

    // Definimos la apertura del cono de esta luz.
    glUniform1f(glGetUniformLocation(shaderProgram, "cutOff2"), glm::cos(glm::radians(20.0f)));
    glUniform1f(glGetUniformLocation(shaderProgram, "outerCutOff2"), glm::cos(glm::radians(28.0f)));
}

void configurarLucesGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco, bool luzEncendidaBrazo)
{
    // Configuramos por separado el foco frontal y la luz del brazo.
    configurarFocoFrontalGrua(grua, shaderProgram, luzEncendidaFoco);
    configurarLuzBrazoGrua(grua, shaderProgram, luzEncendidaBrazo);

    // Indicamos que, por defecto, lo que se dibuje a continuación
    // no debe tratarse como si fuera el propio foco visible.
    glUniform1i(glGetUniformLocation(shaderProgram, "esFoco"), 0);
}