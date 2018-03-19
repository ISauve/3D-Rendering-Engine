#version 330 core

in vec3 color;

uniform vec3 lightColor;
uniform vec3 lightPos;

out vec4 outColor;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    outColor = vec4(ambient * color, 1.0);
}