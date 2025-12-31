#ifndef PARTICLE_GENERATOR_HPP
#define PARTICLE_GENERATOR_HPP

#include "shader.h"
#include <cstdlib>
#include <memory>
#include <algorithm>
#include <random>

struct Particle{
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float Life;

    Particle() : Position(0.0f), Velocity(1.0f), Color(0.5f), Life(1.0f) {}
    Particle(glm::vec3 Pos) : Position(Pos) {}
};

class ParticleGenerator{
    public:

        ParticleGenerator(size_t nbParticles, glm::mat4 projection,
                          glm::mat4 view, glm::vec3 init_pos) : 
        _nrParticles(nbParticles), _proj(projection), _view(view), _init_pos(init_pos) {
            init();
        };

        void RespawnParticle(Particle& particle){
            float random = ((rand() % 100) - 50) / 10.0f;
            float rColor = 0.5f + ((rand() % 100) / 100.0f);
            particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
            particle.Life = 1.0f;
            particle.Velocity = particle.Velocity * 0.1f;
        }

        unsigned int findFirstDeadParticle(){
            for (unsigned int i = _lastUsedParticle ; i < _particles.size() ; i++){
                if (_particles[i].Life <= 0.0f){
                    _lastUsedParticle = i;
                    return i;
                }
            }

            for (unsigned int i = 0 ; i < _lastUsedParticle ; i++){
                if(_particles[i].Life <= 0.0f){
                    _lastUsedParticle = i;
                    return i;
                }
            }

            _lastUsedParticle = 0;
            return 0;
        }

        void update(){
            for (unsigned int i = 0; i < _nrParticles; i++){
                int unusedParticle = findFirstDeadParticle();
                RespawnParticle(_particles[unusedParticle]);
            }
            for (auto& p : _particles){
                p.Life -= dt;
                if (p.Life > 0.0f){
                    p.Velocity += glm::vec3(0.0f, -9.81f, 0.0f) * dt * 0.5f;
                    p.Position += p.Velocity * dt;
                    //p.Color.a -= dt * 2.5f;
                }
            }
            //std::erase_if(_particles, [](const Particle& p){
            //                                return p.Life <= 0.0f;
            //});
        }
        void draw(){
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            _shader->use();
            glm::mat4 model;
            for (auto& p : _particles){
                if (p.Life > 0.0f){
                    model = glm::translate(glm::mat4(1.0f), glm::vec3(p.Position.x, p.Position.y, 0.0f));
                    _shader->setVec4("color", p.Color);
                    _shader->setMat4("projection", _proj);
                    _shader->setMat4("model", model);
                    _shader->setMat4("view", _view);
                    glBindVertexArray(_VAO);
                    glDrawArrays(GL_TRIANGLES, 0,6);
                    glBindVertexArray(0);
                }
            }
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        };
    private:

        void init(){

            std::mt19937 generator (std::random_device{}());
            std::uniform_real_distribution<float> x (-0.5f, 0.5f);
            std::uniform_real_distribution<float> y (-0.1f, 0.0f);

            float quad[] = {0.0f, 1.0f, //First triangle
                            1.0f, 0.0f, 
                            0.0f, 0.0f, 
                            0.0f, 1.0f, //Second triangle
                            1.0f, 1.0f, 
                            1.0f, 0.0f};
            unsigned int VBO;
            glGenVertexArrays(1, &_VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(_VAO);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glBindVertexArray(0);

            for (unsigned int i = 0; i < _nrParticles; i++){
                //std::cout << _init_pos.x ;
                glm::vec3 Pos = glm::vec3(_init_pos.x + x(generator), _init_pos.y + y(generator), 0.0f);
                _particles.push_back(Particle(Pos));
            }
            _shader = std::make_unique<Shader>("../shaders/particle_shader/particle_vs.glsl", 
                                               "../shaders/particle_shader/particle_fs.glsl");
        }

        std::vector<Particle> _particles;
        unsigned int _lastUsedParticle = 0;
        unsigned int nrNewParticle = 2;
        size_t _nrParticles;
        float dt = 0.001;
        std::unique_ptr<Shader> _shader;
        unsigned int _VAO;
        glm::mat4 _proj;
        glm::mat4 _view;
        glm::vec3 _init_pos;
};












#endif