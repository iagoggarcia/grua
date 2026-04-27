#include "grua.h"
#include "esfera.h"
#include "cubo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

extern GLuint VAO_CUBO;
extern GLuint VAO_esfera;

struct ArbustoEscena {
    float x, y, z;
    float ancho, alto, grosor;
    float rotY;
};

//creamos esta funcion para poder reutilizarla en vez de crear la matriz cada vez que queramos crear una pieza
//le añadimos un flag para el escalado porque nos va a hacer falta para la articulacion y el brazo
static glm::mat4 crearMatrizModelo(const Pieza& pieza, int escala) {
    glm::mat4 model = glm::mat4(1.0f);

    //colocamos cada pieza en su sitio segun la inicializacion de la grua que hicimos
    //la escalamos si es necesario
    model = glm::translate(model, pieza.posicion);
    model = glm::rotate(model, glm::radians(pieza.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(pieza.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(pieza.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));
    if (escala == 1)
        model = glm::scale(model, pieza.escala);

    return model;
}

static glm::mat4 crearModeloGrua(const Grua& grua) {
    // Construimos el modelo global de la grúa en el mundo.
    glm::mat4 modeloGrua = glm::mat4(1.0f);

    // Colocamos la grúa en su posición global y aplicamos su orientación.
    modeloGrua = glm::translate(modeloGrua, grua.posicion);
    modeloGrua = glm::rotate(modeloGrua, glm::radians(grua.direccion), glm::vec3(0.0f, 1.0f, 0.0f));

    return modeloGrua;
}

static void usarColor(GLuint shaderProgram, float r, float g, float b) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), r, g, b);
}

static void dejarColor(GLuint shaderProgram) {
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);
}

static void enviarModelo(GLuint shaderProgram, const glm::mat4& model) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
}

static void dibujarCuboBase() {
    glBindVertexArray(VAO_CUBO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

static void dibujarEsferaBase() {
    glBindVertexArray(VAO_esfera);
    glDrawArrays(GL_TRIANGLES, 0, verticesEsferaSize / (8 * sizeof(float)));
}

// Esta función dibuja una pieza de la grúa usando la geometría del cubo.
// Recibe la transformación del padre para poder mantener el modelado jerárquico, y además el color con el que se quiere dibujar la pieza.

static void dibujarPieza(const Pieza& pieza, const glm::mat4& modeloPadre, GLuint shaderProgram, GLuint texturaGrua, float r, float g, float b) {

    glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarArbusto"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaGrua);
    glUniform1i(glGetUniformLocation(shaderProgram, "textura1"), 0);

    // Calculamos la matriz modelo final de la pieza.
    // A la transformación local de la pieza le aplicamos antes la transformación acumulada del padre, de forma que la pieza quede colocada respecto a él y herede su posición y orientación.
    glm::mat4 model = modeloPadre * crearMatrizModelo(pieza, 1);

    // Enviamos al shader la matriz modelo para colocar correctamente la pieza en la escena.
    enviarModelo(shaderProgram, model);

    // Dibujamos la pieza usando la geometría base del cubo.
    dibujarCuboBase(); // 36 vértices forman los 12 triángulos del cubo

    // Restauramos el shader para que no siga usando este color uniforme en los siguientes objetos que se dibujen.
    dejarColor(shaderProgram);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 0);
}

static glm::mat4 crearBaseRueda(const glm::vec3& posicionLocal, float giroruedas, const glm::mat4& modeloPadre) {
    // Partimos de la transformación acumulada del padre para que la rueda
    // herede correctamente su posición y orientación.
    glm::mat4 baseRueda = modeloPadre;

    // Colocamos la rueda en su posición local respecto al padre
    // y aplicamos su giro sobre su eje.
    baseRueda = glm::translate(baseRueda, posicionLocal);
    baseRueda = glm::rotate(baseRueda, glm::radians(giroruedas), glm::vec3(1.0f, 0.0f, 0.0f));

    return baseRueda;
}

static void dibujarMarcaRueda(const glm::mat4& baseRueda, GLuint shaderProgram) {
    // Cambiamos el color a rojo para dibujar una marca visual
    // que permita apreciar mejor el giro de la rueda.
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 1.0f, 0.0f, 0.0f);

    // La marca parte de la misma base transformada de la rueda,
    // por lo que hereda tanto su posición como su rotación.
    glm::mat4 modelMarca = baseRueda;
    modelMarca = glm::translate(modelMarca, glm::vec3(0.0f, 0.55f, 0.0f));
    modelMarca = glm::scale(modelMarca, glm::vec3(0.12f, 0.12f, 0.12f));

    // Enviamos al shader la matriz modelo de la marca.
    enviarModelo(shaderProgram, modelMarca);

    // Dibujamos la marca como un cubo pequeño.
    dibujarCuboBase();
}

static void dibujarRueda(const glm::vec3& posicionLocal, float giroruedas, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b) {
    glm::mat4 baseRueda = crearBaseRueda(posicionLocal, giroruedas, modeloPadre);

    // Indicamos al shader que vamos a usar un color uniforme
    // y le pasamos el color de la rueda.
    usarColor(shaderProgram, r, g, b);

    // Construimos la matriz final de la rueda escalando la esfera
    // para darle el tamaño y la forma deseados.
    glm::mat4 modelRueda = baseRueda;
    modelRueda = glm::scale(modelRueda, glm::vec3(0.22f, 0.50f, 0.50f));

    // Enviamos la matriz modelo de la rueda al shader.
    enviarModelo(shaderProgram, modelRueda);

    // Dibujamos la rueda usando la geometría de la esfera.
    // El número de vértices se obtiene a partir del tamaño total del array,
    // ya que cada vértice ocupa 8 floats.
    dibujarEsferaBase();

    // dibujarMarcaRueda(baseRueda, shaderProgram);

    // Restauramos el shader para que no siga usando color uniforme
    // en los siguientes objetos que se dibujen.
    dejarColor(shaderProgram);
}

static void aplicarRozamiento(Grua& grua, float deltaTime) {

    // Reducimos progresivamente la velocidad de la grúa en función del tiempo
    // para simular el efecto del rozamiento.
    grua.velocidad *= (1.0f - 1.5f * deltaTime);

    // Si la velocidad es ya muy pequeña, la fijamos a 0 para evitar
    // que siga disminuyendo indefinidamente sin llegar nunca a pararse.
    if (std::abs(grua.velocidad) < 0.05f)
        grua.velocidad = 0.0f;
}

static void limitarGruaAlSuelo(Grua& grua) {
    const float limiteX = 28.0f;
    const float limiteZ = 28.0f;

    // Limitamos la posición de la grúa para que no pueda salir
    // de la zona del suelo definida en los ejes X y Z.
    if (grua.posicion.x < -limiteX) grua.posicion.x = -limiteX;
    if (grua.posicion.x >  limiteX) grua.posicion.x =  limiteX;
    if (grua.posicion.z < -limiteZ) grua.posicion.z = -limiteZ;
    if (grua.posicion.z >  limiteZ) grua.posicion.z =  limiteZ;
}

void inicializarGrua(Grua& grua) {
    // Inicializamos el estado global de la grúa:
    // posición en el mundo, dirección y velocidad inicial.
    grua.posicion = glm::vec3(0.0f, 0.5f, 2.0f);
    grua.direccion = 0.0f;
    grua.velocidad = 0.0f;

    // Inicializamos la base de la grúa.
    grua.base.posicion = glm::vec3(0.0f, -0.55f, 0.0f);
    grua.base.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.base.escala = glm::vec3(2.2f, 0.7f, 4.2f);

    // Inicializamos la cabina.
    grua.cabina.posicion = glm::vec3(0.0f, 0.10f, 1.45f);
    grua.cabina.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.cabina.escala = glm::vec3(2.2f, 0.9f, 1.0f);

    // Inicializamos la articulación vertical que une la base con el brazo.
    grua.articulacion.posicion = glm::vec3(0.0f, -0.25f, 0.0f);
    grua.articulacion.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.articulacion.escala = glm::vec3(0.55f, 0.35f, 0.55f);

    // Inicializamos el brazo de la grúa.
    grua.brazo.posicion = glm::vec3(0.0f, 0.25f, 0.0f);
    grua.brazo.rotacion = glm::vec3(0.0f, 0.0f, 0.0f);
    grua.brazo.escala = glm::vec3(0.25f, 3.2f, 0.25f);

    // El giro inicial de las ruedas es nulo.
    grua.giroruedas = 0.0f;
}

static void dibujarFocoFrontal(const Grua& grua, const glm::mat4& modeloGrua, GLuint shaderProgram) {
    // Indicamos al shader que este objeto se dibujará con un color uniforme
    // y que debe tratarse como el foco visible de la grúa.
    usarColor(shaderProgram, 0.75f, 0.75f, 0.65f);
    glUniform1i(glGetUniformLocation(shaderProgram, "esFoco"), 1);

    // Color del foco visible.
    glUniform3f(glGetUniformLocation(shaderProgram, "colorUniform"), 0.75f, 0.75f, 0.65f);

    // Partimos del modelo global de la grúa para que el foco
    // herede su posición y orientación.
    glm::mat4 model = modeloGrua;

    // Colocamos el foco en la parte frontal de la cabina.
    model = glm::translate(model, glm::vec3(
        0.0f,
        grua.cabina.posicion.y - 0.10f,
        grua.cabina.posicion.z + grua.cabina.escala.z / 2.0f + 0.16f
    ));

    // Escalamos la esfera para darle la forma y tamaño del foco.
    model = glm::scale(model, glm::vec3(0.22f, 0.22f, 0.08f));

    // Enviamos la matriz modelo al shader.
    enviarModelo(shaderProgram, model);

    // Dibujamos el foco usando la geometría de la esfera.
    dibujarEsferaBase();

    // Restauramos el estado del shader para no afectar al resto de objetos.
    glUniform1i(glGetUniformLocation(shaderProgram, "esFoco"), 0);
    dejarColor(shaderProgram);
}


// Dibujamos el brazo aparte porque no solo depende de la transformación global de la grúa,
// sino también de la articulación, ya que está unido a ella dentro del modelado jerárquico.
// En este caso usamos el modelo de la articulación sin su escala para evitar que ese escalado
// se herede en el brazo y deforme su movimiento o su tamaño.
static void dibujarBrazo(const Pieza& brazo, const glm::mat4& modeloArticulacion, GLuint shaderProgram, float r, float g, float b) {
    // Indicamos al shader que use un color uniforme y le pasamos el color del brazo.
    usarColor(shaderProgram, r, g, b);

    // Partimos del modelo de la articulación para que el brazo herede su posición y orientación.
    glm::mat4 model = modeloArticulacion;

    // Aplicamos la transformación local del brazo.
    model = glm::translate(model, brazo.posicion);
    model = glm::rotate(model, glm::radians(brazo.rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(brazo.rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(brazo.rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Desplazamos el cubo medio brazo hacia arriba para que el giro y la unión
    // con la articulación queden colocados correctamente en un extremo.
    model = glm::translate(model, glm::vec3(0.0f, brazo.escala.y / 2.0f, 0.0f));

    // Escalamos el cubo para darle la forma final del brazo.
    model = glm::scale(model, brazo.escala);

    // Enviamos al shader la matriz modelo final del brazo.
    enviarModelo(shaderProgram, model);

    // Dibujamos el brazo usando la geometría del cubo.
    dibujarCuboBase();

    // Restauramos el shader para que no siga usando color uniforme en otros objetos.
    dejarColor(shaderProgram);
}

static void dibujarArbusto(GLuint shaderProgram, GLuint texturaArbusto, float x, float y, float z, float ancho, float alto, float grosor, float rotY)
{
    // Usamos la geometría del cubo como base para representar el arbusto.
    glBindVertexArray(VAO_CUBO);

    // Activamos en el shader el modo de textura para arbustos
    // y desactivamos otros modos de dibujado.
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTexturaSuelo"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarArbusto"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarColorUniform"), 0);

    // Asociamos la textura del arbusto a la unidad de textura 0.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaArbusto);
    glUniform1i(glGetUniformLocation(shaderProgram, "textura1"), 0);

    // Construimos la matriz modelo del arbusto a partir de su posición,
    // su rotación sobre el eje Y y su escala.
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(ancho, alto, grosor));

    // Enviamos la matriz modelo al shader y dibujamos el arbusto.
    enviarModelo(shaderProgram, model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Restauramos el estado para no afectar al resto de objetos.
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarArbusto"), 0);
}

static void prepararShaderGrua(GLuint shaderProgram) {
    // Desactivamos los modos de texturizado y de arbusto para que,
    // al dibujar la grúa, el shader use únicamente color e iluminación.
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarTexturaSuelo"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "usarArbusto"), 0);
}

static void dibujarMediaEsferaArticulacion(const Pieza& articulacion, const glm::mat4& modeloPadre, GLuint shaderProgram, float r, float g, float b) {
    usarColor(shaderProgram, r, g, b);

    glm::mat4 model = modeloPadre * crearMatrizModelo(articulacion, 0);
    model = glm::scale(model, articulacion.escala);

    enviarModelo(shaderProgram, model);
    dibujarEsferaBase();

    dejarColor(shaderProgram);
}

static void dibujarPiezasPrincipales(const Grua& grua, const glm::mat4& modeloGrua, GLuint texturaGrua, GLuint shaderProgram) {
    // Dibujamos las piezas que dependen directamente del modelo global de la grúa.
    dibujarPieza(grua.base, modeloGrua, shaderProgram, texturaGrua, 0.9f, 0.0f, 0.0f);
    dibujarPieza(grua.cabina, modeloGrua, shaderProgram, texturaGrua, 0.9f, 0.0f, 0.0f);
    dibujarFocoFrontal(grua, modeloGrua, shaderProgram);
    dibujarMediaEsferaArticulacion(grua.articulacion, modeloGrua, shaderProgram, 0.0f, 1.0f, 1.0f);
}

static void dibujarBrazoGrua(const Grua& grua, const glm::mat4& modeloGrua, GLuint shaderProgram) {
    // Calculamos el modelo de la articulación sin aplicar su escala,
    // para que el brazo herede su posición y rotación pero no quede deformado.
    glm::mat4 modeloArt = modeloGrua * crearMatrizModelo(grua.articulacion, 0);

    // Dibujamos el brazo como hijo de la articulación.
    dibujarBrazo(grua.brazo, modeloArt, shaderProgram, 0.9f, 0.7f, 0.0f);
}

static void dibujarRuedasGrua(const Grua& grua, const glm::mat4& modeloGrua, GLuint shaderProgram) {
    const glm::vec3 posicionesRuedas[] = {
        glm::vec3(-1.25f, -1.0f,  1.55f),
        glm::vec3( 1.25f, -1.0f,  1.55f),
        glm::vec3(-1.25f, -1.0f, -1.55f),
        glm::vec3( 1.25f, -1.0f, -1.55f)
    };

    // Dibujamos las cuatro ruedas en sus posiciones locales respecto a la grúa,
    // aplicándoles el giro acumulado para simular el movimiento.
    for (const glm::vec3& posicion : posicionesRuedas)
        dibujarRueda(posicion, grua.giroruedas, modeloGrua, shaderProgram, 0.05f, 0.05f, 0.05f);
}

void dibujarGrua(const Grua& grua, GLuint shaderProgram, GLuint texturaGrua) {
    prepararShaderGrua(shaderProgram);

    glm::mat4 modeloGrua = crearModeloGrua(grua);

    dibujarPiezasPrincipales(grua, modeloGrua, texturaGrua, shaderProgram);
    dibujarBrazoGrua(grua, modeloGrua, shaderProgram);
    dibujarRuedasGrua(grua, modeloGrua, shaderProgram);
}

static void dibujarArbustosEscena(GLuint shaderProgram, GLuint texturaArbusto) {
    const ArbustoEscena arbustos[] = {
        {-18.0f, 0.0f, -12.0f, 3.4f, 3.0f, 0.05f,   0.0f},
        {-18.0f, 0.0f, -12.0f, 3.4f, 3.0f, 0.05f,  90.0f},
        { 16.0f, 0.0f,  10.0f, 3.2f, 2.8f, 0.05f,  20.0f},
        { 16.0f, 0.0f,  10.0f, 3.2f, 2.8f, 0.05f, 110.0f},
        {-26.0f, 0.0f,  15.0f, 3.3f, 2.9f, 0.05f,  15.0f},
        {-26.0f, 0.0f,  15.0f, 3.3f, 2.9f, 0.05f, 105.0f},
        { 22.0f, 0.0f, -18.0f, 3.6f, 3.0f, 0.05f,  35.0f},
        { 22.0f, 0.0f, -18.0f, 3.6f, 3.0f, 0.05f, 125.0f},
        { -8.0f, 0.0f,  24.0f, 3.0f, 2.7f, 0.05f,  10.0f},
        { -8.0f, 0.0f,  24.0f, 3.0f, 2.7f, 0.05f, 100.0f},
        { 28.0f, 0.0f,  20.0f, 3.5f, 3.1f, 0.05f,  25.0f},
        { 28.0f, 0.0f,  20.0f, 3.5f, 3.1f, 0.05f, 115.0f},
        {-30.0f, 0.0f, -22.0f, 3.2f, 2.9f, 0.05f,  40.0f},
        {-30.0f, 0.0f, -22.0f, 3.2f, 2.9f, 0.05f, 130.0f},
        {  8.0f, 0.0f, -26.0f, 3.3f, 2.8f, 0.05f,   5.0f},
        {  8.0f, 0.0f, -26.0f, 3.3f, 2.8f, 0.05f,  95.0f},
        {-22.0f, 0.0f,  30.0f, 3.4f, 3.0f, 0.05f,  30.0f},
        {-22.0f, 0.0f,  30.0f, 3.4f, 3.0f, 0.05f, 120.0f},
        { 30.0f, 0.0f,  -6.0f, 3.1f, 2.7f, 0.05f,  50.0f},
        { 30.0f, 0.0f,  -6.0f, 3.1f, 2.7f, 0.05f, 140.0f}
    };

    for (const ArbustoEscena& arbusto : arbustos) {
        dibujarArbusto(
            shaderProgram,
            texturaArbusto,
            arbusto.x,
            arbusto.y,
            arbusto.z,
            arbusto.ancho,
            arbusto.alto,
            arbusto.grosor,
            arbusto.rotY
        );
    }
}

void dibujarEscena(const Grua& grua, GLuint shaderProgram, GLuint texturaSuelo, GLuint texturaArbusto, GLuint texturaGrua, GLuint texturaFondo, glm::vec3 camPos) {
    crearFondo(texturaFondo, camPos, shaderProgram);
    crearSuelo(shaderProgram, texturaSuelo);
    dibujarGrua(grua, shaderProgram, texturaGrua);
    dibujarArbustosEscena(shaderProgram, texturaArbusto);
}

void actualizarGrua(Grua& grua, float deltaTime) {
    // Convertimos la dirección de la grúa a radianes para usar funciones trigonométricas.
    float rad = glm::radians(grua.direccion);

    // Actualizamos la posición de la grúa en el plano XZ según su dirección,
    // su velocidad actual y el tiempo transcurrido entre frames.
    grua.posicion.x += std::sin(rad) * grua.velocidad * deltaTime;
    grua.posicion.z += std::cos(rad) * grua.velocidad * deltaTime;

    // Actualizamos el giro visual de las ruedas en función de la velocidad de desplazamiento.
    const float factorGiroRueda = 120.0f;
    grua.giroruedas += grua.velocidad * factorGiroRueda * deltaTime;

    // Aplicamos rozamiento para que la grúa vaya perdiendo velocidad progresivamente.
    aplicarRozamiento(grua, deltaTime);

    // Limitamos su movimiento para que no pueda salir de la zona del suelo.
    limitarGruaAlSuelo(grua);
}
