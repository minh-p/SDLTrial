#version 460

// Only accepting positive values what's why we're using uint.
const uint triangleIndices[6] = uint[6](0, 1, 2, 3, 2, 1);
// these vertices will be transformed to obtain final quad geometry.
const vec2 vertexPos[4] = vec2[4](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, 1.0f),
    vec2(1.0f, 1.0f)
);

// Must Follow GLSL std140
struct SpriteData {
    vec3 Position;
    float Rotation;
    vec2 Scale;
    // The padding needed for std140. Must align to 16 bytes.
    vec2 Padding;
    float TexU, TexV, TexW, TexH;
    vec4 Color;
};

layout(std140, binding = 0, set = 0) buffer SpriteBuffer {
    // Compared to a uniform buffer, we don't have to specify instance count.
    // The equivalent of a StructuredBuffer (HLSL) in GLSL is Shader Storage Buffer Object.
    // https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object.
    // There can only be on array of variable size.
    // It has to be the bottommost variable.
    SpriteData DataBuffer[];
};

// Used to transfrom the vertex position from world space to screen space.
layout(std140, binding = 0, set = 1) uniform UniformBlock {
    // In GLSL, variable starts automatically at offset 0.
    mat4 ViewProjectionMatrix;
};

layout (location = 0) out vec2 Texcoord;
layout (location = 1) out vec4 Color;


void main() {
    // The only input we have is the id. It's implicitly defined.
    // by glDrawArrays.
    // This an id that ranges from 0 to the number of vertices in the draw call.
    uint id = uint(gl_VertexIndex);
    // Integer division by 6 gives an index to use with the DataBuffer.
    uint spriteIndex = id / 6;

    // Divisible gives texcoords index.
    // e.g. 0 is top left, 1 is top-right, 2 is bottom-left and 3 is bottom-right.
    uint vert = uint(triangleIndices[id % 6]);

    SpriteData sprite = DataBuffer[spriteIndex];

    vec2 texcoord[4] = vec2[4](
        vec2(sprite.TexU,               sprite.TexV),
        vec2(sprite.TexU + sprite.TexW, sprite.TexV),
        vec2(sprite.TexU,               sprite.TexV + sprite.TexH),
        vec2(sprite.TexU + sprite.TexW, sprite.TexV + sprite.TexH)
    );

    float c = cos(sprite.Rotation);
    float s = sin(sprite.Rotation);

    mat2 rotation = mat2(c, s, -s, c);
    vec2 coord = rotation * (sprite.Scale * vertexPos[vert]);
    vec3 coordWithDepth = vec3(coord + sprite.Position.xy, sprite.Position.z);

    gl_Position = ViewProjectionMatrix * vec4(coordWithDepth, 1.0);
    Texcoord = texcoord[vert];
    Color = sprite.Color;
}
