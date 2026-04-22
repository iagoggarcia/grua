#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D textura1;

uniform bool usarColorUniform;
uniform vec3 colorUniform;

uniform bool usarTextura;
uniform bool usarTexturaSuelo;
uniform bool usarArbusto;
uniform bool esFondo;
uniform bool esFoco;

uniform bool luzEncendida;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform float cutOff;
uniform float outerCutOff;

void main()
{
    // Fondo: sin iluminación, solo textura
    if (esFondo)
    {
        FragColor = texture(textura1, TexCoord);
        return;
    }

    // Determinar color base
    vec4 colorBase;

    if (usarColorUniform)
    {
        colorBase = vec4(colorUniform, 1.0);
    }
    else if (usarTextura || usarTexturaSuelo || usarArbusto)
    {
        colorBase = texture(textura1, TexCoord);

        // Transparencia para arbusto
        if (usarArbusto && colorBase.a < 0.1)
            discard;
    }
    else
    {
        colorBase = vec4(1.0, 1.0, 1.0, 1.0);
    }

    // La esferita del foco debe verse brillante, sin iluminación extra
    if (esFoco)
    {
        FragColor = colorBase;
        return;
    }

    // Si la luz está apagada, dejar algo de luz ambiente para no ver negro absoluto
    vec3 norm = normalize(Normal);
    vec3 result;

    float ambientStrength = 0.65;
    vec3 ambient = ambientStrength * colorBase.rgb;

    if (!luzEncendida)
    {
        result = ambient;
        FragColor = vec4(result, colorBase.a);
        return;
    }

    // Spotlight
    vec3 lightDirection = normalize(lightPos - FragPos);

    float theta = dot(lightDirection, normalize(-lightDir));
    float epsilon = cutOff - outerCutOff;
    float intensidadFoco = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // Difusa
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * colorBase.rgb;

    // Especular
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    float specularStrength = 0.35;
    vec3 specular = specularStrength * spec * lightColor;

    diffuse *= intensidadFoco;
    specular *= intensidadFoco;

    result = ambient + diffuse + specular;

    FragColor = vec4(result, colorBase.a);
}