#version 330 core

in vec3 color;

out vec4 outColor;

uniform float transparency;

void main() {
    outColor = vec4(color, transparency);
}