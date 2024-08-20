#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 cameraPosition; // Pozycja kamery
uniform float fogDensity;    // Gêstoœæ mg³y
uniform vec3 fogColor;
uniform vec3 directionr;
uniform vec3 directionl;
uniform bool reflector;
uniform float cutOff;

void main()
{
 if(!reflector)
    {
    vec3 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2).rgb;

    float distance = length(cameraPosition - FragPos);
    
    // Oblicz wspó³czynnik mg³y (tutaj eksponencjalny)
    float fogFactor = 1.0 - exp(-fogDensity * distance);

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Final result
    vec3 lighting = (ambient + diffuse + specular) * texColor;
    vec4 color = vec4(lighting, 1.0);
    FragColor = mix(color,vec4(fogColor, 1.0),  fogFactor);
    }
    else
    {
    
    vec3 lightDir = normalize(cameraPosition - FragPos);
    
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-directionr)); 
    if(theta > cutOff)
    {
    vec3 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2).rgb;

    float distance = length(cameraPosition - FragPos);
    
    // Oblicz wspó³czynnik mg³y (tutaj eksponencjalny)
    float fogFactor = 1.0 - exp(-fogDensity * distance);

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(cameraPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Final result
    vec3 lighting = (ambient + diffuse + specular) * texColor;
    vec4 color = vec4(lighting, 1.0);
    FragColor = mix(color,vec4(fogColor, 1.0),  fogFactor);
    
    }
    else
    {
    vec3 lightDir = normalize(cameraPosition - FragPos);
    
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-directionl)); 
    if(theta > cutOff)
    {
    vec3 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2).rgb;

    float distance = length(cameraPosition - FragPos);
    
    // Oblicz wspó³czynnik mg³y (tutaj eksponencjalny)
    float fogFactor = 1.0 - exp(-fogDensity * distance);

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(cameraPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Final result
    vec3 lighting = (ambient + diffuse + specular) * texColor;
    vec4 color = vec4(lighting, 1.0);
    FragColor = mix(color,vec4(fogColor, 1.0),  fogFactor);
    }
    else
    {
    vec3 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2).rgb;
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    FragColor = vec4(ambient *  texColor, 1.0);
    }

    }
    }
}