#include "luces.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void configurarFocoFrontalGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco)
{
    glUniform1i(glGetUniformLocation(shaderProgram, "luz1Encendida"), luzEncendidaFoco ? 1 : 0);

    if (luzEncendidaFoco)
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor1"), 2.2f, 2.2f, 1.8f);
    else
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor1"), 0.0f, 0.0f, 0.0f);

    float rad = glm::radians(grua.direccion);

    glm::vec3 frente  = glm::normalize(glm::vec3(std::sin(rad), 0.0f, std::cos(rad)));
    glm::vec3 derecha = glm::normalize(glm::vec3(std::cos(rad), 0.0f, -std::sin(rad)));
    glm::vec3 arriba  = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 focoLocal(
        0.0f,
        grua.cabina.posicion.y - 0.10f,
        grua.cabina.posicion.z + grua.cabina.escala.z / 2.0f + 0.16f
    );

    glm::vec3 lightPos = grua.posicion
        + derecha * focoLocal.x
        + arriba  * focoLocal.y
        + frente  * focoLocal.z;

    glm::vec3 lightDir = glm::normalize(frente + glm::vec3(0.0f, -0.18f, 0.0f));

    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos1"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightDir1"), lightDir.x, lightDir.y, lightDir.z);

    glUniform1f(glGetUniformLocation(shaderProgram, "cutOff1"), glm::cos(glm::radians(12.5f)));
    glUniform1f(glGetUniformLocation(shaderProgram, "outerCutOff1"), glm::cos(glm::radians(18.0f)));
}

void configurarLuzBrazoGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaBrazo)
{
    glUniform1i(glGetUniformLocation(shaderProgram, "luz2Encendida"), luzEncendidaBrazo ? 1 : 0);

    if (luzEncendidaBrazo)
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor2"), 1.7f, 1.7f, 1.5f);
    else
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor2"), 0.0f, 0.0f, 0.0f);

    // Transformación global de la grúa
    glm::mat4 modeloGrua = glm::mat4(1.0f);
    modeloGrua = glm::translate(modeloGrua, grua.posicion);
    modeloGrua = glm::rotate(modeloGrua, glm::radians(grua.direccion), glm::vec3(0.0f, 1.0f, 0.0f));

    // Transformación de la articulación (sin escala)
    glm::mat4 modeloArt = modeloGrua;
    modeloArt = modeloArt * glm::translate(glm::mat4(1.0f), grua.articulacion.posicion);
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Transformación del brazo (sin escala final)
    glm::mat4 modeloBrazo = modeloArt;
    modeloBrazo = glm::translate(modeloBrazo, grua.brazo.posicion);
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Punto en la punta del brazo, en coordenadas locales
    glm::vec4 puntaLocal(0.0f, 0.0f, grua.brazo.escala.z, 1.0f);
    glm::vec3 lightPos = glm::vec3(modeloBrazo * puntaLocal);

    // Apunta al suelo
    glm::vec3 lightDir = glm::vec3(0.0f, -1.0f, 0.0f);

    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos2"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightDir2"), lightDir.x, lightDir.y, lightDir.z);

    glUniform1f(glGetUniformLocation(shaderProgram, "cutOff2"), glm::cos(glm::radians(20.0f)));
    glUniform1f(glGetUniformLocation(shaderProgram, "outerCutOff2"), glm::cos(glm::radians(28.0f)));
}

void configurarLucesGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco, bool luzEncendidaBrazo)
{
    configurarFocoFrontalGrua(grua, shaderProgram, luzEncendidaFoco);
    configurarLuzBrazoGrua(grua, shaderProgram, luzEncendidaBrazo);

    glUniform1i(glGetUniformLocation(shaderProgram, "esFoco"), 0);
}