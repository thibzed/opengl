#include "Sphere.hpp" //Must be before GLFW include because Sphere.hpp holds the include for glad. 
#include <GLFW/glfw3.h>
#include "main.h"
#define _USE_MATH_DEFINES

#include <chrono>
#include <random>

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

    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    int nb_Spheres = 20;
    std::vector<Sphere> Spheres_vector;
    
    int nb_points = 80 * 80;
    std::vector<float> white = {1.0f,1.0f,1.0f};

    //for (int i = 0; i < nb_Spheres ; i++){
    //    glm::vec3 center = {dist(generator), dist(generator), dist(generator)};
    //    Sphere S (0.5f, nb_points, white, center);
    //    Spheres_vector.push_back(std::move(S));
    //}

    Sphere S(0.5f, white, glm::vec3(0.0f));

    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

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
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, Sphere_center1);
        //glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
//
        //glBindVertexArray(VAO[0]);
        //glDrawArrays(GL_POINTS,0,nb_points);
//
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, Sphere_center2);
        //glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));

        //glBindVertexArray(VAO[1]);
        //for (const auto &s : Spheres_vector){
        //    unsigned int VAO = s.get_VAO();
        //    glm::mat4 model_s = s.get_model();
        //    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model_s));
        //    glBindVertexArray(VAO);
        //    glDrawArrays(GL_POINTS,0,nb_points);
        //}
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        S.render(view, projection);
        //unsigned int VAO_S1 = S1.get_VAO();
        //glm::mat4 model_S1 = S1.get_model();
        //glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model_S1));
        //glBindVertexArray(VAO_S1);
        //glDrawArrays(GL_POINTS,0,nb_points);
//
        //unsigned int VAO_S2 = S2.get_VAO();
        //glm::mat4 model_S2 = S2.get_model();
        //glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model_S2));
        //glBindVertexArray(VAO_S2);
        //glDrawArrays(GL_POINTS,0,nb_points);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}