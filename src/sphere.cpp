#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "main.h"
#define _USE_MATH_DEFINES
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int WIDTH = 800;
const int HEIGHT = 600;

float lastX = (float)WIDTH / 2.0f;
float lastY = (float)HEIGHT / 2.0f;
bool firstMouse = true;
float fov = 45.0f;

float roll,pitch;
float yaw = - 90.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 Pos;\n"
    "layout (location = 1) in vec3 Color;\n"

    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"

    "out vec4 vertexColor;\n"

    "void main()\n"
    "{\n"
    "gl_Position = projection * view * model * vec4(Pos, 1.0f);\n"
    "vertexColor = vec4(Color,1.0f);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor;\n"

    "void main()\n"
    "{\n"
    "   FragColor = vertexColor;\n"
    "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}

//Même avec clavier Azerty il faut utiliser ces réglages pour ZQSD (équivalent de WASD sur qwerty.)
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 2.5f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cameraPos += cameraSpeed * cameraFront;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cameraPos -= cameraSpeed * cameraFront;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){

    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    fov -= (float)yoffset;
    if (fov < 1.0f){
        fov = 1.0f;
    }
    if(fov > 45.0f){
        fov = 45.0f;
    }
}

int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH,HEIGHT, "Test", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
    }    

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    int nb_lat = 100;
    int nb_long = 100;
    int nb_points = nb_lat * nb_long;
    float radius = 0.5f;

    float* points = new float[nb_points * 6];
    for (int i = 0; i < nb_lat; i++) {
        float lat = M_PI * (-0.5f + (float)i / (nb_lat - 1)); // -PI/2 à PI/2
        for (int j = 0; j < nb_long; j++) {
            float lon = 2.0f * M_PI * (float)j / nb_long; // 0 à 2*PI

            float x = radius * cos(lat) * cos(lon);
            float y = radius * sin(lat);
            float z = radius * cos(lat) * sin(lon);

            int base = (i * nb_long + j) * 6;
            points[base] = x;
            points[base + 1] = y;
            points[base + 2] = z;
            points[base + 3] = 1.0f;
            points[base + 4] = 1.0f;
            points[base + 5] = 1.0f;
        }
    }

    unsigned int VBO,VAO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER, nb_points * 6 * sizeof(float), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window)){
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
        glEnable(GL_DEPTH_TEST);
        
        glUseProgram(shaderProgram);

        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glm::mat4 projection = glm::perspective(glm::radians(fov) , (float)WIDTH / (float)HEIGHT,0.1f,100.0f);
        glUniformMatrix4fv(projectionLoc,1,GL_FALSE,glm::value_ptr(projection));

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glm::mat4 view = glm::lookAt(cameraPos , cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1,GL_FALSE,glm::value_ptr(view));

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS,0,nb_points);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    delete[] points;
    return 0;
}