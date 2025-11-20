#ifndef FORCE_HPP
#define FORCE_HPP

#define _USE_MATH_DEFINES //MinGW does not activate by default

#include <vector>
#include <cmath>
#include <iostream>

class Force {

    public:
    Force(double f_x, double f_y, double f_z);
    ~Force();

    std::vector<double> get_component();

    private:
    std::vector<double> components_;
    double amplitude;
    void compute_amplitude(std::vector<double>);
};

#endif