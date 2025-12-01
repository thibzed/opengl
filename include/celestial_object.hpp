#ifndef ORBITAL_SYSTEM_HPP
#define ORBITAL_SYSTEM_HPP

#include "sphere.hpp"
#include <vector>
#include <memory>
#define _USE_MATH_DEFINES
#include <cmath>

const float G = 6.674e-11f;
const float AU = 1.496e11f;
const float SCALE = 1.5f;
const float dt = 6*3600.0f;

class CelestialObject{

    public:
        CelestialObject (const std::vector<float>& r0, const std::vector<float>& v0, float radius, float mass,
                         const std::vector<float>& color, bool compute_orbit = false, float T_revolution = 24 * 3600.0f,
                         const char* vertexShader = "../shaders/sphere/sphere.vs",
                         const char * fragmentShader = "../shaders/sphere/sphere.fs") : 
        _r(r0), _r_prev(r0), _v(v0), _a({0.0f, 0.0f, 0.0f}), _total_force({0.0f, 0.0f, 0.0f}),
        _m(mass), _radius(radius), _orbitFlag(compute_orbit), _T_revolution(T_revolution),
        _sphere(radius, color,glm::make_vec3(r0.data()), vertexShader, fragmentShader),
        _orbitShader("../shaders/orbit/orbit.vs", "../shaders/orbit/orbit.fs") {}
        void integrate(){
            //Verlet integration
            float rx = 2 * _r[0] - _r_prev[0] + dt * dt * _total_force[0];
            float ry = 2 * _r[1] - _r_prev[1] + dt * dt * _total_force[1];
            float rz = 2 * _r[2] - _r_prev[2] + dt * dt * _total_force[2];
            std::cout << "rx : " << rx << "ry : " << ry  << "rz : " << rz << std::endl;
            _r = {rx, ry, rz};
            _r_prev = _r;
            _a = _total_force;
        }
        //std::vector<float> compute_acceleration(){
        //    float ax, ay, az;
        //    for(auto& orbiter : _orbiters){
        //        std::vector<float> force = orbiter.compute_force(orbiter);
        //        ax += force[0];
        //        ay += force[1];
        //        az += force[2];
        //    }
        //    return {ax, ay, az};
        //}
        std::vector<float> compute_force_from(CelestialObject* orbiter){
            std::vector<float> orbiter_pos = orbiter->get_pos();
            float dx = orbiter_pos[0] - _r[0];
            float dy = orbiter_pos[1] - _r[1];
            float dz = orbiter_pos[2] - _r[2];
            
            float norm = sqrt(dx * dx + dy * dy + dz * dz);
            float inv = 1.0f / (norm * norm * norm);
            float force_magnitude = G * orbiter->get_mass() * inv;
            return {force_magnitude * dx, force_magnitude * dy, force_magnitude * dz}; 
        }
        void add_force(const std::vector<float>& force){
            _total_force[0] += force[0];
            _total_force[1] += force[1];
            _total_force[2] += force[2];
        }
        void reset_forces(){
            _total_force = {0.0f, 0.0f, 0.0f};
        }
        void render(glm::mat4 view, glm::mat4 projection){
            glm::vec3 scaled_pos = {SCALE * _r[0] / AU, SCALE * _r[1] / AU, SCALE * _r[2] / AU};
            _sphere.set_position(scaled_pos);
            _sphere.render(view, projection);
        }
        void setup_verlet(){
            _r_prev[0] = _r[0] - _v[0] * dt + 0.5f * dt * dt * _total_force[0];
            _r_prev[1] = _r[1] - _v[1] * dt + 0.5f * dt * dt * _total_force[1];
            _r_prev[2] = _r[2] - _v[2] * dt + 0.5f * dt * dt * _total_force[2];
        }
        std::vector<float> get_pos(){
            return _r;
        }
        float get_mass(){
            return _m;
        }

    private:
        Sphere _sphere;
        float _radius;
        float _omega = 0.0f;
        float _angle = 0.0f;
        float _T_revolution; //Revolution on itself
        float _m; //Mass of the object

        bool _orbitFlag;

        std::vector<float> _r_prev;
        std::vector<float> _r_new;
        std::vector<float> _a_new;

        std::vector<float> _r;
        std::vector<float> _v;
        std::vector<float> _a;
        std::vector<float> _total_force;

        unsigned int _orbitVAO;
        unsigned int _orbitVBO;
        Shader _orbitShader;
        std::vector<float> _orbit;

        std::vector<CelestialObject> _orbiters;
};

class OrbitalSystem {
    public:
        OrbitalSystem(CelestialObject* center): _center(center) {}

        void add_orbiters(CelestialObject* orbiter){
            _orbiters.push_back(orbiter);
        }
        void add_subsystem(std::shared_ptr<OrbitalSystem> subsystem) {
            _subsystems.push_back(subsystem);
        }
        void initialize(){
            for (auto& orbiter: _orbiters){
                orbiter->reset_forces();
                orbiter->setup_verlet();
            }
        }
        void render(glm::mat4 view, glm::mat4 projection){
            if (_center && _center_is_fixed){_center->render(view, projection);}
            
            for (auto& orbiter: _orbiters){
                orbiter->render(view, projection);
            }
        }
        void compute_all_forces(){
            if (_center){_center->reset_forces();}
            for (auto& orbiter: _orbiters){orbiter->reset_forces();}
            for (auto& orbiter: _orbiters){
                std::vector<float> force_center_orbiter = orbiter->compute_force_from(_center);
                orbiter->add_force(force_center_orbiter);
            }
        }
        void integrate(){
            if(_center && !_center_is_fixed){
                _center->integrate();
            }
            for (auto& orbiter : _orbiters){
                std::cout << orbiter << "integrate" << std::endl;
                orbiter->integrate();
            }
        }
        void step(){
            compute_all_forces();
            integrate();
        }
        void fix_center(bool fixed){
            _center_is_fixed = fixed;
        }

    private :
        CelestialObject* _center;
        std::vector<CelestialObject*> _orbiters;
        std::vector<std::shared_ptr<OrbitalSystem>> _subsystems;
        bool _center_is_fixed = false;
};




#endif