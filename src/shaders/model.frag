#version 330 core

// Lighting data
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;
in vec2 TexCoords2D;
in vec3 WorldCoords;

uniform sampler2D diffuse_texture_0;
uniform sampler2D specular_texture_0;
// add more when necessary

out vec4 outColor;

void main() {
    // If the object is not lit, just reture the diffuse texture color
    if (lightColor == vec3(0.0)) {
        outColor = texture(diffuse_texture_0, TexCoords2D);
        return;
    }

    /* Configurable parameters */
    float ambientStr = 0.4;
    float diffStr = 1.0;
    float specularStrength = 1.2;
    float specularShine = 32;

    /* 1) Ambient lighting */
    vec4 ambient = ambientStr * vec4(lightColor, 1.0) * texture(diffuse_texture_0, TexCoords2D);

    /* 2) Diffuse lighting */
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - WorldCoords);
    float diffImpact = dot(norm, lightDir);
    diffImpact = max(diffImpact, 0.0);
    vec4 diffuse = diffStr * diffImpact * vec4(lightColor, 1.0) * texture(diffuse_texture_0, TexCoords2D);

    /* 3) Specular lighting */
    vec3 viewDir = normalize(viewPos - WorldCoords);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specImpact = max(dot(viewDir, reflectDir), 0.0);
    specImpact = pow(specImpact, specularShine);
    vec4 specular = specularStrength * specImpact * vec4(lightColor, 1.0) * texture(specular_texture_0, TexCoords2D);

    outColor = ambient + diffuse + specular;
}