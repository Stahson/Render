#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <list>

#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include "Model.h"
#include "Camera.h"
#include "Bezierr.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
glm::vec3** CreateBezier();
void FullFillPoints(float** data, BezierPath Bezier);
void CreateTriangles();
glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& normal);
struct Triangles {
    glm::vec3 vertices[3];
    glm::vec3 normals[3];
};
class Triangle {
public:
    glm::vec3 vertices[3];  // Wspó³rzêdne wierzcho³ków trójk¹ta
    glm::vec3 normal;       // Wektor normalny trójk¹ta
    //glm::mat4 modelMatrix;  // Macierz modelu (4x4) dla trójk¹ta

    // Konstruktor dla wygody
    Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& normalVec, const glm::mat4& model)
        : normal(normalVec) {
        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;
    }
    Triangle()
        : vertices{ glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) }, // Inicjalizujemy wierzcho³ki jako (0, 0, 0)
        normal(glm::vec3(0.0f, 0.0f, 1.0f))                        // Domyœlny wektor normalny skierowany wzd³u¿ osi Z
                                       // Domyœlna macierz modelu to macierz jednostkowa
    {
    }
};
void TrianglesMake(int n, BezierPath Bezier, list<Triangle>& triangles);
void drawTriangle(Triangle triangle, Shader shader, glm::mat4 view, glm::mat4 projection);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;
float fov = 45.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3** BezierPoints;
//BezierPath Bezier= BezierPath(BezierPoints);




int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    
    /*glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));*/
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("VS/VertexShader.vs", "FS/FragmentShader.frag");
    Shader lightCubeShader("VS/Light.vs", "FS/LightFrag.frag");
    Shader ourShaderB("VS/BezierShader.vs", "FS/Bezier.frag");
    
    // you can name your shader files however you like

    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //float vertices[] = {
    //    // positions          // colors           // texture coords
    //     0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
    //     0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
    //    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
    //    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
    //};
    float vertices[] = {
        // Pozycje          // Wektory normalne // Koordynaty tekstur
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,   0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    float Lvertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
            };

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    //    unsigned int VBO;
    //    glGenBuffers(1, &VBO);
    //    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    

   
    


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int VBOL;
    glGenBuffers(1, &VBOL);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    glBindBuffer(GL_ARRAY_BUFFER, VBOL);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Lvertices), Lvertices, GL_STATIC_DRAW);
    // LIGHT
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBOL);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
  





    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);






    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("floyd.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



    unsigned int texture3;
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("moro.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);


    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture1", 0);
    
    ourShader.setInt("texture2", 1);
    
    /*glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));*/
    
    //glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    /*glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);*/
    /*glm::mat4 view;
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));*/
    
    /*int modelLoc = glGetUniformLocation(ourShader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));*/
    
   /* glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);*/

    // render loop
     
    

     glm::vec3 cubePositions[] = {
         glm::vec3(0.0f,  0.0f,  0.0f),
         glm::vec3(2.0f,  5.0f, -15.0f),
         glm::vec3(-1.5f, -2.2f, -2.5f),
         glm::vec3(-3.8f, -2.0f, -12.3f),
         glm::vec3(2.4f, -0.4f, -3.5f),
         glm::vec3(-1.7f,  3.0f, -7.5f),
         glm::vec3(1.3f, -2.0f, -2.5f),
         glm::vec3(1.5f,  2.0f, -2.5f),
         glm::vec3(1.5f,  0.2f, -1.5f),
         glm::vec3(-1.3f,  1.0f, -1.5f)
     };
     
         
     BezierPoints = CreateBezier();
     BezierPath Bezier = BezierPath(BezierPoints);
     /*float** dataB = new float* [144];
     for (int i = 0; i < 144; ++i) {
         dataB[i] = new float[8]; 
     }
     FullFillPoints(dataB,Bezier);*/
     int n = 10;
     //Triangle* triangles = new Triangle[9 * 2 * n * n];
     list<Triangle> triangles;
     TrianglesMake(n,Bezier,triangles);
     //unsigned int VBOB;
     //glGenBuffers(1, &VBOB);
     // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

     //glBindBuffer(GL_ARRAY_BUFFER, VBOB);
     //glBufferData(GL_ARRAY_BUFFER, 144*9*sizeof(float), dataB, GL_STATIC_DRAW);
     
     // LIGHT
     //unsigned int BezierVAO;
     //glGenVertexArrays(1, &BezierVAO);
     //glBindVertexArray(BezierVAO);

     //glBindBuffer(GL_ARRAY_BUFFER, VBOB);
     // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
     //glEnableVertexAttribArray(0);
     //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)3);
     //glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)6);
     //glEnableVertexAttribArray(2);







    /* glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
     glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
     glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
     glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
     glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
     glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
     */
     stbi_set_flip_vertically_on_load(true);
     Shader ourShader1("VS/ModelShader.vs", "FS/FragmentModel.frag");
     
     ourShader1.setInt("texture1", 0);
     Model ourModel("C:/Users/stasi/source/repos/OpenGL/OpenGL/SU-27CGLOWPOLY.obj");
     
     

     
     
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        const float radius = 50.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render the triangle
        //ourShader.use();
        /*glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);*/

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        
        glBindTexture(GL_TEXTURE_2D, texture2);
       
        
        // render loop
        //glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        //transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        // get matrix's uniform location and set matrix
        ourShader.use();
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        ourShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

        


        /*glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;*/
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        //projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        //view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
         //view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
       ourShader.setMat4("view", view);
        /*unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));*/
        
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glm::mat4 model = glm::mat4(1.0f);
        ; // Pocz¹tkowa macierz jednostkowa

        // Translacja, jeœli chcesz przesun¹æ ca³¹ siatkê Béziera (np. przesuniêcie w górê o 1 jednostkê)
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

        // Rotacja, np. o 45 stopni wokó³ osi Y
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // Skalowanie, np. podwojenie rozmiaru siatki
       // model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        /*ourShaderB.use();
        ourShaderB.setMat4("view", view);
        ourShaderB.setMat4("projection", projection);
        ourShaderB.setMat4("model", model);
        ourShaderB.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShaderB.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        ourShaderB.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        glBindVertexArray(BezierVAO);
        glDrawArrays(GL_TRIANGLES, 0, 144);*/

        for (const Triangle& triangle : triangles) {
            drawTriangle(triangle, ourShaderB, view, projection);
        }
        /*for (int i = 0; i < 18; i++)
        {
            drawTriangle(triangles[i],ourShaderB,view, projection);
        }*/

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture3);
 
        ourShader1.use();
        ourShader1.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader1.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        ourShader1.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        //ourShader.setMat4("model", model);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 viewCam = camera.GetViewMatrix();
        glm::mat4 Plane = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        //view =  Plane;
        
           
       

        float rotationSpeed = 0.5;
        float angle = glfwGetTime();
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::translate(model, glm::vec3(camX, 0.0f, camZ));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = model * rotation;
        ourShader1.use();
        ourShader1.setMat4("model", model);
        ourShader1.setMat4("view", view);
        ourShader1.setMat4("projection", projection);
        ourModel.Draw(ourShader1);
        

        

       // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    
    //float xpos = static_cast<float>(xposIn);
    //float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

        /*if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    */
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    /*fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;*/
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}



glm::vec3** CreateBezier() {
    glm::vec3** array = new glm::vec3 * [4];
    for (int i = 0; i < 4; ++i) {
        array[i] = new glm::vec3[4];
    }

    // Wype³nianie tablicy wartoœciami
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float x = static_cast<float>(i) / 3.0f; // x od 0 do 1
            float y = static_cast<float>(j) / 3.0f; // y od 0 do 1
            float z = 0.0f; // z równe 0
            array[i][j] = glm::vec3(x, y, z);
        }
    }
    array[1][2] = glm::vec3(1.0f/3,2.0f/3,1.0f);
    array[2][3] = glm::vec3(2.0f / 3, 2.0f / 3, 0.5f);
    return array;
}

void CreateTriangles()
{

}

void FullFillPoints(float** data,BezierPath Bezier)
{
    int index = 0;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if(!(i%4==0&&j%4==0))
            {
                float u = static_cast<float>(i) / 11.0f;
                float v = static_cast<float>(j) / 11.0f;

                // Obliczanie punktu na powierzchni
                glm::vec3 point = glm::vec3(u, v, Bezier.evaluate(u, v));



                // Obliczanie wektora normalnego jako iloczyn wektorowy pochodnych
                glm::vec3 normal = Bezier.evaluateDerivative(u, v);

                // Wype³nianie danych
                data[index][0] = point.x;
                data[index][1] = point.y;
                data[index][2] = point.z;
                data[index][3] = normal.x;
                data[index][4] = normal.y;
                data[index][5] = normal.z;
                data[index][6] = u;
                data[index][7] = v;

            }
        }
        index++;
    }
}
void TrianglesMake(int n, BezierPath Bezier, list<Triangle>& triangles)
{
    int numDivisions = 4;  // podzia³ 4x4 daje nam 16 kwadratów (32 trójk¹ty)
    int index = 0;
    //Triangle* triangles = new Triangle[9*2*n*n];
    float nf = static_cast<float>(n);
    Triangle tr1;
    for (int i = 0; i < numDivisions*n-1; i++) {
        for (int j = 0; j < numDivisions*n-1; j++) {
            float u = 1.0f / (3.0f*nf) * static_cast<float>(i);
            float v = 1.0f / (3.0f*nf) * static_cast<float>(j);
            glm::vec3 p0= glm::vec3(0.0f);
            glm::vec3 p1 = glm::vec3(0.0f);
            glm::vec3 p2 = glm::vec3(0.0f);
            glm::vec3 p3 = glm::vec3(0.0f);

            // Definiujemy 4 punkty siatki dla danego podzia³u (kwadratu)
            /*if (i % n == 0 && j % n == 0) 
            {
                 p0 = glm::vec3(u,v,BezierPoints[i/n][j/n].z);
            }*/
           // else
           // {
                 p0 = glm::vec3(u, v, Bezier.evaluate(u, v));
           // }
            //if ((i+1) % n == 0 && j % n == 0)
            //{
            //    p1 = glm::vec3(u + 1.0f / (3.0f * nf), v, BezierPoints[(i+1) / n][j / n].z);
            //    /*p1.x = u + (1.0f / (3.0f * 1.0f));*/
            //    //int c = 0;
            //}
          //  else
          //  {
                p1 = glm::vec3(u + 1.0f / (3.0f * nf), v, Bezier.evaluate(u + 1 / (3 * n), v));
           // }
            /*if (i % n == 0 && (j+1) % n == 0)
            {
                p2 = glm::vec3(u, v + 1.0f / (3.0f * nf), BezierPoints[i / n][(j + 1 )/ n].z);
            }*/
           // else
           // {
                p2 = glm::vec3(u, v + 1.0f / (3.0f * nf), Bezier.evaluate(u, v + 1.0f / (3.0f * nf)));
           // }
            /*if ((i + 1) % n == 0 && (j + 1) % n == 0)
            {
                p3 = glm::vec3(u+1.0f/(3.0f*nf), v + 1.0f / (3.0f * nf), BezierPoints[(i +1)/ n][(j + 1) / n].z);
            }*/
           // else
           // {
                p3 = glm::vec3(u + 1.0f / (3.0f * nf), v + 1.0f / (3.0f * nf), Bezier.evaluate(u + 1.0f / (3.0f * nf), v + 1.0f / (3.0f * nf)));
           // }
            
            
           // glm::vec3 p2 = glm::vec3(u , v + 1 / (3 * n), Bezier.evaluate(u, v + 1 / (3 * n)));
            //glm::vec3 p3 = glm::vec3(u + 1 / (3 * n), v + 1 / (3 * n), Bezier.evaluate(u + 1 / (3 * n), v + 1 / (3 * n)));
            //glm::vec3 p0 = BezierPoints[i][j];
            //glm::vec3 p1 = BezierPoints[i + 1][j];
            //glm::vec3 p2 = BezierPoints[i][j + 1];
            //glm::vec3 p3 = BezierPoints[i + 1][j + 1];

            // Pierwszy trójk¹t (p0, p1, p2)
            tr1.vertices[0] = p0;
            tr1.vertices[1] = p1;
            tr1.vertices[2] = p2;
            //triangles[2 * j * 3*n + 2 * i].vertices[0] = p0;
            //triangles[2 * j * 3*n + 2 * i].vertices[1] = p1;
            //triangles[2 * j * 3*n + 2 * i].vertices[2] = p2;
            if (i == 1 && j == 1)
            {
                glm::vec3 vec = p0+p1+p2+p3;
            }
            
            // Oblicz wektor normalny dla trójk¹ta
            glm::vec3 uVec1 = p1 - p0;
            glm::vec3 vVec1 = p2 - p0;
            tr1.normal = glm::normalize(glm::cross(uVec1, vVec1));
            //triangles[2 * j * 3*n + 2 * i].normal = glm::normalize(glm::cross(uVec1, vVec1));
            triangles.emplace_back(tr1);
            // Macierz modelu (przyk³adowa, jednostkowa)
            //triangles[2 * j * 3*n + 2 * i].modelMatrix = createModelMatrix(p0, glm::vec3(1.0f, 1.0f, 1.0f), glm::normalize(glm::cross(uVec1, vVec1)));

            Triangle tr2;
            tr2.vertices[0] = p3;
            tr2.vertices[1] = p1;
            tr2.vertices[2] = p2;

            // Drugi trójk¹t (p1, p3, p2)
            //triangles[2*j*3*n+2*i+1].vertices[0] = p3;
            //triangles[2 * j * 3*n + 2 * i + 1].vertices[1] = p1;
            //triangles[2 * j * 3*n + 2 * i + 1].vertices[2] = p2;

            // Oblicz wektor normalny dla trójk¹ta
            glm::vec3 uVec2 = p3 - p1;
            glm::vec3 vVec2 = p2 - p1;
            tr2.normal= glm::normalize(glm::cross(uVec2, vVec2));
            //triangles[2 * j * 3*n + 2 * i + 1].normal = glm::normalize(glm::cross(uVec2, vVec2));
            triangles.emplace_back(tr2);
            // Macierz modelu (przyk³adowa, jednostkowa)
            //triangles[index].modelMatrix = glm::mat4(1.0f);
            //triangles[2 * j * 3*n + 2 * i + 1].modelMatrix = createModelMatrix(p0, glm::vec3(1.0f, 1.0f, 1.0f), glm::normalize(glm::cross(uVec2, vVec2)));
            index=index+2;
            if (2 * j * 3*n + 2 * i > 17)
            {
                int c = 0;
            }
        }
    }
    //return triangles;
}


//glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& normal) {
//    // 1. Macierz jednostkowa
//    glm::mat4 model = glm::mat4(1.0f);
//
//    // 2. Translacja (ustawienie pozycji trójk¹ta w przestrzeni)
//    model = glm::translate(model, position);
//
//    // 3. Rotacja (ustawienie orientacji trójk¹ta)
//    // Zak³adamy, ¿e normalny wektor jest poprawnie znormalizowany
//    glm::vec3 defaultNormal(0.0f, 0.0f, 1.0f);  // Domyœlnie "górny" wektor
//    glm::vec3 axis = glm::cross(defaultNormal, normal); // Oœ rotacji
//    float angle = acos(glm::dot(defaultNormal, normal)); // K¹t rotacji
//    model = glm::rotate(model, angle, axis);
//
//    // 4. Skalowanie (jeœli potrzebne)
//    model = glm::scale(model, scale);
//
//    return model;
//}
//glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& normal) {
//    // 1. Macierz jednostkowa
//    glm::mat4 model = glm::mat4(1.0f);
//
//    // 2. Translacja (ustawienie pozycji trójk¹ta w przestrzeni)
//    model = glm::translate(model, position);
//
//    // 3. Rotacja (ustawienie orientacji trójk¹ta)
//    glm::vec3 defaultNormal(0.0f, 0.0f, 1.0f);  // Domyœlny wektor normalny (prostopad³y do p³aszczyzny XY)
//
//    // Sprawdzanie, czy wektor normalny jest ró¿ny od domyœlnego wektora normalnego
//    if (glm::length(normal - defaultNormal) > 0.0001f) {
//        glm::vec3 axis = glm::normalize(glm::cross(defaultNormal, normal)); // Oœ rotacji
//        float angle = acos(glm::dot(defaultNormal, glm::normalize(normal))); // K¹t rotacji
//        model = glm::rotate(model, angle, axis);
//    }
//
//    // 4. Skalowanie (jeœli potrzebne)
//    //model = glm::scale(model, scale);
//
//    return model;
//}
glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& normal) {
    // 1. Macierz jednostkowa
    glm::mat4 model = glm::mat4(1.0f);

    // 2. Translacja (ustawienie pozycji trójk¹ta w przestrzeni)
    model = glm::translate(model, position);

    // 3. Rotacja (ustawienie orientacji trójk¹ta)
    glm::vec3 defaultNormal(0.0f, 0.0f, 1.0f);  // Domyœlny wektor normalny (prostopad³y do p³aszczyzny XY)

    if (glm::length(normal - defaultNormal) > 0.0001f) {
        glm::vec3 axis = glm::normalize(glm::cross(defaultNormal, normal)); // Oœ rotacji
        float dot = glm::dot(defaultNormal, glm::normalize(normal));
        dot = glm::clamp(dot, -1.0f, 1.0f); // Zabezpieczenie przed przekroczeniem zakresu
        float angle = acos(dot); // K¹t rotacji

        // Rotacja tylko jeœli k¹t nie jest bliski zeru (co oznacza, ¿e normalne s¹ bardzo podobne)
        if (std::abs(angle) > 0.0001f) {
            model = glm::rotate(model, angle, axis);
        }
    }

    // 4. Skalowanie (jeœli potrzebne)
    model = glm::scale(model, scale);

    return model;
}
void drawTriangle(Triangle triangle, Shader shader, glm::mat4 view, glm::mat4 projection) {
    // Tworzenie tablicy float[] zawieraj¹cej dane wierzcho³ków i normalnych
    float vertices[] = {
        // Wierzcho³ek 0
        triangle.vertices[0].x, triangle.vertices[0].y, triangle.vertices[0].z, // Pozycja
        triangle.normal.x, triangle.normal.y, triangle.normal.z,                 // Normalna

        // Wierzcho³ek 1
        triangle.vertices[1].x, triangle.vertices[1].y, triangle.vertices[1].z, // Pozycja
        triangle.normal.x, triangle.normal.y, triangle.normal.z,                 // Normalna

        // Wierzcho³ek 2
        triangle.vertices[2].x, triangle.vertices[2].y, triangle.vertices[2].z, // Pozycja
        triangle.normal.x, triangle.normal.y, triangle.normal.z                  // Normalna
    };
    shader.use();
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
    shader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

    shader.setMat4("model", glm::mat4x4(1.0f));
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // 1. W³¹cz shader
    //glUseProgram(shaderProgram);

    //// 2. Wys³anie macierzy model, view, projection do shadera
    //GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    //GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    //GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(triangle.modelMatrix));
    //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    //glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // 3. Tworzenie i bindowanie VAO i VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // 4. Buforowanie danych do VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 5. Interpretacja danych w shaderze
    // Pozycje
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normalne
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 6. Rysowanie trójk¹ta
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 7. Od³¹czenie VAO i VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Usuniêcie VAO i VBO, jeœli nie bêd¹ ju¿ potrzebne
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

//void generateTriangles(int numDivisions, glm::vec3** BezierPoints, std::vector<Triangles>& triangles) {
//    int index = 0;
//
//    for (int i = 0; i < numDivisions - 1; ++i) {
//        for (int j = 0; j < numDivisions - 1; ++j) {
//            // Punkty siatki
//            glm::vec3 p0 = BezierPoints[i][j];
//            glm::vec3 p1 = BezierPoints[i + 1][j];
//            glm::vec3 p2 = BezierPoints[i][j + 1];
//            glm::vec3 p3 = BezierPoints[i + 1][j + 1];
//
//            // Pierwszy trójk¹t (p0, p1, p2)
//            Triangles tri1;
//            tri1.vertices[0] = p0;
//            tri1.vertices[1] = p1;
//            tri1.vertices[2] = p2;
//
//            // Obliczanie normalnych dla pierwszego trójk¹ta
//            glm::vec3 normal1 = glm::normalize(glm::cross(p1 - p0, p2 - p0));
//            tri1.normals[0] = normal1;
//            tri1.normals[1] = normal1;
//            tri1.normals[2] = normal1;
//
//            triangles.push_back(tri1);
//
//            // Drugi trójk¹t (p1, p3, p2)
//            Triangles tri2;
//            tri2.vertices[0] = p1;
//            tri2.vertices[1] = p3;
//            tri2.vertices[2] = p2;
//
//            // Obliczanie normalnych dla drugiego trójk¹ta
//            glm::vec3 normal2 = glm::normalize(glm::cross(p3 - p1, p2 - p1));
//            tri2.normals[0] = normal2;
//            tri2.normals[1] = normal2;
//            tri2.normals[2] = normal2;
//
//            triangles.push_back(tri2);
//        }
//    }
//}


























//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//int CreateVertex();
//
//
////const char* vertexShaderSource = "#version 330 core\n"
////"layout (location = 0) in vec3 aPos;\n"
////"out vec4 vertexColor;\n"
////"void main()\n"
////"{\n"
////"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
////"   vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
////"}\0";
//
//const char* vertexShaderSource = "#version 330 core\n"
//"layout (location = 0) in vec3 aPos;\n"
//"layout (location = 1) in vec3 aColor;\n"
//"out vec3 ourColor;\n"
//"void main()\n"
//"{\n"
//"   gl_Position = vec4(aPos, 1.0);\n"
//"   ourColor = aColor;\n"
//"}\0";
//
//const char* fragmentShaderSource = "#version 330 core\n"
//"out vec4 FragColor;\n"
//"in vec3 ourColor;\n"
//"void main()\n"
//"{\n"
//"   FragColor = vec4(ourColor, 1.0);\n"
//"}\0";
//
////const char* fragmentShaderSource = "#version 330 core\n"
////"out vec4 FragColor;\n"
////"uniform vec4 fColor;\n"
////"void main()\n"
////"{\n"
////"   FragColor = fColor;\n"
////"}\0";
//
//int main()
//{
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//
//    // Tworzenie okna
//    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//
//    // Zaincjalizowanie Glad
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//
//    glViewport(0, 0, 800, 600);
//
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//
//    // Vertex
//    unsigned int EBO = CreateVertex();
//
//    //float vertices[] = {
//    //    -0.5f, -0.5f, 0.0f, // left  
//    //     0.5f, -0.5f, 0.0f, // right 
//    //     0.0f,  0.5f, 0.0f  // top    
//    //};
//
//
//    unsigned int VAO;
//    glGenVertexArrays(1, &VAO);
//
//    /*unsigned int VBO;
//    glGenBuffers(1, &VBO);*/
//    glBindVertexArray(VAO);
//   /* glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);*/
//
//
//    // Vertex Shader
//    unsigned int vertexShader;
//    vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//    glCompileShader(vertexShader);
//
//    int  success;
//    char infoLog[512];
//    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//    if (!success)
//    {
//        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
//
//    
//    // Fragment Shader
//     
//    unsigned int fragmentShader;
//    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//    glCompileShader(fragmentShader);
//
//
//    // Shader Programme
//    unsigned int shaderProgram;
//    shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//
//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//    if (!success) {
//        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//    }
//
//
//    
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    // color attribute
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//
//    
//  
//
//
//
//    // LOOP
//    while (!glfwWindowShouldClose(window))
//    {
//        processInput(window);
//
//        // Rendering here
//        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        glUseProgram(shaderProgram);
//        
//        // Uniform
//        /*float timeValue = glfwGetTime();
//        float greenValue = sin(timeValue) / 2.0f + 0.5f;
//        int vertexColorLocation = glGetUniformLocation(shaderProgram, "fColor");
//        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/
//
//        glBindVertexArray(VAO);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//        //glDrawArrays(GL_TRIANGLES, 0, 3);
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//    glfwTerminate();
//    return 0;
//}
//
//int CreateVertex()
//{
//    float vertices[] = {
//        // positions         // colors
//         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
//        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
//         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
//    };
//    //unsigned int indices[] = {  // note that we start from 0!
//    //0, 1, 3,   // first triangle
//    //1, 2, 3    // second triangle
//    //};
//    //unsigned int EBO;
//    //glGenBuffers(1, &EBO);
//    unsigned int VBO;
//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/
//    return VBO;
//}
//
//
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//}
//
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    glViewport(0, 0, width, height);
//}