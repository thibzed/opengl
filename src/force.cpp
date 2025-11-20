#include "force.hpp"

Force::Force(double f_x, double f_y, double f_z){
    components_ = std::vector<double>(3);
    components_[0] = f_x;
    components_[1] = f_y;
    components_[2] = f_z;

    Force::compute_amplitude(components_);
}

Force::~Force(){}

double Force::get_component(){
    return abs(components_[2]);
}

void Force::compute_amplitude(std::vector<double> components){
    double f_x = components[0];
    double f_y = components[1];
    double f_z = components[2];

    amplitude = sqrt(f_x * f_x + f_y * f_y + f_z * f_z);
}