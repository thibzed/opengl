#include "force.hpp"

std::vector<double> position_t(const double t, const double v0, const double g, const double alpha, const double beta, const std::vector<double> init_pos){

    double x_t = v0 * cos(beta) * t + init_pos[0];
    double y_t = v0 * cos(alpha) * t + init_pos[1];
    double z_t = - 0.5 * g * t * t + v0 * sin(alpha) * t + init_pos[2];

    std::vector<double> pos_t = {x_t , y_t , z_t};
    return pos_t;
}

std::vector<std::vector<double>> compute_traj_frictionless (const double v0,  int nb_points,const std::vector<double> init_angle, const std::vector<double> init_pos, Force Poids){

    std::vector<std::vector<double>> points;

    double alpha = init_angle[0] * M_PI / 180;
    double beta = init_angle[1] * M_PI / 180;
    double g = Poids.get_component();

    double t0 = 0.0;
    double tf = (v0 * sin(alpha) + sqrt(v0 * v0 * sin(alpha) * sin(alpha) + 2 * g * init_pos[2])) / g; 
    double delta_t = (tf - t0) / nb_points;

    for (int i = 0 ; i < nb_points ; i++){
        double t = i * delta_t;
        std::vector<double> pos_t = position_t(t, v0, g, alpha, beta, init_pos);
        points.push_back(pos_t);
    }

    std::cout << points[nb_points - 1][2];

    return points;
}

//int main(){
//    Force Poids (0 , 0 , -9.81);
//    std::vector<double> CI_pos = {0 , 0 , 0};
//    std::vector<double> CI_angle = {30 , 45}; // {Alpha , Beta}
//    double v0 = 20;
//    compute_traj_frictionless(v0, 1000, CI_angle, CI_pos, Poids);
//    return 0;
//}


