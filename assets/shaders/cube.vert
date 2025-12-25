#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aNormal;
layout(location = 3) in float aBlockType;

out vec2 TexCoord;
out float BlockType;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 uChunkOffset;

void main()
{
    vec3 worldPos = aPos + uChunkOffset;
    gl_Position = projection * view * vec4(worldPos, 1.0);
    float fx = fract(worldPos.x);
    float fy = fract(worldPos.y);
    float fz = fract(worldPos.z);

    int face = int(aNormal + 0.5);
    if (face == 0) Normal = vec3(0, 1, 0); // Top
    else if (face == 1) Normal = vec3(0, -1, 0); // Bottom
    else if (face == 2) Normal = vec3(0, 0, -1); // North
    else if (face == 3) Normal = vec3(0, 0, 1); // South
    else if (face == 4) Normal = vec3(1, 0, 0); // West
    else Normal = vec3(-1, 0, 0); // East
    if (face == 4 || face == 5)
        TexCoord = aPos.zy;
    else if (face == 0 || face == 1)
        TexCoord = aPos.xz;
    else
        TexCoord = aPos.xy;
    BlockType = aBlockType;
}
