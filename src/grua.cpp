#include "grua.h"
#include "esfera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern unsigned int VAO_cubo;
extern GLuint VAO_esfera;


static glm::mat4 crearMatrizModelo(const Pieza& pieza) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, pieza.posicion);

    model = glm::rotate(model, glm::radians(pieza.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(pieza.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(pieza.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, pieza.escala);

    return model;
}

static void dibujarPieza(const Pieza& pieza, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), r, g, b);

    glm::mat4 model = modeloPadre * crearMatrizModelo(pieza);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

static void dibujarRueda(const glm::vec3& posicionLocal, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), r, g, b);

    glm::mat4 model = modeloPadre;
    model = glm::translate(model, posicionLocal);

    // Grosor en X, radio en Y y Z
    model = glm::scale(model, glm::vec3(0.22f, 0.50f, 0.50f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO_esfera);
    glDrawArrays(GL_TRIANGLES, 0, verticesEsferaSize / (8 * sizeof(float)));

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

void inicializarGrua(Grua& grua) {
    // Estado global de la grúa en el mundo
    grua.posicion = glm::vec3(0.0f, 0.5f, 2.0f);
    grua.direccion = 0.0f;
    grua.velocidad = 0.0f;

    // Piezas en coordenadas LOCALES respecto a la grúa

    // Base
    grua.base.posicion = glm::vec3(0.0f, -0.45f, 0.0f);
    grua.base.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.base.escala = glm::vec3(3.4f, 1.1f, 3.4f);

    // Cabina
    grua.cabina.posicion = glm::vec3(0.0f, 0.45f, 0.0f);
    grua.cabina.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.cabina.escala = glm::vec3(1.8f, 1.0f, 1.8f);

    // Articulación
    grua.articulacion.posicion = glm::vec3(0.0f, 2.35f, 0.0f);
    grua.articulacion.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.articulacion.escala = glm::vec3(0.3f, 3.0f, 0.3f);

    // Brazo horizontal
    grua.brazo.posicion = glm::vec3(0.0f, 3.95f, 2.5f);
    grua.brazo.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.brazo.escala = glm::vec3(0.2f, 0.2f, 5.0f);
}

void dibujarGrua(const Grua& grua, GLuint shaderProgram) {
    glm::mat4 modeloGrua = glm::mat4(1.0f);

    // Transformación global de toda la grúa
    modeloGrua = glm::translate(modeloGrua, grua.posicion);
    modeloGrua = glm::rotate(modeloGrua, glm::radians(grua.direccion), glm::vec3(0.0f, 1.0f, 0.0f));

    dibujarPieza(grua.base, modeloGrua, shaderProgram, 0.3f, 0.3f, 0.3f);
    dibujarPieza(grua.cabina, modeloGrua, shaderProgram, 0.9f, 0.5f, 0.1f);
    dibujarPieza(grua.articulacion, modeloGrua, shaderProgram, 0.9f, 0.7f, 0.0f);
    dibujarPieza(grua.brazo, modeloGrua, shaderProgram, 0.9f, 0.7f, 0.0f);

    // las 4 ruedas
    dibujarRueda(glm::vec3(-1.5f, -1.0f,  1.7f), modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3( 1.5f, -1.0f,  1.7f), modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3(-1.5f, -1.0f, -1.7f), modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
    dibujarRueda(glm::vec3( 1.5f, -1.0f, -1.7f), modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
}