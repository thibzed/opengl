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
        
        Cube(const Cube&) = delete; //Désactive la copie.
        Cube& operator=(const Cube&) = delete; //Interdit l'affectation par copie.

        Cube(Cube&& other) noexcept : //Constructeur de move std::move
        _cube_vertices(std::move(other._cube_vertices)), _VAO(other._VAO), _VBO(other._VBO), 
        _hasTexture(other._hasTexture), _hasDualTexture(other._hasDualTexture), 
        _textureDiffuse(other._textureDiffuse), _textureSpecular(other._textureSpecular),
        _shader(std::move(other._shader)), _color(other._color), _material(other._material),
        _renderMode(other._renderMode), _scale(other._scale), _center(other._center), _rotation(other._rotation),
        _rotationAxis(other._rotationAxis), _model(other._model), _modeldirty(other._modeldirty){
            other._VAO = 0;
            other._VBO = 0;
            other._textureDiffuse = 0;
            other._textureSpecular = 0;
        }

        Cube& operator=(Cube&& other) noexcept { //Affectation par move, other est l'object temporaire dont on vole les ressources, this est le nouvel object qui récup les ressources.
            if (this != &other) {
                if(_VAO != 0) glDeleteVertexArrays(1, &_VAO);
                if(_VBO != 0) glDeleteBuffers(1, &_VBO);
                if(_hasTexture && _textureDiffuse != 0) glDeleteTextures(1, &_textureDiffuse);
                if(_hasDualTexture){
                    if(_textureDiffuse != 0) glDeleteTextures(1, &_textureDiffuse);
                    if(_textureSpecular != 0) glDeleteTextures(1, &_textureSpecular);
                }
            _cube_vertices = std::move(other._cube_vertices);
             _VBO = other._VBO;
            _VAO = other._VAO;
            _hasTexture = other._hasTexture;
            _hasDualTexture = other._hasDualTexture;
            _textureDiffuse = other._textureDiffuse;
            _textureSpecular = other._textureSpecular;
            _shader = std::move(other._shader);
            _color = other._color;
            _material = other._material;
            _renderMode = other._renderMode;
            _scale = other._scale;
            _center = other._center;
            _rotationAxis = other._rotationAxis;
            _rotation = other._rotation;
            _model = other._model;
            _modeldirty = other._modeldirty;

            other._VAO = 0;
            other._VBO = 0;
            other._textureDiffuse = 0;
            other._textureSpecular = 0;
            }
            return *this;
        }

        enum class RenderMode {Material, Texture, Color};

        struct Material {
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
            float shininess;
        };

    ~Cube(){
        glDeleteVertexArrays(1,&_VAO);
        glDeleteBuffers(1,&_VBO);
        if(_hasTexture){glDeleteTextures(1,&_textureDiffuse);}
        else if(_hasDualTexture){
            glDeleteTextures(1,&_textureDiffuse);
            glDeleteTextures(1,&_textureSpecular);
        }
    };

    void render(const glm::mat4& view, const glm::mat4& projection, 
                const glm::vec3& lightPos, const glm::vec3& cameraPos){
        _shader.use();
        _shader.setMat4("model", get_model());
        _shader.setMat4("view",view);
        _shader.setMat4("projection", projection);
        if (_renderMode == RenderMode::Color){
            _shader.setVec3("objectColor", _color);
        }
        else if (_renderMode == RenderMode::Material){
            _shader.setVec3("material.ambient", _material.ambient);
            _shader.setVec3("material.diffuse", _material.diffuse);
            _shader.setVec3("material.specular", _material.specular);
            _shader.setFloat("material.shininess",_material.shininess);

            _shader.setVec3("viewPos", cameraPos);
            _shader.setVec3("light.position", lightPos);
            _shader.setVec3("light.ambient", glm::vec3(0.2f));
            _shader.setVec3("light.diffuse", glm::vec3(0.5f));
            _shader.setVec3("light.specular", glm::vec3(1.0f));
        }
        else if (_renderMode == RenderMode::Texture){
            _shader.setVec3("viewPos", cameraPos);
            _shader.setVec3("light.position", lightPos);
            _shader.setVec3("light.direction",lightPos);
            _shader.setVec3("light.ambient", glm::vec3(0.2f));
            _shader.setVec3("light.diffuse", glm::vec3(0.5f));
            _shader.setVec3("light.specular", glm::vec3(1.0f));
            _shader.setFloat("material.shininess", 64.0f);
            //_shader.setFloat("light.constant", 1.0f);
            //_shader.setFloat("light.linear", 0.09f);
            //_shader.setFloat("light.quadratic", 0.032f);

            if(_hasDualTexture){
                _shader.setInt("material.diffuse", 0);
                _shader.setInt("material.specular",1);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, _textureDiffuse);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, _textureSpecular);
            }
            else{
                _shader.setInt("material.diffuse",0);
                _shader.setInt("material.specular",0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, _textureDiffuse);
            }
        }
        glBindVertexArray(_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void set_light_attenuation(float constant, float linear, float quadratic){
        _shader.use();
        _shader.setFloat("light.constant",constant);
        _shader.setFloat("light.linear", linear);
        _shader.setFloat("light.quadratic", quadratic);
    }

    static Cube withColor(glm::vec3 center, glm::vec3 color,
                          const std::string& vertexShader = "../shaders/cube_shader/basic_cube/cube.vs",
                          const std::string& fragmentShader = "../shaders/cube_shader/basic_cube/cube.fs"){
        return Cube(center, color, vertexShader, fragmentShader);
    }
    static Cube withMaterial(glm::vec3 center, const Material& materialProperties,
                             const std::string& vertexShader = "../shaders/cube_shader/material/cube_material.vs",
                             const std::string& fragmentShader = "../shaders/cube_shader/material/cube_material.fs"){
        return Cube(center, materialProperties, vertexShader, fragmentShader);
    }
    static Cube withTexture(glm::vec3 center, const std::string& path,
                            const std::string& vertexShader = "../shaders/cube_shader/texture_diffuse/cube_texture_diffuse.vs",
                            const std::string& fragmentShader = "../shaders/cube_shader/texture_diffuse/cube_texture_diffuse.fs"){
        return Cube(center, path, vertexShader, fragmentShader);
    }
    static Cube withDualTexture(glm::vec3 center, const std::string& diffusePath, const std::string& specularPath,
                                const std::string& shaderVertex = "../shaders/cube_shader/texture_specular/cube_texture_specular.vs",
                                const std::string& shaderFragment = "../shaders/cube_shader/texture_specular/cube_texture_specular.fs"){
        return Cube(center, diffusePath, specularPath, shaderVertex, shaderFragment);
    }

    unsigned int get_VAO() const {
        return _VAO;
    }
    Shader& get_shader() { //Passage par réfèrence car on ne peut plus copier le shader pour des raisons de sécurité. 
        return _shader;
    }
    unsigned int get_textureDiffuse() const {
        return _textureDiffuse;
    }
    unsigned int get_textureSpecular() const {
        return _textureSpecular;
    }
    glm::vec3 get_color() const {
        return _color;
    }
    void set_color(glm::vec3 color) {
        _color = color;
    }
    void set_position(glm::vec3 newPos) {
        if (_center != newPos){
            _center = newPos;
            _modeldirty = true;
        }
    }
    void set_scale(glm::vec3 scale) {
        if(_scale != scale){
            _scale = scale;
            _modeldirty = true;
        }
    }
    void set_rotation(float angle, glm::vec3 axis){
        if (_rotation != angle || _rotationAxis != axis){
            _rotation = angle;
            _rotationAxis = axis;
            _modeldirty = true;
        }
    }
    glm::mat4 get_model(){
        if(_modeldirty){
            updateModel();
            _modeldirty = false;
        }
        return _model;
    }

    private:
        std::vector<float> _cube_vertices;
        unsigned int _VBO;
        unsigned int _VAO;

        bool _hasTexture = false;
        bool _hasDualTexture = false;
        unsigned int _textureDiffuse = 0;
        unsigned int _textureSpecular = 0;
        Shader _shader;
        glm::vec3 _color;
        Material _material;
        RenderMode _renderMode;
        
        glm::vec3 _scale = glm::vec3(1.0f);
        glm::vec3 _center = glm::vec3(0.0f);
        glm::vec3 _rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        float _rotation = 0.0f;

        glm::mat4 _model = glm::mat4(1.0f);
        bool _modeldirty = true;

        Cube(glm::vec3 center, glm::vec3 color,
             const std::string& vertexShader, const std::string& fragmentShader) : _center(center), _hasTexture(false), _color(color),
             _renderMode(RenderMode::Color), _hasDualTexture(false),
             _shader(vertexShader.c_str(),fragmentShader.c_str()){

            initVerticesNoTexture();
            setupBuffer();
        }

        Cube(glm::vec3 center, const Material& materialProperties,
             const std::string& vertexShader, const std::string& fragmentShader) : _center(center), _hasTexture(false),
             _renderMode(RenderMode::Material), _material(materialProperties), _hasDualTexture(false),
             _shader(vertexShader.c_str(),fragmentShader.c_str()){
            
            initVerticesNoTexture();
            setupBuffer();
        }

        Cube(glm::vec3 center, const std::string& pathTexture,
             const std::string& vertexShader,
             const std::string& fragmentShader) : _center(center), _hasTexture(true),
             _renderMode(RenderMode::Texture), _hasDualTexture(false),
             _shader(vertexShader.c_str(),fragmentShader.c_str()){

            initVerticesWithTexture();
            loadTexture(pathTexture, _textureDiffuse);
            setupBuffer();
        }

        Cube(glm::vec3 center, const std::string& pathDiffuseTexture, const std::string& pathSpecularTexture,
             const std::string& shaderVertex,
             const std::string& shaderFragment) : _center(center), _hasTexture(true),
             _renderMode(RenderMode::Texture), _hasDualTexture(true),
             _shader(shaderVertex.c_str(),shaderFragment.c_str()){
                
                initVerticesWithTexture();
                loadTexture(pathDiffuseTexture, _textureDiffuse);
                loadTexture(pathSpecularTexture, _textureSpecular);
                setupBuffer();
                }

        void updateModel(){
            _model = glm::mat4(1.0f);
            _model = glm::translate(_model, _center);
            _model = glm::rotate(_model, glm::radians(_rotation), _rotationAxis);
            _model = glm::scale(_model,_scale);
        }
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
        void loadTexture(const std::string& path, unsigned int& textureID){
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

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
#endif