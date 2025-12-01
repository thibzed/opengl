#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sphere.hpp"
#include "planet.hpp"

const int WIDTH = 1200;
const int HEIGHT = 1200;

float lastX = (float)WIDTH / 2.0f;
float lastY = (float)HEIGHT / 2.0f;
bool firstMouse = true;
float fov = 45.0f;

float roll,pitch;
float yaw = - 90.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
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

//const float PHYSICS_DT =  dt;
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
    glm::vec3 center_S (0.0f);
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

    Planet Sun({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.25, {1.0f, 0.647f, 0.0f}, false);
    Planet Earth({1.496e11f, 0.0f, 0.0f}, {0.0f , 29780.0f, 0.0f}, R_Earth_scale, 
             {0.0f, 0.0f, 0.886}, true, 24 * 3600.0f , 
             "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    Planet Mars({2.279e11f, 0.0f, 0.0f}, {0.0f , 24080.0f, 0.0f}, R_Mars_scale, 
             {0.7f, 0.35f, 0.2f}, true, 24 * 3600.0f , 
             "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    Planet Venus({1.08e11f, 0.0f, 0.0f}, {0.0f, 35025.0f, 0.0f}, R_Venus_scale, 
             {0.94f, 0.89f, 0.78f}, true, 24 * 3600.0f , 
             "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    Planet Mercury({5.80e10f, 0.0f, 0.0f}, {0.0f, 47360.0f, 0.0f}, R_Mercury_scale, 
             {0.5f, 0.5f, 0.5f}, true, 24 * 3600.0f , 
             "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    //Planet Moon({1e11f, 0.0f, 0.0f}, {0.0f, 1620.0f, 0.0f}, R_Moon_scale, 
    //            {0.5f, 0.5f, 0.5f}, true, 24 * 3600.0f , 
    //            "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");

    Earth.get_sphere().get_shader().use();
    Earth.get_sphere().get_shader().setVec3("light.position",center_S);
    Earth.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Earth.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Earth.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Earth.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Earth.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Earth.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Earth.get_sphere().get_shader().setFloat("specularStrenght", 0.2);
    Earth.get_sphere().get_shader().setFloat("shininess",32.0f);

    Mars.get_sphere().get_shader().use();
    Mars.get_sphere().get_shader().setVec3("light.position",center_S);
    Mars.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Mars.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Mars.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Mars.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Mars.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Mars.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Mars.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Mars.get_sphere().get_shader().setFloat("shininess",8.0f);

    Venus.get_sphere().get_shader().use();
    Venus.get_sphere().get_shader().setVec3("light.position",center_S);
    Venus.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Venus.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Venus.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Venus.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Venus.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Venus.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Venus.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Venus.get_sphere().get_shader().setFloat("shininess",8.0f);

    Mercury.get_sphere().get_shader().use();
    Mercury.get_sphere().get_shader().setVec3("light.position",center_S);
    Mercury.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Mercury.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Mercury.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Mercury.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Mercury.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Mercury.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Mercury.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Mercury.get_sphere().get_shader().setFloat("shininess",1.0f);

    //Moon.get_sphere().get_shader().use();
    //Moon.get_sphere().get_shader().setVec3("light.position",center_S);
    //Moon.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    //Moon.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    //Moon.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    //Moon.get_sphere().get_shader().setFloat("light.constant",1.0f);
    //Moon.get_sphere().get_shader().setFloat("light.linear",0.5f);
    //Moon.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    //Moon.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    //Moon.get_sphere().get_shader().setFloat("shininess",1.0f);


    while(!glfwWindowShouldClose(window)){
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        processInput(window);
        glm::vec3 pos_Earth;
        glm::vec3 pos_Mars;
        glm::vec3 pos_Venus;
        glm::vec3 pos_Mercury;
        glm::vec3 pos_Moon;

        physicsAccumulator += deltaTime * TIME_MULTIPLIER;
        while(physicsAccumulator >= dt){
            Earth.compute_step();
            Mars.compute_step();
            Venus.compute_step();
            Mercury.compute_step();
            //Moon.compute_step();
            pos_Earth = glm::make_vec3(Earth.get_pos().data());
            pos_Mars = glm::make_vec3(Mars.get_pos().data());
            pos_Venus = glm::make_vec3(Venus.get_pos().data());
            pos_Mercury = glm::make_vec3(Mercury.get_pos().data());
            //pos_Moon = glm::make_vec3(Moon.get_pos().data());
            pos_Earth = {SCALE * (pos_Earth[0] / AU), SCALE * (pos_Earth[1] / AU), 0.0f};
            pos_Mars = {SCALE * (pos_Mars[0] / AU), SCALE * (pos_Mars[1] / AU), 0.0f};
            pos_Venus = {SCALE * (pos_Venus[0] / AU), SCALE * (pos_Venus[1] / AU), 0.0f};
            pos_Mercury = {SCALE * (pos_Mercury[0] / AU), SCALE * (pos_Mercury[1] / AU), 0.0f};
            //pos_Moon = {SCALE * (pos_Moon[0] / AU), SCALE * (pos_Moon[1] / AU), 0.0f};
            physicsAccumulator -= dt;
        }
        
        Earth.get_sphere().get_shader().setVec3("light.position", center_S);
        Mars.get_sphere().get_shader().setVec3("light.position",center_S);
        Venus.get_sphere().get_shader().setVec3("light.position", center_S);
        Mercury.get_sphere().get_shader().setVec3("light.position", center_S);
        //Moon.get_sphere().get_shader().setVec3("light.position", center_S);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 projection = glm::perspective(glm::radians(fov) , (float)WIDTH / (float)HEIGHT,0.1f,100.0f);
        glm::mat4 view = glm::lookAt(cameraPos , cameraPos + cameraFront, cameraUp);

        Earth.get_sphere().set_position(pos_Earth);
        Mars.get_sphere().set_position(pos_Mars);
        Venus.get_sphere().set_position(pos_Venus);
        Mercury.get_sphere().set_position(pos_Mercury);
        Sun.get_sphere().render(view, projection);
        Earth.get_sphere().render(view, projection);
        Mars.get_sphere().render(view, projection);
        Venus.get_sphere().render(view, projection);
        Mercury.get_sphere().render(view, projection);
        glPointSize(1.0f);
        Earth.render_orbit(view, projection);
        Mars.render_orbit(view, projection);
        Venus.render_orbit(view, projection);
        Mercury.render_orbit(view, projection);
        Sun.render(view, projection, center_S);
        Earth.render(view, projection, pos_Earth);
        //Moon.render(view, projection, pos_Moon);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}