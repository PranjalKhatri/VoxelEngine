#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float BlockType;
in vec3 Normal;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

uniform Light SunLight;
uniform sampler2D textureAtlas;

void main()
{
    float totalTiles = 6.0;
    float tileHeight = 1.0 / totalTiles;

    float localX = fract(TexCoord.x);
    float localY = fract(TexCoord.y);

    float vOffset = BlockType * tileHeight;
    vec2 finalUV = vec2(localX, vOffset + (localY * tileHeight));

    vec4 texColor = texture(textureAtlas, finalUV);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-SunLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);

    float faceBrightness = 1.0;
    if (abs(norm.x) > 0.5) faceBrightness = 0.8; // East/West are slightly darker
    if (abs(norm.z) > 0.5) faceBrightness = 0.6; // North/South are darker
    if (norm.y < -0.5) faceBrightness = 0.5; // Bottom is darkest
    // Top (Y > 0.5) stays at 1.0 (brightest)

    vec3 ambient = SunLight.ambient * faceBrightness;
    vec3 diffuse = SunLight.diffuse * diff;

    vec3 finalColor = texColor.rgb * (ambient + diffuse);
    FragColor = vec4(finalColor, 1.0);
}
