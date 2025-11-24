#include <cube.hpp>

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
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

const char* sourceVertexShader = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    
    "void main()\n"
    "{\n"
    	"gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\0";

const char * sourceFragmentShader = "#version 330 core\n"
    "out vec4 FragColor\n;"

    "void main()\n"
    "{\n"
        "FragColor = vec4(1.0);\n" 
    "}\0";

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

    Cube C1(glm::vec3(0.0f));
    unsigned int cube_vao = C1.get_VAO();
    glm::mat4 model = C1.get_model();
    Shader cube_shader = C1.get_shader();

    Cube C2(glm::vec3(1.0f) , "../img/container2.png");
    unsigned int TextureCube_vao = C2.get_VAO();
    glm::mat4 TextureCubeModel = C2.get_model();
    Shader TextureCube_shader = C2.get_shader();
    TextureCube_shader.setInt("material.diffuse",0);
    TextureCube_shader.setInt("material.specular",0);

    while(!glfwWindowShouldClose(window)){

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

        cube_shader.use();
        //vertex shader
        glm::mat4 projection = glm::perspective(glm::radians(fov) , (float)WIDTH / (float)HEIGHT,0.1f,100.0f);
        cube_shader.setMat4("projection", projection);
        glm::mat4 view = glm::lookAt(cameraPos , cameraPos + cameraFront, cameraUp);
        cube_shader.setMat4("view", view);
        cube_shader.setMat4("model", model);
        //fragment shader
        cube_shader.setVec3("material.ambient",glm::vec3(1.0f, 0.5f, 0.31f));
        cube_shader.setVec3("material.diffuse",glm::vec3(1.0f, 0.5f, 0.31f));
        cube_shader.setVec3("material.specular",glm::vec3(0.5f, 0.5f, 0.5f));
        cube_shader.setFloat("material.shininess",32.0f);

        cube_shader.setVec3("viewPos", cameraPos);
        cube_shader.setVec3("light.position", lightPos);
        cube_shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        cube_shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        cube_shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        
        glBindVertexArray(cube_vao);
        glDrawArrays(GL_TRIANGLES,0,36);

        TextureCube_shader.use();
        TextureCube_shader.setMat4("projection", projection);
        TextureCube_shader.setMat4("view", view);
        TextureCube_shader.setMat4("model", TextureCubeModel);
        
        TextureCube_shader.setVec3("light.position", lightPos);
        TextureCube_shader.setVec3("viewPos", cameraPos);

        TextureCube_shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        TextureCube_shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        TextureCube_shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        TextureCube_shader.setFloat("material.shininess", 64.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 1);

        glBindVertexArray(TextureCube_vao);
        glDrawArrays(GL_TRIANGLES,0,36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}