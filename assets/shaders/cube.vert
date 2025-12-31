#version 330 core

layout(location = 0) in uint aencodedData;
layout(location = 1) in vec4 avertexColor;

out vec2 TexCoord;
flat out uint BlockType;
out vec3 Normal;
flat out vec4 vertexColor;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 uChunkOffset;

const uint X_MASK = 0x1Fu; // 5 bits
const uint Y_MASK = 0x7Fu; // 7 bits
const uint Z_MASK = 0x1Fu; // 5 bits
const uint N_MASK = 0x7u; // 3 bits
const uint ID_MASK = 0xFFFu; // 12 bits

const uint X_SHIFT = 0u;
const uint Y_SHIFT = 5u;
const uint Z_SHIFT = 12u;
const uint N_SHIFT = 17u;
const uint ID_SHIFT = 20u;

const vec3 normals[6] = vec3[](
        vec3(0.0, 1.0, 0.0), // 0: Top
        vec3(0.0, -1.0, 0.0), // 1: Bottom
        vec3(0.0, 0.0, -1.0), // 2: North (-Z)
        vec3(0.0, 0.0, 1.0), // 3: South (+Z)
        vec3(1.0, 0.0, 0.0), // 4: West (+X)
        vec3(-1.0, 0.0, 0.0) // 5: East (-X)
    );

void main()
{
    uint ux = (aencodedData >> X_SHIFT) & X_MASK;
    uint uy = (aencodedData >> Y_SHIFT) & Y_MASK;
    uint uz = (aencodedData >> Z_SHIFT) & Z_MASK;
    uint un = (aencodedData >> N_SHIFT) & N_MASK;
    uint uid = (aencodedData >> ID_SHIFT) & ID_MASK;

    vec3 aPos = vec3(float(ux), float(uy), float(uz));
    vec3 worldPos = aPos + uChunkOffset;
    int face = int(un);

    Normal = normals[face];
    BlockType = uid;

    gl_Position = projection * view * vec4(worldPos, 1.0);

    if (face == 4 || face == 5)
        TexCoord = aPos.zy;
    else if (face == 0 || face == 1)
        TexCoord = aPos.xz;
    else
        TexCoord = aPos.xy;

    vertexColor = avertexColor;
}
