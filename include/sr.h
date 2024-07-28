#ifndef SR_H
#define SR_H

#include <bitset>
#include <cmath>
#include <iostream>
#include <random>

void seed(int64_t seed);

double cast_fp_nearest(double in, int man_bits, int exp_bits,
                    bool subnormals = true, bool saturate = false);

double cast_fp_stochastic(double in, int man_bits, int exp_bits, int rand_bits,
                    bool subnormals = true, bool saturate = false);

double sum_vec_nearest(std::vector<double> &in, int batch_size, int man_bits, int exp_bits,
                    bool subnormals = true, bool saturate = false);

double sum_vec_stochastic(std::vector<double> &in, int batch_size, int man_bits, int exp_bits, int prng_bits,
                    bool subnormals = true, bool saturate = false);
#endif