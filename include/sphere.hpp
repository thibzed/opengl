#ifndef Sphere_HPP
#define Sphere_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include "shader.h"

#include <iostream>
#include <vector>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sphere{

    public : 

        Sphere(const Sphere&) = delete;
        Sphere& operator=(const Sphere&) = delete;

        Sphere(Sphere&& other) noexcept : //move constructeur
        _points(std::move(other._points)), _R(other._R), _color(other._color), 
        _center(other._center), _VBO(other._VBO), _VAO(other._VAO),
        _model(other._model), _shader(std::move(other._shader)), _EBO(other._EBO){
            other._VBO = 0;
            other._VAO = 0;
            other._EBO = 0;
        }
        Sphere& operator=(Sphere&& other) noexcept {
            if (this != &other) {
                if(_VAO != 0) glDeleteVertexArrays(1, &_VAO);
                if(_VBO != 0) glDeleteBuffers(1, &_VBO);
                if(_EBO != 0) glDeleteBuffers(1, &_EBO);
            _points = std::move(other._points);
            _R = other._R;
            _color = other._color;
            _center = other._center;
            _VBO = other._VBO;
            _VAO = other._VAO;
            _EBO = other._EBO;
            _model = other._model;
            _shader = std::move(other._shader);

            other._VBO = 0;
            other._VAO = 0;
            other._EBO = 0;
            }
        return *this;
        }

        Sphere(float radius,int nb_points,
               const std::vector<float>& color, const glm::vec3 center) :
               _R(radius) , _color(color), _center(center),
               _shader("../shaders/sphere/sphere.vs", "../shaders/sphere/sphere.fs"){
            
            if(nb_points < 64){
                throw std::invalid_argument("Not enought points to correctly render a Sphere");
            }
            int nb_lat = floor(sqrt(nb_points));
            int nb_long = nb_lat;
            _points.resize(nb_lat * nb_long * 6);

            for (int i = 0 ; i < nb_lat; i++){
                float lat = M_PI * (-0.5f + (float)i/(nb_lat - 1));
                float y = _R * sin(lat);
                for(int j = 0 ; j < nb_long ; j++){
                    float lon = 2.0f * M_PI * j / nb_long;
                    
                    float x = _R * cos(lat) * cos(lon);
                    float z = _R * cos(lat) * sin(lon);

                    int base = (i * nb_long + j) * 6;
                    _points[base] = x;
                    _points[base + 1] = y;
                    _points[base + 2] = z;
                    _points[base + 3] = _color[0];
                    _points[base + 4] = _color[1];
                    _points[base + 5] = _color[2];
                }
            }
            
            glGenVertexArrays(1,&_VAO);
            glGenBuffers(1,&_VBO);

            glBindVertexArray(_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, _VBO);
            glBufferData(GL_ARRAY_BUFFER, _points.size() * sizeof(float) , _points.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            _model = glm::translate(_model, _center);
        }

        Sphere(float radius, const std::vector<float>& color, const glm::vec3 center) : 
               _R(radius), _color(color), _center(center),
               _shader("../shaders/sphere/sphere.vs", "../shaders/sphere/sphere.fs"){
                //Part of code comes from https://www.songho.ca/opengl/gl_sphere.html#sphere
                int subdivision = 80;
                float dlat = M_PI / subdivision;
                float dlong = 2 * M_PI / subdivision;
                float inv = 1.0f / _R;
                for (unsigned int i = 0; i < subdivision ; i++){
                    float lat = M_PI *(-0.5f + (float)i / (subdivision - 1));  
                    float y = _R * sin(lat);
                    for (unsigned int j = 0 ; j < subdivision ; j++){
                        float lon = 2 * M_PI * j / (subdivision - 1);
                        float x = _R * cos(lat) * cos(lon);
                        float z = _R * cos(lat) * sin(lon);

                        float nx = x * inv;
                        float ny = y * inv;
                        float nz = z * inv;

                        _points.push_back(x);
                        _points.push_back(y);
                        _points.push_back(z);
                        _points.push_back(nx);
                        _points.push_back(ny);
                        _points.push_back(nz);
                        _points.push_back(color[0]);
                        _points.push_back(color[1]);
                        _points.push_back(color[2]);
                    }
                }

                int k1, k2;
                for (int i = 0 ; i < subdivision ; ++i){
                    k1 = i * subdivision;
                    k2 = k1 + subdivision;
                    for (int j = 0 ; j < subdivision; ++j, ++k1, ++k2){
                        if(i !=0){
                            _indices.push_back(k1);
                            _indices.push_back(k2);
                            _indices.push_back(k1 + 1);
                        }
                        if(i != (subdivision - 1)){
                            _indices.push_back(k1 + 1);
                            _indices.push_back(k2);
                            _indices.push_back(k2 + 1);
                        }
                        _lineIndices.push_back(k1);
                        _lineIndices.push_back(k2);
                        if(i != 0){
                            _lineIndices.push_back(k1);
                            _lineIndices.push_back(k1 + 1);
                        }
                    }
                }

                glGenVertexArrays(1,&_VAO);
                glGenBuffers(1,&_VBO);
                glGenBuffers(1,&_EBO);

                glBindVertexArray(_VAO);
                glBindBuffer(GL_ARRAY_BUFFER, _VBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

                glBufferData(GL_ARRAY_BUFFER, _points.size() * sizeof(float), _points.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,9 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE,9 * sizeof(float), (void*)(3*sizeof(float)));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2,3,GL_FLOAT, GL_FALSE,9 * sizeof(float), (void*)(6*sizeof(float)));
                glEnableVertexAttribArray(2);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(int), _indices.data(), GL_STATIC_DRAW);
                _model = glm::translate(_model, _center);
            }

        ~Sphere(){
            glDeleteBuffers(1,&_VBO);
            glDeleteVertexArrays(1,&_VAO);
        }

        void render(glm::mat4 view, glm::mat4 projection){
            _shader.use();
            _shader.setMat4("model", _model);
            _shader.setMat4("view",view);
            _shader.setMat4("projection", projection);
            glBindVertexArray(_VAO);
            glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

        }

        std::vector<float> get_points() const {
            return _points;
        }

        float get_radius() const {
            return _R;
        }
        
        glm::vec3 get_center() const {
            return _center;
        }

        glm::mat4 get_model() const {
            return _model;
        }

        unsigned int get_VAO() const {
            return _VAO;
        }

    private:
        std::vector<float> _points;
        float _R;
        std::vector <float> _color;
        glm::vec3 _center;
        std::vector <unsigned int> _indices;
        std::vector <int> _lineIndices;
        unsigned int _VBO;
        unsigned int _VAO;
        unsigned int _EBO;
        Shader _shader;
        glm::mat4 _model = glm::mat4 (1.0f);
};

#endif