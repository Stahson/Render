#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  // Dodajemy atrybut dla normalnych
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;   // Przekazujemy normalne do fragment shadera
out vec3 FragPos;  // Przekazujemy pozycj� fragmentu do fragment shadera

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Przekszta�cenie normalnych
    Normal = mat3(transpose(inverse(model))) * aNormal;  // Przekszta�canie normalnych przez macierz modelu

    // Obliczenie pozycji fragmentu w przestrzeni �wiata
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}