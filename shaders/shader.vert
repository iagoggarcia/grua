#version 330 core

// Atributos que llegan desde el VAO:
// posición del vértice, normal y coordenadas de textura.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// Datos que enviamos al fragment shader interpolados para cada fragmento.
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

// Matrices de transformación enviadas desde C++.
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculamos la posición del vértice en coordenadas de mundo.
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transformamos la normal al espacio de mundo.
    // Se usa la matriz normal, que es la transpuesta de la inversa de model,
    // para mantener correcta la orientación de las normales.
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pasamos las coordenadas de textura al fragment shader.
    TexCoord = aTexCoord;

    // Calculamos la posición final del vértice en espacio de recorte,
    // aplicando primero view y después projection.
    gl_Position = projection * view * vec4(FragPos, 1.0);
}