#version 460

layout(location = 0) in vec3 aPos;       // Vertex position
layout(location = 1) in vec3 aNormal;    // Vertex normal
layout(location = 2) in vec2 aTexCoord;  // Texture coordinates

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform vec3 LightPosition; // Light position in world space

out vec3 LightDir;  // Light direction in tangent space
out vec3 ViewDir;   // View direction in tangent space
out vec2 TexCoord;  // Texture coordinates

void main()
{
    // Transform vertex position to world space
    vec3 FragPos = vec3(ModelMatrix * vec4(aPos, 1.0));

    // Calculate light direction in world space
    LightDir = LightPosition - FragPos;

    // Calculate view direction in world space
    ViewDir = vec3(inverse(ViewMatrix) * vec4(0.0, 0.0, 1.0, 1.0)) - FragPos;

    // Pass texture coordinates
    TexCoord = aTexCoord;

    // Final vertex position
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.0);
}