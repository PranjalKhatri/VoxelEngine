#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main()
{
    FragColor = texture(tex1, TexCoord);
    // FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    // FragColor = mix(texture(tex1, TexCoord), texture(tex2, TexCoord), 0.2);
}
