#include "grua.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern unsigned int VAO_cubo;

void dibujarBase(GLuint shaderProgram) {
    // Activar color uniforme y establecer color de la base (gris oscuro)
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 0.4f, 0.4f, 0.4f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.45f, 2.0f));
    model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Desactivar para que el resto de piezas usen sus colores de vértice
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

void dibujarCabina(GLuint shaderProgram) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 0.8f, 0.5f, 0.1f); // naranja

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.05f + 0.4f, 2.0f)); // 0.05 = techo base, 0.4 = mitad cabina
    model = glm::scale(model, glm::vec3(1.5f, 0.8f, 1.5f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

void dibujarBrazo(GLuint shaderProgram) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 0.9f, 0.7f, 0.0f); // amarillo

    glm::mat4 model = glm::mat4(1.0f);
    // Techo cabina = 0.85, mástil altura = 3.0, centro en 0.85 + 1.5 = 2.35
    model = glm::translate(model, glm::vec3(0.0f, 2.35f, 2.0f));
    model = glm::scale(model, glm::vec3(0.3f, 3.0f, 0.3f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

void dibujarPluma(GLuint shaderProgram) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 0.9f, 0.7f, 0.0f);

    glm::mat4 model = glm::mat4(1.0f);
    // En Y: tope mástil = 3.85, pluma altura = 0.2, centro en 3.85 + 0.1 = 3.95
    // En Z: centrada en 2.0 + desplazada hacia delante la mitad de su longitud
    float longitudPluma = 5.0f;
    model = glm::translate(model, glm::vec3(0.0f, 3.95f, 2.0f + longitudPluma / 2.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, longitudPluma));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}