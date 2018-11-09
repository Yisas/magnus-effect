#version 330 core
out vec4 fragmentColor;

in vec2 textureCoordinates;
in vec3 normal;
in vec3 fragmentPosition;

uniform sampler2D textureDiffuse;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main()
{
    // ambient
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 fragmentNormal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    float diffuseContribution = max(dot(fragmentNormal, lightDirection), 0.0);
    vec3 diffuse = diffuseContribution * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, fragmentNormal);  
    float specularContribution = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
    vec3 specular = specularStrength * specularContribution * lightColor;
    
    // color
    vec4 textureColor = texture(textureDiffuse, textureCoordinates);
    fragmentColor = vec4((ambient + diffuse + specular), 1.0) * textureColor;
}