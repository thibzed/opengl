#ifndef PLANET_HPP
#define PLANET_HPP

#include "sphere.hpp"
#include <vector>

class Planet{

    public:
        Planet (const std::vector<float>& r0, const std::vector<float>& v0, float radius, float dt,
                const std::vector<float>& color,
                const char* vertexShader = "../shaders/sphere/sphere.vs",
                const char * fragmentShader = "../shaders/sphere/sphere.fs") : 
        _r(r0), _v(v0), _radius(radius), _dt(dt), 
        _sphere(radius, color, glm::make_vec3(r0.data()), vertexShader, fragmentShader){
            setup_init_condition();
        }

        std::vector<float> get_pos(){
            return _r;
        }

        Sphere& get_sphere(){
            return _sphere;
        }
        void compute_step(){
            //Verlet integration
            float rx = 2 * _r[0] - _r_prev[0] + _dt * _dt * _a[0];
            float ry = 2 * _r[1] - _r_prev[1] + _dt * _dt * _a[1];
            float rz = 2 * _r[2] - _r_prev[2] + _dt * _dt * _a[2];   
            _r_new = {rx, ry, rz};
            float norm = sqrt(rx * rx + ry * ry + rz * rz);
            float inv = 1.0f / (norm * norm * norm);
            float ax = - (_G * _M * inv) * rx;
            float ay = - (_G * _M * inv) * ry;
            float az = - (_G * _M * inv) * rz;
            _a = {ax, ay, az};
            _r_prev = _r;
            _r = _r_new;
        }

    private: 
        Sphere _sphere;
        float _G = 6.674e-11f;
        float _M = 1.989e30f; // Sun mass
        float _radius;
        float _dt = 0.0f;

        std::vector<float> _r_prev;
        std::vector<float> _r_new;
        std::vector<float> _a_new;

        std::vector<float> _rmoins1;
        std::vector<float> _r;
        std::vector<float> _v;
        std::vector<float> _a;

        void init_sphere(){

        }

        void setup_init_condition(){
            float norm = sqrt(_r[0] * _r[0] + _r[1] * _r[1] + _r[2] * _r[2]);
            float ax = (-_G * _M / (norm * norm * norm)) * _r[0];
            float ay = (-_G * _M / (norm * norm * norm)) * _r[1];
            float az = (-_G * _M / (norm * norm * norm)) * _r[2];
            _a = {ax, ay, az};

            float r_moins1x = _r[0] - _v[0] * _dt + 0.5 * (_dt * _dt) * _a[0];
            float r_moins1y = _r[1] - _v[1] * _dt + 0.5 * (_dt * _dt) * _a[1];
            float r_moins1z = _r[2] - _v[2] * _dt + 0.5 * (_dt * _dt) * _a[2];
            _r_prev = {r_moins1x, r_moins1y, r_moins1z};
        }
};













#endif