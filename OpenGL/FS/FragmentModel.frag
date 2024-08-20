#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;  

uniform sampler2D texture1;  
uniform vec3 lightPos;       
uniform vec3 viewPos;        
uniform vec3 lightColor;     
uniform vec3 objectColor;    
uniform vec3 cameraPosition; // Pozycja kamery
uniform float fogDensity;    // G�sto�� mg�y
uniform vec3 fogColor;

void main()
{
    // Pobranie koloru tekstury
    vec3 texColor = texture(texture1, TexCoords).rgb;
    
    float distance = length(cameraPosition - FragPos);
    
    // Oblicz wsp�czynnik mg�y (tutaj eksponencjalny)
    float fogFactor = 1.0 - exp(-fogDensity * distance );

    // Ambient Lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse Lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular Lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);  
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Sumowanie komponent�w o�wietlenia
    vec3 lighting = (ambient + diffuse + specular) * texColor;

    // Ostateczny kolor fragmentu
    vec4 color = vec4(lighting, 1.0);
    FragColor = mix( color,vec4(fogColor, 1.0), fogFactor);
}