#version 460

in vec3 LightDir;  // Light direction in tangent space
in vec3 ViewDir;   // View direction in tangent space
in vec2 TexCoord;  // Texture coordinates

layout(binding = 0) uniform sampler2D ColorTex;     // Color texture
layout(binding = 1) uniform sampler2D NormalMapTex; // Normal map texture

layout(location = 0) out vec4 FragColor;

// Light and material uniforms
uniform struct LightInfo {
    vec4 Position; // Light position
    vec3 La;       // Ambient light intensity
    vec3 L;        // Diffuse and specular light intensity
} Light;

uniform struct MaterialInfo {
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ka;            // Ambient reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
} Material;

// Blinn-Phong lighting calculation
vec3 blinnPhong(vec3 n) {
    vec3 diffuse = vec3(0), spec = vec3(0);

    // Sample the color texture
    vec3 texColor = texture(ColorTex, TexCoord).rgb;

    // Ambient component
    vec3 ambient = Light.La * texColor;

    // Normalize the light direction
    vec3 s = normalize(LightDir);

    // Diffuse component
    float sDotN = max(dot(s, n), 0.0);
    diffuse = texColor * sDotN;

    // Specular component (Blinn-Phong)
    if (sDotN > 0.0) {
        vec3 v = normalize(ViewDir);
        vec3 h = normalize(v + s); // Halfway vector
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
    }

    // Combine ambient, diffuse, and specular components
    return ambient + (diffuse + spec) * Light.L;
}

void main() {
    // Sample the normal map and transform to tangent space
    vec3 norm = texture(NormalMapTex, TexCoord).xyz;
    norm.xy = 2.0 * norm.xy - 1.0; // Unpack from [0, 1] to [-1, 1]

    // Normalize the normal
    norm = normalize(norm);

    // Calculate the final color using Blinn-Phong lighting
    FragColor = vec4(blinnPhong(norm), 1.0);
}