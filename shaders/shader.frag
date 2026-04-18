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

uniform bool esFoco;
uniform bool luzEncendida;

void main()
{
    // La esferita del foco se ve brillante cuando la luz está encendida
    if (esFoco) {
        vec3 colorFocoApagado = colorUniform;
        vec3 colorFocoEncendido = vec3(1.0f, 1.0f, 0.85f);

        if (luzEncendida)
            FragColor = vec4(colorFocoEncendido, 1.0f);
        else
            FragColor = vec4(colorFocoApagado, 1.0f);

        return;
    }

    vec3 objectColor;

    if (usarColorUniform)
        objectColor = colorUniform;
    else
        objectColor = vec3(0.15f, 0.5f, 0.15f);

    vec3 norm = normalize(Normal);

    // luz ambiente general
    float ambientI = 0.7f;
    vec3 ambient = ambientI * vec3(1.0f, 1.0f, 1.0f) * objectColor;

    vec3 lightVector = normalize(lightPos - FragPos);

    float theta = dot(normalize(-lightVector), normalize(lightDir));

    float epsilon = cutOff - outerCutOff;
    float intensidad = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(norm, lightVector), 0.0);

    float distancia = length(lightPos - FragPos);
    float atenuacion = 1.0 / (1.0 + 0.09f * distancia + 0.032f * distancia * distancia);

    vec3 diffuse = diff * intensidad * atenuacion * lightColor * objectColor * 2.5f;

    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0f);
}