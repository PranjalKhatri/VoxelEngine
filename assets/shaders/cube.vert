#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 uChunkOffset;

void main()
{
    vec3 worldPos = aPos + uChunkOffset;
    gl_Position = projection * view * vec4(worldPos, 1.0);
    TexCoord = aTexCoord;
}
