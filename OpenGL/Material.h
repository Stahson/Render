#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

// Struktura materia³u
struct Material {
    std::string name;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ns;
    std::string texturePath;
};

// Funkcja do wczytywania pliku .mtl
std::unordered_map<std::string, Material> loadMTL(const std::string& filename) {
    std::unordered_map<std::string, Material> materials;
    std::ifstream file(filename);
    std::string line;
    Material currentMaterial;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string identifier;
        ss >> identifier;

        if (identifier == "newmtl") {
            if (!currentMaterial.name.empty()) {
                materials[currentMaterial.name] = currentMaterial;
            }
            ss >> currentMaterial.name;
        }
        else if (identifier == "Ka") {
            ss >> currentMaterial.Ka[0] >> currentMaterial.Ka[1] >> currentMaterial.Ka[2];
        }
        else if (identifier == "Kd") {
            ss >> currentMaterial.Kd[0] >> currentMaterial.Kd[1] >> currentMaterial.Kd[2];
        }
        else if (identifier == "Ks") {
            ss >> currentMaterial.Ks[0] >> currentMaterial.Ks[1] >> currentMaterial.Ks[2];
        }
        else if (identifier == "Ns") {
            ss >> currentMaterial.Ns;
        }
        else if (identifier == "map_Kd") {
            ss >> currentMaterial.texturePath;
        }
    }
    if (!currentMaterial.name.empty()) {
        materials[currentMaterial.name] = currentMaterial;
    }
    return materials;
}