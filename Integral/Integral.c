#include "Integral.h"

double func(double x){

    return pow(sin(x), 2) + pow(cos(x) * cos(x), 3.7);
}

double definite_integral(double from, double to){
    
    double res = 0;

    for (double x = from; x < to; x += DELTA_X){

        res += func(x) * DELTA_X;
    }
    
    return res; 
}