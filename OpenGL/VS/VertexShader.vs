#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  // Dodajemy atrybut dla normalnych
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;   // Przekazujemy normalne do fragment shadera
out vec3 FragPos;  // Przekazujemy pozycjê fragmentu do fragment shadera

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Przekszta³cenie normalnych
    Normal = mat3(transpose(inverse(model))) * aNormal;  // Przekszta³canie normalnych przez macierz modelu

    // Obliczenie pozycji fragmentu w przestrzeni œwiata
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}