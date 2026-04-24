#include "texturas.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint cargarTextura(const char* ruta)
{
    GLuint textura;

    // Generamos y enlazamos un identificador de textura 2D.
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);

    // Configuramos el comportamiento de la textura:
    // repetición en los bordes y filtrado lineal con mipmaps.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Invertimos la imagen verticalmente al cargarla
    // para ajustarla al sistema de coordenadas de OpenGL.
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(ruta, &width, &height, &nrChannels, 0);

    if (data)
    {
        // Elegimos el formato de la textura según el número de canales de la imagen.
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        // Enviamos la imagen a OpenGL y generamos sus mipmaps.
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Si la textura tiene canal alfa, evitamos repetir los bordes
        // para que no aparezcan artefactos de transparencia.
        if (nrChannels == 4) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        // Mostramos por consola información útil sobre la textura cargada.
        std::cout << "Textura cargada correctamente: " << ruta << " (" << width << "x" << height << ", canales: " << nrChannels << ")" << std::endl;
    }
    else
    {
        // Informamos del error si la imagen no pudo cargarse.
        std::cout << "Error cargando textura: " << ruta << std::endl;
    }

    // Liberamos la memoria de la imagen cargada en CPU.
    stbi_image_free(data);

    // Devolvemos el identificador de la textura creada.
    return textura;
}