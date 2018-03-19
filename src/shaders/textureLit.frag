#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 WorldCoords;

out vec4 outColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform sampler2D textureSample;

// implements the Phong lighting model
void main() {
    /* 1) Ambient lighting */
    float ambientStr = 0.3;
    vec3 ambient = ambientStr * lightColor;

    /* 2) Diffuse lighting */
    // Calculate direction between light source and fragment's position (world position)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - WorldCoords);

    // Calculate the diffuse light "impact" by taking the dot product of lightDir & the surface normal
    float diffStr = dot(norm, lightDir);
    diffStr = max(diffStr, 0.0);    // make sure the value is not negative
    vec3 diffuse = diffStr * lightColor;

    /* 2) Specular lighting */

    vec4 lighting = vec4(ambient + diffuse, 1.0);
    outColor = texture(textureSample, TexCoords) * lighting;
}