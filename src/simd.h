#ifndef SIMD_H
#define SIMD_H

#if defined(__APPLE__ ) || defined(__linux)
#include "smmintrin.h"
#include "immintrin.h"
#include "emmintrin.h"
#endif

union i32_4x
{
    __m128i p;
    i32 e[4];
    
    i32_4x(i32 _p) 
    {
        p = _mm_set1_epi32(_p);
    }
    
    i32_4x(i32 _a, i32 _b, i32 _c, i32 _d)
    {
        p = _mm_setr_epi32(_a, _b, _c, _d);
    }
    
    i32_4x& operator=(const i32 v)
    {
        p = _mm_set1_epi32(v);
        
        return *this;
    }
    
    i32_4x& operator+= (i32_4x b)
    {
        this->p = _mm_add_epi32(p, b.p);
        return *this;
    }
};

inline i32_4x operator+ (i32_4x a, i32_4x b)
{
    i32_4x res(0);
    
    res.p = _mm_add_epi32(a.p, b.p);
    
    return res;
}


inline i32_4x operator- (i32_4x a, i32_4x b)
{
    i32_4x res(0);
    
    res.p = _mm_sub_epi32(a.p, b.p);
    
    return res;
}

inline i32_4x operator-= (i32_4x a, i32_4x b)
{
    return a - b;
}

inline i32_4x operator* (i32_4x a, i32_4x b)
{
    i32_4x res(0);
    
    res.p = _mm_mul_epi32(a.p, b.p);
    
    return res;
}

inline i32_4x operator*= (i32_4x a, i32_4x b)
{
    return a * b;
}

inline i32_4x operator/ (i32_4x a, i32_4x b)
{
    assert(false);
    return i32_4x(0);
}

inline i32_4x operator/= (i32_4x a, i32_4x b)
{
    assert(false);
    return i32_4x(0);
}

union r32_4x
{
    __m128 p;
    r32 e[4];
    u32 u[4];
    
    r32_4x()
    {
        p = _mm_set1_ps(0.0f);
    }
    
    explicit r32_4x(r32 _p) 
    {
        p = _mm_set1_ps(_p);
    }
    
    r32_4x(r32 _a, r32 _b, r32 _c, r32 _d)
    {
        p = _mm_setr_ps(_a, _b, _c, _d);
    }
    
    r32_4x(u32 _a, u32 _b, u32 _c, u32 _d)
    {
        p = _mm_setr_ps(*(r32*)&_a, 
                        *(r32*)&_b, 
                        *(r32*)&_c, 
                        *(r32*)&_d);
    }
    
    r32_4x(u32 _a)
    {
        p = _mm_set1_ps(*(float*)&_a);
    }
    
    r32_4x(__m128 v)
    {
        p = v;
    }
    
    inline r32_4x& operator=(const r32& v)
    {
        p = _mm_set1_ps(v);
        
        return *this;
    }
    
    inline r32_4x& operator=(const r32_4x& v)
    {
        p = v.p;
        
        return *this;
    }
    
    inline r32_4x& operator *=(r32_4x v)
    {
        p = _mm_mul_ps(p, v.p);
        
        return *this;
    }
};

inline r32_4x operator+ (r32_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    res.p = _mm_add_ps(a.p, b.p);
    
    return res;
}

inline r32_4x operator+ (r32_4x a, r32 b)
{
    r32_4x res(0.0f);
    
    res.p = _mm_add_ps(a.p, _mm_set1_ps(b));
    
    return res;
}

inline r32_4x operator+ (r32 a, r32_4x b)
{
    r32_4x res(0.0f);
    
    res.p = _mm_add_ps(_mm_set1_ps(a), b.p);
    
    return res;
}

inline r32_4x operator- (r32_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    res.p = _mm_sub_ps(a.p, b.p);
    
    return res;
}

inline r32_4x operator-(r32 a, r32_4x b)
{
    r32_4x res(a);

    res.p = _mm_sub_ps(res.p, b.p);

    return res;
}

inline r32_4x operator-(r32_4x v)
{
    return _mm_xor_ps(v.p, _mm_set1_ps(-0.0f));
}

inline r32_4x operator* (r32_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    res.p = _mm_mul_ps(a.p, b.p);
    
    return res;
}

inline r32_4x operator/ (r32_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    res.p = _mm_div_ps(a.p, b.p);
    
    return res;
}

union h64_4x
{
    struct
    {
        union
        {
            __m128d p;
            r64 e[2];
        };
    };
    
    h64_4x(r64 _p) 
    {
        p = _mm_set1_pd(_p);
    }
    
    h64_4x(__m128d _p) 
    {
        p = _p;
    }
    
    h64_4x(r64 _a, r64 _b)
    {
        p = _mm_setr_pd(_a, _b);
    }
    
    inline h64_4x& operator=(const r64& v)
    {
        p = _mm_set1_pd(v);
        
        return *this;
    }
    
    inline h64_4x& operator=(const h64_4x& v)
    {
        p = v.p;
        
        return *this;
    }
    
    inline h64_4x operator+ (h64_4x b)
    {
        h64_4x res(0.0f);
        
        res.p = _mm_add_pd(p, b.p);
        
        return res;
    }
    
    inline h64_4x& operator+=(h64_4x b)
    {
        this->p = _mm_add_pd(p, b.p);
        return *this;
    }
    
    inline h64_4x operator- (h64_4x b)
    {
        h64_4x res(0.0f);
        
        res.p = _mm_sub_pd(p, b.p);
        
        return res;
    }
    
    inline h64_4x& operator-=(h64_4x b)
    {
        this->p = _mm_sub_pd(p, b.p);
        return *this;
    }
    
    inline h64_4x operator* (h64_4x b)
    {
        h64_4x res(0.0f);
        
        res.p = _mm_mul_pd(p, b.p);
        
        return res;
    }
    
    inline h64_4x& operator*=(h64_4x b)
    {
        this->p = _mm_mul_pd(p, b.p);
        return *this;
    }
    
    inline h64_4x operator/ (h64_4x b)
    {
        h64_4x res(0.0f);
        
        res.p = _mm_div_pd(p, b.p);
        
        return res;
    }
    
    inline h64_4x& operator/=(h64_4x b)
    {
        this->p = _mm_div_pd(p, b.p);
        return *this;
    }
};

inline r32_4x u32_to_r32(r32_4x a)
{
    r32_4x result;
    
    result.p = _mm_cvtepi32_ps(_mm_castps_si128(a.p));
    
    return(result);
}

#if defined(__APPLE__) || defined(_WIN32) || defined(__linux)
// Used to store 4 doubles in one SIMD constructions
union r64_4x
{
    struct
    {
        h64_4x upper_bits;
        h64_4x lower_bits;
    };
    r64 e[4];
    u32 u[4];
    
    
explicit    r64_4x(r64 v)
    {
        upper_bits = h64_4x(v);
        lower_bits = h64_4x(v);
    }
    
explicit    r64_4x(r64 v1, r64 v2, r64 v3, r64 v4)
    {
        upper_bits = h64_4x(v1, v2);
        lower_bits = h64_4x(v3, v4);
    }

explicit r64_4x(r32_4x v) : r64_4x((r64)v.e[0], (r64)v.e[1], (r64)v.e[2], (r64)v.e[3])
    {}
    
explicit    r64_4x(__m128d upper, __m128d lower)
    {
        upper_bits = h64_4x(upper);
        lower_bits = h64_4x(lower);
    }
    
    inline r64_4x& operator=(const r64_4x& v)
    {
        upper_bits = h64_4x(v.e[0], v.e[1]);
        lower_bits = h64_4x(v.e[2], v.e[3]);
        
        return *this;
    }
    
    inline r64_4x& operator=(const r64& v)
    {
        upper_bits = h64_4x(v, v);
        lower_bits = h64_4x(v, v);
        
        return *this;
    }
    
    inline r64_4x operator+ (r64_4x b)
    {
        r64_4x res(0.0);
        
        res.upper_bits = upper_bits + b.upper_bits;
        res.lower_bits = lower_bits + b.lower_bits;
        
        return res;
    }
    
    inline r64_4x& operator+= (r64_4x b)
    {
        upper_bits += b.upper_bits;
        lower_bits += b.lower_bits;
        
        return *this;
    }
    
    inline r64_4x operator- (r64_4x b)
    {
        r64_4x res(0.0);
        
        res.upper_bits = upper_bits - b.upper_bits;
        res.lower_bits = lower_bits - b.lower_bits;
        
        return res;
    }
    
    inline r64_4x operator-= (r64_4x b)
    {
        upper_bits -= b.upper_bits;
        lower_bits -= b.lower_bits;
        return *this;
    }
    
    inline r64_4x operator* (r64_4x b)
    {
        r64_4x res(0.0);
        
        res.upper_bits = upper_bits * b.upper_bits;
        res.lower_bits = lower_bits * b.lower_bits;
        
        return res;
    }
    
    inline r64_4x& operator*= (r64_4x b)
    {
        upper_bits *= b.upper_bits;
        lower_bits *= b.lower_bits;
        return *this;
    }
    
    inline r64_4x operator/ (r64_4x b)
    {
        r64_4x res(0.0);
        
        res.upper_bits = upper_bits / b.upper_bits;
        res.lower_bits = lower_bits / b.lower_bits;
        
        return res;
    }
    
    inline r64_4x& operator/= (r64_4x b)
    {
        upper_bits /= b.upper_bits;
        lower_bits /= b.lower_bits;
        return *this;
    }
};

#define shift_right_simd(a, imm) r32_4x(_mm_castsi128_ps(_mm_srli_epi32(_mm_castps_si128(a.p), imm)))
#define shift_left_simd(a, imm) r32_4x(_mm_castsi128_ps(_mm_slli_epi32(_mm_castps_si128(a.p), imm)))

inline r32_4x operator+(r32_4x a, r64_4x b)
{
    r32_4x res(0.0f);
    
    double *_v_upper = (double*)&b.upper_bits;
    double *_v_lower = (double*)&b.lower_bits;
    
    res = a + r32_4x((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]);
    
    return res;
}

inline r32_4x operator+(r64_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    double *_v_upper = (double*)&a.upper_bits;
    double *_v_lower = (double*)&a.lower_bits;
    
    res = r32_4x((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]) + b;
    
    return res;
}

inline r32_4x operator*(r32_4x a, r64_4x b)
{
    r32_4x res(0.0f);
    
    double *_v_upper = (double*)&b.upper_bits;
    double *_v_lower = (double*)&b.lower_bits;
    
    res = a * r32_4x((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]);
    
    return res;
}

inline r32_4x operator*(r64_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    double *_v_upper = (double*)&a.upper_bits;
    double *_v_lower = (double*)&a.lower_bits;
    
    res = r32_4x((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]) * b;
    
    return res;
}

inline r32_4x operator/(r32_4x a, r64_4x b)
{
    r32_4x res(0.0f);
    
    double *_v_upper = (double*)&b.upper_bits;
    double *_v_lower = (double*)&b.lower_bits;
    
    res = a / r32_4x((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]);
    
    return res;
}

inline r32_4x operator/(r64_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    double *_v_upper = (double*)&a.upper_bits;
    double *_v_lower = (double*)&a.lower_bits;
    
    res = r32_4x((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]) / b;
    
    return res;
}

inline r64_4x operator-(r64 left, r64_4x right)
{
    return r64_4x(left) - right;
}

inline r64_4x& operator-=(r64_4x &left, r64 right)
{
    left = left - r64_4x(right);
    return left;
}

inline r32_4x operator^(r32_4x a, r32_4x b)
{
    r32_4x result;
    
    result.p = _mm_xor_ps(a.p, b.p);
    
    return(result);
}

inline r32_4x & operator^=(r32_4x &a, r32_4x b)
{
    a = a ^ b;
    
    return(a);
}

inline r32_4x operator&(r32_4x a, r32_4x b)
{
    r32_4x result;
    
    result.p = _mm_and_ps(a.p, b.p);
    
    return(result);
}

inline r32_4x & operator&=(r32_4x &a, r32_4x b)
{
    a = a & b;
    
    return(a);
}

inline r32_4x operator|(r32_4x a, r32_4x b)
{
    r32_4x result;
    
    result.p = _mm_or_ps(a.p, b.p);
    
    return(result);
}

inline r32_4x& operator|=(r32_4x &a, r32_4x b)
{
    a = a | b;
    
    return(a);
}

inline r64_4x simd_min(r64_4x left, r64_4x right)
{
    __m128d min_upper = _mm_min_pd(left.upper_bits.p, right.upper_bits.p);
    __m128d min_lower = _mm_min_pd(left.lower_bits.p, right.lower_bits.p);
    return r64_4x(min_upper, min_lower);
}

inline r64_4x simd_min(r64 left, r64_4x right)
{
    __m128d min_upper = _mm_min_pd(_mm_set1_pd(left), right.upper_bits.p);
    __m128d min_lower = _mm_min_pd(_mm_set1_pd(left), right.lower_bits.p);
    return r64_4x(min_upper, min_lower);
}

inline r32_4x simd_min(r32 left, r32_4x right)
{
    __m128 min = _mm_min_ps(_mm_set1_ps(left), right.p);
    return r32_4x(min);
}

inline r64_4x simd_max(r64_4x left, r64_4x right)
{
    __m128d max_upper = _mm_max_pd(left.upper_bits.p, right.upper_bits.p);
    __m128d max_lower = _mm_max_pd(left.lower_bits.p, right.lower_bits.p);
    return r64_4x(max_upper, max_lower);
}

inline r64_4x simd_max(r64 left, r64_4x right)
{
    __m128d max_upper = _mm_max_pd(_mm_set1_pd(left), right.upper_bits.p);
    __m128d max_lower = _mm_max_pd(_mm_set1_pd(left), right.lower_bits.p);
    return r64_4x(max_upper, max_lower);
}

inline r32_4x simd_max(r32 left, r32_4x right)
{
    __m128 max = _mm_max_ps(_mm_set1_ps(left), right.p);
    return r32_4x(max);
}

inline b32 equal_epsilon(r64_4x v, r64 cmp, r64 epsilon)
{
    __m128d vcmp_me_up = _mm_cmplt_pd(_mm_set1_pd(cmp - epsilon), v.upper_bits.p);
    __m128d vcmp_pe_up = _mm_cmpgt_pd(_mm_set1_pd(cmp + epsilon), v.upper_bits.p);
    __m128d vcmp_me_lo = _mm_cmplt_pd(_mm_set1_pd(cmp - epsilon), v.lower_bits.p);
    __m128d vcmp_pe_lo = _mm_cmpgt_pd(_mm_set1_pd(cmp + epsilon), v.lower_bits.p);
    
    i32 cmp_me_up = _mm_movemask_pd(vcmp_me_up);
    i32 cmp_pe_up = _mm_movemask_pd(vcmp_pe_up);
    i32 cmp_me_lo = _mm_movemask_pd(vcmp_me_lo);
    i32 cmp_pe_lo = _mm_movemask_pd(vcmp_pe_lo);
    
    return cmp_me_up != 0 && cmp_pe_up != 0 && cmp_me_lo != 0 && cmp_pe_lo != 0;
}

inline b32 any_lt(i32_4x v, i32 val)
{
    __m128i vcmp = _mm_cmplt_epi32(v.p, _mm_set1_epi32(val));
    i32 cmp = _mm_movemask_epi8(vcmp);
    
    return cmp != 0;
}

inline b32 any_lt(r64_4x v, r64_4x val)
{
    __m128d vcmp_lt_up = _mm_cmplt_pd(v.upper_bits.p, val.upper_bits.p);
    __m128d vcmp_lt_lo = _mm_cmplt_pd(v.lower_bits.p, val.lower_bits.p);
    
    i32 cmp_lt_up = _mm_movemask_pd(vcmp_lt_up);
    i32 cmp_lt_lo = _mm_movemask_pd(vcmp_lt_lo);
    
    return cmp_lt_up != 0 && cmp_lt_lo != 0;
}

inline b32 any_lt(r64_4x v, r64 val)
{
    __m128d vcmp_lt_up = _mm_cmplt_pd(v.upper_bits.p, _mm_set1_pd(val));
    __m128d vcmp_lt_lo = _mm_cmplt_pd(v.lower_bits.p, _mm_set1_pd(val));
    
    i32 cmp_lt_up = _mm_movemask_pd(vcmp_lt_up);
    i32 cmp_lt_lo = _mm_movemask_pd(vcmp_lt_lo);
    
    return cmp_lt_up != 0 && cmp_lt_lo != 0;
}

inline b32 any_lt_eq(r64_4x v, r64_4x val)
{
    __m128d vcmp_lt_up = _mm_cmplt_pd(v.upper_bits.p, val.upper_bits.p);
    __m128d vcmp_lt_lo = _mm_cmplt_pd(v.lower_bits.p, val.lower_bits.p);
    
    __m128d vcmp_eq_up = _mm_cmpeq_pd(v.upper_bits.p, val.upper_bits.p);
    __m128d vcmp_eq_lo = _mm_cmpeq_pd(v.lower_bits.p, val.lower_bits.p);
    
    i32 cmp_lt_up = _mm_movemask_pd(vcmp_lt_up);
    i32 cmp_lt_lo = _mm_movemask_pd(vcmp_lt_lo);
    i32 cmp_eq_up = _mm_movemask_pd(vcmp_eq_up);
    i32 cmp_eq_lo = _mm_movemask_pd(vcmp_eq_lo);
    
    return cmp_lt_up != 0 || (cmp_eq_up != 0 && cmp_lt_lo != 0) || cmp_eq_lo != 0;
}

inline b32 any_lt_eq(r64_4x v, r64 val)
{
    __m128d vcmp_lt_up = _mm_cmplt_pd(v.upper_bits.p, _mm_set1_pd(val));
    __m128d vcmp_lt_lo = _mm_cmplt_pd(v.lower_bits.p, _mm_set1_pd(val));
    
    __m128d vcmp_eq_up = _mm_cmpeq_pd(v.upper_bits.p, _mm_set1_pd(val));
    __m128d vcmp_eq_lo = _mm_cmpeq_pd(v.lower_bits.p, _mm_set1_pd(val));
    
    i32 cmp_lt_up = _mm_movemask_pd(vcmp_lt_up);
    i32 cmp_lt_lo = _mm_movemask_pd(vcmp_lt_lo);
    i32 cmp_eq_up = _mm_movemask_pd(vcmp_eq_up);
    i32 cmp_eq_lo = _mm_movemask_pd(vcmp_eq_lo);
    
    return cmp_lt_up != 0 || (cmp_eq_up != 0 && cmp_lt_lo != 0) || cmp_eq_lo != 0;
}


inline b32 any_nz(r64_4x v)
{
    __m128d upper_vcmp = _mm_cmplt_pd(_mm_setzero_pd(), v.upper_bits.p);
    i32 upper_cmp = _mm_movemask_pd(upper_vcmp);
    
    __m128d lower_vcmp = _mm_cmplt_pd(_mm_setzero_pd(), v.lower_bits.p);
    i32 lower_cmp = _mm_movemask_pd(lower_vcmp);
    
    return lower_cmp != 0 || upper_cmp != 0;
}

inline b32 any_nz(r32_4x v)
{
    __m128 vcmp = _mm_cmplt_ps(_mm_setzero_ps(), v.p);
    i32 cmp = _mm_movemask_ps(vcmp);
    
    return cmp != 0;
}


#else
union r64_4x
{
    __m256d p;
    r64 e[4];
    
    r64_4x(r64 v)
    {
        p = _mm256_set1_pd(v);
    }
    
    r64_4x(r64 v1, r64 v2, r64 v3, r64 v4)
    {
        p = _mm256_set_pd(v1, v2, v3, v4);
    }
    
    r64_4x(__m256d v)
    {
        double *_v = (double*)&v;
        p = _mm256_set_pd(_v[0], _v[1], _v[2], _v[3]);
    }
    
    inline r64_4x& operator=(const r64_4x& v)
    {
        p = _mm256_set_pd(v.e[0], v.e[1], v.e[2], v.e[3]);
        
        return *this;
    }
    
    inline r64_4x& operator=(const r64& v)
    {
        p = _mm256_set1_pd(v);
        
        return *this;
    }
    
    inline r64_4x operator+ (r64_4x b)
    {
        r64_4x res(0.0);
        
        res.p = _mm256_add_pd(p, b.p);
        
        return res;
    }
    
    inline r64_4x& operator+= (r64_4x b)
    {
        p = _mm256_add_pd(p, b.p);
        
        return *this;
    }
    
    inline r64_4x operator- (r64_4x b)
    {
        r64_4x res(0.0);
        
        res.p = _mm256_sub_pd(p, b.p);
        
        return res;
    }
    
    inline r64_4x operator-= (r64_4x b)
    {
        p = _mm256_sub_pd(p, b.p);
        
        return *this;
    }
    
    inline r64_4x operator* (r64_4x b)
    {
        r64_4x res(0.0);
        
        res.p = _mm256_mul_pd(p, b.p);
        
        return res;
    }
    
    inline r64_4x operator* (r32 b)
    {
        r64_4x res(0.0);
        
        res.p = _mm256_mul_pd(p, _mm256_set1_pd(b));
        
        return res;
    }
    
    inline r64_4x operator* (r64 b)
    {
        r64_4x res(0.0);
        
        res.p = _mm256_mul_pd(p, _mm256_set1_pd(b));
        
        return res;
    }
    
    inline r64_4x& operator*= (r64_4x b)
    {
        p = _mm256_mul_pd(p, b.p);
        
        return *this;
    }
    
    inline r64_4x operator/ (r64_4x b)
    {
        r64_4x res(0.0);
        
        res.p = _mm256_div_pd(p, b.p);
        
        return res;
    }
    
    inline r64_4x& operator/= (r64_4x b)
    {
        p = _mm256_div_pd(p, b.p);
        return *this;
    }
    
    
};

inline r32_4x operator+(r32_4x a, r64_4x b)
{
    r32_4x res(0.0f);
    
    double *_v = (double*)&b.p;
    
    res = a + r32_4x((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]);
    
    return res;
}

inline r32_4x operator+(r64_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    double *_v = (double*)&b.p;
    
    res = r32_4x((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]) + b;
    
    return res;
}

inline r32_4x operator*(r32_4x a, r64_4x b)
{
    r32_4x res(0.0f);
    
    double *_v = (double*)&b.p;
    
    res = a * r32_4x((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]);
    
    return res;
}

inline r32_4x operator*(r64_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    double *_v = (double*)&a.p;
    
    res = r32_4x((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]) * b;
    
    return res;
}

inline r32_4x operator/(r32_4x a, r64_4x b)
{
    r32_4x res(0.0f);
    
    double *_v = (double*)&b.p;
    
    res = a / r32_4x((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]);
    
    return res;
}

inline r32_4x operator/(r64_4x a, r32_4x b)
{
    r32_4x res(0.0f);
    
    double *_v = (double*)&a.p;
    
    res = r32_4x((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]) / b;
    
    return res;
}

inline r64 operator-(r64 left, r64_4x right)
{
    return left - right.e[0];
}

inline r32 operator-(r32 left, r64_4x right)
{
    return left - (r32)right.e[0];
}

inline r64_4x simd_min(r64_4x left, r64_4x right)
{
    __m256d min = _mm256_min_pd(left.p, right.p);
    return r64_4x(min);
}

inline r64_4x simd_min(r64 left, r64_4x right)
{
    __m256d min = _mm256_min_pd(_mm256_set1_pd(left), right.p);
    return r64_4x(min);
}

inline r32_4x simd_min(r32 left, r32_4x right)
{
    __m128 min = _mm_min_ps(_mm_set1_ps(left), right.p);
    return r32_4x(min);
}

inline r32_4x simd_max(r32 left, r32_4x right)
{
    __m128 max = _mm_max_ps(_mm_set1_ps(left), right.p);
    return r32_4x(max);
}

inline r32_4x simd_max(r64 left, r64_4x right)
{
    __m256d max = _mm256_max_ps(_mm256_set1_ps(left), right.p);
    return r64_4x(max);
}

inline r32_4x simd_max(r64_4x left, r64_4x right)
{
    __m256d max = _mm256_max_ps(left.p, right.p);
    return r64_4x(max);
}

inline b32 equal_epsilon(r64_4x v, r64 cmp, r64 epsilon)
{
    __m256d vcmp_me = _mm256_cmp_pd(_mm256_set1_pd(cmp - epsilon), v.p, _CMP_LT_OQ);
    __m256d vcmp_pe = _mm256_cmp_pd(_mm256_set1_pd(cmp + epsilon), v.p, _CMP_GT_OQ);
    
    i32 cmp_me = _mm256_movemask_pd(vcmp_me);
    i32 cmp_pe = _mm256_movemask_pd(vcmp_pe);
    
    return cmp_me != 0 && cmp_pe != 0;
}

inline b32 any_lt(i32_4xv, i32 val)
{
    __m128i vcmp = _mm_cmplt_epi32(v.p, _mm_set1_epi32(val));
    i32 cmp = _mm_movemask_epi8(vcmp);
    
    return cmp != 0;
}

inline b32 any_lt(r64_4x v, r64_4x val)
{
    __m256d vcmp_lt = _mm256_cmp_pd(v.p, val.p, _CMP_LT_OQ);
    i32 cmp_lt = _mm256_movemask_pd(vcmp_lt);
    
    return cmp_lt != 0;
}

inline b32 any_lt(r64_4x v, r64 val)
{
    __m256d vcmp = _mm256_cmp_pd(v.p, _mm_set1_pd(val), _CMP_LT_OQ);
    
    i32 cmp_lt = _mm256_movemask_pd(vcmp);
    
    return cmp_lt != 0;
}

inline b32 any_lt_eq(r64_4x v, r64_4x val)
{
    __m256d vcmp_lt = _mm256_cmp_pd(v.p, val.p, _CMP_LT_OQ);
    i32 cmp_lt = _mm256_movemask_pd(vcmp_lt);
    __m256d vcmp_eq = _mm256_cmp_pd(v.p, val.p, _CMP_EQ_OQ);
    i32 cmp_eq = _mm256_movemask_pd(vcmp_eq);
    
    return cmp_lt != 0 || cmp_eq != 0;
}

inline b32 any_lt_eq(r64_4x v, r64 val)
{
    __m256d vcmp_lt = _mm256_cmp_pd(v.p, _mm256_set1_pd(val), _CMP_LT_OQ);
    i32 cmp_lt = _mm256_movemask_pd(vcmp_lt);
    __m256d vcmp_eq = _mm256_cmp_pd(v.p, mm256_set1_pd(val), _CMP_EQ_OQ);
    i32 cmp_eq = _mm256_movemask_pd(vcmp_eq);
    
    return cmp_lt != 0 || cmp_eq != 0;
}


inline b32 any_nz(r64_4x v)
{
    __m256d vcmp = _mm256_cmp_pd(_mm256_setzero_pd(), v.p, _CMP_LT_OQ);
    i32 cmp = _mm256_movemask_pd(vcmp);
    
    return cmp != 0;
}

#endif

union Vec2_4x
{
    struct
    {
        r32_4x x;
        r32_4x y;
    };
    __m128 p[2];
    r32 e[8];
    u32 u[8];
    
    Vec2_4x()
    {
        x = r32_4x(0.0f);
        y= r32_4x(0.0f);
    }
    
    Vec2_4x(r32 _x, r32 _y)
    {
        x = r32_4x(_x);
        y = r32_4x(_y);
    }

    Vec2_4x(r32_4x _x, r32_4x _y)
    {
        x = _x;
        y = _y;
    }
    
    Vec2_4x(r32 v)
    {
        x = r32_4x(v);
        y = r32_4x(v);
    }
    
    Vec2_4x(math::Vec2 vec)
    {
        x = r32_4x(vec.x);
        y = r32_4x(vec.y);
    }
    
    Vec2_4x(math::Vec2 v1, math::Vec2 v2, math::Vec2 v3, math::Vec2 v4)
    {
        x = r32_4x(v1.x, v2.x, v3.x, v4.x);
        y = r32_4x(v1.y, v2.y, v3.y, v4.y);
    }
    
    inline Vec2_4x& operator=(const Vec2_4x& v)
    {
        x = v.x;
        y = v.y;
        
        return *this;
    }
    
    inline Vec2_4x operator+(Vec2_4x& v)
    {
        Vec2_4x res(0.0f);
        
        res.x = x + v.x;
        res.y = y + v.y;
        
        return res;
    }
    
    inline Vec2_4x operator+(r32_4x v)
    {
        Vec2_4x res(0.0f);
        
        res.x = x + v;
        res.y = y + v;
        
        return res;
    }
    
    inline Vec2_4x operator+(r32 v)
    {
        Vec2_4x res(0.0f);
        
        r32_4x _v = r32_4x(v);
        
        res.x = x + _v;
        res.y = y + _v;
        
        return res;
    }
    
    inline Vec2_4x operator-(Vec2_4x& v)
    {
        Vec2_4x res(0.0f);
        
        res.x = x - v.x;
        res.y = y - v.y;
        
        return res;
    }
    
    inline Vec2_4x operator-(r32_4x v)
    {
        Vec2_4x res(0.0f);
        
        res.x = x - v;
        res.y = y - v;
        
        return res;
    }
    
    inline Vec2_4x operator-(r32 v)
    {
        Vec2_4x res(0.0f);
        
        r32_4x _v = r32_4x(v);
        
        res.x = x - _v;
        res.y = y - _v;
        
        return res;
    }
    
    inline Vec2_4x operator*(Vec2_4x& v)
    {
        Vec2_4x res(0.0f);
        
        res.x = x * v.x;
        res.y = y * v.y;
        
        return res;
    }
    
    inline Vec2_4x operator*(r32_4x v)
    {
        Vec2_4x res(0.0f);
        
        res.x = x * v;
        res.y = y * v;
        
        return res;
    }

    inline Vec2_4x operator*(r32 v)
    {
        Vec2_4x res(0.0f);
        
        r32_4x _v = r32_4x(v);
        
        res.x = x * _v;
        res.y = y * _v;
        
        return res;
    }
    
    inline Vec2_4x operator*(math::Vec2 v)
    {
        Vec2_4x res(0.0f);

        res.x = x * r32_4x(v.x);
        res.y = y * r32_4x(v.y);

        return res;
    }
    
    
    inline Vec2_4x operator/(Vec2_4x& v)
    {
        Vec2_4x res(0.0f);
        
        res.x = x / v.x;
        res.y = y / v.y;
        
        return res;
    }
    
    inline Vec2_4x operator/(r32_4x v)
    {
        Vec2_4x res(0.0f);
        
        res.x = x / v;
        res.y = y / v;
        
        return res;
    }
    
    inline Vec2_4x operator/(r32 v)
    {
        Vec2_4x res(0.0f);
        
        r32_4x _v = r32_4x(v);
        
        res.x = x / _v;
        res.y = y / _v;
        
        return res;
    }
};

union Vec3_4x
{
    struct
    {
        r32_4x x;
        r32_4x y;
        r32_4x z;
    };
    __m128 p[3];
    r32 e[12];
    u32 u[12];
    
    Vec3_4x() : Vec3_4x(0.0f) {}
    
    Vec3_4x(r32 _x, r32 _y, r32 _z)
    {
        x = r32_4x(_x);
        y = r32_4x(_y);
        z = r32_4x(_z);
    }
    
    Vec3_4x(Vec2_4x v)
    {
        x = v.x;
        y = v.y;
        z = 0.0f;
    }
    
    Vec3_4x(r32 v)
    {
        x = r32_4x(v);
        y = r32_4x(v);
        z = r32_4x(v);
    }
    
    Vec3_4x(math::Vec3 vec)
    {
        x = r32_4x(vec.x);
        y = r32_4x(vec.y);
        z = r32_4x(vec.z);
    }
    
    Vec3_4x(math::Vec3 v1, math::Vec3 v2, math::Vec3 v3, math::Vec3 v4)
    {
        x = r32_4x(v1.x, v2.x, v3.x, v4.x);
        y = r32_4x(v1.y, v2.y, v3.y, v4.y);
        z = r32_4x(v1.z, v2.z, v3.z, v4.z);
    }
    
    inline Vec3_4x& operator=(const Vec3_4x& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        
        return *this;
    }
    
    inline Vec3_4x operator+(Vec3_4x& v)
    {
        Vec3_4x res(0.0f);
        
        res.x = x + v.x;
        res.y = y + v.y;
        res.z = z + v.z;
        
        return res;
    }
    
    inline Vec3_4x operator+(r32_4x a)
    {
        Vec3_4x res(0.0f);
        
        res.x = x + a;
        res.y = y + a;
        res.z = z + a;
        
        return res;
    }
    
    inline Vec3_4x operator+(r32 a)
    {
        Vec3_4x res(0.0f);
        
        r32_4x _a = r32_4x(a);
        
        res.x = x + _a;
        res.y = y + _a;
        res.z = z + _a;
        
        return res;
    }
    
    inline Vec3_4x operator+(math::Vec3 b)
    {
        Vec3_4x res(0.0f);
        
        res.x = x + b.x;
        res.y = y + b.y;
        res.z = z + b.z;
        
        return res;
    }
    
    inline Vec3_4x& operator+= (Vec3_4x b)
    {
        x = x + b.x;
        y = y + b.y;
        z = z + b.z;
        
        return *this;
    }
    
    inline Vec3_4x operator-(Vec3_4x& a)
    {
        Vec3_4x res(0.0f);
        
        res.x = x - a.x;
        res.y = y - a.y;
        res.z = z - a.z;
        
        return res;
    }
    
    inline Vec3_4x operator-(r32_4x a)
    {
        Vec3_4x res(0.0f);
        
        res.x = x - a;
        res.y = y - a;
        res.z = z - a;
        
        return res;
    }
    
    inline Vec3_4x operator-(r32 a)
    {
        Vec3_4x res(0.0f);
        
        r32_4x _a = r32_4x(a);
        
        res.x = x - _a;
        res.y = y - _a;
        res.z = z - _a;
        
        return res;
    }
    
    inline Vec3_4x operator*(Vec3_4x& a)
    {
        Vec3_4x res(0.0f);
        
        
        res.x = x * a.x;
        res.y = y * a.y;
        res.z = z * a.z;
        
        return res;
    }
    
    inline Vec3_4x operator*(r32_4x a)
    {
        Vec3_4x res(0.0f);
        
        res.x = x * a;
        res.y = y * a;
        res.z = z * a;
        
        return res;
    }
    
    inline void operator*=(Vec3_4x b)
    {
        x *= b.x;
        y *= b.y;
        z *= b.z;
    }
    
    inline Vec3_4x operator*(r32 a)
    {
        Vec3_4x res(0.0f);
        
        r32_4x _a = r32_4x(a);
        
        res.x = x * _a;
        res.y = y * _a;
        res.z = z * _a;
        
        return res;
    }

    inline Vec3_4x operator*(math::Vec3 v)
    {
        Vec3_4x res(0.0f);

        res.x = x * r32_4x(v.x);
        res.y = y * r32_4x(v.y);
        res.z = z * r32_4x(v.z);

        return res;
    }
    
    inline Vec3_4x operator*(r64 a)
    {
        Vec3_4x res(0.0f);
        
        r64_4x _a = r64_4x(a);
        
        res.x = x * _a;
        res.y = y * _a;
        res.z = z * _a;
        
        return res;
    }
    
    inline Vec3_4x operator/(Vec3_4x& a)
    {
        Vec3_4x res(0.0f);
        
        res.x = x / a.x;
        res.y = y / a.y;
        res.z = z / a.z;
        
        return res;
    }
    
    inline Vec3_4x operator/(r32_4x a)
    {
        Vec3_4x res(0.0f);
        
        res.x = x / a;
        res.y = y / a;
        res.z = z / a;
        
        return res;
    }
    
    inline Vec3_4x operator/(r32 a)
    {
        Vec3_4x res(0.0f);
        
        r32_4x _a = r32_4x(a);
        
        res.x = x / _a;
        res.y = y / _a;
        res.z = z / _a;
        
        return res;
    }
};

inline Vec3_4x operator+(math::Vec3 a, Vec3_4x b)
{
    Vec3_4x res(0.0f);
    
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    
    return res;
}

union Vec4_4x
{
    struct
    {
        union
        {
            r32_4x x;
            r32_4x r;
        };
        
        union
        {
            r32_4x y;
            r32_4x g;
        };
        
        union
        {
            r32_4x z;
            r32_4x b;
        };
        
        union
        {
            r32_4x w;
            r32_4x a;
        };
    };
    __m128 p[4];
    r32 e[16];
    u32 u[16];
    
    Vec4_4x(r32 _x, r32 _y, r32 _z, r32 _w)
    {
        x = r32_4x(_x);
        y = r32_4x(_y);
        z = r32_4x(_z);
        w = r32_4x(_w);
    }
    
    Vec4_4x(r32 v)
    {
        x = r32_4x(v);
        y = r32_4x(v);
        z = r32_4x(v);
        w = r32_4x(v);
    }
    
    Vec4_4x(math::Vec4 vec)
    {
        x = r32_4x(vec.x);
        y = r32_4x(vec.y);
        z = r32_4x(vec.z);
        w = r32_4x(vec.w);
    }
    
    Vec4_4x(math::Vec4 v1, math::Vec4 v2, math::Vec4 v3, math::Vec4 v4)
    {
        x = r32_4x(v1.x, v2.x, v3.x, v4.x);
        y = r32_4x(v1.y, v2.y, v3.y, v4.y);
        z = r32_4x(v1.z, v2.z, v3.z, v4.z);
        w = r32_4x(v1.w, v2.w, v3.w, v4.w);
    }
    
    inline Vec4_4x& operator=(const Vec4_4x& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        
        return *this;
    }
    
    inline Vec4_4x operator+(Vec4_4x& v)
    {
        Vec4_4x res(0.0f);
        
        res.x = x + v.x;
        res.y = x + v.y;
        res.z = x + v.z;
        res.w = x + v.w;
        
        return res;
    }
    
    inline Vec4_4x operator+(r32_4x v)
    {
        Vec4_4x res(0.0f);
        
        res.x = x + v;
        res.y = x + v;
        res.z = x + v;
        res.w = x + v;
        
        return res;
    }
    
    inline Vec4_4x operator+(r32 v)
    {
        Vec4_4x res(0.0f);
        
        r32_4x _v = r32_4x(v);
        
        res.x = x + _v;
        res.y = x + _v;
        res.z = x + _v;
        res.w = x + _v;
        
        return res;
    }
    
    inline Vec4_4x operator-(Vec4_4x& v)
    {
        Vec4_4x res(0.0f);
        
        res.x = x - v.x;
        res.y = x - v.y;
        res.z = x - v.z;
        res.w = x - v.w;
        
        return res;
    }
    
    inline Vec4_4x operator-(r32_4x v)
    {
        Vec4_4x res(0.0f);
        
        res.x = x - v;
        res.y = x - v;
        res.z = x - v;
        res.w = x - v;
        
        return res;
    }
    
    inline Vec4_4x operator-(r32 v)
    {
        Vec4_4x res(0.0f);
        
        r32_4x _v = r32_4x(v);
        
        res.x = x - _v;
        res.y = x - _v;
        res.z = x - _v;
        res.w = x - _v;
        
        return res;
    }
    
    inline Vec4_4x operator*(Vec4_4x& v)
    {
        Vec4_4x res(0.0f);
        
        res.x = x * v.x;
        res.y = x * v.y;
        res.z = x * v.z;
        res.w = x * v.w;
        
        return res;
    }
    
    inline Vec4_4x operator*(r32_4x v)
    {
        Vec4_4x res(0.0f);
        
        res.x = x * v;
        res.y = x * v;
        res.z = x * v;
        res.w = x * v;
        
        return res;
    }
    
    inline Vec4_4x operator*(r32 v)
    {
        Vec4_4x res(0.0f);
        
        r32_4x _v = r32_4x(v);
        
        res.x = x * _v;
        res.y = x * _v;
        res.z = x * _v;
        res.w = x * _v;
        
        return res;
    }

    inline Vec4_4x operator*(math::Vec4 v)
    {
        Vec4_4x res(0.0f);

        res.x = x * r32_4x(v.x);
        res.y = y * r32_4x(v.y);
        res.z = z * r32_4x(v.z);
        res.w = w * r32_4x(v.w);

        return res;
    }
    
    inline Vec4_4x operator/(Vec4_4x& v)
    {
        Vec4_4x res(0.0f);
        
        res.x = x / v.x;
        res.y = x / v.y;
        res.z = x / v.z;
        res.w = x / v.w;
        
        return res;
    }
    
    inline Vec4_4x operator/(r32_4x v)
    {
        Vec4_4x res(0.0f);
        
        res.x = x / v;
        res.y = x / v;
        res.z = x / v;
        res.w = x / v;
        
        return res;
    }
    
    inline Vec4_4x operator/(r32 v)
    {
        Vec4_4x res(0.0f);
        
        r32_4x _v = r32_4x(v);
        
        res.x = x / _v;
        res.y = x / _v;
        res.z = x / _v;
        res.w = x / _v;
        
        return res;
    }
};


inline b32 any_nz(Vec3_4x v)
{
    return any_nz(v.x) || any_nz(v.y) || any_nz(v.z);
}

inline b32 all_zero(r32_4x v)
{
    return false;
}

r32_4x operator-(r32 left, r64_4x right)
{
    return r32_4x(left - (r32)right.e[0], left - (r32)right.e[1], left - (r32)right.e[2], left - (r32)right.e[3]);
}

using Rgba_4x = Vec4_4x;

// Extra math

namespace math
{
    inline r32_4x lerp(r32_4x a, r64_4x t, r32_4x b)
    {
        r32_4x res(0.0f);
        
        r64_4x min = simd_min(1.0, t);
        r32_4x inverse_min = 1.0f - min;
        r32_4x a_times_inverse = inverse_min * a;
        
        res = a_times_inverse + (t * b);
        return res;
    }
    
    inline r32_4x lerp(r32_4x a, r32_4x t, r32_4x b)
    {
        r32_4x res(0.0f);
        
        r32_4x min = simd_min(1.0, t);
        r32_4x inverse_min = 1.0f - min;
        r32_4x a_times_inverse = inverse_min * a;
        
        res = a_times_inverse + (t * b);
        return res;
    }

    inline r32_4x linear_tween(r32_4x b, r32_4x t, r32_4x _c)
    {
        r32_4x c = _c - b;
        return c * t + b;
    }

    inline r32_4x ease_in_quad(r32_4x b, r32_4x t, r32_4x _c)
    {
        r32_4x c = _c - b;
        return c * t * t + b;
    }

    inline r32_4x ease_out_quad(r32_4x b, r32_4x t, r32_4x _c)
    {
        r32_4x c = _c - b;
        return (-c) * t * (t - 2.0f) + b;
    }

    inline r32_4x ease_in_out_quad(r32_4x b, r32_4x t, r32_4x _c)
    {
        r32_4x c = _c - b;

        r32_4x res(0.0f);

        t = t * 2.0f;
        for(i32 i = 0; i < 4; i++)
        {
            r32 l_t = t.e[i];
            r32 l_c = _c.e[i];
            r32 l_b = b.e[i];
            if(l_t < 1.0)
            {
                res.e[i] = (l_c / 2.0f) * (l_t * l_t) + l_b;
            }
            else
            {
                l_t--;
                res.e[i] = -l_c / 2.0f * (l_t * (l_t - 2.0f) - 1.0f) + l_b;
            }
        }
    }

    inline r32_4x ease_in_cubic(r32_4x b, r32_4x t, r32_4x _c)
    {
        r32_4x c = _c - b;
        return c * t * t * t+ b;
    }

    inline r32_4x ease_out_cubic(r32_4x b, r32_4x t, r32_4x _c)
    {
        r32_4x c = _c - b;
        return (-c) * (t * t * t + 2.0f) + b;
    }

    inline r32_4x ease_in_out_cubic(r32_4x b, r32_4x t, r32_4x _c)
    {
        r32_4x c = _c - b;

        r32_4x res(0.0f);

        t = t * 2.0f;
        for(i32 i = 0; i < 4; i++)
        {
            r32 l_t = t.e[i];
            r32 l_c = _c.e[i];
            r32 l_b = b.e[i];
            if(l_t < 1.0)
            {
                res.e[i] = (l_c / 2.0f) * (l_t * l_t * l_t) + l_b;
            }
            else
            {
                l_t -= 2.0f;
                res.e[i] = l_c / 2.0f * (l_t * l_t * l_t + 2.0f) + l_b;
            }
        }
    }
    
    inline r64_4x lerp(r64_4x a, r64_4x t, r64_4x b)
    {
        r64_4x res(0.0);
        
        r64_4x min = simd_min(1.0, t);
        r64_4x inverse_min = 1.0 - min;
        r64_4x a_times_inverse = inverse_min * a;
        
        res = a_times_inverse + (t * b);
        return res;
    }
    
    inline Vec2_4x lerp(Vec2_4x a, r64_4x t, Vec2_4x b)
    {
        Vec2_4x res(0.0f);
        res.x = lerp(a.x, t, b.x);
        res.y = lerp(a.y, t, b.y);
        
        return res;
    }
    
    inline Vec3_4x lerp(Vec3_4x a, r64_4x t, Vec3_4x b)
    {
        Vec3_4x res(0.0f);
        res.x = lerp(a.x, t, b.x);
        res.y = lerp(a.y, t, b.y);
        res.z = lerp(a.z, t, b.z);
        
        return res;
    }
    
    inline Vec4_4x lerp(Vec4_4x a, r64_4x t, Vec4_4x b)
    {
        Vec4_4x res(0.0f);
        res.x = lerp(a.x, t, b.x);
        res.y = lerp(a.y, t, b.y);
        res.z = lerp(a.z, t, b.z);
        res.w = lerp(a.w, t, b.w);
        
        return res;
    }
    
    inline r32_4x r_sqrt(r32_4x v)
    {
        return _mm_rsqrt_ps(v.p);
    }
    
    inline Vec3_4x normalize(Vec3_4x v)
    {
	Vec3_4x res(0.0f);
	if(!any_nz(v))
	{
	    return(res);
	}

	r32_4x rec_sqrt = r_sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	
        res.x = rec_sqrt * v.x;
        res.y = rec_sqrt * v.y;
        res.z = rec_sqrt * v.z;
        
        return res;
    }
    
    inline r32 sqrt(r32 val)
    {
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(val)));
    }
}

inline math::Vec2 to_vec2(Vec2_4x vec, i32 index)
{
    assert(index >= 0 && index <= 3);
    math::Vec2 res(0.0f);
    
    res.x = vec.x.e[index];
    res.y = vec.y.e[index];
    
    return res;
}

inline math::Vec3 to_vec3(Vec3_4x vec, i32 index)
{
    assert(index >= 0 && index <= 3);
    math::Vec3 res(0.0f);
    
    res.x = vec.x.e[index];
    res.y = vec.y.e[index];
    res.z = vec.z.e[index];
    
    return res;
}

inline math::Vec4 to_vec4(Vec4_4x vec, i32 index)
{
    assert(index >= 0 && index <= 3);
    math::Vec4 res(0.0f);
    
    res.x = vec.x.e[index];
    res.y = vec.y.e[index];
    res.z = vec.z.e[index];
    res.w = vec.w.e[index];
    
    return res;
}

inline void r32_4x_to_float4(r32_4x v, float* f1, float* f2, float* f3, float* f4)
{
    __m128 x_mm_0 = _mm_load_ps((float*)&v);
    __m128 x_mm_1 = _mm_set1_ps(1.0f);
    __m128 x_mm_2 = _mm_set1_ps(1.0f);
    __m128 x_mm_3 = _mm_set1_ps(1.0f);
    
    __m128 x_mm_4 = _mm_unpacklo_ps(x_mm_0, x_mm_1);
    __m128 x_mm_6 = _mm_unpackhi_ps(x_mm_0, x_mm_1);
    __m128 x_mm_5 = _mm_unpacklo_ps(x_mm_2, x_mm_3);
    __m128 x_mm_7 = _mm_unpackhi_ps(x_mm_2, x_mm_3);
    
    x_mm_0 = _mm_shuffle_ps(x_mm_4, x_mm_5, _MM_SHUFFLE(1, 0, 1, 0));
    x_mm_1 = _mm_shuffle_ps(x_mm_4, x_mm_5, _MM_SHUFFLE(3, 2, 3, 2));
    x_mm_2 = _mm_shuffle_ps(x_mm_6, x_mm_7, _MM_SHUFFLE(1, 0, 1, 0));
    x_mm_3 = _mm_shuffle_ps(x_mm_6, x_mm_7, _MM_SHUFFLE(3, 2, 3, 2));
    
    _mm_store_ps(f1, x_mm_0);
    _mm_store_ps(f2, x_mm_1);
    _mm_store_ps(f3, x_mm_2);
    _mm_store_ps(f4, x_mm_3);
}


inline void vec2_4x_to_float4(Vec2_4x v, float* f1, float* f2, float* f3, float* f4)
{
    __m128 x_mm_0 = _mm_load_ps((float*)&v.x);
    __m128 x_mm_1 = _mm_load_ps((float*)&v.y);
    __m128 x_mm_2 = _mm_set1_ps(1.0f);
    __m128 x_mm_3 = _mm_set1_ps(1.0f);
    
    __m128 x_mm_4 = _mm_unpacklo_ps(x_mm_0, x_mm_1);
    __m128 x_mm_6 = _mm_unpackhi_ps(x_mm_0, x_mm_1);
    __m128 x_mm_5 = _mm_unpacklo_ps(x_mm_2, x_mm_3);
    __m128 x_mm_7 = _mm_unpackhi_ps(x_mm_2, x_mm_3);
    
    x_mm_0 = _mm_shuffle_ps(x_mm_4, x_mm_5, _MM_SHUFFLE(1, 0, 1, 0));
    x_mm_1 = _mm_shuffle_ps(x_mm_4, x_mm_5, _MM_SHUFFLE(3, 2, 3, 2));
    x_mm_2 = _mm_shuffle_ps(x_mm_6, x_mm_7, _MM_SHUFFLE(1, 0, 1, 0));
    x_mm_3 = _mm_shuffle_ps(x_mm_6, x_mm_7, _MM_SHUFFLE(3, 2, 3, 2));
    
    _mm_store_ps(f1, x_mm_0);
    _mm_store_ps(f2, x_mm_1);
    _mm_store_ps(f3, x_mm_2);
    _mm_store_ps(f4, x_mm_3);
}

inline void vec3_4x_to_float4(Vec3_4x v, float* f1, float* f2, float* f3, float* f4)
{
    __m128 x_mm_0 = _mm_load_ps((float*)&v.x);
    __m128 x_mm_1 = _mm_load_ps((float*)&v.y);
    __m128 x_mm_2 = _mm_load_ps((float*)&v.z);
    __m128 x_mm_3 = _mm_set1_ps(1.0f);
    
    __m128 x_mm_4 = _mm_unpacklo_ps(x_mm_0, x_mm_1);
    __m128 x_mm_6 = _mm_unpackhi_ps(x_mm_0, x_mm_1);
    __m128 x_mm_5 = _mm_unpacklo_ps(x_mm_2, x_mm_3);
    __m128 x_mm_7 = _mm_unpackhi_ps(x_mm_2, x_mm_3);
    
    x_mm_0 = _mm_shuffle_ps(x_mm_4, x_mm_5, _MM_SHUFFLE(1, 0, 1, 0));
    x_mm_1 = _mm_shuffle_ps(x_mm_4, x_mm_5, _MM_SHUFFLE(3, 2, 3, 2));
    x_mm_2 = _mm_shuffle_ps(x_mm_6, x_mm_7, _MM_SHUFFLE(1, 0, 1, 0));
    x_mm_3 = _mm_shuffle_ps(x_mm_6, x_mm_7, _MM_SHUFFLE(3, 2, 3, 2));
    
    _mm_store_ps(f1, x_mm_0);
    _mm_store_ps(f2, x_mm_1);
    _mm_store_ps(f3, x_mm_2);
    _mm_store_ps(f4, x_mm_3);
}

inline void vec4_4x_to_float4(Vec4_4x v, float* f1, float* f2, float* f3, float* f4)
{
    __m128 x_mm_0 = _mm_load_ps((float*)&v.r);
    __m128 x_mm_1 = _mm_load_ps((float*)&v.g);
    __m128 x_mm_2 = _mm_load_ps((float*)&v.b);
    __m128 x_mm_3 = _mm_load_ps((float*)&v.a);
    
    __m128 x_mm_4 = _mm_unpacklo_ps(x_mm_0, x_mm_1);
    __m128 x_mm_6 = _mm_unpackhi_ps(x_mm_0, x_mm_1);
    __m128 x_mm_5 = _mm_unpacklo_ps(x_mm_2, x_mm_3);
    __m128 x_mm_7 = _mm_unpackhi_ps(x_mm_2, x_mm_3);
    
    x_mm_0 = _mm_shuffle_ps(x_mm_4, x_mm_5, _MM_SHUFFLE(1, 0, 1, 0));
    x_mm_1 = _mm_shuffle_ps(x_mm_4, x_mm_5, _MM_SHUFFLE(3, 2, 3, 2));
    x_mm_2 = _mm_shuffle_ps(x_mm_6, x_mm_7, _MM_SHUFFLE(1, 0, 1, 0));
    x_mm_3 = _mm_shuffle_ps(x_mm_6, x_mm_7, _MM_SHUFFLE(3, 2, 3, 2));
    
    _mm_store_ps(f1, x_mm_0);
    _mm_store_ps(f2, x_mm_1);
    _mm_store_ps(f3, x_mm_2);
    _mm_store_ps(f4, x_mm_3);
}

// Memory stuff for SIMD
// Generally using _mm_malloc and _mm_free is good, since it makes it possible to pass in alignment parameters
// We have alignment in MemoryArenas

#define member_size(type, member) sizeof(((type *)0)->member)

#define push_array_simd(arena, count, type) (type *)_push_size_simd(arena, (count) * sizeof(type), (member_size(type, e[0])))
#define push_size_simd(arena, type) (type*)_push_size_simd(arena, sizeof(type), (member_size(type, e[0])))
inline void* _push_size_simd(MemoryArena *arena, umm size_init, u32 alignment)
{
    alignment = math::multiple_of_number_uint(alignment, 16);
    return push_size_(arena, size_init, default_with_alignment(alignment));
}

#endif
