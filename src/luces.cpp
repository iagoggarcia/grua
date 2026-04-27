#include "luces.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// envía un color al shader dependiendo de si la luz está encendida o no
static void enviarColorLuz(GLuint shaderProgram, const char* nombreUniform, bool encendida, const glm::vec3& colorEncendido) {
    if (encendida)
        glUniform3f(glGetUniformLocation(shaderProgram, nombreUniform), colorEncendido.x, colorEncendido.y, colorEncendido.z);
    else
        glUniform3f(glGetUniformLocation(shaderProgram, nombreUniform), 0.0f, 0.0f, 0.0f);
}

// envía un vector vec3 al shader
static void enviarVector3(GLuint shaderProgram, const char* nombreUniform, const glm::vec3& valor) {
    glUniform3f(glGetUniformLocation(shaderProgram, nombreUniform), valor.x, valor.y, valor.z);
}

// envía los ángulos del cono de luz (interno y externo)
static void enviarConoLuz(GLuint shaderProgram, const char* cutOffUniform, const char* outerCutOffUniform, float cutOff, float outerCutOff) {
    glUniform1f(glGetUniformLocation(shaderProgram, cutOffUniform), glm::cos(glm::radians(cutOff)));
    glUniform1f(glGetUniformLocation(shaderProgram, outerCutOffUniform), glm::cos(glm::radians(outerCutOff)));
}

static glm::mat4 crearModeloGruaLuces(const Grua& grua) {
    // Construimos la transformación global de la grúa.
    glm::mat4 modeloGrua = glm::mat4(1.0f);
    modeloGrua = glm::translate(modeloGrua, grua.posicion);
    modeloGrua = glm::rotate(modeloGrua, glm::radians(grua.direccion), glm::vec3(0.0f, 1.0f, 0.0f));

    return modeloGrua;
}

static glm::mat4 crearModeloArticulacionLuces(const Grua& grua, const glm::mat4& modeloGrua) {
    // Construimos la transformación de la articulación sin aplicar su escala, para no deformar la referencia usada por el brazo.
    glm::mat4 modeloArt = modeloGrua;
    modeloArt = modeloArt * glm::translate(glm::mat4(1.0f), grua.articulacion.posicion);
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modeloArt = modeloArt * glm::rotate(glm::mat4(1.0f), glm::radians(grua.articulacion.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    return modeloArt;
}

static glm::mat4 crearModeloBrazoLuces(const Grua& grua, const glm::mat4& modeloArt) {
    // Construimos la transformación del brazo sin aplicar aún su escala final.
    glm::mat4 modeloBrazo = modeloArt;
    modeloBrazo = glm::translate(modeloBrazo, grua.brazo.posicion);
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modeloBrazo = glm::rotate(modeloBrazo, glm::radians(grua.brazo.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    return modeloBrazo;
}

static void calcularEjesGrua(const Grua& grua, glm::vec3& frente, glm::vec3& derecha, glm::vec3& arriba) {
    // Convertimos la dirección global de la grúa a radianes
    float rad = glm::radians(grua.direccion);

    // Calculamos los vectores locales de orientación de la grúa:
    // frente, derecha y arriba.
    frente  = glm::normalize(glm::vec3(std::sin(rad), 0.0f, std::cos(rad)));
    derecha = glm::normalize(glm::vec3(std::cos(rad), 0.0f, -std::sin(rad)));
    arriba  = glm::vec3(0.0f, 1.0f, 0.0f);
}

static glm::vec3 calcularPosicionFocoFrontal(const Grua& grua, const glm::vec3& frente, const glm::vec3& derecha, const glm::vec3& arriba) {
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

    return lightPos;
}

// calcula la dirección del foco frontal
static glm::vec3 calcularDireccionFocoFrontal(const glm::vec3& frente) {
    // El foco apunta hacia delante con una ligera inclinación hacia abajo
    return glm::normalize(frente + glm::vec3(0.0f, -0.18f, 0.0f));
}

// calcula la posición de la luz situada en el extremo del brazo
static glm::vec3 calcularPosicionLuzBrazo(const Grua& grua) {
    glm::mat4 modeloGrua = crearModeloGruaLuces(grua);
    glm::mat4 modeloArt = crearModeloArticulacionLuces(grua, modeloGrua);
    glm::mat4 modeloBrazo = crearModeloBrazoLuces(grua, modeloArt);

    // Definimos un punto en la punta del brazo en coordenadas locales.
    glm::vec4 puntaLocal(0.0f, grua.brazo.escala.y, 0.0f, 1.0f);

    // Transformamos ese punto a coordenadas globales para obtener
    // la posición real de la luz en el mundo.
    glm::vec3 lightPos = glm::vec3(modeloBrazo * puntaLocal);

    return lightPos;
}

void configurarFocoFrontalGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaFoco)
{
    // Indicamos al shader si el foco frontal está encendido o apagado.
    glUniform1i(glGetUniformLocation(shaderProgram, "luz1Encendida"), luzEncendidaFoco ? 1 : 0);

    // Si está encendido, le damos un color amarillento intenso
    // si no, su color es negro y no aporta iluminación
    enviarColorLuz(shaderProgram, "lightColor1", luzEncendidaFoco, glm::vec3(2.2f, 2.2f, 1.8f));

    glm::vec3 frente;
    glm::vec3 derecha;
    glm::vec3 arriba;
    calcularEjesGrua(grua, frente, derecha, arriba);

    glm::vec3 lightPos = calcularPosicionFocoFrontal(grua, frente, derecha, arriba);
    glm::vec3 lightDir = calcularDireccionFocoFrontal(frente);

    // Enviamos al shader la posición y dirección del foco.
    enviarVector3(shaderProgram, "lightPos1", lightPos);
    enviarVector3(shaderProgram, "lightDir1", lightDir);

    // Definimos la apertura interna y externa del cono de luz.
    enviarConoLuz(shaderProgram, "cutOff1", "outerCutOff1", 12.5f, 18.0f);
}

void configurarLuzBrazoGrua(const Grua& grua, GLuint shaderProgram, bool luzEncendidaBrazo)
{
    // Indicamos al shader si la luz del brazo está encendida o apagada.
    glUniform1i(glGetUniformLocation(shaderProgram, "luz2Encendida"), luzEncendidaBrazo ? 1 : 0);

    // Si está encendida, le damos un color claro; si no, queda anulada.
    enviarColorLuz(shaderProgram, "lightColor2", luzEncendidaBrazo, glm::vec3(1.7f, 1.7f, 1.5f));

    glm::vec3 lightPos = calcularPosicionLuzBrazo(grua);

    // La luz del brazo apunta verticalmente hacia el suelo.
    glm::vec3 lightDir = glm::vec3(0.0f, -1.0f, 0.0f);

    // Enviamos posición y dirección al shader.
    enviarVector3(shaderProgram, "lightPos2", lightPos);
    enviarVector3(shaderProgram, "lightDir2", lightDir);

    // Definimos la apertura del cono de esta luz.
    enviarConoLuz(shaderProgram, "cutOff2", "outerCutOff2", 20.0f, 28.0f);
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