#version 330 core
out vec4 fragmentColor;

in vec3 normal;
in vec3 fragmentPosition;
in vec4 fragmentLightSpacePosition;
in vec2 textureCoordinates;

uniform vec3 viewPosition;
uniform vec3 lightColor;
uniform vec3 lightPosition;

uniform sampler2D textureDiffuse1;
uniform sampler2D shadowMap;

float calculateShadow(vec4 fragmentLightSpacePosition)
{
    // compute the fragment coordinates on shadow map
    vec3 projectionCoordinates = fragmentLightSpacePosition.xyz / fragmentLightSpacePosition.w;
    projectionCoordinates = projectionCoordinates * 0.5 + 0.5; // transform to [0,1] range
    
    // find the closest depth value from the light perspective
    float closestDepth = texture(shadowMap, projectionCoordinates.xy).r; 
    
    // get the depth of fragment from the light perspective
    float currentDepth = projectionCoordinates.z;

    // calculate the shadow bias
    vec3 fragmentNormal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    //float bias = max(0.05 * (1.0 - dot(fragmentNormal, lightDirection)), 0.005);
    float bias = 0;
    
    // check if the fragment is in shadow and percentage-closer filtering for smoother shadow edges
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float filterDepth = texture(shadowMap, projectionCoordinates.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > filterDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // prevent creation of shadows outside of the light frustum
    if(projectionCoordinates.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    // ambient
    float ambientStrength = 0.25;
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
    float shadow = calculateShadow(fragmentLightSpacePosition);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    vec3 textureColor = texture(textureDiffuse1, textureCoordinates).rgb;
    fragmentColor = vec4(textureColor * lighting, 1.0);
}