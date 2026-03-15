#include "cubo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


void crearSuelo(GLuint shaderProgram, unsigned int VAO_cubo) {
    glBindVertexArray(VAO_cubo);
    for (int x = -10; x < 10; x++) {
        for (int z = -10; z < 10; z++) {
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, glm::vec3(x, -1.0f, z));
            model = glm::scale(model, glm::vec3(1.0f, 0.1f, 1.0f));

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}