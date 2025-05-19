#version 460

layout(location = 0) in vec2 Texcoord;
layout(location = 1) in vec4 Color;

layout(location = 0) out vec4 FragColor;

layout(set = 2, binding = 0) uniform sampler2D Texture;

void main() {
    FragColor = Color * texture(Texture, Texcoord);
}
