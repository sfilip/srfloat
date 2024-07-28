#include <bitset>
#include <cmath>
#include <iostream>
#include <random>
#include <cstdio>
#include "sr.h"
#include "common.h"

std::mt19937_64 gen((int64_t)time(0));
std::uniform_int_distribution<uint64_t> dis(0ul);

uint64_t extract_exponent(double *a) {
    uint64_t temp = DOUBLE_TO_BITS(a);
    temp = (temp << 1 >> 53);
    return temp - 1023;
}

void seed(int64_t seed) {
  gen.seed(seed);
  dis = std::uniform_int_distribution<uint64_t>(0ul);
}

uint64_t round_bitwise_nearest(uint64_t target, int man_bits) {
    uint64_t down = target << (11 + man_bits) >> (11 + man_bits);
    uint64_t machine_eps = 1L << (51 - man_bits);
    // tie breaking rule offset
    int offset = (down == machine_eps);
    uint64_t add_r = target + machine_eps;
    // apply the mask
    // this is the analogue of how you would do round
    // to neareset integer using the floor function:
    // round(x) = floor(x + 0.5)
    return add_r & ~ ((1l << (52 - man_bits + offset)) - 1);
}

uint64_t round_bitwise_stochastic(uint64_t target, int man_bits, int prng_bits) {
    int bits_shift_out = 52 - man_bits - prng_bits;
    uint64_t mask_format = (1L << (52 - man_bits)) - 1;
    uint64_t mask_round = (1L << bits_shift_out) - 1;
    uint64_t rand_prob = ((dis(gen)) & mask_format);
    uint64_t add_r = (target & ~mask_round) + rand_prob;
    uint64_t quantized = add_r & ~mask_format;
    return quantized;
}

uint64_t clip_exponent_w_subnormals(int exp_bits, int man_bits, uint64_t old_num,
                                  uint64_t quantized_num, bool saturate = false) {
  if (quantized_num == 0)
    return quantized_num;

  int64_t quantized_exponent_store = quantized_num << 1 >> 53;
  int64_t min_exponent_store = -((1L << (exp_bits - 1)) - 2) - man_bits + 1023;

  uint64_t old_sign = old_num >> 63 << 63;
  // underflow or round to smallest non zero subnormal value
  if (quantized_exponent_store < min_exponent_store) {
    uint64_t offset = (quantized_exponent_store == (min_exponent_store - 1));
    quantized_num += offset * (1UL << 52);
    quantized_num = old_sign | quantized_num;
    quantized_num = offset * quantized_num;
  }
  return quantized_num;
}

uint64_t clip_exponent_wo_subnormals(int exp_bits, int man_bits, uint64_t old_num,
                            uint64_t quantized_num, bool saturate = false) {
  if (quantized_num == 0)
    return quantized_num;

  int64_t quantized_exponent_store = quantized_num << 1 >> 53;
  int64_t max_exponent_store = (1L << (exp_bits - 1)) - 1 + 1023;
  int64_t min_exponent_store = -((1L << (exp_bits - 1)) - 2) + 1023;

  uint64_t old_sign = old_num >> 63 << 63;
  // saturate or overflow
  if (quantized_exponent_store > max_exponent_store) {
    if (saturate) {
      uint64_t max_man = 
          (uint64_t)-1L << 12 >> 12 >> (52 - man_bits) << (52 - man_bits);
      uint64_t max_num = ((uint64_t)max_exponent_store << 52) | max_man;
      quantized_num = old_sign | max_num;
    } else {
      quantized_num = ((((uint64_t)1L << 63) - 1) ^ (((uint64_t)1L << 52) - 1));
      quantized_num = old_sign | quantized_num;
    }
  } // underflow or round to smallest nonzero normal value
  else if (quantized_exponent_store < min_exponent_store) {
    uint64_t offset = (quantized_exponent_store == (min_exponent_store - 1)) && ((old_num << 12 >> 12) > (1ul << 51));
    quantized_num = offset * (min_exponent_store << 52);
    quantized_num = old_sign | quantized_num;
  }

  return quantized_num;
}

double cast_fp_nearest(double in, int man_bits, int exp_bits,
                    bool subnormals, bool saturate) {

  uint64_t target, quantized_bits;
  target = DOUBLE_TO_BITS(&in);
  double quantized;

  int target_exp = (target << 1 >> 53) - 1023;
  int min_exp = -((1 << (exp_bits - 1)) - 2);
  bool is_subnormal = (target_exp < min_exp);

  if (subnormals & is_subnormal) {
    int exp_diff = man_bits - (min_exp - target_exp);
    int not_uflow = exp_diff > -1 || ((exp_diff == -1) && ((target << 12) > 0));
    quantized_bits = not_uflow * round_bitwise_nearest(target, exp_diff);
    quantized_bits = clip_exponent_w_subnormals(exp_bits, man_bits, target, quantized_bits, saturate);
    quantized = BITS_TO_DOUBLE(&quantized_bits);
  // handle NaN/inf inputs
  } else if (target_exp == 1024) {
    quantized = in;
  // normal value range or overflow
  } else {
    quantized_bits = round_bitwise_nearest(target, man_bits);
    quantized_bits =
        clip_exponent_wo_subnormals(exp_bits, man_bits, target, quantized_bits, saturate);
    quantized = BITS_TO_DOUBLE(&quantized_bits);
  }

  return quantized;

}

double cast_fp_stochastic(double in, int man_bits, int exp_bits, int prng_bits,
                    bool subnormals, bool saturate) {

  uint64_t target, quantized_bits;
  target = DOUBLE_TO_BITS(&in);
  double quantized;

  int target_exp = (target << 1 >> 53) - 1023;
  int min_exp = -((1 << (exp_bits - 1)) - 2);
  bool is_subnormal = (target_exp < min_exp);

  if (subnormals & is_subnormal) {
    double shift_float, val;
    int64_t shift_bits = ((1023L + min_exp) << 52) | (target >> 63 << 63);
    shift_float = BITS_TO_DOUBLE(&shift_bits);
    val = in + shift_float;
    target = DOUBLE_TO_BITS(&val);
    quantized_bits = round_bitwise_stochastic(target, man_bits, prng_bits);
    quantized = BITS_TO_DOUBLE(&quantized_bits) - shift_float;
  } else {
    quantized_bits = round_bitwise_stochastic(target, man_bits, prng_bits);
    quantized_bits =
        clip_exponent_wo_subnormals(exp_bits, man_bits, target, quantized_bits, saturate);
    quantized = BITS_TO_DOUBLE(&quantized_bits);
  }
  return quantized;
}

double sum_vec_stochastic(std::vector<double> &in, int batch_size, int man_bits, int exp_bits, int prng_bits,
                    bool subnormals, bool saturate) {
    
    int batches = (int)((in.size() + batch_size - 1) / batch_size);
    double total_sum = 0.0;
    double batch_sum = 0.0;
    for (int bidx = 0; bidx < batches; ++bidx) {
      batch_sum = 0.0;
      for (int idx = batch_size * bidx; idx < std::min((int)in.size(), batch_size * (bidx + 1)); ++idx) {
        batch_sum = cast_fp_stochastic(batch_sum + in[idx], man_bits, exp_bits, prng_bits, subnormals, saturate);
      }
      total_sum = cast_fp_stochastic(total_sum + batch_sum, man_bits, exp_bits, prng_bits, subnormals, saturate);
    }

    return total_sum;
}

double sum_vec_nearest(std::vector<double> &in, int batch_size, int man_bits, int exp_bits,
                    bool subnormals, bool saturate) {
    
    int batches = (int)((in.size() + batch_size - 1) / batch_size);
    double total_sum = 0.0;
    double batch_sum = 0.0;
    for (int bidx = 0; bidx < batches; ++bidx) {
      batch_sum = 0.0;
      for (int idx = batch_size * bidx; idx < std::min((int)in.size(), batch_size * (bidx + 1)); ++idx) {
        batch_sum = cast_fp_nearest(batch_sum + in[idx], man_bits, exp_bits, subnormals, saturate);
      }
      total_sum = cast_fp_nearest(total_sum + batch_sum, man_bits, exp_bits, subnormals, saturate);
    }

    return total_sum;
}