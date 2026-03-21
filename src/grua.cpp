#include "grua.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern unsigned int VAO_cubo;

void dibujarGrua(GLuint shaderProgram) {

    glm::mat4 model = glm::mat4(1.0f);

    // base de la grúa
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE,glm::value_ptr(model));

    glBindVertexArray(VAO_cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}