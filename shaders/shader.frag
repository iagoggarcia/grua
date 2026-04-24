#version 330 core

// Datos interpolados que llegan desde el vertex shader.
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// Textura activa usada cuando el objeto es texturizado.
uniform sampler2D textura1;

// Color uniforme para objetos que no usan textura.
uniform bool usarColorUniform;
uniform vec3 colorUniform;

// Flags que indican cómo debe dibujarse el fragmento.
uniform bool usarTextura;
uniform bool usarTexturaSuelo;
uniform bool usarArbusto;
uniform bool esFondo;
uniform bool esFoco;

// Posición de la cámara, necesaria para calcular el componente especular.
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

// Calcula la contribución de un spotlight sobre el fragmento actual.
// Devuelve la suma del componente difuso y especular.
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
    // Dirección desde el fragmento hacia la luz.
    vec3 lightDirection = normalize(lightPos - fragPos);

    // Comprobamos cuánto entra el fragmento dentro del cono del foco.
    float theta = dot(lightDirection, normalize(-lightDir));
    float epsilon = cutOff - outerCutOff;
    float intensidadFoco = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // Componente difuso de Phong.
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * colorBaseRGB;

    // Componente especular de Phong.
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    float specularStrength = 0.35;
    vec3 specular = specularStrength * spec * lightColor;

    // Atenuamos ambas componentes según la intensidad del cono.
    diffuse *= intensidadFoco;
    specular *= intensidadFoco;

    return diffuse + specular;
}

void main()
{
    // Si estamos dibujando el fondo, simplemente usamos la textura
    // sin aplicar iluminación.
    if (esFondo)
    {
        FragColor = texture(textura1, TexCoord);
        return;
    }

    vec4 colorBase;

    // Elegimos el color base del objeto según el modo de dibujado.
    if (usarColorUniform)
    {
        colorBase = vec4(colorUniform, 1.0);
    }
    else if (usarTextura || usarTexturaSuelo || usarArbusto)
    {
        colorBase = texture(textura1, TexCoord);

        // En el arbusto descartamos fragmentos casi transparentes
        // para simular recorte de la textura.
        if (usarArbusto && colorBase.a < 0.1)
            discard;
    }
    else
    {
        // Color por defecto si no se usa ni textura ni color uniforme.
        colorBase = vec4(1.0, 1.0, 1.0, 1.0);
    }

    // Si estamos dibujando el foco visible, no aplicamos iluminación:
    // mostramos directamente su color.
    if (esFoco)
    {
        FragColor = colorBase;
        return;
    }

    // Normalizamos la normal y calculamos la dirección hacia la cámara.
    vec3 norm = normalize(Normal);
    vec3 viewDirection = normalize(viewPos - FragPos);

    // Componente ambiental global.
    float ambientStrength = 0.65;
    vec3 ambient = ambientStrength * colorBase.rgb;

    // Partimos de la iluminación ambiental.
    vec3 iluminacion = ambient;

    // Añadimos la contribución del foco frontal si está encendido.
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

    // Añadimos la contribución de la luz del brazo si está encendida.
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

    // Color final del fragmento, conservando el alfa original.
    FragColor = vec4(iluminacion, colorBase.a);
}