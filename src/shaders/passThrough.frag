#version 330 core

in vec3 color;

out vec4 outColor;

void main() {
    //vec3 lightColor = vec3(0.5, 0.5, 0.5);  // white
    //float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * lightColor;
    //outColor = vec4(ambient * color, 1.0);

    outColor = vec4(color, 1.0);
}