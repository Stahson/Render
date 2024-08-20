#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Struktura dla siatki Beziera 4x4
class BezierPath {
public:
    glm::vec3 controlPoints[4][4];  // 16 punktów kontrolnych

    BezierPath(glm::vec3** points)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                controlPoints[i][j] = points[i][j];
            }
        }
    }
    // Funkcja obliczaj¹ca wartoœæ na powierzchni dla parametrów u, v
    float evaluate(float u, float v) const {
        float result=0.0f;
        
        // Obliczanie wartoœci na powierzchni przy u¿yciu wielomianów Bernsteina
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float bernsteinU = bernstein(i, u);
                float bernsteinV = bernstein(j, v);
                result += controlPoints[i][j].z * bernsteinU * bernsteinV;
            }
        }
        return result;
    }
    glm::vec3 evaluateDerivative(float u, float v) const {
        float du = 0.0f;
        float dv = 0.0f;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float bernsteinU = bernstein(i, u);
                float bernsteinV = bernstein(j, v);
                float bernsteinUPrime = bernsteinDerivative(i, u);
                float bernsteinVPrime = bernsteinDerivative(j, v);

                du += controlPoints[i][j].z * bernsteinUPrime * bernsteinV;
                dv += controlPoints[i][j].z * bernsteinU * bernsteinVPrime;
            }
        }
        glm::vec3 U = glm::vec3(1,v,du);
        glm::vec3 V = glm::vec3(u, 1, dv);
        glm::vec3 result= glm::cross(U,V);
        return result;
    }

    // Funkcja obliczaj¹ca wielomian Bernsteina dla danego stopnia i parametru t
    float bernstein(int i, float t) const {
        switch (i) {
        case 0: return (1 - t) * (1 - t) * (1 - t);
        case 1: return 3 * t * (1 - t) * (1 - t);
        case 2: return 3 * t * t * (1 - t);
        case 3: return t * t * t;
        }
        return 0.0f;
    }
    float bernsteinDerivative(int i, float t) const {
        switch (i) {
        case 0: return -3 * (1 - t) * (1 - t);
        case 1: return 3 * (1 - t) * (1 - t) - 6 * t * (1 - t);
        case 2: return 6 * t * (1 - t) - 3 * t * t;
        case 3: return 3 * t * t;
        }
        return 0.0f;
    }

};