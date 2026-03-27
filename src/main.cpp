#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "lecturaShader_0_9.h"

#include "grua.h"
#include "cubo.h"
#include "esfera.h"

void processInput(GLFWwindow* window, Grua& grua, int& modoCamara);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Configuración ventana
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// shaders
extern GLuint setShaders(const char* nVertx, const char* nFrag);
GLuint shaderProgram;

unsigned int VAO_cubo;
unsigned int VBO_cubo;
GLuint VAO_esfera;
GLuint VBO_esfera;

void openGlInit() {
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Grúa", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error creando ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error inicializando GLAD" << std::endl;
        return -1;
    }

    openGlInit();
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // -------- CUBO --------
    glGenVertexArrays(1, &VAO_cubo);
    glGenBuffers(1, &VBO_cubo);

    glBindVertexArray(VAO_cubo);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_cubo);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    crearEsfera(VAO_esfera, VBO_esfera);

    shaderProgram = setShaders("shaders/shader.vert", "shaders/shader.frag");

    // -------- GRÚA --------
    Grua grua;
    inicializarGrua(grua);

    // 1 = primera persona, 2 = tercera persona, 3 = cenital
    int modoCamara = 2;

    while (!glfwWindowShouldClose(window)) {
        processInput(window, grua, modoCamara);

        float rad = glm::radians(grua.direccion);
        grua.posicion.x += sin(rad) * grua.velocidad;
        grua.posicion.z += cos(rad) * grua.velocidad;

        // Rozamiento simple
        grua.velocidad *= 0.98f;

        // Limitar la grúa para que no salga del suelo
        float limiteX = 19.0f;
        float limiteZ = 19.0f;

        if (grua.posicion.x < -limiteX) grua.posicion.x = -limiteX;
        if (grua.posicion.x >  limiteX) grua.posicion.x =  limiteX;
        if (grua.posicion.z < -limiteZ) grua.posicion.z = -limiteZ;
        if (grua.posicion.z >  limiteZ) grua.posicion.z =  limiteZ;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::vec3 frente = glm::vec3(sin(rad), 0.0f, cos(rad));
        glm::vec3 camPos;
        glm::vec3 camTarget;

        if (modoCamara == 1) {
            // Primera persona: no metida dentro de la grúa
            camPos = grua.posicion - frente * 2.0f + glm::vec3(0.0f, 3.5f, 0.0f);
            camTarget = grua.posicion + frente * 12.0f + glm::vec3(0.0f, 3.0f, 0.0f);
        }
        else if (modoCamara == 2) {
            // Tercera persona
            camPos = grua.posicion - frente * 24.0f + glm::vec3(0.0f, 8.0f, 0.0f);
            camTarget = grua.posicion + glm::vec3(0.0f, 2.0f, 0.0f);
        }
        else {
            // Cenital más lejana
            camPos = glm::vec3(50.0f, 20.0f, 8.0f);
            camTarget = glm::vec3(0.0f, 0.0f, 0.0f);;
        }

        glm::mat4 view = glm::lookAt(
            camPos,
            camTarget,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)width / (float)height,
            0.1f,
            100.0f
        );

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // -------- SUELO --------
        crearSuelo(shaderProgram, VAO_cubo);

        // -------- GRÚA --------
        dibujarGrua(grua, shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO_cubo);
    glDeleteBuffers(1, &VBO_cubo);
    glDeleteVertexArrays(1, &VAO_esfera);
    glDeleteBuffers(1, &VBO_esfera);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow* window, Grua& grua, int& modoCamara)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        grua.velocidad += 0.001f;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        grua.velocidad -= 0.001f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        grua.direccion += 1.0f;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        grua.direccion -= 1.0f;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        modoCamara = 1;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        modoCamara = 2;

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        modoCamara = 3;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}