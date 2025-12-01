#ifndef PLANET_HPP
#define PLANET_HPP

#include "sphere.hpp"
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>

const float SCALE = 1.5f;
const float AU = 1.496e11f; //Astronomical Unit (distance from sun to Earth).
const float dt = 6 * 3600.0f;

class Planet{

    public:
        Planet (const std::vector<float>& r0, const std::vector<float>& v0, float radius,
                const std::vector<float>& color, bool compute_orbit, float T_revolution = 24 * 3600.0f,
                const char* vertexShader = "../shaders/sphere/sphere.vs",
                const char * fragmentShader = "../shaders/sphere/sphere.fs") : 
        _r(r0), _v(v0), _radius(radius), _orbitFlag(compute_orbit), _T_revolution(T_revolution),
        _sphere(radius, color,glm::make_vec3(r0.data()), vertexShader, fragmentShader),
        _orbitShader("../shaders/orbit/orbit.vs", "../shaders/orbit/orbit.fs") {
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
            float rx = 2 * _r[0] - _r_prev[0] + dt * dt * _a[0];
            float ry = 2 * _r[1] - _r_prev[1] + dt * dt * _a[1];
            float rz = 2 * _r[2] - _r_prev[2] + dt * dt * _a[2];   
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
        void render_orbit(glm::mat4 view, glm::mat4 projection){
            _orbitShader.use();
            _orbitShader.setMat4("model", glm::mat4(1.0f));
            _orbitShader.setMat4("view", view);
            _orbitShader.setMat4("projection", projection);
            glBindVertexArray(_orbitVAO);
            glDrawArrays(GL_POINTS,0,_orbit.size() / 3);
        }
        void render(glm::mat4 view, glm::mat4 projection, glm::vec3 Pos){
            _sphere.set_position(Pos);
            //_sphere.set_rotation(_angle, glm::vec3(0.0f, 1.0f, 0.0f));
            //_angle += _omega * dt;
            //if (_angle >= 360.0f) _angle = 0.0f;
            _sphere.render(view, projection);
            if (_orbitFlag){
                render_orbit(view, projection);
            }
        }

    private: 
        Sphere _sphere;
        float _G = 6.674e-11f;
        float _M = 1.989e30f; // Sun mass
        float _radius = 0.0f;
        float _T = 0.0f; //Revolution around the sun
        float _omega = 0.0f;
        float _angle = 0.0f;
        float _T_revolution = 0.0f; //Revolution on itself

        bool _orbitFlag = false;

        std::vector<float> _r_prev;
        std::vector<float> _r_new;
        std::vector<float> _a_new;

        std::vector<float> _rmoins1;
        std::vector<float> _r;
        std::vector<float> _v;
        std::vector<float> _a;

        unsigned int _orbitVAO;
        unsigned int _orbitVBO;
        Shader _orbitShader;
        std::vector<float> _orbit;

        void setup_init_condition(){
            float norm = sqrt(_r[0] * _r[0] + _r[1] * _r[1] + _r[2] * _r[2]);
            float ax = (-_G * _M / (norm * norm * norm)) * _r[0];
            float ay = (-_G * _M / (norm * norm * norm)) * _r[1];
            float az = (-_G * _M / (norm * norm * norm)) * _r[2];
            _a = {ax, ay, az};

            float r_moins1x = _r[0] - _v[0] * dt + 0.5 * (dt * dt) * _a[0];
            float r_moins1y = _r[1] - _v[1] * dt + 0.5 * (dt * dt) * _a[1];
            float r_moins1z = _r[2] - _v[2] * dt + 0.5 * (dt * dt) * _a[2];
            _r_prev = {r_moins1x, r_moins1y, r_moins1z};

            _T = sqrt((4 * M_PI * M_PI * _r[0] * _r[0] * _r[0]) / (_G * _M)); //Third Kepler law.  
            _omega = 360.0 / _T_revolution; //Angular velocity
            if(_orbitFlag){
                //Save initial state
                auto r_init =_r;
                auto r_prev_init = _r_prev;
                auto a_init = _a;

                int nb_points = floor(_T / dt) + 1;
                int percentage = 50;
                _orbit.push_back(SCALE * _r[0] / AU);
                _orbit.push_back(SCALE * _r[1] / AU);
                _orbit.push_back(SCALE * _r[2] / AU);
                for (size_t i = 1 ; i < nb_points ; i++){
                    compute_step();
                    if ((i % percentage) == 0){
                        _orbit.push_back(SCALE * _r[0] / AU);
                        _orbit.push_back(SCALE * _r[1] / AU);
                        _orbit.push_back(SCALE * _r[2] / AU);
                    }
                }
                //Reestablish initial state
                _r = r_init;
                _r_prev = r_prev_init;
                _a = a_init;

                glGenVertexArrays(1, &_orbitVAO);
                glGenBuffers(1, &_orbitVBO);

                glBindVertexArray(_orbitVAO);
                glBindBuffer(GL_ARRAY_BUFFER, _orbitVBO);

                glBufferData(GL_ARRAY_BUFFER, _orbit.size() * sizeof(float), _orbit.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);
            }
        }
};













#endif