//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include "main.h"
//#include <cmath>
//
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//
//const char *vertexShaderSource = "#version 330 core\n"
//    "layout (location = 0) in vec3 aPos;\n"
//    "layout (location = 1) in vec3 aColor;\n"
//    "layout (location = 2) in vec2 aTexCoord;\n"
//
//    "out vec3 ourColor;\n"
//    "out vec2 TexCoord;\n"
//
//    "void main()\n"
//    "{\n"
//    "   gl_Position = vec4(aPos, 1.0);\n"
//    "   ourColor = aColor;\n"
//    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
//    "}\0";
//
//const char *fragmentShaderSource = "#version 330 core\n"
//    "out vec4 FragColor;\n"
//
//    "in vec3 ourColor;\n"
//    "in vec2 TexCoord;\n"
//
//    "uniform sampler2D texture1;\n"
//    "uniform sampler2D texture2;\n"
//
//    "void main()\n"
//    "{\n"
//    "   FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord),0.2);\n"
//    "}\0";
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height){
//    glViewport(0,0,width,height);
//}
//
//void processInput(GLFWwindow *window){
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//}
//int main(){
//
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    GLFWwindow* window = glfwCreateWindow(800,600, "Test", NULL, NULL);
//    if(window == NULL){
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
//    std::cout << "Failed to initialize GLAD" << std::endl;
//    return -1;
//    }    
//
//    float vertices[] = {
//        // positions          // colors           // texture coords
//         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
//         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
//        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
//    };
//
//    unsigned int indices[] = {
//        0, 1, 3,
//        1, 2, 3
//    };
//
//    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
//    glCompileShader(vertexShader);
//
//    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
//    glCompileShader(fragmentShader);
//
//    unsigned int shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//    
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//
//    glViewport(0,0,800,600);
//
//    unsigned int VBO,VAO,EBO;
//    glGenVertexArrays(1,&VAO);
//    glGenBuffers(1,&VBO);
//    glGenBuffers(1,&EBO);
//
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
//    glEnableVertexAttribArray(2);
//
//    unsigned int texture1, texture2;
//    glGenTextures(1, &texture1);
//    glBindTexture(GL_TEXTURE_2D, texture1);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    
//    int width, height, nrChannels;
//    unsigned char *data1 = stbi_load("../img/wood.jpg", &width, &height, &nrChannels,0);
//    if (data1){
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
//        glGenerateMipmap(GL_TEXTURE_2D);
//        std::cout << "Texture loaded." << std::endl;
//    }
//    else{
//        std::cout << "Failed to load texture." <<std::endl;
//    }
//
//    stbi_image_free(data1);
//
//    glGenTextures(1, &texture2);
//    glBindTexture(GL_TEXTURE_2D, texture2);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    unsigned char *data2 = stbi_load("../img/awesomeface.png", &width, &height, &nrChannels,0);
//    if (data2){
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,0,GL_RGBA,GL_UNSIGNED_BYTE,data2);//GL_RGBA because png
//        glGenerateMipmap(GL_TEXTURE_2D);
//        std::cout << "Texture awesome loaded." << std::endl;
//    }
//    else{
//        std::cout << "Failed to load awesome texture." << std::endl;
//    }
//
//    stbi_image_free(data2);
//
//    glUseProgram(shaderProgram);
//    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"),0);
//    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"),1);
//
//    while (!glfwWindowShouldClose(window)){
//        processInput(window);
//
//        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture1);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, texture2);
//
//        glUseProgram(shaderProgram);
//        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_INT,0);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//    glfwTerminate();
//    return 0;
//}