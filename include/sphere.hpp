#ifndef SPHERE_HPP
#define SHPERE_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include<iostream>
#include <vector>

class sphere{

    public : 
        sphere(float radius,int nb_points, const std::vector<float>& color) : _R(radius) , _points(nullptr), _color(color){
            
            if(nb_points < 64){
                throw std::invalid_argument("Not enought points to correctly render a sphere");
            }
            std::cout << "INSIDE" << std::endl;
            int nb_lat = floor(sqrt(nb_points));
            int nb_long = nb_lat;
            _points = new float[nb_lat * nb_long * 6]; 

            for (int i = 0 ; i < nb_lat; i++){
                float lat = M_PI * (-0.5f + (float)i/(nb_lat - 1));
                std::cout << i << std::endl;
                for(int j = 0 ; j < nb_long ; j++){
                    float lon = 2.0f * M_PI * (float)j / nb_long;
                    
                    float x = _R * cos(lat) * cos(lon);
                    float y = _R * sin(lat);
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
        };

        ~sphere(){
            delete[] _points;
        };

        float* get_points(){
            return _points;
        };

        float get_radius(){
            return _R;
        };

    private:
        float* _points;
        float _R;
        std::vector <float> _color;
};

#endif