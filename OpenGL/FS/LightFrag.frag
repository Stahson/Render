#version 330 core
out vec4 FragColor;

uniform vec3 cameraPosition; // Pozycja kamery
uniform float fogDensity;    // G�sto�� mg�y
uniform vec3 fogColor;       // Kolor mg�y
void main()
{
    float distance = length(cameraPosition - vec3(1.0f,1.0f,1.0f));
    
    // Oblicz wsp�czynnik mg�y (tutaj eksponencjalny)
    float fogFactor = 1.0 - exp(-fogDensity * distance );
    
    vec4 color = vec4(1.0); 
    FragColor = mix(color, vec4(fogColor, 1.0), fogFactor);
}