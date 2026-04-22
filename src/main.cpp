#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "lecturaShader_0_9.h"

#include "grua.h"
#include "cubo.h"
#include "esfera.h"
#include "tiempo.h"
#include "camaras.h"
#include "texturas.h"
#include "luces.h"

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

glm::mat4 projection;
float deltaTime = 0.0f;

// Estado global de la escena
Grua grua;
int modoCamara = 2;
bool luzEncendidaFoco = true;
bool teclaIPulsada = false;
bool luzEncendidaBrazo = true;
bool teclaTPulsada = false;

// Configuración ventana
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// shaders
extern GLuint setShaders(const char* nVertx, const char* nFrag);
GLuint shaderProgram;

void liberar(){
    liberarEsfera();
    liberarCubo();
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}



void openGlInit() {
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error inicializando GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    openGlInit();
    framebuffer_size_callback(window, SCR_WIDTH, SCR_HEIGHT);

    crearCubo();
    crearEsfera();

    shaderProgram = setShaders("shaders/shader.vert", "shaders/shader.frag");

    GLuint texturaSuelo = cargarTextura("texturas/hierba.jpg");
    GLuint texturaArbusto = cargarTextura("texturas/arbusto.png");
    GLuint texturaFondo = cargarTextura("texturas/cielo2.jpg");

    inicializarGrua(grua);

    float ultimoFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        deltaTime = lapsoDeltaTime(ultimoFrame);

        processInput(window);
        actualizarGrua(grua, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        DatosCamara camara = calcularCamara(grua, modoCamara);

        configurarCamaraShader(camara, shaderProgram, projection);
        configurarLucesGrua(grua, shaderProgram, luzEncendidaFoco, luzEncendidaBrazo);
        dibujarEscena(grua, shaderProgram, texturaSuelo, texturaArbusto, texturaFondo, camara.camPos);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    liberar();
    return 0;
}

void processInput(GLFWwindow* window)
{
    const float aceleracion = 10.0f;
    const float velocidadGiro = 80.0f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        grua.velocidad += aceleracion * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        grua.velocidad -= aceleracion * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && std::abs(grua.velocidad) > 0.05f)
        grua.direccion += velocidadGiro * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && std::abs(grua.velocidad) > 0.05f)
        grua.direccion -= velocidadGiro * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        modoCamara = 1;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        modoCamara = 2;

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        modoCamara = 3;

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        grua.brazo.rotacion.x -= 40.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        grua.brazo.rotacion.x += 40.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !teclaIPulsada) {
        luzEncendidaFoco = !luzEncendidaFoco;
        teclaIPulsada = true;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
        teclaIPulsada = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !teclaTPulsada) {
        luzEncendidaBrazo = !luzEncendidaBrazo;
        teclaTPulsada = true;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
        teclaTPulsada = false;
    }

    if (grua.brazo.rotacion.x < -60.0f)
        grua.brazo.rotacion.x = -60.0f;

    if (grua.brazo.rotacion.x > 40.0f)
        grua.brazo.rotacion.x = 40.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    projection = glm::perspective(
        glm::radians(45.0f),
        (float)width / (float)height,
        0.1f,
        100.0f
    );
}