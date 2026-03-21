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