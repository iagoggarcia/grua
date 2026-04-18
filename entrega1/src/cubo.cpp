#include "cubo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


void crearSuelo(GLuint shaderProgram, unsigned int VAO_cubo) {
    glBindVertexArray(VAO_cubo);
    for (int x = -20; x < 20; x++) {
        for (int z = -20; z < 20; z++) {
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, glm::vec3(x, -1.0f, z)); // y=-1 porque si no la base de la grúa se atraviesa por el suelo
            model = glm::scale(model, glm::vec3(1.0f, 0.1f, 1.0f));

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

float vertices[] = {
    // posiciones                 // colores

    // cara trasera
    -0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f, 0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,

    // cara frontal
    -0.5f,-0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
     0.5f,-0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f,-0.5f, 0.5f, 0.15f, 0.5f, 0.15f,

    // cara izquierda
    -0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f, 0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f,-0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,

    // cara derecha
     0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f,-0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,

    // cara inferior
    -0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f,-0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
     0.5f,-0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f,-0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f,-0.5f,-0.5f, 0.15f, 0.5f, 0.15f,

    // cara superior
    -0.5f, 0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f,-0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
     0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f, 0.5f, 0.5f, 0.15f, 0.5f, 0.15f,
    -0.5f, 0.5f,-0.5f, 0.15f, 0.5f, 0.15f
};

const unsigned int verticesSize = sizeof(vertices);