#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "celestial_object.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

float lastX = (float)WIDTH / 2.0f;
float lastY = (float)HEIGHT / 2.0f;
bool firstMouse = true;
float fov = 45.0f;

float roll,pitch;
float yaw = - 90.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
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

const float TIME_MULTIPLIER = 500000.0f;
float physicsAccumulator = 0.0f;
float timeAccumulator = 0.0f;

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

    CelestialObject Sun({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.25, 
                         2e30f ,{1.000, 0.647, 0.000}, false);
    CelestialObject Earth({1.496e11f, 0.0f, 0.0f}, {0.0f , 29780.0f, 0.0f}, 0.1, 
                          6e24f, {0.0f, 0.0f, 0.886}, false, 24 * 3600.0f, 
                          "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");

    OrbitalSystem solarSystem(&Sun);
    solarSystem.fix_center(true);
    solarSystem.add_orbiters(&Earth);

    while(!glfwWindowShouldClose(window)){
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        processInput(window);

        glm::mat4 projection = glm::perspective(glm::radians(fov) , (float)WIDTH / (float)HEIGHT,0.1f,100.0f);
        glm::mat4 view = glm::lookAt(cameraPos , cameraPos + cameraFront, cameraUp);

        physicsAccumulator += deltaTime * TIME_MULTIPLIER;
        while(physicsAccumulator >= dt){
            solarSystem.step();
            solarSystem.render(view, projection);
            physicsAccumulator -= dt;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        solarSystem.render(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}