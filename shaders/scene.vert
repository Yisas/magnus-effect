#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinates;

out vec3 normal;
out vec3 fragmentPosition;
out vec4 fragmentLightSpacePosition;
out vec2 textureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    fragmentPosition = vec3(model * vec4(aPosition, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;
    fragmentLightSpacePosition = lightSpaceMatrix * vec4(fragmentPosition, 1.0);
    textureCoordinates = aTextureCoordinates;
    
    gl_Position = projection * view * vec4(fragmentPosition, 1.0);
}