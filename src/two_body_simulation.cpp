#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sphere.hpp"
#include "planet.hpp"

const int WIDTH = 800;
const int HEIGHT = 800;

float lastX = (float)WIDTH / 2.0f;
float lastY = (float)HEIGHT / 2.0f;
bool firstMouse = true;
float fov = 45.0f;

float roll,pitch;
float yaw = - 90.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
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

const float PHYSICS_DT = 6 * 3600.0f;
const float TIME_MULTIPLIER = 500000.0f;
float physicsAccumulator = 0.0f;
const float SCALE = 0.75f;
float timeAccumulator = 0.0f;

std::vector<float> S_pos;
std::vector<float> r;
std::vector<float> a;
std::vector<float> r_moins1;
std::vector<float> v;
float m_S = 1.989e30f;
float G = 6.674e-11f;
float dt =  PHYSICS_DT; 

std::vector<float> r_prev;
std::vector<float> r_new;
std::vector<float> a_new;

void setup_init_condition(){

    S_pos = {0.0f , 0.0f , 0.0f};
    r = {1.496e11f, 0.0f, 0.0f}; //r0
    v = {0.0f , 29780.0f, 0.0f}; //v0
    float norm = sqrt(r[0] * r[0] + r[1] * r[1] + r[2] * r[2]);
    float ax = (-G * m_S / (norm * norm * norm)) * r[0];
    float ay = (-G * m_S / (norm * norm * norm)) * r[1];
    float az = (-G * m_S / (norm * norm * norm)) * r[2];
    a = {ax, ay, az}; //a0

    float r_moins1x = r[0] - v[0] * dt + 0.5 * (dt * dt) * a[0];
    float r_moins1y = r[1] - v[1] * dt + 0.5 * (dt * dt) * a[1];
    float r_moins1z = r[2] - v[2] * dt + 0.5 * (dt * dt) * a[2];
    r_prev = {r_moins1x, r_moins1y, r_moins1z}; //r-1
}

void next_step (){
    //First new position with Verlet integration
    float rx = 2 * r[0] - r_prev[0] + dt * dt * a[0];
    float ry = 2 * r[1] - r_prev[1] + dt * dt * a[1];
    float rz = 2 * r[2] - r_prev[2] + dt * dt * a[2];
    r_new = {rx, ry, rz};
    float norm = sqrt(rx * rx + ry * ry + rz * rz);
    float inv = 1.0f / (norm * norm * norm);
    //Then update acceleration
    float ax = - (G * m_S * inv) * rx;
    float ay = - (G * m_S * inv) * ry;
    float az = - (G * m_S * inv) * rz;
    a = {ax, ay, az};
    //Finally update position
    r_prev = r;
    r = r_new;
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

    setup_init_condition();
    glm::vec3 pos_T (r[0] / 1.496e11f, r[1] / 1.496e11f, 0.0f);
    glm::vec3 center_S (0.0f);

    //Sphere S(0.25, {1.000, 0.647, 0.000}, center_S);
    //Sphere T(0.0625, {0.0f, 0.0f, 0.886}, pos_T, "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");


    Planet S({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.25, dt, {1.000, 0.647, 0.000});
    Planet T({1.496e11f, 0.0f, 0.0f}, {0.0f , 29780.0f, 0.0f}, 0.0625, dt, {0.0f, 0.0f, 0.886}, "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    T.get_sphere().get_shader().use();
    T.get_sphere().get_shader().setVec3("light.position",center_S);
    T.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    T.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    T.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    T.get_sphere().get_shader().setFloat("light.constant",1.0f);
    T.get_sphere().get_shader().setFloat("light.linear",0.5f);
    T.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    T.get_sphere().get_shader().setFloat("specularStrenght", 0.2);
    T.get_sphere().get_shader().setFloat("shininess",32.0f);

    Planet M({2.279e11f, 0.0f, 0.0f}, {0.0f , 24080.0f, 0.0f}, 0.0625, dt, {0.7f, 0.35f, 0.2f}, "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    M.get_sphere().get_shader().use();
    M.get_sphere().get_shader().setVec3("light.position",center_S);
    M.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    M.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    M.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    M.get_sphere().get_shader().setFloat("light.constant",1.0f);
    M.get_sphere().get_shader().setFloat("light.linear",0.5f);
    M.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    M.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    M.get_sphere().get_shader().setFloat("shininess",8.0f);

    while(!glfwWindowShouldClose(window)){
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        processInput(window);
        glm::vec3 pos_T;
        glm::vec3 pos_M;

        physicsAccumulator += deltaTime * TIME_MULTIPLIER;
        while(physicsAccumulator >= PHYSICS_DT){
            T.compute_step();
            M.compute_step();
            pos_T = glm::make_vec3(T.get_pos().data());
            pos_M = glm::make_vec3(M.get_pos().data());
            pos_T = {SCALE * (pos_T[0] / 1.496e11f), SCALE * (pos_T[1] / 1.496e11f), 0.0f};
            pos_M = {SCALE * (pos_M[0] / 1.496e11f), SCALE * (pos_M[1] / 1.496e11f), 0.0f};
            physicsAccumulator -= PHYSICS_DT;
        }
        
        T.get_sphere().get_shader().setVec3("light.direction", pos_T - center_S);
        M.get_sphere().get_shader().setVec3("light.direction", pos_M - center_S);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 projection = glm::perspective(glm::radians(fov) , (float)WIDTH / (float)HEIGHT,0.1f,100.0f);
        glm::mat4 view = glm::lookAt(cameraPos , cameraPos + cameraFront, cameraUp);

        T.get_sphere().set_position(pos_T);
        M.get_sphere().set_position(pos_M);
        S.get_sphere().render(view, projection);
        T.get_sphere().render(view, projection);
        M.get_sphere().render(view, projection);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}