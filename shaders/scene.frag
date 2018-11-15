#version 330 core
out vec4 fragmentColor;

in vec3 normal;
in vec3 fragmentPosition;
in vec2 textureCoordinates;

uniform sampler2D textureDiffuse;
uniform vec3 viewPosition;
uniform vec3 lightColor;
uniform vec3 lightPosition;

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
    vec3 lighting = ambient + diffuse + specular;
    fragmentColor = textureColor * vec4(lighting, 1.0);
}