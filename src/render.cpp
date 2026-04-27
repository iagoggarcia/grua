#include "render.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "lecturaShader_0_9.h"
#include "cubo.h"
#include "esfera.h"
#include "camaras.h"
#include "texturas.h"
#include "luces.h"

static glm::mat4 projection;

GLFWwindow* inicializarVentana(unsigned int ancho, unsigned int alto, const char* titulo) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(ancho, alto, titulo, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error creando ventana" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error inicializando GLAD" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    return window;
}

void openGlInit() {
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

glm::mat4 obtenerProjection() {
    return projection;
}

void cargarRecursos(RecursosRender& recursos) {
    crearCubo();
    crearEsfera();

    recursos.shaderProgram = setShaders("shaders/shader.vert", "shaders/shader.frag");

    recursos.texturaGrua = cargarTextura("texturas/grua.jpg");
    recursos.texturaSuelo = cargarTextura("texturas/hierba.jpg");
    recursos.texturaArbusto = cargarTextura("texturas/arbusto.png");
    recursos.texturaFondo = cargarTextura("texturas/cielo2.jpg");
}

void renderizarFrame(const Grua& grua, const EstadoEntrada& estado, const RecursosRender& recursos) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(recursos.shaderProgram);

    DatosCamara camara = calcularCamara(grua, estado.modoCamara);

    configurarCamaraShader(camara, recursos.shaderProgram, projection);
    configurarLucesGrua(grua, recursos.shaderProgram, estado.luzEncendidaFoco, estado.luzEncendidaBrazo);
    dibujarEscena(grua, recursos.shaderProgram, recursos.texturaSuelo, recursos.texturaArbusto, recursos.texturaGrua, recursos.texturaFondo, camara.camPos);
}

void liberar(const RecursosRender& recursos){
    liberarEsfera();
    liberarCubo();
    glDeleteProgram(recursos.shaderProgram);
    glfwTerminate();
}
