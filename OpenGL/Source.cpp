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
bool jclick = false;
bool reflector = false;
bool Night = true;

float fogDensity = 0.0f; // Gêstoœæ mg³y
glm::vec3 fogColor(0.5f, 0.5f, 0.5f);

struct Triangles {
    glm::vec3 vertices[3];
    glm::vec3 normals[3];
};
class Triangle {
public:
    glm::vec3 vertices[3];  
    glm::vec3 normal;       
    

    // Konstruktor dla wygody
    Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& normalVec, const glm::mat4& model)
        : normal(normalVec) {
        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;
    }
    Triangle()
        : vertices{ glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) }, 
        normal(glm::vec3(0.0f, 0.0f, 1.0f))                        
                                      
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
    
    

   
    


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

 
    

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
   
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
    data = stbi_load("face.jpg", &width, &height, &nrChannels, 0);
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
    
     int n = 10;
     //Triangle* triangles = new Triangle[9 * 2 * n * n];
     list<Triangle> triangles;
     TrianglesMake(n,Bezier,triangles);
   





   
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
        const float radius = 35.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        float angle = glfwGetTime();
        if (jclick)
        {
            camera.jet = true;
            camera.JetPass(glm::vec3(0.3f*camX,0.0f,0.3f*camZ),angle);
            //camera.Yaw = angle;
            camera.Process();
            
        }
       
        // render
        // ------
        if (Night) 
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render the triangle
        //ourShader.use();
        /*glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);*/

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        
        glBindTexture(GL_TEXTURE_2D, texture2);
       
        
        
        ourShader.use();
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        ourShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        ourShader.setFloat("fogDensity", fogDensity);
        ourShader.setVec3("fogColor", fogColor.x, fogColor.y, fogColor.z);
        ourShader.setVec3("cameraPosition", camera.Position.x, camera.Position.y, camera.Position.z);
        glm::vec3 right = glm::normalize(glm::cross(camera.Front, camera.Up));
        glm::vec3 shiftedright = camera.Front + right * 0.05f;
        glm::vec3 shiftedleft = camera.Front - right * 0.05f;
        ourShader.setVec3("directionr", shiftedright.x, shiftedright.y, shiftedright.z);
        ourShader.setVec3("directionl", shiftedleft.x, shiftedleft.y, shiftedleft.z);
        ourShader.setBool("reflector", reflector);
        ourShader.setFloat("cutOff", glm::cos(glm::radians(3.0f)));
        


       
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        ourShader.setMat4("projection", projection);
        
       ourShader.setMat4("view", view);
        
        
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            if (i == 6)
            {
                // Oblicz k¹t obrotu na podstawie czasu
                float time = glfwGetTime();
                float angle = time * glm::radians(50.0f); // Szybkoœæ obrotu mo¿esz dostosowaæ (50.0f)

                // Obróæ wokó³ osi y
                model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else
            {
                if (i == 2)
                {
                    // Oblicz k¹t obrotu na podstawie czasu
                    float time = glfwGetTime();
                    float angle = time * glm::radians(100.0f); // Szybkoœæ obrotu mo¿esz dostosowaæ (50.0f)

                    // Obróæ wokó³ osi y
                    model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
                }
                else
                {
                    // Sta³a rotacja dla pozosta³ych szeœcianów
                    float angle = 20.0f * i;
                    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                }
            }


            
            
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glm::mat4 model = glm::mat4(1.0f);
        ; 
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

        
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

       

        for (const Triangle& triangle : triangles) {
            drawTriangle(triangle, ourShaderB, view, projection);
        }
        

        lightCubeShader.use();
        lightCubeShader.setFloat("fogDensity", fogDensity);
        lightCubeShader.setVec3("fogColor", fogColor.x,fogColor.y,fogColor.z);
        lightCubeShader.setVec3("cameraPosition", camera.Position.x, camera.Position.y, camera.Position.z);
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        if (!reflector) 
        {
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture3);
 
        ourShader1.use();
        ourShader1.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader1.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        ourShader1.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        ourShader1.setFloat("fogDensity", fogDensity);
        ourShader1.setVec3("fogColor", fogColor.x, fogColor.y, fogColor.z);
        ourShader1.setVec3("cameraPosition", camera.Position.x, camera.Position.y, camera.Position.z);
        ourShader1.setVec3("direction", camera.Front.x, camera.Front.y, camera.Front.z);
        ourShader1.setBool("reflector", reflector);
        ourShader1.setFloat("cutOff", glm::cos(glm::radians(7.5f)));
        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	
        
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 viewCam = camera.GetViewMatrix();
        glm::mat4 Plane = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        
        
           
       

        float rotationSpeed = 0.5;
       
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
    
    if (!jclick) {
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        {
            Night = true;
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            fogDensity = 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            reflector = true;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            reflector = false;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            Night = false;
        }

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
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            jclick = true;
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            jclick = false;
            camera.jet = false;
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            fogDensity = 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            reflector = true;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            reflector = false;
        }
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        {
            Night = true;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            Night = false;
        }
    }
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

       
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  
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
            float x = static_cast<float>(i) / 3.0f; 
            float y = static_cast<float>(j) / 3.0f; 
            float z = 0.0f; 
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
    int numDivisions = 4;  
    int index = 0;
    
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

            
                 p0 = glm::vec3(u, v, Bezier.evaluate(u, v));
        
                p1 = glm::vec3(u + 1.0f / (3.0f * nf), v, Bezier.evaluate(u + 1 / (3 * n), v));
         
                p2 = glm::vec3(u, v + 1.0f / (3.0f * nf), Bezier.evaluate(u, v + 1.0f / (3.0f * nf)));
         
                p3 = glm::vec3(u + 1.0f / (3.0f * nf), v + 1.0f / (3.0f * nf), Bezier.evaluate(u + 1.0f / (3.0f * nf), v + 1.0f / (3.0f * nf)));
           // }
            
            
          
            tr1.vertices[0] = p0;
            tr1.vertices[1] = p1;
            tr1.vertices[2] = p2;
            
            if (i == 1 && j == 1)
            {
                glm::vec3 vec = p0+p1+p2+p3;
            }
            
          
            glm::vec3 uVec1 = p1 - p0;
            glm::vec3 vVec1 = p2 - p0;
            tr1.normal = glm::normalize(glm::cross(uVec1, vVec1));
            
            triangles.emplace_back(tr1);
            
            Triangle tr2;
            tr2.vertices[0] = p1;
            tr2.vertices[1] = p3;
            tr2.vertices[2] = p2;

            
            glm::vec3 uVec2 = p3 - p1;
            glm::vec3 vVec2 = p2 - p1;
            tr2.normal= glm::normalize(glm::cross(uVec2, vVec2));
            
            triangles.emplace_back(tr2);
            
            index=index+2;
            if (2 * j * 3*n + 2 * i > 17)
            {
                int c = 0;
            }
        }
    }
    //return triangles;
}



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

        
        if (std::abs(angle) > 0.0001f) {
            model = glm::rotate(model, angle, axis);
        }
    }

    
    model = glm::scale(model, scale);

    return model;
}
void drawTriangle(Triangle triangle, Shader shader, glm::mat4 view, glm::mat4 projection) {

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
    shader.setFloat("fogDensity", fogDensity);
    shader.setVec3("fogColor", fogColor.x, fogColor.y, fogColor.z);
    shader.setVec3("cameraPosition", camera.Position.x, camera.Position.y, camera.Position.z);
    glm::mat4x4 model = glm::mat4x4(1.0f);
    float angle = glm::radians(270.0f);  
    glm::vec3 axisX(1.0f, 0.0f, 0.0f);  

    model = glm::rotate(model, angle, axisX);
    model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    glm::vec3 right = glm::normalize(glm::cross(camera.Front, camera.Up));
    glm::vec3 shiftedright = camera.Front + right * 0.05f;
    glm::vec3 shiftedleft = camera.Front - right * 0.05f;
    shader.setVec3("directionr", shiftedright.x, shiftedright.y, shiftedright.z);
    shader.setVec3("directionl", shiftedleft.x, shiftedleft.y, shiftedleft.z);
    shader.setBool("reflector", reflector);
    shader.setFloat("cutOff", glm::cos(glm::radians(3.0f)));

  
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

   
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glDrawArrays(GL_TRIANGLES, 0, 3);

 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

 
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

