#ifndef FIREWORK_HPP
#define FIREWORK_HPP

#include <chrono>
#include <random>
#include <memory>

#include "sphere.hpp"
#include "particle_generator.hpp"

using namespace std::chrono;

GLfloat quad[] = {1.0f  , 1.0f,
                  1.0f  , -1.0f,
                  -1.0f , -1.0f,
                  -1.0f , 1.0f};

GLuint quad_indices[] = {0 , 1 , 2,
                         0, 1 , 3};

struct particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float Life;

    particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f){}
};

class firework {
    public:
        firework(float R, const glm::vec3& center) :
        _duration_explosion(2.0), _center(center), _animate_explosion(true),
        _animate_trainee(true), _expire(false)
        {
            start();
        };
        ~firework(){};

        void start(){
            _start_time = steady_clock::now();
            _animate_trainee = true;

            std::mt19937 generator(std::random_device{}());
            std::uniform_real_distribution<float> color_dist(0.0f, 1.0f);
            std::uniform_real_distribution<float> radius_dist(0.5f, 1.0f);

            _color = {color_dist(generator), color_dist(generator), color_dist(generator)};
            _radius = radius_dist(generator);
            _sphere = std::make_unique<Sphere>(_radius, _color, _center);
        }
        float get_scale_factor(){
            auto now = steady_clock::now();
            duration<float> elapsed = now - _start_time;
            float t = elapsed.count();

            if(t >= _duration_explosion){
                _animate_explosion = false;
                _expire = true;
                return 0.0f;
            }

            float normalized = t / _duration_explosion ;
            float scale_factor = sin(normalized * M_PI / 2.0f) * _radius;
            //float scale_factor;
            //if(normalized < 0.5f){
            //    scale_factor = 2.0f * normalized * normalized;
            //}
            //else {
            //    scale_factor = 1.0f - (std::pow(-2.0f * normalized + 2.0f, 2.0f) / 2.0f) * _radius;
            //}
            return scale_factor;
        }
        void animate_trainee(){
            
        }

        void animate(glm::mat4 view, glm::mat4 projection){
            if (_animate_explosion){
                glm::vec3 scale_vec = {get_scale_factor(),get_scale_factor(),get_scale_factor()};
                _sphere->set_scale(scale_vec);
                _sphere->render_points(view, projection);
            }
        }
        bool isExpired() const {
            return _expire;
        }

    private:
        
        bool _animate_trainee;
        bool _animate_explosion;
        bool _expire;
        steady_clock::time_point _start_time;
        float _radius;
        float _duration_explosion;
        glm::vec3 _center;
        std::vector<float> _color;
        std::unique_ptr<Sphere> _sphere;
};

#endif