#include <glad/glad.h>
#include <GLFW/glfw3.h>

// estas dos hacen falta para el model y demás:
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "lecturaShader_0_9.h"

#include "grua.h"
#include "cubo.h"

void processInput(GLFWwindow* window);

// Configuración ventana
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// shaders
extern GLuint setShaders(const char* nVertx, const char* nFrag);
GLuint shaderProgram;

unsigned int VAO_cubo;
unsigned int VBO_cubo;

float sueloVertices[] = {
    // posiciones        // color
    -10.0f, 0.0f, -10.0f,  0.2f, 0.7f, 0.2f,
     10.0f, 0.0f, -10.0f,  0.2f, 0.7f, 0.2f,
     10.0f, 0.0f,  10.0f,  0.2f, 0.7f, 0.2f,

     10.0f, 0.0f,  10.0f,  0.2f, 0.7f, 0.2f,
    -10.0f, 0.0f,  10.0f,  0.2f, 0.7f, 0.2f,
    -10.0f, 0.0f, -10.0f,  0.2f, 0.7f, 0.2f
};

unsigned int VAO_suelo, VBO_suelo;

void openGlInit() {
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // color de fondo de la ventana
    glEnable(GL_DEPTH_TEST);
}

int main() {
    // Inicializar GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Grúa", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error creando ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error inicializando GLAD" << std::endl;
        return -1;
    }

    // Configuración OpenGL
    openGlInit();

    // -------- SUELO --------
    glGenVertexArrays(1, &VAO_suelo);
    glGenBuffers(1, &VBO_suelo);

    glBindVertexArray(VAO_suelo);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_suelo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sueloVertices), sueloVertices, GL_STATIC_DRAW);

    // posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // -------- CUBO --------
    glGenVertexArrays(1, &VAO_cubo);
    glGenBuffers(1, &VBO_cubo);

    glBindVertexArray(VAO_cubo);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_cubo);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    // posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Cargar shaders
    shaderProgram = setShaders("shaders/shader.vert", "shaders/shader.frag");

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // limpiar pantalla
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = glm::lookAt(
            glm::vec3(25.0f, 15.0f, 5.0f),  // posición cámara
            glm::vec3(0.0f, 0.0f, 0.0f),    // mira al centro
            glm::vec3(0.0f, 1.0f, 0.0f)     // arriba
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / SCR_HEIGHT,
            0.1f,
            100.0f
        );

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // -------- SUELO --------
        glBindVertexArray(VAO_suelo);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // -------- CUBO SUELTO --------
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, 0.5f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO_cubo);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // -------- GRÚA --------
        dibujarGrua(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}