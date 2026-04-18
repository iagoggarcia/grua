#include "grua.h"
#include "esfera.h"
#include "cubo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

extern unsigned int VAO_cubo;
extern GLuint VAO_esfera;

static glm::mat4 crearMatrizModelo(const Pieza& pieza, int escala) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, pieza.posicion);
    model = glm::rotate(model, glm::radians(pieza.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(pieza.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(pieza.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));
    if(escala == 1)
    model = glm::scale(model, pieza.escala);

    return model;
}

static void dibujarPieza(const Pieza& pieza, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), r, g, b);

    glm::mat4 model = modeloPadre * crearMatrizModelo(pieza, 1);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

static void dibujarRueda(const glm::vec3& posicionLocal, float giroruedas, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b) {
    glm::mat4 baseRueda = modeloPadre;
    baseRueda = glm::translate(baseRueda, posicionLocal);
    baseRueda = glm::rotate(baseRueda, glm::radians(giroruedas), glm::vec3(1.0f, 0.0f, 0.0f));

    // RUEDA NEGRA
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), r, g, b);

    glm::mat4 modelRueda = baseRueda;
    modelRueda = glm::scale(modelRueda, glm::vec3(0.22f, 0.50f, 0.50f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelRueda));

    glBindVertexArray(VAO_esfera);
    glDrawArrays(GL_TRIANGLES, 0, verticesEsferaSize / (8 * sizeof(float)));

    // MARCA ROJA
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 1.0f, 0.0f, 0.0f);

    glm::mat4 modelMarca = baseRueda;
    modelMarca = glm::translate(modelMarca, glm::vec3(0.0f, 0.55f, 0.0f));
    modelMarca = glm::scale(modelMarca, glm::vec3(0.12f, 0.12f, 0.12f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMarca));

    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

static void aplicarRozamiento(Grua& grua, float deltaTime) {
    grua.velocidad *= (1.0f - 1.5f * deltaTime);

    if (std::abs(grua.velocidad) < 0.05f)
        grua.velocidad = 0.0f; //metemos esto porque si no, cuando giramos y dejamos qeu se pare por rozamiento, la velocidad se aproxima a 0 pero nunca llega a 0
        //entonces por debajo de un umbral la paramos, que si no se puede mover infinitamente estando "parada" hasta que la paremos nosotros
}

static void limitarGruaAlSuelo(Grua& grua) {
    const float limiteX = 19.0f;
    const float limiteZ = 19.0f;

    if (grua.posicion.x < -limiteX) grua.posicion.x = -limiteX;
    if (grua.posicion.x >  limiteX) grua.posicion.x =  limiteX;
    if (grua.posicion.z < -limiteZ) grua.posicion.z = -limiteZ;
    if (grua.posicion.z >  limiteZ) grua.posicion.z =  limiteZ;
}

void inicializarGrua(Grua& grua) {
    grua.posicion = glm::vec3(0.0f, 0.5f, 2.0f);
    grua.direccion = 0.0f;
    grua.velocidad = 0.0f;

    grua.base.posicion = glm::vec3(0.0f, -0.55f, 0.0f);
    grua.base.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.base.escala = glm::vec3(2.2f, 0.7f, 4.2f);

    grua.cabina.posicion = glm::vec3(0.0f, 0.10f, 1.45f);
    grua.cabina.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.cabina.escala = glm::vec3(2.2f, 0.9f, 1.0f);

    grua.articulacion.posicion = glm::vec3(0.0f, 1.65f, 0.0f);
    grua.articulacion.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.articulacion.escala = glm::vec3(0.3f, 2.8f, 0.3f);

    grua.brazo.posicion = glm::vec3(0.0f, 1.4f, 0.0f);
    grua.brazo.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.brazo.escala = glm::vec3(0.2f, 0.2f, 5.0f);

    grua.giroruedas = 0.0f;
}

static void dibujarFocoFrontal(const Grua& grua, const glm::mat4& modeloGrua, GLuint shaderProgram) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "esFoco"), 1);

    // color base del foco apagado
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 0.75f, 0.75f, 0.65f);

    glm::mat4 model = modeloGrua;

    // MISMA posición que usamos en main.cpp para lightPos
    model = glm::translate(model, glm::vec3(
        0.0f,
        grua.cabina.posicion.y - 0.10f,
        grua.cabina.posicion.z + grua.cabina.escala.z / 2.0f + 0.16f
    ));

    // esfera más grande y más plana
    model = glm::scale(model, glm::vec3(0.22f, 0.22f, 0.08f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO_esfera);
    glDrawArrays(GL_TRIANGLES, 0, verticesEsferaSize / (8 * sizeof(float)));

    glUniform1i(glGetUniformLocation(shaderProgram, "esFoco"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

static void dibujarBrazo(const Pieza& brazo, const glm::mat4& modeloArticulacion, GLuint shaderProgram, float r, float g, float b) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), r, g, b);

    glm::mat4 model = modeloArticulacion;

    // Punto de anclaje del brazo respecto a la articulación
    model = glm::translate(model, brazo.posicion);

    // Rotación del brazo alrededor de su base
    model = glm::rotate(model, glm::radians(brazo.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(brazo.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(brazo.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Desplazamos media longitud para que el cubo quede anclado por un extremo
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, brazo.escala.z / 2.0f));

    // Escalamos el cubo para formar el brazo
    model = glm::scale(model, brazo.escala);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

void dibujarGrua(const Grua& grua, GLuint shaderProgram) {
    glm::mat4 modeloGrua = glm::mat4(1.0f);

    modeloGrua = glm::translate(modeloGrua, grua.posicion);
    modeloGrua = glm::rotate(modeloGrua, glm::radians(grua.direccion), glm::vec3(0.0f, 1.0f, 0.0f));

    dibujarPieza(grua.base, modeloGrua, shaderProgram, 0.9f, 0.0f, 0.0f);
    dibujarPieza(grua.cabina, modeloGrua, shaderProgram, 0.9f, 0.0f, 0.0f);
    dibujarFocoFrontal(grua, modeloGrua, shaderProgram);
    dibujarPieza(grua.articulacion, modeloGrua, shaderProgram, 0.0f, 1.0f, 1.0f);

    glm::mat4 modeloArt = modeloGrua * crearMatrizModelo(grua.articulacion,0);
    dibujarBrazo(grua.brazo, modeloArt, shaderProgram, 0.9f, 0.7f, 0.0f);

    dibujarRueda(glm::vec3(-1.25f, -1.0f,  1.55f), grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3( 1.25f, -1.0f,  1.55f), grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3(-1.25f, -1.0f, -1.55f), grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3( 1.25f, -1.0f, -1.55f), grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
}

void dibujarEscena(const Grua& grua, GLuint shaderProgram, GLuint VAO_cubo) {
    crearSuelo(shaderProgram, VAO_cubo);
    dibujarGrua(grua, shaderProgram);
}

void actualizarGrua(Grua& grua, float deltaTime) {

    float rad = glm::radians(grua.direccion);

    grua.posicion.x += std::sin(rad) * grua.velocidad * deltaTime;
    grua.posicion.z += std::cos(rad) * grua.velocidad * deltaTime;

    const float factorGiroRueda = 120.0f;
    grua.giroruedas += grua.velocidad * factorGiroRueda * deltaTime;

    aplicarRozamiento(grua, deltaTime);
    limitarGruaAlSuelo(grua);
}