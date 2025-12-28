#version 330 core

layout(location = 0) in uint aencodedData;

out vec2 TexCoord;
flat out uint BlockType;
out vec3 Normal;

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

void main()
{
    uint ux = (aencodedData >> X_SHIFT) & X_MASK;
    uint uy = (aencodedData >> Y_SHIFT) & Y_MASK;
    uint uz = (aencodedData >> Z_SHIFT) & Z_MASK;
    uint un = (aencodedData >> N_SHIFT) & N_MASK;
    uint uid = (aencodedData >> ID_SHIFT) & ID_MASK;

    vec3 aPos = vec3(float(ux), float(uy), float(uz));
    BlockType = uid;

    vec3 worldPos = aPos + uChunkOffset;
    gl_Position = projection * view * vec4(worldPos, 1.0);

    int face = int(un);
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
}
