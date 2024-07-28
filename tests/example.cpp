#include "srfloat.h"
#include "sr.h"
#include <iostream>
#include <vector>

// configuration for the low precision format (M, E, R)
const int man_bits = 2;
const int exp_bits = 5;
const int r_bits = 10;
// typedefs for SR-enabled floating-point formats
// this is just for convenience to more easily declare
// and initialize values afterwards
typedef srfloat<exp_bits, man_bits, r_bits> fp8e5m2_r10;
typedef srfloat<5, 10, r_bits> float16_r10;

// PRNG generator code for uniform and normal distributions
// these generate double precision values 
std::random_device rd_d;
std::mt19937 gen_d(rd_d());
std::uniform_real_distribution<double> dis_uniform(0.0, 1.0);
std::normal_distribution<double> dis_normal(0.0, 1.0);

int main()
{
    double mx = 3.14;
    double my = 2.0;
    std::cout << "double test, mx * my = " << mx * my << std::endl;


    fp8e5m2_r10 x{0.2};
    fp8e5m2_r10 y{1.75};
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;

    for(int i{0}; i < 10; ++i) {
        std::cout << "Iteration " << i << std::endl;
        std::cout << "SR(x + y) = " << x + y << std::endl;
        std::cout << "x + y     = " << x.v + y.v << std::endl;
        std::cout << "bin(SR(x + y)) = " << (x + y).bin() << std::endl;
    }

    // simple summation with imperfect SR
    float16_r10 sum_sr{0.0};
    // 'exact' (i.e., double) summation
    double sum_exact{0.0};

    // declare the vector of random values to use 
    int N = 100;
    std::vector<fp8e5m2_r10> vec(N);
    
    // initialize the vector elements
    for (int i{0}; i < N; ++i) {
        vec[i] = dis_normal(gen_d);
    }

    // do the two summations: with imperfect SR and RN-double one
    // REMARK: if you use compound +=, the result will be stored in 
    // the precision of the target variable with imperfect SR
    // (e.g. here, fp8 operands are accumulated in a fp16 result
    // with SR being done in fp16)
    for (int i{0}; i < N; ++i) {
        sum_sr += vec[i];
        sum_exact += vec[i].v;
    }

    std::cout << "SR(sum_vec) = " << sum_sr << std::endl;
    std::cout << "sum_exact   = " << sum_exact << std::endl;

    return 0;
}