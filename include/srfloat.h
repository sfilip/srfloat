#ifndef SRFLOAT_H
#define SRFLOAT_H

#include "sr.h"
#include "common.h"
#include <iostream>

#define SRFLOAT_T(v) int EXP_W##v, int MAN_W##v, int RNG_W##v 
#define SRFLOAT_DEC(v) srfloat<EXP_W##v, MAN_W##v, RNG_W##v >

#define SR_MAX(a, b) (a) > (b) ? (a) : (b)

enum rounding_mode {RNE, SR};

template<SRFLOAT_T()>
class srfloat {

    public:

    template <SRFLOAT_T(2)> friend class srfloat;

    srfloat() {
        v = 0.0;
    }

    srfloat(const double in) {
        switch (rm)
        {
        case rounding_mode::RNE:
            v = cast_fp_nearest(in, MAN_W, EXP_W);
            break;
        
        default: // SR
            v = cast_fp_stochastic(in, MAN_W, EXP_W, RNG_W);
            break;
        }

    }

    srfloat(const float in) {
        switch (rm)
        {
        case rounding_mode::RNE:
            v = cast_fp_nearest((double)in, MAN_W, EXP_W);
            break;
        
        default: // SR
            v = cast_fp_stochastic((double)in, MAN_W, EXP_W, RNG_W);
            break;
        }
    }

    template<SRFLOAT_T(2)> srfloat(const SRFLOAT_DEC(2)& in) {
        switch (rm)
        {
        case rounding_mode::RNE:
            v = cast_fp_nearest(in.v, MAN_W, EXP_W);
            break;
        
        default: // SR
            v = cast_fp_stochastic(in.v, MAN_W, EXP_W, RNG_W);
            break;
        }
    }

    ~srfloat() {}

    inline srfloat& operator =(const double rhs) {
        switch (rm)
        {
        case rounding_mode::RNE:
            v = cast_fp_nearest(rhs, MAN_W, EXP_W);
            break;
        
        default: // SR
            v = cast_fp_stochastic(rhs, MAN_W, EXP_W, RNG_W);
            break;
        }
        return *this;
    }

    inline srfloat& operator =(const float rhs) {
        switch (rm)
        {
        case rounding_mode::RNE:
            v = cast_fp_nearest((double)rhs, MAN_W, EXP_W);
            break;
        
        default: // SR
            v = cast_fp_stochastic((double)rhs, MAN_W, EXP_W, RNG_W);
            break;
        }
        return *this;
    }

    bool operator ==(const double rhs) {
        return v == rhs;
    }

    bool operator ==(const float rhs) {
        return v == rhs;
    }

    template<SRFLOAT_T(2)> bool operator ==(const SRFLOAT_DEC(2)& rhs) {
        return v == rhs.v;
    }

    bool operator !=(const double rhs) {
        return v != rhs;
    }

    bool operator !=(const float rhs) {
        return v != rhs;
    }

    template<SRFLOAT_T(2)> bool operator !=(const SRFLOAT_DEC(2)& rhs) {
        return v != rhs.v;
    }

    bool operator <=(const double rhs) {
        return v <= rhs;
    }

    bool operator <=(const float rhs) {
        return v <= rhs;
    }

    template<SRFLOAT_T(2)> bool operator <=(const SRFLOAT_DEC(2)& rhs) {
        return v <= rhs.v;
    }

    bool operator >=(const double rhs) {
        return v >= rhs;
    }

    bool operator >=(const float rhs) {
        return v >= rhs;
    }

    template<SRFLOAT_T(2)> bool operator >=(const SRFLOAT_DEC(2)& rhs) {
        return v >= rhs.v;
    }

    bool operator <(const double rhs) {
        return v < rhs;
    }

    bool operator <(const float rhs) {
        return v < rhs;
    }

    template<SRFLOAT_T(2)> bool operator <(const SRFLOAT_DEC(2)& rhs) {
        return v < rhs.v;
    }

    bool operator >(const double rhs) {
        return v > rhs;
    }

    bool operator >(const float rhs) {
        return v > rhs;
    }

    template<SRFLOAT_T(2)> bool operator >(const SRFLOAT_DEC(2)& rhs) {
        return v > rhs.v;
    }

    srfloat operator +() const {
        return *this;
    }

    srfloat operator -() const {
        srfloat res{*this};
        res.v = -res.v;
        return res;
    }

    template<SRFLOAT_T(2)> struct arith_rt {
        enum {
            add_sub_exp_w = SR_MAX(EXP_W, EXP_W2),
            add_sub_man_w = SR_MAX(MAN_W, MAN_W2),
            add_sub_rng_w = SR_MAX(RNG_W, RNG_W2),
            mul_exp_w = SR_MAX(EXP_W, EXP_W2),
            mul_man_w = SR_MAX(MAN_W, MAN_W2),
            mul_rng_w = SR_MAX(RNG_W, RNG_W2)
        };
        typedef srfloat<add_sub_exp_w, add_sub_man_w, add_sub_rng_w> add_sub_t;
        typedef srfloat<mul_exp_w, mul_man_w, mul_rng_w> mul_t;

    };

    template<SRFLOAT_T(2)> srfloat& operator +=(const SRFLOAT_DEC(2) &rhs) {
        double nv = this->v + rhs.v;
        switch (rm)
        {
        case rounding_mode::RNE:
            this->v = cast_fp_nearest(nv, MAN_W, EXP_W);
            break;
        
        default: // SR
            this->v = cast_fp_stochastic(nv, MAN_W, EXP_W, RNG_W);
            break;
        }
        return *this;
    }

    template<SRFLOAT_T(2)> srfloat& operator -=(const SRFLOAT_DEC(2) &rhs) {
        double nv = this->v - rhs.v;
        switch (rm)
        {
        case rounding_mode::RNE:
            this->v = cast_fp_nearest(nv, MAN_W, EXP_W);
            break;
        
        default: // SR
            this->v = cast_fp_stochastic(nv, MAN_W, EXP_W, RNG_W);
            break;
        }
        return *this;
    }

    template<SRFLOAT_T(2)> srfloat& operator *=(const SRFLOAT_DEC(2) &rhs) {
        double nv = this->v * rhs.v;
        switch (rm)
        {
        case rounding_mode::RNE:
            this->v = cast_fp_nearest(nv, MAN_W, EXP_W);
            break;
        
        default: // SR
            this->v = cast_fp_stochastic(nv, MAN_W, EXP_W, RNG_W);
            break;
        }
        return *this;
    }

    template<SRFLOAT_T(2)> typename arith_rt<EXP_W2, MAN_W2, RNG_W2>::add_sub_t operator +(const SRFLOAT_DEC(2) &rhs) const {
        typename arith_rt<EXP_W2, MAN_W2, RNG_W2>::add_sub_t r;
        r = *this;
        r += rhs;
        return r;
    }

    template<SRFLOAT_T(2)> typename arith_rt<EXP_W2, MAN_W2, RNG_W2>::add_sub_t operator -(const SRFLOAT_DEC(2) &rhs) const {
        typename arith_rt<EXP_W2, MAN_W2, RNG_W2>::add_sub_t r;
        r = *this;
        r -= rhs;
        return r;
    }

    template<SRFLOAT_T(2)> typename arith_rt<EXP_W2, MAN_W2, RNG_W2>::add_sub_t operator *(const SRFLOAT_DEC(2) &rhs) const {
        typename arith_rt<EXP_W2, MAN_W2, RNG_W2>::mul_t r;
        r = *this;
        r *= rhs;
        return r;
    }

    std::bitset<64> bin() {
        return std::bitset<64>(DOUBLE_TO_BITS(&v));
    }


    static void srfloat_set_default_rounding_mode(rounding_mode nrm) {
        rm = nrm;
    }

    double v;
    // default rounding mode is SR
    static rounding_mode rm;

};

template <SRFLOAT_T()>
rounding_mode SRFLOAT_DEC()::rm = rounding_mode::SR;

template <SRFLOAT_T()>
std::ostream& operator <<(std::ostream &stream, const SRFLOAT_DEC() &rhs) {
    return stream << rhs.v;
}

#endif