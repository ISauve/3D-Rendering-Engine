#version 330 core

in vec2 TexCoords;

out vec4 outColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform sampler2D textureSample;

void main() {
    if (lightColor != vec3(0.0f)) {
        float ambientStr = 0.5;
        vec4 ambient = vec4(ambientStr * lightColor.x, ambientStr * lightColor.y, ambientStr * lightColor.z, 1.0);

        // TODO: diffuse & specular lighting

        vec4 lighting = ambient;

         outColor = texture(textureSample, TexCoords) * lighting;
    } else {
        // No lighting has been specified
        outColor = texture(textureSample, TexCoords);
    }
}