#include "grua.h"
#include "esfera.h"
#include "cubo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

extern GLuint VAO_CUBO;
extern GLuint VAO_esfera;

static glm::mat4 crearMatrizModelo(const Pieza& pieza, int escala) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, pieza.posicion);
    model = glm::rotate(model, glm::radians(pieza.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(pieza.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(pieza.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));
    if (escala == 1)
        model = glm::scale(model, pieza.escala);

    return model;
}

static void dibujarPieza(const Pieza& pieza, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), r, g, b);

    glm::mat4 model = modeloPadre * crearMatrizModelo(pieza, 1);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO_CUBO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

static void dibujarRueda(const glm::vec3& posicionLocal, float giroruedas, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b) {
    glm::mat4 baseRueda = modeloPadre;
    baseRueda = glm::translate(baseRueda, posicionLocal);
    baseRueda = glm::rotate(baseRueda, glm::radians(giroruedas), glm::vec3(1.0f, 0.0f, 0.0f));

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), r, g, b);

    glm::mat4 modelRueda = baseRueda;
    modelRueda = glm::scale(modelRueda, glm::vec3(0.22f, 0.50f, 0.50f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelRueda));

    glBindVertexArray(VAO_esfera);
    glDrawArrays(GL_TRIANGLES, 0, verticesEsferaSize / (8 * sizeof(float)));

    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 1.0f, 0.0f, 0.0f);

    glm::mat4 modelMarca = baseRueda;
    modelMarca = glm::translate(modelMarca, glm::vec3(0.0f, 0.55f, 0.0f));
    modelMarca = glm::scale(modelMarca, glm::vec3(0.12f, 0.12f, 0.12f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMarca));

    glBindVertexArray(VAO_CUBO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

static void aplicarRozamiento(Grua& grua, float deltaTime) {
    grua.velocidad *= (1.0f - 1.5f * deltaTime);

    if (std::abs(grua.velocidad) < 0.05f)
        grua.velocidad = 0.0f;
}

static void limitarGruaAlSuelo(Grua& grua) {
    const float limiteX = 28.0f;
    const float limiteZ = 28.0f;

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

    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 0.75f, 0.75f, 0.65f);

    glm::mat4 model = modeloGrua;

    model = glm::translate(model, glm::vec3(
        0.0f,
        grua.cabina.posicion.y - 0.10f,
        grua.cabina.posicion.z + grua.cabina.escala.z / 2.0f + 0.16f
    ));

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
    model = glm::translate(model, brazo.posicion);
    model = glm::rotate(model, glm::radians(brazo.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(brazo.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(brazo.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, brazo.escala.z / 2.0f));
    model = glm::scale(model, brazo.escala);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO_CUBO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

static void dibujarArbusto(GLuint shaderProgram, GLuint texturaArbusto, float x, float y, float z,float ancho, float alto, float grosor,float rotY)
{
    glBindVertexArray(VAO_CUBO);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTexturaSuelo"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarArbusto"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaArbusto);
    glUniform1i(glGetUniformLocation(shaderProgram, "textura1"), 0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(ancho, alto, grosor));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarArbusto"), 0);
}

void dibujarGrua(const Grua& grua, GLuint shaderProgram) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTexturaSuelo"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarArbusto"), 0);

    glm::mat4 modeloGrua = glm::mat4(1.0f);

    modeloGrua = glm::translate(modeloGrua, grua.posicion);
    modeloGrua = glm::rotate(modeloGrua, glm::radians(grua.direccion), glm::vec3(0.0f, 1.0f, 0.0f));

    dibujarPieza(grua.base, modeloGrua, shaderProgram, 0.9f, 0.0f, 0.0f);
    dibujarPieza(grua.cabina, modeloGrua, shaderProgram, 0.9f, 0.0f, 0.0f);
    dibujarFocoFrontal(grua, modeloGrua, shaderProgram);
    dibujarPieza(grua.articulacion, modeloGrua, shaderProgram, 0.0f, 1.0f, 1.0f);

    glm::mat4 modeloArt = modeloGrua * crearMatrizModelo(grua.articulacion, 0);
    dibujarBrazo(grua.brazo, modeloArt, shaderProgram, 0.9f, 0.7f, 0.0f);

    dibujarRueda(glm::vec3(-1.25f, -1.0f,  1.55f), grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3( 1.25f, -1.0f,  1.55f), grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3(-1.25f, -1.0f, -1.55f), grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3( 1.25f, -1.0f, -1.55f), grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
}

void dibujarEscena(const Grua& grua, GLuint shaderProgram, GLuint texturaSuelo, GLuint texturaArbusto, GLuint texturaFondo, glm::vec3 camPos) {
    crearFondo(texturaFondo, camPos, shaderProgram);
    crearSuelo(shaderProgram, texturaSuelo);
    dibujarGrua(grua, shaderProgram);

    dibujarArbusto(shaderProgram, texturaArbusto, -18.0f, 0.0f, -12.0f, 2.2f, 2.0f, 0.05f, 0.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, -18.0f, 0.0f, -12.0f, 2.2f, 2.0f, 0.05f, 90.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, 16.0f, 0.0f, 10.0f, 2.0f, 1.8f, 0.05f, 20.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, 16.0f, 0.0f, 10.0f, 2.0f, 1.8f, 0.05f, 110.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, -26.0f, 0.0f, 15.0f, 2.1f, 1.9f, 0.05f, 15.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, -26.0f, 0.0f, 15.0f, 2.1f, 1.9f, 0.05f, 105.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, 22.0f, 0.0f, -18.0f, 2.4f, 2.0f, 0.05f, 35.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, 22.0f, 0.0f, -18.0f, 2.4f, 2.0f, 0.05f, 125.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, -8.0f, 0.0f, 24.0f, 1.8f, 1.7f, 0.05f, 10.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, -8.0f, 0.0f, 24.0f, 1.8f, 1.7f, 0.05f, 100.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, 28.0f, 0.0f, 20.0f, 2.3f, 2.1f, 0.05f, 25.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, 28.0f, 0.0f, 20.0f, 2.3f, 2.1f, 0.05f, 115.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, -30.0f, 0.0f, -22.0f, 2.0f, 1.9f, 0.05f, 40.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, -30.0f, 0.0f, -22.0f, 2.0f, 1.9f, 0.05f, 130.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, 8.0f, 0.0f, -26.0f, 2.1f, 1.8f, 0.05f, 5.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, 8.0f, 0.0f, -26.0f, 2.1f, 1.8f, 0.05f, 95.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, -22.0f, 0.0f, 30.0f, 2.2f, 2.0f, 0.05f, 30.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, -22.0f, 0.0f, 30.0f, 2.2f, 2.0f, 0.05f, 120.0f);

    dibujarArbusto(shaderProgram, texturaArbusto, 30.0f, 0.0f, -6.0f, 1.9f, 1.7f, 0.05f, 50.0f);
    dibujarArbusto(shaderProgram, texturaArbusto, 30.0f, 0.0f, -6.0f, 1.9f, 1.7f, 0.05f, 140.0f);
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