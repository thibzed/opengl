#ifndef CUBE_HPP
#define CUBE_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Cube{

    public:
        Cube(glm::vec3 center) : _center(center) , _hasTexture(false), _shader("../shaders/cube_shader/material/cube_material.vs", "../shaders/cube_shader/material/cube_material.fs"){
            
            _model = glm::translate(_model, _center);    
            initVerticesNoTexture();
            setupBuffer();
        };
        Cube(glm::vec3 center, const std::string& pathTexture) : _center(center), _hasTexture(true), _shader("../shaders/cube_shader/texture_diffuse/cube_texture_diffuse.vs", "../shaders/cube_shader/texture_diffuse/cube_texture_diffuse.fs"){

            _model = glm::translate(_model, _center);
            initVerticesWithTexture();
            loadTexture(pathTexture);
            setupBuffer();
        }

    ~Cube(){};
    unsigned int get_VAO() const {
        return _VAO;
    }
    glm::mat4 get_model() const {
        return _model;
    }
    Shader get_shader() const {
        return _shader;
    }
    unsigned int get_texture() const {
        return _texture;
    }

    private:
        std::vector<float> _cube_vertices;
        unsigned int _VBO;
        unsigned int _VAO;
        bool _hasTexture;
        unsigned int _texture;
        Shader _shader;

        glm::vec3 _center;
        glm::mat4 _model = glm::mat4(1.0f);

        void initVerticesNoTexture(){
            _cube_vertices = {
                // positions          // normals          
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  
                 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  
                 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  
                 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  
                -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  
                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  

                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 
                -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 
                -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 
                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 

                 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 
                 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 
                 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 
                 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 
                 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 
                 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 

                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 
                 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 
                 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 
                 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 
                -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 
                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 

                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  
                 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  
                 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  
                 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  
                -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  
                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 
            };
        }
        void initVerticesWithTexture(){
            _cube_vertices = {
                // positions          // normals           // texture coords
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
                 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
                 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

                 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
                 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
                 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
                 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
                 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
                 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
            };
        }
        void setupBuffer(){
            glGenVertexArrays(1,&_VAO);
            glGenBuffers(1,&_VBO);

            glBindVertexArray(_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, _VBO);

            if(_hasTexture){
                glBufferData(GL_ARRAY_BUFFER, _cube_vertices.size() * sizeof(float), _cube_vertices.data(),GL_STATIC_DRAW);
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(3*sizeof(float)));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(6*sizeof(float)));
                glEnableVertexAttribArray(2);
            }
            else{
                glBufferData(GL_ARRAY_BUFFER, _cube_vertices.size() * sizeof(float), _cube_vertices.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
                glEnableVertexAttribArray(1);
            }
        }
        void loadTexture(const std::string& path){
            glGenTextures(1, &_texture);
            glBindTexture(GL_TEXTURE_2D, _texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height, nrChannels;
            unsigned char *data = stbi_load(path.c_str(), &width , &height, &nrChannels, 0);
            if (data){
                GLenum format;
                if(nrChannels == 1) format = GL_RED;
                else if(nrChannels == 3) format = GL_RGB;
                else if(nrChannels == 4) format = GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                std::cout << "Texture at " << path << " loaded." << std::endl;
            }
            else{
                std::cerr << "Failed to load texture at " << path << std::endl;
            }
            stbi_image_free(data);
        }
};
//Still need to be update for specular and diffuse texture cube. 
#endif