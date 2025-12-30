#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

#include "../include/firework.hpp"

const int WIDTH = 1200;
const int HEIGHT = 1000;

struct Data {
    std::vector<std::unique_ptr<firework>> firework;
    glm::mat4 view;
    glm::mat4 projection;
};

std::mt19937 generator(std::random_device{}());
std::uniform_real_distribution<float> dist(-3.0f, 2.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        double xposition, yposition;
        glfwGetCursorPos(window, &xposition, &yposition);

        Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
        glm::mat4 invVP = glm::inverse(data->projection * data->view);

        float x_ndc = (2.0f * xposition / WIDTH)- 1.0f;
        float y_ndc = 1.0f - (2.0f * yposition / HEIGHT);

        glm::vec4 nearPoint = invVP * glm::vec4(x_ndc, y_ndc, -1.0f, 1.0f);
        glm::vec4 farPoint = invVP * glm::vec4(x_ndc, y_ndc, 1.0f, 1.0f);

        nearPoint /= nearPoint.w;
        farPoint /= farPoint.w;

        glm::vec3 rayDir = glm::normalize(glm::vec3(farPoint - nearPoint));
        glm::vec3 rayOrigin = glm::vec3(nearPoint);

        float z_plane = dist(generator);
        float t = (z_plane -rayOrigin.z) / rayDir.z;
        glm::vec3 center = rayOrigin + t * rayDir;
        center.z = z_plane;

        glm::vec3 start_point = {center.x, center.y - 2.0f, center.z};

        //data->firework.push_back(std::make_unique<Sphere>(0.5f, std::vector<float>{1.0f,1.0f,1.0f}, center));
        data->firework.push_back(std::make_unique<firework>(0.5f, center));
        std::cout << data->firework.size() << std::endl;
        //trainee(center);
    }
}

int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Firework", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Data data;
    data.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
    data.projection = glm::perspective(glm::radians(45.0f),
                                       (float)WIDTH / (float)HEIGHT,
                                       0.1f, 100.0f);                       
    glfwSetWindowUserPointer(window, &data);
    glEnable(GL_DEPTH_TEST);

    Sphere S(0.5f, {1.0f, 1.0f, 1.0f}, glm::vec3(0.0f,0.0f,0.0f));

    while(!glfwWindowShouldClose(window)){

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto& f : data.firework){
            //f->set_scale({sin(glfwGetTime()),sin(glfwGetTime()),sin(glfwGetTime())});
            //f->render_points(data.view, data.projection);
            f->animate(data.view, data.projection);
        }
        std::erase_if(data.firework, [](const std::unique_ptr<firework>& f){
                                            return f->isExpired();
                                        });

        S.render_points(data.view, data.projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}