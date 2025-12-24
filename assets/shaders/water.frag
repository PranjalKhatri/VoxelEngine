#version 330 core
out vec4 FragColor;

in vec3 Normal;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

uniform Light SunLight;

void main()
{
    vec4 waterColor = vec4(0.0, 0.45, 0.65, 0.25);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-SunLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);

    float faceBrightness = 1.0;
    if (abs(norm.x) > 0.5) faceBrightness = 0.8;
    if (abs(norm.z) > 0.5) faceBrightness = 0.6;
    if (norm.y < -0.5) faceBrightness = 0.5;

    vec3 ambient = SunLight.ambient * faceBrightness;
    vec3 diffuse = SunLight.diffuse * diff;

    vec3 finalColor = waterColor.rgb * (ambient + diffuse);
    FragColor = vec4(finalColor, waterColor.a);
}
