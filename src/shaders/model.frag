#version 330 core

in vec3 Normal;
in vec2 TexCoords2D;

uniform sampler2D diffuse_texture_0;
uniform sampler2D specular_texture_0;
// add more when necessary

out vec4 outColor;

void main() {
    outColor = texture(diffuse_texture_0, TexCoords2D); // + texture(specular_texture_0, TexCoords2D);
    //outColor = vec4(Normal, 1.0);
}