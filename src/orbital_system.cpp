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

    float R_Mercury = 2439.7f;
    float R_Venus = 6051.8f;
    float R_Earth = 6371.0f; //kms
    float R_Mars = 3389.5f;
    float R_Jupiter = 69911.0f;
    float R_Saturn = 58232.0f;
    float R_Uranus = 25362.0f;
    float R_Neptune = 24622.0f;

    float R_Moon = 1737.4f;

    float R_Mercury_scale = R_Mercury / 63710.0f;
    float R_Venus_scale = R_Venus / 63710.0f;
    float R_Earth_scale = R_Earth / 63710.0f; //R_T is divided by 10. 
    float R_Mars_scale = R_Mars / 63710.0f; //All other radius are scaled in comparaison with Earth radius
    float R_Jupiter_scale = R_Jupiter / 63710.0f;
    float R_Saturn_scale = R_Saturn / 63710.0f;
    float R_Uranus_scale = R_Uranus / 63710.0f;
    float R_Neptune_scale = R_Neptune / 63710.0f;

    float R_Moon_scale = R_Moon / 63710.0f;

    //Planet Mass
    float M_Sun = 1.99e30f;
    float M_Mercury = 3.3e23f;
    float M_Venus = 4.87e24f;
    float M_Earth = 5.97e24f;
    float M_Mars = 6.42e23f;
    float M_Jupiter = 1.90e27f;
    float M_Saturn = 5.68e26f;
    float M_Uranus = 8.68e25f;
    float M_Neptune = 1.02e26f;
    //Satelitte Mass
    float M_Moon = 7.35e22f;

    //Planet velocity
    std::vector<float> V_Sun     = {0.0f, 0.0f, 0.0f};
    std::vector<float> V_Mercury = {0.0f, 47360.0f, 0.0f};
    std::vector<float> V_Venus   = {0.0f, 35025.0f, 0.0f};
    std::vector<float> V_Earth   = {0.0f, 29780.0f, 0.0f};
    std::vector<float> V_Mars    = {0.0f, 24130.0f, 0.0f};
    std::vector<float> V_Jupiter = {0.0f, 13058.0f, 0.0f};
    std::vector<float> V_Saturn  = {0.0f, 9680.0f, 0.0f};
    std::vector<float> V_Uranus  = {0.0f, 6796.0f, 0.0f};
    std::vector<float> V_Neptune = {0.0f, 5432.0f, 0.0f};
    //Satelitte velocity
    std::vector<float> V_Moon    = {0.0f, 29780.0f + 1022.0f, 0.0f};

    //Planet initial position
    std::vector<float> P_Sun     = {0.0f, 0.0f, 0.0f};
    std::vector<float> P_Mercury = {5.80e10f, 0.0f, 0.0f};
    std::vector<float> P_Venus   = {1.08e11f, 0.0f, 0.0f};
    std::vector<float> P_Earth   = {1.50e11f, 0.0f, 0.0f};
    std::vector<float> P_Mars    = {2.28e11f, 0.0f, 0.0f};
    std::vector<float> P_Jupiter = {7.79e11f, 0.0f, 0.0f};
    std::vector<float> P_Saturn  = {1.43e12f, 0.0f, 0.0f};
    std::vector<float> P_Uranus  = {2.92e12f, 0.0f, 0.0f};
    std::vector<float> P_Neptune = {4.47e12f, 0.0f, 0.0f};
    //Satellite initial position
    std::vector<float> P_Moon    = {1.50e11f + 3e8f, 0.0f, 0.0f};

    //Planet Color
    std::vector<float> C_Sun      = {1.000, 0.647, 0.000};
    std::vector<float> C_Mercury  = {0.5f, 0.5f, 0.5f};
    std::vector<float> C_Venus    = {0.94f, 0.89f, 0.78f};
    std::vector<float> C_Earth    = {0.0f, 0.0f, 0.886};
    std::vector<float> C_Mars     = {0.7f, 0.35f, 0.2f};
    std::vector<float> C_Jupiter  = {0.85f, 0.75f, 0.60f};   
    std::vector<float> C_Saturn   = {0.90f, 0.82f, 0.67f};   
    std::vector<float> C_Uranus   = {0.55f, 0.80f, 0.82f};   
    std::vector<float> C_Neptune  = {0.10f, 0.30f, 0.70f};  
    //Satellite Color
    std::vector<float> C_Moon     = {0.5f, 0.5f, 0.5f};

    OrbitalSystem solarSystem;
    glm::vec3 sunCenter(0.0f);

    CelestialObject Sun(P_Sun, V_Sun, 0.25, M_Sun, C_Sun, nullptr, false, 24 * 3600.0f,
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere.fs");
    CelestialObject Mercury(P_Mercury, V_Mercury, R_Mercury_scale, M_Mercury, C_Mercury, nullptr, false, 24 * 3600.0f, 
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Venus(P_Venus, V_Venus, R_Venus_scale, M_Venus, C_Venus, nullptr, false, 24 * 3600.0f, 
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Earth(P_Earth, V_Earth, R_Earth_scale, M_Earth, C_Earth, nullptr, false, 24 * 3600.0f, 
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Mars(P_Mars, V_Mars, R_Mars_scale, M_Mars, C_Mars, nullptr, false, 24 * 3600.0f,
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Jupiter(P_Jupiter, V_Jupiter, R_Jupiter_scale, M_Jupiter, C_Jupiter, nullptr, false, 24 * 3600.0f,
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Saturn(P_Saturn, V_Saturn, R_Saturn_scale, M_Saturn, C_Saturn, nullptr, false, 24 * 3600.0f,
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Uranus(P_Uranus, V_Uranus, R_Uranus_scale, M_Uranus, C_Uranus, nullptr, false, 24 * 3600.0f,
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");
    CelestialObject Neptune(P_Neptune, V_Neptune, R_Neptune_scale, M_Neptune, C_Neptune, nullptr, false, 24 * 3600.0f,
                                "../shaders/sphere/sphere.vs", "../shaders/sphere/sphere_directionnal.fs");

    CelestialObject Moon(P_Moon, V_Moon, R_Moon_scale, M_Moon ,C_Moon, nullptr, false, 24 * 3600.0f, 
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

    Jupiter.set_orbitalSystem(&solarSystem);
    Jupiter.set_orbitalCenter(&Sun);

    Saturn.set_orbitalSystem(&solarSystem);
    Saturn.set_orbitalCenter(&Sun);

    Uranus.set_orbitalSystem(&solarSystem);
    Uranus.set_orbitalCenter(&Sun);

    Neptune.set_orbitalSystem(&solarSystem);
    Neptune.set_orbitalCenter(&Sun);

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

    Jupiter.get_sphere().get_shader().use();
    Jupiter.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Jupiter.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Jupiter.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Jupiter.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Jupiter.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Jupiter.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Jupiter.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Jupiter.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Jupiter.get_sphere().get_shader().setFloat("shininess",8.0f);

    Saturn.get_sphere().get_shader().use();
    Saturn.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Saturn.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Saturn.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Saturn.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Saturn.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Saturn.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Saturn.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Saturn.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Saturn.get_sphere().get_shader().setFloat("shininess",8.0f);

    Uranus.get_sphere().get_shader().use();
    Uranus.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Uranus.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Uranus.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Uranus.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Uranus.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Uranus.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Uranus.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Uranus.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Uranus.get_sphere().get_shader().setFloat("shininess",8.0f);

    Neptune.get_sphere().get_shader().use();
    Neptune.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Neptune.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Neptune.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Neptune.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Neptune.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Neptune.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Neptune.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Neptune.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Neptune.get_sphere().get_shader().setFloat("shininess",8.0f);

    Moon.get_sphere().get_shader().use();
    Moon.get_sphere().get_shader().setVec3("light.position",sunCenter);
    Moon.get_sphere().get_shader().setVec3("light.ambient",glm::vec3(0.2f));
    Moon.get_sphere().get_shader().setVec3("light.diffuse", glm::vec3(1.0f,1.0f,0.95f));
    Moon.get_sphere().get_shader().setVec3("light.specular",glm::vec3(1.0f, 1.0f, 1.0f));
    Moon.get_sphere().get_shader().setFloat("light.constant",1.0f);
    Moon.get_sphere().get_shader().setFloat("light.linear",0.5f);
    Moon.get_sphere().get_shader().setFloat("light.quadratic",0.25f);
    Moon.get_sphere().get_shader().setFloat("specularStrenght", 0.05);
    Moon.get_sphere().get_shader().setFloat("shininess",1.0f);

    solarSystem.fix_center(true);
    auto EarthSystem = std::make_shared<OrbitalSystem>();
    EarthSystem->define_center(&Earth);
    EarthSystem->add_orbiters(&Moon);

    solarSystem.add_subsystem(EarthSystem);

    solarSystem.add_orbiters(&Venus);
    solarSystem.add_orbiters(&Mercury);
    solarSystem.add_orbiters(&Mars);
    solarSystem.add_orbiters(&Jupiter);
    solarSystem.add_orbiters(&Saturn);
    solarSystem.add_orbiters(&Neptune);
    solarSystem.add_orbiters(&Uranus);

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