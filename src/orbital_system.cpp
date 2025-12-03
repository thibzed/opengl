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


    float R_Earth = 6371.0f; //kms
    float R_Mars = 3389.5f;
    float R_Venus = 6051.8f;
    float R_Mercury = 2439.7f;
    float R_Moon = 1737.4f;

    float R_Earth_scale = R_Earth / 63710.0f; //R_T is divided by 10. 
    float R_Mars_scale = R_Mars / 63710.0f; //All other radius are scaled in comparaison with Earth radius
    float R_Venus_scale = R_Venus / 63710.0f;
    float R_Mercury_scale = R_Mercury / 63710.0f;
    float R_Moon_scale = R_Moon / 63710.0f;

    OrbitalSystem solarSystem;
    glm::vec3 sunCenter(0.0f);

    CelestialObject Sun({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.25, 
                         2e30f ,{1.000, 0.647, 0.000});
    CelestialObject Earth({1.496e11f, 0.0f, 0.0f}, {0.0f , 29780.0f, 0.0f}, R_Earth_scale, 
                          6e24f, {0.0f, 0.0f, 0.886}, nullptr, false, 24 * 3600.0f, 
                          "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Mars({2.279e11f, 0.0f, 0.0f}, {0.0f , 24080.0f, 0.0f}, R_Mars_scale, 
                          6e24f, {0.7f, 0.35f, 0.2f}, nullptr, false, 24 * 3600.0f ,
                          "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Venus({1.08e11f, 0.0f, 0.0f}, {0.0f, 35025.0f, 0.0f}, R_Venus_scale, 
                          5e24f, {0.94f, 0.89f, 0.78f}, nullptr, false, 24 * 3600.0f , 
                           "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Mercury({5.80e10f, 0.0f, 0.0f}, {0.0f, 47360.0f, 0.0f}, R_Mercury_scale, 
                            3.3e23f, {0.5f, 0.5f, 0.5f}, nullptr, false, 24 * 3600.0f , 
                            "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Moon({1.496e11f + 3e8, 0.0f, 0.0f}, {0.0f, 29780.0f + 1022.0f, 0.0f}, R_Moon_scale, 
                          7.35e22 ,{0.5f, 0.5f, 0.5f}, nullptr, false, 24 * 3600.0f , 
                          "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    Moon.set_display_scale(60.0f);

    solarSystem.define_center(&Sun);
    Earth.set_orbitalSystem(&solarSystem);
    Earth.set_orbitalCenter(&Sun);

    Mars.set_orbitalSystem(&solarSystem);
    Mars.set_orbitalCenter(&Sun);

    Venus.set_orbitalSystem(&solarSystem);
    Venus.set_orbitalCenter(&Sun);

    Mercury.set_orbitalSystem(&solarSystem);
    Mercury.set_orbitalCenter(&Sun);

    Earth.get_sphere().get_shader().use();
    Earth.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Earth.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Earth.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Earth.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Earth.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Earth.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Earth.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Earth.get_sphere().get_shader().setFloat("specularStrenght", 0.2);
    Earth.get_sphere().get_shader().setFloat("shininess",32.0f);

    Mars.get_sphere().get_shader().use();
    Mars.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Mars.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Mars.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Mars.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Mars.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Mars.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Mars.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Mars.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Mars.get_sphere().get_shader().setFloat("shininess",8.0f);

    Venus.get_sphere().get_shader().use();
    Venus.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Venus.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Venus.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Venus.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Venus.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Venus.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Venus.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Venus.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Venus.get_sphere().get_shader().setFloat("shininess",8.0f);

    Mercury.get_sphere().get_shader().use();
    Mercury.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Mercury.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Mercury.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Mercury.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Mercury.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Mercury.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Mercury.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Mercury.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Mercury.get_sphere().get_shader().setFloat("shininess",1.0f);

    solarSystem.fix_center(true);
    auto EarthSystem = std::make_shared<OrbitalSystem>();
    EarthSystem->define_center(&Earth);
    EarthSystem->add_orbiters(&Moon);

    solarSystem.add_subsystem(EarthSystem);

    solarSystem.add_orbiters(&Mars);
    solarSystem.add_orbiters(&Venus);
    solarSystem.add_orbiters(&Mercury);
    //solarSystem.add_orbiters(&Mars);
    solarSystem.initialize();

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