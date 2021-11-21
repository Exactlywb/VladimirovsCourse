#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 viewPos;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightPos = vec3(1.0, 1.0, 1.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    float ambient_strength = 0.5;
    vec3 ambient = lightColor * ambient_strength;


    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    float specularStrength = 0.5f;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = fragColor;

    outColor = vec4(result, 1.0);
}