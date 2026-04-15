#include "tiempo.h"
#include <GLFW/glfw3.h>

float lapsoDeltaTime(float& ultimoFrame) {
    float tiempoActual = glfwGetTime();
    float deltaTime = tiempoActual - ultimoFrame;
    ultimoFrame = tiempoActual;
    return deltaTime;
}