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

class OrbitalSystem;

class CelestialObject{

    public:
        CelestialObject (const std::vector<float>& r0, const std::vector<float>& v0, float radius, float mass, const std::vector<float>& color, 
                         OrbitalSystem* orbitalSystem = nullptr,
                         bool compute_orbit = false, float T_revolution = 24 * 3600.0f,
                         const char* vertexShader = "../shaders/sphere/sphere.vs",
                         const char * fragmentShader = "../shaders/sphere/sphere.fs") : 
        _r(r0), _r_prev(r0), _v(v0), _a({0.0f, 0.0f, 0.0f}), _total_acceleration({0.0f, 0.0f, 0.0f}),
        _m(mass), _radius(radius), _orbitalSystem(orbitalSystem), _orbitalCenter(nullptr),
        _orbitFlag(compute_orbit), _T_revolution(T_revolution),
        _sphere(radius, color,glm::make_vec3(r0.data()), vertexShader, fragmentShader),
        _orbitShader("../shaders/orbit/orbit.vs", "../shaders/orbit/orbit.fs"){}
        ~CelestialObject(){}
        void integrate(){
            //Verlet integration
            float rx = 2 * _r[0] - _r_prev[0] + dt * dt * _total_acceleration[0];
            float ry = 2 * _r[1] - _r_prev[1] + dt * dt * _total_acceleration[1];
            float rz = 2 * _r[2] - _r_prev[2] + dt * dt * _total_acceleration[2];
            _r_prev = _r;
            _r = {rx, ry, rz};
            _a = _total_acceleration;
        }
        std::vector<float> compute_acceleration_from(CelestialObject* orbiter){
            std::vector<float> orbiter_pos = orbiter->get_pos();
            float dx = orbiter_pos[0] - _r[0];
            float dy = orbiter_pos[1] - _r[1];
            float dz = orbiter_pos[2] - _r[2];

            float norm = sqrt(dx * dx + dy * dy + dz * dz);
            double inv = 1.0f / (norm * norm * norm);
            float acceleration_magnitude = G * orbiter->get_mass() * inv;

            return {acceleration_magnitude * dx, acceleration_magnitude * dy, acceleration_magnitude * dz}; 
        }
        void add_acceleration(const std::vector<float>& acceleration){
            _total_acceleration[0] += acceleration[0];
            _total_acceleration[1] += acceleration[1];
            _total_acceleration[2] += acceleration[2];
        }
        void reset_acceleration(){
            _total_acceleration = {0.0f, 0.0f, 0.0f};
        }
        void render(glm::mat4 view, glm::mat4 projection){
            glm::vec3 scaled_pos = {SCALE * _r[0] / AU, SCALE * _r[1] / AU, SCALE * _r[2] / AU};
            if (_orbitalCenter != nullptr && _orbitalCenter->get_orbitalCenter() != nullptr){ //Condition to verify if CelestialObject is a satellite.
                std::vector<float> center_pos = _orbitalCenter->get_pos();
                glm::vec3 center_scaled = {SCALE * center_pos[0] / AU, SCALE * center_pos[1] / AU, SCALE * center_pos[2] / AU};
                glm::vec3 offset = scaled_pos - center_scaled;
                
                scaled_pos = center_scaled + offset * _display_scale;
            }
            _sphere.set_position(scaled_pos);
            _sphere.render(view, projection);
        }
        void setup_verlet(){
            _r_prev[0] = _r[0] - _v[0] * dt + 0.5f * dt * dt * _total_acceleration[0];
            _r_prev[1] = _r[1] - _v[1] * dt + 0.5f * dt * dt * _total_acceleration[1];
            _r_prev[2] = _r[2] - _v[2] * dt + 0.5f * dt * dt * _total_acceleration[2];
        }
        std::vector<float> get_pos(){
            return _r;
        }
        void set_pos(const std::vector<float>& pos){
            _r = pos;
        }
        float get_mass(){
            return _m;
        }
        Sphere& get_sphere(){
            return _sphere;
        }
        CelestialObject* get_orbitalCenter(){
            return _orbitalCenter;
        }
        std::vector<CelestialObject*> get_orbitersBody(){
            return _orbitersBody;
        }
        void set_orbitalSystem(OrbitalSystem* orbitalSystem){
            _orbitalSystem = orbitalSystem;
        }
        void set_orbitalCenter(CelestialObject* orbitalCenter){
            _orbitalCenter = orbitalCenter;
        }
        OrbitalSystem* get_OrbitalSystem() const {
            return _orbitalSystem;
        }
        void set_display_scale(float scale){
            _display_scale = scale;
        }

    private:
        Sphere _sphere;
        float _radius;
        //float _omega = 0.0f;
        //float _angle = 0.0f;
        float _T_revolution; //Revolution on itself
        float _m; //Mass of the object

        bool _orbitFlag;
        float _display_scale = SCALE;

        std::vector<float> _r_prev;
        std::vector<float> _r_new;
        std::vector<float> _a_new;

        std::vector<float> _r;
        std::vector<float> _v;
        std::vector<float> _a;
        std::vector<float> _total_acceleration;

        unsigned int _orbitVAO;
        unsigned int _orbitVBO;
        Shader _orbitShader;
        std::vector<float> _orbit;

        std::vector<CelestialObject*> _orbitersBody; //Object orbiting around CelestialObject
        CelestialObject* _orbitalCenter; //Around is the object the CelestialObject orbit around. Sun for Earth, Earth for moon. 
        OrbitalSystem*   _orbitalSystem; 
};

class OrbitalSystem {
    public:
        OrbitalSystem(){}

        ~OrbitalSystem(){}

        void define_center(CelestialObject* center){
            _center = center;
        }
        void add_orbiters(CelestialObject* orbiter){
            _orbiters.push_back(orbiter);
            orbiter->set_orbitalCenter(_center);
        }
        void add_subsystem(std::shared_ptr<OrbitalSystem> subsystem) {
            _subsystems.push_back(subsystem);
            _center->get_orbitersBody().push_back(subsystem->_center);
        }
        void initialize(){
            if (_center && _center->get_orbitalCenter()){ //if _center = True and _center->get_orbitalCenter() != nullptr it means its not the sun -> Initialize
                _center->reset_acceleration();
                std::vector<float> acceleration_center_centerOrbit = _center->compute_acceleration_from(_center->get_orbitalCenter());
                _center->add_acceleration(acceleration_center_centerOrbit);
                _center->setup_verlet();
            }
            for (auto& orbiter: _orbiters){
                orbiter->reset_acceleration();
                std::vector<float> acceleration_center_orbiter = orbiter->compute_acceleration_from(_center);
                orbiter->add_acceleration(acceleration_center_orbiter);
                orbiter->setup_verlet();
            }
            for(auto& subsystem: _subsystems){
                subsystem->initialize();
            }
        }
        void render(glm::mat4 view, glm::mat4 projection){
            if (_center){_center->render(view, projection);}
            
            for (auto& orbiter: _orbiters){
                orbiter->render(view, projection);
            }
            for(auto& subsystem: _subsystems){
                subsystem->render(view, projection);
            }
        }
        void compute_all_accelerations(){
            //Interations are : Sun -> Planet (both orbiters and center of subsystem)(Reverse interaction not interesting as we don't want the sun to move)
            //                  Planet (orbiters) -> Planet (orbiters)
            //                  Planet (orbiters) -> Planet (center of subsystem)
            //                  Planet (center of subsystem) -> Planet (center of subsystem)
            //                  Sun -> Satellite 
            //                  Planet (center of subsystem) -> Satellite

            //First all accelerations is reiniatialized.
            if (_center && _center_is_fixed){_center->reset_acceleration();}
            for (auto& orbiter: _orbiters){orbiter->reset_acceleration();}
            for (auto& subsystem: _subsystems){
                subsystem->get_center()->reset_acceleration();
                for (auto& satellite: subsystem->get_orbiters())
                    {satellite->reset_acceleration();}
            }

            //Interation from the sun
            //First on planet that are not into a subsystem
            for (auto& orbiter : _orbiters){
                std::vector<float> a_sun_orbiter = orbiter->compute_acceleration_from(_center);
                orbiter->add_acceleration(a_sun_orbiter);
            }
            //Then planet into a subsystem
            for (auto& subsystem: _subsystems){
                std::vector<float> a_sun_center_subsystem = subsystem->get_center()->compute_acceleration_from(_center);
                subsystem->get_center()->add_acceleration(a_sun_center_subsystem);
            }

            //Interactions between planets (orbiters & orbiters) 
            for (size_t i = 0; i < _orbiters.size(); i++){
                for(size_t j = i + 1; j < _orbiters.size(); j++){ //Start at j+1 not to compute twice the same forces since we use third law of Newton. 
                    std::vector<float> acceleration_ji = _orbiters[i]->compute_acceleration_from(_orbiters[j]); //Let say J = Mars, I = earth force_ji = force applied by Mars on Earth.
                    _orbiters[i]->add_acceleration(acceleration_ji);
                    std::vector<float> acceleration_ij = _orbiters[j]->compute_acceleration_from(_orbiters[i]);
                    _orbiters[j]->add_acceleration(acceleration_ij);
                }
            }

            //Interactions between planets (orbiters & subsystem center)
            for (auto& orbiter: _orbiters){
                for (auto& subsystem:_subsystems){
                    CelestialObject* planet = subsystem->get_center();

                    std::vector<float> acceleration_orbiter_planet = planet->compute_acceleration_from(orbiter);
                    planet->add_acceleration(acceleration_orbiter_planet);
                    std::vector<float> acceleration_planet_orbiter = orbiter->compute_acceleration_from(planet);
                    orbiter->add_acceleration(acceleration_planet_orbiter);
                }
            }

            //Interactions between planets (subsystem center & subsystem center)
            for (size_t i = 0; i < _subsystems.size(); i++){
                CelestialObject* planet_i = _subsystems[i]->get_center();
                for (size_t j = i + 1; j < _subsystems.size(); j++){
                    CelestialObject* planet_j = _subsystems[j]->get_center();

                    std::vector<float> acceleration_ji = planet_i->compute_acceleration_from(planet_j); 
                    planet_i->add_acceleration(acceleration_ji);
                    std::vector<float> acceleration_ij = planet_j->compute_acceleration_from(planet_i);
                    planet_j->add_acceleration(acceleration_ij); 
                }
            }

            for (auto& subsystem: _subsystems){
                subsystem->compute_satellite_acceleration();
            }
        }

        void compute_satellite_acceleration(){
            for (auto& satellite : _orbiters){satellite->reset_acceleration();}
            for (auto& satellite : _orbiters){
                //Interaction between center and satellites
                std::vector <float> a_center_satellite = satellite->compute_acceleration_from(_center);
                satellite->add_acceleration(a_center_satellite);
                std::vector<float> a_sun_satellite = satellite->compute_acceleration_from(_center->get_OrbitalSystem()->get_center());
                satellite->add_acceleration(a_sun_satellite);
                if (!_center_is_fixed){
                    std::vector <float> a_satellite_center = _center->compute_acceleration_from(satellite);
                    _center->add_acceleration(a_satellite_center);
                }
            }
            for (size_t i = 0; i < _orbiters.size(); i++){
                //Interaction between satellites
                for (size_t j = i + 1; j < _orbiters.size(); j++){
                    std::vector<float> a_ji = _orbiters[i]->compute_acceleration_from(_orbiters[j]);
                    _orbiters[i]->add_acceleration(a_ji);
                    std::vector<float> a_ij = _orbiters[j]->compute_acceleration_from(_orbiters[i]);
                    _orbiters[j]->add_acceleration(a_ij);
                }
            }   
        }

        void integrate(){
            if(_center && !_center_is_fixed){
                _center->integrate(); //Sun = center + fixed in solar system / Earth = center in Earth Moon system but not in Solar system so not fixed. 
            }
            for (auto& orbiter : _orbiters){
                orbiter->integrate();
            }
            for (auto& subsystem : _subsystems){
                //std::vector<float> r_center = subsystem->get_center()->get_pos();
                //    for (auto& satellite: subsystem->get_orbiters()){
                //    std::vector<float> r_sat = satellite->get_pos();
                //    std::cout << r_sat[0] << " , " << r_sat[1] << " , " << r_sat[2] << std::endl;
                //    std::vector<float> r_sat_center_frame;
                //    r_sat_center_frame.push_back(r_center[0] + r_sat[0]);
                //    r_sat_center_frame.push_back(r_center[1] + r_sat[1]);
                //    r_sat_center_frame.push_back(r_center[2] + r_sat[2]);
                //    satellite->set_pos(r_sat_center_frame);
                //}
                subsystem->integrate();
                

            }
        }
        void step(){
            compute_all_accelerations();
            integrate();
        }
        void fix_center(bool fixed){
            _center_is_fixed = fixed;
        }
        CelestialObject* get_center(){
            return _center;
        }
        std::vector<CelestialObject*> get_orbiters(){
            return _orbiters;
        }
        std::vector<std::shared_ptr<OrbitalSystem>> get_subsystems(){
            return _subsystems;
        }

    private :
        CelestialObject* _center;
        std::vector<CelestialObject*> _orbiters;
        std::vector<std::shared_ptr<OrbitalSystem>> _subsystems;
        bool _center_is_fixed = false;
};


#endif