#include "tiempo.h"
#include <GLFW/glfw3.h>

float lapsoDeltaTime(float& ultimoFrame) {
    // Obtenemos el tiempo actual desde que comenzó la ejecución del programa.
    float tiempoActual = glfwGetTime();

    // Calculamos cuánto tiempo ha pasado desde el frame anterior.
    float deltaTime = tiempoActual - ultimoFrame;

    // Actualizamos la referencia temporal para el siguiente frame.
    ultimoFrame = tiempoActual;

    // Devolvemos el tiempo transcurrido entre frames.
    return deltaTime;
}