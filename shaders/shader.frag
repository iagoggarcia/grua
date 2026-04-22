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

uniform vec3 viewPos;

// Luz 1: foco frontal
uniform bool luz1Encendida;
uniform vec3 lightPos1;
uniform vec3 lightDir1;
uniform vec3 lightColor1;
uniform float cutOff1;
uniform float outerCutOff1;

// Luz 2: foco del brazo
uniform bool luz2Encendida;
uniform vec3 lightPos2;
uniform vec3 lightDir2;
uniform vec3 lightColor2;
uniform float cutOff2;
uniform float outerCutOff2;

vec3 calcularSpotlight(
    vec3 norm,
    vec3 fragPos,
    vec3 viewDirection,
    vec3 lightPos,
    vec3 lightDir,
    vec3 lightColor,
    float cutOff,
    float outerCutOff,
    vec3 colorBaseRGB
)
{
    vec3 lightDirection = normalize(lightPos - fragPos);

    float theta = dot(lightDirection, normalize(-lightDir));
    float epsilon = cutOff - outerCutOff;
    float intensidadFoco = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * colorBaseRGB;

    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    float specularStrength = 0.35;
    vec3 specular = specularStrength * spec * lightColor;

    diffuse *= intensidadFoco;
    specular *= intensidadFoco;

    return diffuse + specular;
}

void main()
{
    if (esFondo)
    {
        FragColor = texture(textura1, TexCoord);
        return;
    }

    vec4 colorBase;

    if (usarColorUniform)
    {
        colorBase = vec4(colorUniform, 1.0);
    }
    else if (usarTextura || usarTexturaSuelo || usarArbusto)
    {
        colorBase = texture(textura1, TexCoord);

        if (usarArbusto && colorBase.a < 0.1)
            discard;
    }
    else
    {
        colorBase = vec4(1.0, 1.0, 1.0, 1.0);
    }

    if (esFoco)
    {
        FragColor = colorBase;
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDirection = normalize(viewPos - FragPos);

    float ambientStrength = 0.65;
    vec3 ambient = ambientStrength * colorBase.rgb;

    vec3 iluminacion = ambient;

    if (luz1Encendida)
    {
        iluminacion += calcularSpotlight(
            norm,
            FragPos,
            viewDirection,
            lightPos1,
            lightDir1,
            lightColor1,
            cutOff1,
            outerCutOff1,
            colorBase.rgb
        );
    }

    if (luz2Encendida)
    {
        iluminacion += calcularSpotlight(
            norm,
            FragPos,
            viewDirection,
            lightPos2,
            lightDir2,
            lightColor2,
            cutOff2,
            outerCutOff2,
            colorBase.rgb
        );
    }

    FragColor = vec4(iluminacion, colorBase.a);
}