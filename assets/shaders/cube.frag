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

    if (texColor.a < 0.1)
        discard;

    // Ambient
    vec3 ambient = SunLight.ambient * texColor.rgb;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-SunLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = SunLight.diffuse * diff * texColor.rgb;

    vec3 result = ambient + diffuse;
    FragColor = vec4(result, texColor.a);
}
