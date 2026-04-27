#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "grua.h"
#include "tiempo.h"
#include "input.h"
#include "render.h"

// Estado global de la escena
Grua grua;
EstadoEntrada estadoEntrada;
float deltaTime = 0.0f;

// Configuración ventana
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// shaders
RecursosRender recursos;

int main() {
    GLFWwindow* window = inicializarVentana(SCR_WIDTH, SCR_HEIGHT, "Grúa");
    if (window == nullptr)
        return -1;

    openGlInit();
    framebuffer_size_callback(window, SCR_WIDTH, SCR_HEIGHT);

    cargarRecursos(recursos);
    inicializarGrua(grua);
    inicializarEstadoEntrada(estadoEntrada);

    float ultimoFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        deltaTime = lapsoDeltaTime(ultimoFrame);

        processInput(window, grua, estadoEntrada, deltaTime);
        actualizarGrua(grua, deltaTime);
        renderizarFrame(grua, estadoEntrada, recursos);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    liberar(recursos);
    return 0;
}
