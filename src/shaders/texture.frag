#version 330 core

in vec2 TexCoords;

out vec4 outColor;

uniform sampler2D textureSample;

void main() {
    outColor = texture(textureSample, TexCoords);
}