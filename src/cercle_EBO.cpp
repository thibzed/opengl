#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "main.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 800;
const float RADIUS = 0.5;
const int NB_TRIANGLES = 50;

const char *sourceVertexShader = "#version 330 core\n"
    "layout (location = 0) in vec3 Pos\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(Pos, 1.0);\n"
    "}\0";

const char *sourceFragmentShader = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.5f, 0.5f, 0.5f,1.0f);\n"
    "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

struct Point{
    float x;
    float y;
};

void draw_circle_EBO(const float Radius, const int nb_triangles, const unsigned int shaderProgram){

    float dalpha = 2.0f * M_PI / nb_triangles;

    int taille_vertices = (1 + nb_triangles) * 3;
    float* vertices = new float[taille_vertices];

    vertices[0] = 0.0f;
    vertices[1] = 0.0f;
    vertices[2] = 0.0f;

    for (int i = 0 ; i < nb_triangles ; i++){
        float alpha = i * dalpha;
        int base = (i + 1) * 3;
        vertices[base] = Radius * cos(alpha);
        vertices[base + 1] = Radius * sin(alpha);
        vertices[base + 2] = 0.0f;
    }

    unsigned int* indices = new unsigned int[nb_triangles * 3];

    for (int i = 0; i < nb_triangles ; i++){
        int base = i * 3;
        indices[base] = 0;
        indices[base + 1] = i + 1;
        indices[base + 2] = (i + 1) % nb_triangles + 1;
    }

    unsigned int VBO, VAO, EBO;
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glGenVertexArrays(1,&VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);

    glBufferData(GL_ARRAY_BUFFER , taille_vertices * sizeof(float), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nb_triangles * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES,nb_triangles * 3 , GL_UNSIGNED_INT,0);
    glUseProgram(shaderProgram);

    delete[] vertices;
    delete[] indices;
}

void draw_circles(const float radius, const int nb_triangles, const unsigned int shaderProgram){

    float* vertices = new float[nb_triangles * 9];
    float dalpha = (2 * M_PI) / nb_triangles;
    float alpha = 0;
    
    for (int i = 0 ; i < nb_triangles ; i++){
        int base = i * 9;
        vertices[base] = 0.0f;
        vertices[base + 1] = 0.0f;
        vertices[base + 2] = 0.0f;
        vertices[base + 3] = radius * cos(alpha); 
        vertices[base + 4] = radius * sin(alpha);
        vertices[base + 5] = 0.0f;
        vertices[base + 6] = radius * cos(alpha + dalpha);
        vertices[base + 7] = radius * sin(alpha + dalpha);
        vertices[base + 8] = 0.0f;
        alpha += dalpha;
    }

    unsigned int VBO,VAO;
    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER, nb_triangles * 9 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES,0,3*nb_triangles);
    glUseProgram(shaderProgram);
    
    delete[] vertices;
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
    }    

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &sourceVertexShader, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&sourceFragmentShader, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while(!glfwWindowShouldClose(window)){
        processInput(window);
        //draw_circle_EBO(RADIUS, NB_TRIANGLES, shaderProgram);
        draw_circles(RADIUS, NB_TRIANGLES, shaderProgram);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}