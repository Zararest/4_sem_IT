#include "Integral.h"

double func(double x){

    return x + (x * x) / 2 + (x * x * x) / 3 + 1 / x + 1 / (x * x);
}

double definite_integral(double from, double to){
    
    double res = 0;

    for (double x = from; x < to; x += DELTA_X){

        res += func(x) * DELTA_X;
    }
    
    return res; 
}