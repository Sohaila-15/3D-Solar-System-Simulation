#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoord;
uniform sampler2D tex;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform vec3 Color; // Sun's color
uniform int isSkybox; // Flag to indicate if it's skybox or not

out vec4 FragColor;

void main() {
    // If it is skybox, skip lighting calculations
    if (isSkybox==1) {
        FragColor = texture(tex, texCoord);  // Skybox without lighting
        return;
    }

    // Lighting calculations (only if it's not skybox)
    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);  // View direction from fragment to viewer (camera)
    vec3 halfDir = normalize(lightDir + viewDir);  // Half vector for Blinn-Phong model
    float spec = pow(max(dot(norm, halfDir), 0.0), material.shininess);  // Specular reflection intensity
    spec = clamp(spec, 0.0, 1.0);  // Clamp specular to avoid excessive brightness
    vec3 specular = light.specular * (spec * material.specular);  // Specular reflection color

    // Combine the results
    vec3 result = ambient + diffuse + specular;

    // Apply the sun color (as a global color effect)
    vec3 colorResult = result * Color;  // Apply sun color filter

    // Sample the texture
    vec4 texColor = texture(tex, texCoord);

    // Combine the texture color with the lighting effects
    FragColor = vec4(colorResult, 1.0) * texColor;  // Final color = texture color * lighting effects
}
