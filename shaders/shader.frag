#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform bool usarColorUniform;
uniform vec3 colorUniform;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float cutOff;
uniform float outerCutOff;

void main()
{
    vec3 objectColor;

    if (usarColorUniform)
        objectColor = colorUniform;
    else
        objectColor = vec3(0.15f, 0.5f, 0.15f); // color del suelo

    vec3 norm = normalize(Normal);

    // Luz ambiente general para que no se vea toda la escena negra
    float ambientI = 0.70f;
    vec3 ambient = ambientI * vec3(1.0f, 1.0f, 1.0f) * objectColor;

    // Dirección desde la luz al fragmento
    vec3 lightVector = normalize(lightPos - FragPos);

    // Ángulo respecto al eje del foco
    float theta = dot(normalize(-lightVector), normalize(lightDir));

    // Borde suave del cono
    float epsilon = cutOff - outerCutOff;
    float intensidad = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // Difusa del foco
    float diff = max(dot(norm, lightVector), 0.0);

    // Atenuación con la distancia
    float distancia = length(lightPos - FragPos);
    float atenuacion = 1.0 / (1.0 + 0.09 * distancia + 0.032 * distancia * distancia);

    vec3 diffuse = diff * intensidad * atenuacion * lightColor * objectColor * 2.5f;

    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}