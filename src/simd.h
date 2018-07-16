#ifndef SIMD_H
#define SIMD_H

#if defined(__APPLE__) || defined(__linux)
#include "smmintrin.h"
#include "immintrin.h"
#include "emmintrin.h"
#endif

union S_i32
{
    __m128i p;
    i32 e[4];
    
    S_i32(i32 _p) 
    {
        p = _mm_set1_epi32(_p);
    }
    
    S_i32(i32 _a, i32 _b, i32 _c, i32 _d)
    {
        p = _mm_set_epi32(_a, _b, _c, _d);
    }
    
    S_i32& operator=(const i32 v)
    {
        p = _mm_set1_epi32(v);
        
        return *this;
    }
    
    S_i32& operator+= (S_i32 b)
    {
        this->p = _mm_add_epi32(p, b.p);
        return *this;
    }
};

S_i32 operator+ (S_i32 a, S_i32 b)
{
    S_i32 res(0);
    
    res.p = _mm_add_epi32(a.p, b.p);
    
    return res;
}


S_i32 operator- (S_i32 a, S_i32 b)
{
    S_i32 res(0);
    
    res.p = _mm_sub_epi32(a.p, b.p);
    
    return res;
}

S_i32 operator-= (S_i32 a, S_i32 b)
{
    return a - b;
}

S_i32 operator* (S_i32 a, S_i32 b)
{
    S_i32 res(0);
    
    res.p = _mm_mul_epi32(a.p, b.p);
    
    return res;
}

S_i32 operator*= (S_i32 a, S_i32 b)
{
    return a * b;
}

S_i32 operator/ (S_i32 a, S_i32 b)
{
    assert(false);
    return S_i32(0);
}

S_i32 operator/= (S_i32 a, S_i32 b)
{
    assert(false);
    return S_i32(0);
}

b32 operator<(S_i32 a, S_i32 b)
{
    return false;
}

b32 operator>(S_i32 a, S_i32 b)
{
    return false;
}

void operator++(S_i32& a, i32 i)
{
    
}

void operator--(S_i32& a, i32 i)
{
    
}


union S_r32
{
    __m128 p;
    r32 e[4];
    
    S_r32(const S_r32& v) = default;

    S_r32(r32 _p) 
    {
        p = _mm_set1_ps(_p);
    }
    
    S_r32(r32 _a, r32 _b, r32 _c, r32 _d)
    {
        p = _mm_set_ps(_a, _b, _c, _d);
    }
    
    
    S_r32(__m128 v)
    {
        float*_v = (float*)&v;
        p = _mm_set_ps(_v[0], _v[1], _v[2], _v[3]);
    }
    
    S_r32& operator=(const r32& v)
    {
        p = _mm_set1_ps(v);
        
        return *this;
    }
    
    S_r32& operator=(const S_r32& v)
    {
        p = _mm_set_ps(v.e[0], v.e[1], v.e[2], v.e[3]);
        
        return *this;
    }
    
};

S_r32 operator+ (S_r32 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    res.p = _mm_add_ps(a.p, b.p);
    
    return res;
}

S_r32 operator+ (S_r32 a, r32 b)
{
    S_r32 res(0.0f);
    
    res.p = _mm_add_ps(a.p, _mm_set1_ps(b));
    
    return res;
}

S_r32 operator+ (r32 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    res.p = _mm_add_ps(_mm_set1_ps(a), b.p);
    
    return res;
}

S_r32 operator- (S_r32 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    res.p = _mm_sub_ps(a.p, b.p);
    
    return res;
}

S_r32 operator* (S_r32 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    res.p = _mm_mul_ps(a.p, b.p);
    
    return res;
}

S_r32 operator/ (S_r32 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    res.p = _mm_div_ps(a.p, b.p);
    
    return res;
}

union S_h64
{
    struct
    {
        union
        {
            __m128d p;
            r64 e[2];
        };
    };
    
    S_h64(const S_h64& v) = default;


    S_h64(r64 _p) 
    {
        p = _mm_set1_pd(_p);
    }
    
    S_h64(__m128d _p) 
    {
        p = _mm_set1_pd(*(double*)&_p);
    }
    
    S_h64(r64 _a, r64 _b)
    {
        p = _mm_set_pd(_a, _b);
    }
    
    S_h64& operator=(const r64& v)
    {
        p = _mm_set1_pd(v);
        
        return *this;
    }
    
    S_h64& operator=(const S_h64& v)
    {
        r64 v1 = v.e[0];
        r64 v2 = v.e[1];
        p = _mm_set_pd(v1, v2);
        
        return *this;
    }
    
    S_h64 operator+ (S_h64 b)
    {
        S_h64 res(0.0f);
        
        res.p = _mm_add_pd(p, b.p);
        
        return res;
    }
    
    inline S_h64& operator+=(S_h64 b)
    {
        this->p = _mm_add_pd(p, b.p);
        return *this;
    }
    
    S_h64 operator- (S_h64 b)
    {
        S_h64 res(0.0f);
        
        res.p = _mm_sub_pd(p, b.p);
        
        return res;
    }
    
    inline S_h64& operator-=(S_h64 b)
    {
        this->p = _mm_sub_pd(p, b.p);
        return *this;
    }
    
    S_h64 operator* (S_h64 b)
    {
        S_h64 res(0.0f);
        
        res.p = _mm_mul_pd(p, b.p);
        
        return res;
    }
    
    inline S_h64& operator*=(S_h64 b)
    {
        this->p = _mm_mul_pd(p, b.p);
        return *this;
    }
    
    S_h64 operator/ (S_h64 b)
    {
        S_h64 res(0.0f);
        
        res.p = _mm_div_pd(p, b.p);
        
        return res;
    }
    
    inline S_h64& operator/=(S_h64 b)
    {
        this->p = _mm_div_pd(p, b.p);
        return *this;
    }
};

#if defined(__APPLE__) || defined(__linux)
// Used to store 4 doubles in one SIMD constructions
union S_r64
{
    struct
    {
        S_h64 upper_bits;
        S_h64 lower_bits;
    };
    r64 e[4];
    
    S_r64(const S_r64& o) = default;

    S_r64(r64 v)
    {
        upper_bits = S_h64(v);
        lower_bits = S_h64(v);
    }
    
    S_r64(r64 v1, r64 v2, r64 v3, r64 v4)
    {
        upper_bits = S_h64(v1, v2);
        lower_bits = S_h64(v3, v4);
    }
    
    S_r64(__m128d upper, __m128d lower)
    {
        upper_bits = S_h64(upper);
        lower_bits = S_h64(lower);
    }
    
    S_r64& operator=(const S_r64& v)
    {
        upper_bits = S_h64(v.e[0], v.e[1]);
        lower_bits = S_h64(v.e[2], v.e[3]);
        
        return *this;
    }
    
    S_r64& operator=(const r64& v)
    {
        upper_bits = v;
        lower_bits = v;
        
        return *this;
    }
    
    S_r64 operator+ (S_r64 b)
    {
        S_r64 res(0.0);
        
        res.upper_bits = upper_bits + b.upper_bits;
        res.lower_bits = lower_bits + b.lower_bits;
        
        return res;
    }
    
    S_r64& operator+= (S_r64 b)
    {
        upper_bits += b.upper_bits;
        lower_bits += b.lower_bits;
        
        return *this;
    }
    
    S_r64 operator- (S_r64 b)
    {
        S_r64 res(0.0);
        
        res.upper_bits = upper_bits - b.upper_bits;
        res.lower_bits = lower_bits - b.lower_bits;
        
        return res;
    }
    
    S_r64 operator-= (S_r64 b)
    {
        upper_bits -= b.upper_bits;
        lower_bits -= b.lower_bits;
        return *this;
    }
    
    S_r64 operator* (S_r64 b)
    {
        S_r64 res(0.0);
        
        res.upper_bits = upper_bits * b.upper_bits;
        res.lower_bits = lower_bits * b.lower_bits;
        
        return res;
    }
    
    S_r64& operator*= (S_r64 b)
    {
        upper_bits *= b.upper_bits;
        lower_bits *= b.lower_bits;
        return *this;
    }
    
    S_r64 operator/ (S_r64 b)
    {
        S_r64 res(0.0);
        
        res.upper_bits = upper_bits / b.upper_bits;
        res.lower_bits = lower_bits / b.lower_bits;
        
        return res;
    }
    
    S_r64& operator/= (S_r64 b)
    {
        upper_bits /= b.upper_bits;
        lower_bits /= b.lower_bits;
        return *this;
    }
};


S_r32 operator+(S_r32 a, S_r64 b)
{
    S_r32 res(0.0f);
    
    double *_v_upper = (double*)&b.upper_bits;
    double *_v_lower = (double*)&b.lower_bits;
    
    res = a + S_r32((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]);
    
    return res;
}

S_r32 operator+(S_r64 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    double *_v_upper = (double*)&a.upper_bits;
    double *_v_lower = (double*)&a.lower_bits;
    
    res = S_r32((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]) + b;
    
    return res;
}


S_r32 operator*(S_r32 a, S_r64 b)
{
    S_r32 res(0.0f);
    
    double *_v_upper = (double*)&b.upper_bits;
    double *_v_lower = (double*)&b.lower_bits;
    
    res = a * S_r32((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]);
    
    return res;
}

S_r32 operator*(S_r64 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    double *_v_upper = (double*)&a.upper_bits;
    double *_v_lower = (double*)&a.lower_bits;
    
    res = S_r32((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]) * b;
    
    return res;
}

S_r32 operator/(S_r32 a, S_r64 b)
{
    S_r32 res(0.0f);
    
    double *_v_upper = (double*)&b.upper_bits;
    double *_v_lower = (double*)&b.lower_bits;
    
    res = a / S_r32((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]);
    
    return res;
}

S_r32 operator/(S_r64 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    double *_v_upper = (double*)&a.upper_bits;
    double *_v_lower = (double*)&a.lower_bits;
    
    res = S_r32((float)_v_upper[0], (float)_v_upper[1], (float)_v_lower[0], (float)_v_lower[0]) / b;
    
    return res;
}

r64 operator-(r64 left, S_r64 right)
{
    return left - right.e[0];
}

r32 operator-(r32 left, S_r64 right)
{
    return left - (r32)right.e[0];
}

S_r64 simd_min(S_r64 left, S_r64 right)
{
    __m128d min_upper = _mm_min_pd(left.upper_bits.p, right.upper_bits.p);
    __m128d min_lower = _mm_min_pd(left.lower_bits.p, right.lower_bits.p);
    return S_r64(min_upper, min_lower);
}

S_r64 simd_min(r64 left, S_r64 right)
{
    __m128d min_upper = _mm_min_pd(_mm_set1_pd(left), right.upper_bits.p);
    __m128d min_lower = _mm_min_pd(_mm_set1_pd(left), right.lower_bits.p);
    return S_r64(min_upper, min_lower);
}

S_r32 simd_min(r32 left, S_r32 right)
{
    __m128 min = _mm_min_ps(_mm_set1_ps(left), right.p);
    return S_r32(min);
}

b32 equal_epsilon(S_r64 v, r64 cmp, r64 epsilon)
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

b32 any_lt(S_i32 v, i32 val)
{
    __m128i vcmp = _mm_cmplt_epi32(v.p, _mm_set1_epi32(val));
    i32 cmp = _mm_movemask_epi8(vcmp);
    
    return cmp != 0;
}

b32 any_lt(S_r64 v, S_r64 val)
{
    __m128d vcmp_lt_up = _mm_cmplt_pd(v.upper_bits.p, val.upper_bits.p);
    __m128d vcmp_lt_lo = _mm_cmplt_pd(v.lower_bits.p, val.lower_bits.p);
    
    i32 cmp_lt_up = _mm_movemask_pd(vcmp_lt_up);
    i32 cmp_lt_lo = _mm_movemask_pd(vcmp_lt_lo);
    
    return cmp_lt_up != 0 && cmp_lt_lo != 0;
}

b32 any_lt_eq(S_r64 v, S_r64 val)
{
    __m128d vcmp_lt_up = _mm_cmplt_pd(v.upper_bits.p, val.upper_bits.p);
    __m128d vcmp_lt_lo = _mm_cmplt_pd(v.lower_bits.p, val.lower_bits.p);
    
    __m128d vcmp_eq_up = _mm_cmpeq_pd(v.upper_bits.p, val.upper_bits.p);
    __m128d vcmp_eq_lo = _mm_cmpeq_pd(v.lower_bits.p, val.lower_bits.p);
    
    i32 cmp_lt_up = _mm_movemask_pd(vcmp_lt_up);
    i32 cmp_lt_lo = _mm_movemask_pd(vcmp_lt_lo);
    i32 cmp_eq_up = _mm_movemask_pd(vcmp_eq_up);
    i32 cmp_eq_lo = _mm_movemask_pd(vcmp_eq_lo);
    
    return cmp_lt_up != 0 || cmp_eq_up != 0 || cmp_lt_lo != 0 || cmp_eq_lo != 0;
}

b32 any_nz(S_r64 v)
{
    __m128d upper_vcmp = _mm_cmplt_pd(_mm_setzero_pd(), v.upper_bits.p);
    i32 upper_cmp = _mm_movemask_pd(upper_vcmp);
    
    __m128d lower_vcmp = _mm_cmplt_pd(_mm_setzero_pd(), v.lower_bits.p);
    i32 lower_cmp = _mm_movemask_pd(lower_vcmp);
    
    return lower_cmp != 0 || upper_cmp != 0;
}


#else
union S_r64
{
    __m256d p;
    r64 e[4];
    
    S_r64(const S_r64& v)
    {
        p = _mm256_set_pd(v.e[0], v.e[1], v.e[2], v.e[3]);
    }

    S_r64(r64 v)
    {
        p = _mm256_set1_pd(v);
    }
    
    S_r64(r64 v1, r64 v2, r64 v3, r64 v4)
    {
        p = _mm256_set_pd(v1, v2, v3, v4);
    }
    
    S_r64(__m256d v)
    {
        double *_v = (double*)&v;
        p = _mm256_set_pd(_v[0], _v[1], _v[2], _v[3]);
    }
    
    S_r64& operator=(const S_r64& v)
    {
        p = _mm256_set_pd(v.e[0], v.e[1], v.e[2], v.e[3]);
        
        return *this;
    }
    
    S_r64& operator=(const r64 v)
    {
        this->p = _mm256_set1_pd(v);
        
        return *this;
    }
    
    S_r64 operator+ (S_r64 b)
    {
        S_r64 res(0.0);
        
        res.p = _mm256_add_pd(p, b.p);
        
        return res;
    }
    
    S_r64& operator+= (S_r64 b)
    {
        p = _mm256_add_pd(p, b.p);
        
        return *this;
    }
    
    S_r64 operator- (S_r64 b)
    {
        S_r64 res(0.0);
        
        res.p = _mm256_sub_pd(p, b.p);
        
        return res;
    }
    
    S_r64 operator-= (S_r64 b)
    {
        p = _mm256_sub_pd(p, b.p);
        
        return *this;
    }
    
    S_r64 operator* (S_r64 b)
    {
        S_r64 res(0.0);
        
        res.p = _mm256_mul_pd(p, b.p);
        
        return res;
    }
    
    S_r64 operator* (r32 b)
    {
        S_r64 res(0.0);
        
        res.p = _mm256_mul_pd(p, _mm256_set1_pd(b));
        
        return res;
    }
    
    S_r64 operator* (r64 b)
    {
        S_r64 res(0.0);
        
        res.p = _mm256_mul_pd(p, _mm256_set1_pd(b));
        
        return res;
    }
    
    S_r64& operator*= (S_r64 b)
    {
        p = _mm256_mul_pd(p, b.p);
        
        return *this;
    }
    
    S_r64 operator/ (S_r64 b)
    {
        S_r64 res(0.0);
        
        res.p = _mm256_div_pd(p, b.p);
        
        return res;
    }
    
    S_r64& operator/= (S_r64 b)
    {
        p = _mm256_div_pd(p, b.p);
        return *this;
    }
    
    
};

S_r32 operator+(S_r32 a, S_r64 b)
{
    S_r32 res(0.0f);
    
    double *_v = (double*)&b.p;
    
    res = a + S_r32((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]);
    
    return res;
}

S_r32 operator+(S_r64 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    double *_v = (double*)&b.p;
    
    res = S_r32((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]) + b;
    
    return res;
}


S_r32 operator*(S_r32 a, S_r64 b)
{
    S_r32 res(0.0f);
    
    double *_v = (double*)&b.p;
    
    res = a * S_r32((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]);
    
    return res;
}

S_r32 operator*(S_r64 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    double *_v = (double*)&a.p;
    
    res = S_r32((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]) * b;
    
    return res;
}

S_r32 operator/(S_r32 a, S_r64 b)
{
    S_r32 res(0.0f);
    
    double *_v = (double*)&b.p;
    
    res = a / S_r32((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]);
    
    return res;
}

S_r32 operator/(S_r64 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    double *_v = (double*)&a.p;
    
    res = S_r32((float)_v[0], (float)_v[1], (float)_v[2], (float)_v[3]) / b;
    
    return res;
}

r64 operator-(r64 left, S_r64 right)
{
    return left - right.e[0];
}

r32 operator-(r32 left, S_r64 right)
{
    return left - (r32)right.e[0];
}

S_r64 simd_min(S_r64 left, S_r64 right)
{
    __m256d min = _mm256_min_pd(left.p, right.p);
    return S_r64(min);
}

S_r64 simd_min(r64 left, S_r64 right)
{
    __m256d min = _mm256_min_pd(_mm256_set1_pd(left), right.p);
    return S_r64(min);
}

S_r32 simd_min(r32 left, S_r32 right)
{
    __m128 min = _mm_min_ps(_mm_set1_ps(left), right.p);
    return S_r32(min);
}

b32 equal_epsilon(S_r64 v, r64 cmp, r64 epsilon)
{
    __m256d vcmp_me = _mm256_cmp_pd(_mm256_set1_pd(cmp - epsilon), v.p, _CMP_LT_OQ);
    __m256d vcmp_pe = _mm256_cmp_pd(_mm256_set1_pd(cmp + epsilon), v.p, _CMP_GT_OQ);
    
    i32 cmp_me = _mm256_movemask_pd(vcmp_me);
    i32 cmp_pe = _mm256_movemask_pd(vcmp_pe);
    
    return cmp_me != 0 && cmp_pe != 0;
}

b32 any_lt(S_i32 v, i32 val)
{
    __m128i vcmp = _mm_cmplt_epi32(v.p, _mm_set1_epi32(val));
    i32 cmp = _mm_movemask_epi8(vcmp);
    
    return cmp != 0;
}

b32 any_lt(S_r64 v, S_r64 val)
{
    __m256d vcmp_lt = _mm256_cmp_pd(v.p, val.p, _CMP_LT_OQ);
    i32 cmp_lt = _mm256_movemask_pd(vcmp_lt);
    
    return cmp_lt != 0;
}

b32 any_lt_eq(S_r64 v, S_r64 val)
{
    __m256d vcmp_lt = _mm256_cmp_pd(v.p, val.p, _CMP_LT_OQ);
    i32 cmp_lt = _mm256_movemask_pd(vcmp_lt);
    __m256d vcmp_eq = _mm256_cmp_pd(v.p, val.p, _CMP_EQ_OQ);
    i32 cmp_eq = _mm256_movemask_pd(vcmp_eq);
    
    return cmp_lt != 0 || cmp_eq != 0;
}

b32 any_nz(S_r64 v)
{
    __m256d vcmp = _mm256_cmp_pd(_mm256_setzero_pd(), v.p, _CMP_LT_OQ);
    i32 cmp = _mm256_movemask_pd(vcmp);
    
    return cmp != 0;
}

#endif

union S_Vec2
{
    struct
    {
        S_r32 x;
        S_r32 y;
    };
    
    S_Vec2(const S_Vec2& v) = default;

    S_Vec2(r32 _x, r32 _y)
    {
        x = S_r32(_x);
        y = S_r32(_y);
    }
    
    S_Vec2(r32 v)
    {
        x = S_r32(v);
        y = S_r32(v);
    }
    
    S_Vec2(math::Vec2 vec)
    {
        x = S_r32(vec.x);
        y = S_r32(vec.y);
    }
    
    S_Vec2(math::Vec2 v1, math::Vec2 v2, math::Vec2 v3, math::Vec2 v4)
    {
        x = S_r32(v1.x, v2.x, v3.x, v4.x);
        y = S_r32(v1.y, v2.y, v3.y, v4.y);
    }
    
    S_Vec2& operator=(const math::Vec2& v)
    {
        x = v.x;
        y = v.y;
        
        return *this;
    }
    
    S_Vec2& operator=(const S_Vec2& v)
    {
        x = v.x;
        y = v.y;
        
        return *this;
    }
    
    S_Vec2 operator+(S_Vec2& v)
    {
        S_Vec2 res(0.0f);
        
        res.x = x + v.x;
        res.y = y + v.y;
        
        return res;
    }
    
    S_Vec2 operator+(S_r32 v)
    {
        S_Vec2 res(0.0f);
        
        res.x = x + v;
        res.y = y + v;
        
        return res;
    }
    
    S_Vec2 operator+(r32 v)
    {
        S_Vec2 res(0.0f);
        
        S_r32 _v = S_r32(v);
        
        res.x = x + _v;
        res.y = y + _v;
        
        return res;
    }
    
    S_Vec2 operator-(S_Vec2& v)
    {
        S_Vec2 res(0.0f);
        
        res.x = x - v.x;
        res.y = y - v.y;
        
        return res;
    }
    
    S_Vec2 operator-(S_r32 v)
    {
        S_Vec2 res(0.0f);
        
        res.x = x - v;
        res.y = y - v;
        
        return res;
    }
    
    S_Vec2 operator-(r32 v)
    {
        S_Vec2 res(0.0f);
        
        S_r32 _v = S_r32(v);
        
        res.x = x - _v;
        res.y = y - _v;
        
        return res;
    }
    
    S_Vec2 operator*(S_Vec2& v)
    {
        S_Vec2 res(0.0f);
        
        res.x = x * v.x;
        res.y = y * v.y;
        
        return res;
    }
    
    S_Vec2 operator*(S_r32 v)
    {
        S_Vec2 res(0.0f);
        
        res.x = x * v;
        res.y = y * v;
        
        return res;
    }
    
    S_Vec2 operator*(r32 v)
    {
        S_Vec2 res(0.0f);
        
        S_r32 _v = S_r32(v);
        
        res.x = x * _v;
        res.y = y * _v;
        
        return res;
    }
    
    S_Vec2 operator/(S_Vec2& v)
    {
        S_Vec2 res(0.0f);
        
        res.x = x / v.x;
        res.y = y / v.y;
        
        return res;
    }
    
    S_Vec2 operator/(S_r32 v)
    {
        S_Vec2 res(0.0f);
        
        res.x = x / v;
        res.y = y / v;
        
        return res;
    }
    
    S_Vec2 operator/(r32 v)
    {
        S_Vec2 res(0.0f);
        
        S_r32 _v = S_r32(v);
        
        res.x = x / _v;
        res.y = y / _v;
        
        return res;
    }
};

union S_Vec3
{
    struct
    {
        S_r32 x;
        S_r32 y;
        S_r32 z;
    };

    S_Vec3(const S_Vec3& v) = default;    
    
    S_Vec3(r32 _x, r32 _y, r32 _z)
    {
        x = S_r32(_x);
        y = S_r32(_y);
        z = S_r32(_z);
    }
    
    S_Vec3(r32 v)
    {
        x = S_r32(v);
        y = S_r32(v);
        z = S_r32(v);
    }
    
    S_Vec3(math::Vec3 vec)
    {
        x = S_r32(vec.x);
        y = S_r32(vec.y);
        z = S_r32(vec.z);
    }
    
    S_Vec3(math::Vec3 v1, math::Vec3 v2, math::Vec3 v3, math::Vec3 v4)
    {
        x = S_r32(v1.x, v2.x, v3.x, v4.x);
        y = S_r32(v1.y, v2.y, v3.y, v4.y);
        z = S_r32(v1.z, v2.z, v3.z, v4.z);
    }
    
    S_Vec3& operator=(const S_Vec3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        
        return *this;
    }
    
    S_Vec3 operator+(S_Vec3& v)
    {
        S_Vec3 res(0.0f);
        
        res.x = x + v.x;
        res.y = y + v.y;
        res.z = z + v.z;
        
        return res;
    }
    
    S_Vec3 operator+(S_r32 a)
    {
        S_Vec3 res(0.0f);
        
        res.x = x + a;
        res.y = y + a;
        res.z = z + a;
        
        return res;
    }
    
    S_Vec3 operator+(r32 a)
    {
        S_Vec3 res(0.0f);
        
        S_r32 _a = S_r32(a);
        
        res.x = x + _a;
        res.y = y + _a;
        res.z = z + _a;
        
        return res;
    }
    
    S_Vec3 operator+(math::Vec3 b)
    {
        S_Vec3 res(0.0f);
        
        res.x = x + b.x;
        res.y = y + b.y;
        res.z = z + b.z;
        
        return res;
    }
    
    S_Vec3& operator+= (S_Vec3 b)
    {
        x = x + b.x;
        y = y + b.y;
        z = z + b.z;
        
        return *this;
    }
    
    S_Vec3 operator-(S_Vec3& a)
    {
        S_Vec3 res(0.0f);
        
        res.x = x - a.x;
        res.y = y - a.y;
        res.z = z - a.z;
        
        return res;
    }
    
    S_Vec3 operator-(S_r32 a)
    {
        S_Vec3 res(0.0f);
        
        res.x = x - a;
        res.y = y - a;
        res.z = z - a;
        
        return res;
    }
    
    S_Vec3 operator-(r32 a)
    {
        S_Vec3 res(0.0f);
        
        S_r32 _a = S_r32(a);
        
        res.x = x - _a;
        res.y = y - _a;
        res.z = z - _a;
        
        return res;
    }
    
    
    S_Vec3 operator*(S_Vec3& a)
    {
        S_Vec3 res(0.0f);
        
        
        res.x = x * a.x;
        res.y = y * a.y;
        res.z = z * a.z;
        
        return res;
    }
    
    S_Vec3 operator*(S_r32 a)
    {
        S_Vec3 res(0.0f);
        
        res.x = x * a;
        res.y = y * a;
        res.z = z * a;
        
        return res;
    }
    
    
    S_Vec3 operator*(r32 a)
    {
        S_Vec3 res(0.0f);
        
        S_r32 _a = S_r32(a);
        
        res.x = x * _a;
        res.y = y * _a;
        res.z = z * _a;
        
        return res;
    }
    
    S_Vec3 operator*(r64 a)
    {
        S_Vec3 res(0.0f);
        
        S_r64 _a = S_r64(a);
        
        res.x = x * _a;
        res.y = y * _a;
        res.z = z * _a;
        
        return res;
    }
    
    S_Vec3 operator/(S_Vec3& a)
    {
        S_Vec3 res(0.0f);
        
        res.x = x / a.x;
        res.y = y / a.y;
        res.z = z / a.z;
        
        return res;
    }
    
    S_Vec3 operator/(S_r32 a)
    {
        S_Vec3 res(0.0f);
        
        res.x = x / a;
        res.y = y / a;
        res.z = z / a;
        
        return res;
    }
    
    S_Vec3 operator/(r32 a)
    {
        S_Vec3 res(0.0f);
        
        S_r32 _a = S_r32(a);
        
        res.x = x / _a;
        res.y = y / _a;
        res.z = z / _a;
        
        return res;
    }
};

S_Vec3 operator+(math::Vec3 a, S_Vec3 b)
{
    S_Vec3 res(0.0f);
    
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    
    return res;
}



union S_Vec4
{
    struct
    {
        union
        {
            S_r32 x;
            S_r32 r;
        };
        
        union
        {
            S_r32 y;
            S_r32 g;
        };
        
        union
        {
            S_r32 z;
            S_r32 b;
        };
        
        union
        {
            S_r32 w;
            S_r32 a;
        };
    };
    
    S_Vec4(const S_Vec4& v) = default;


    S_Vec4(r32 _x, r32 _y, r32 _z, r32 _w)
    {
        x = S_r32(_x);
        y = S_r32(_y);
        z = S_r32(_z);
        w = S_r32(_w);
    }
    
    S_Vec4(r32 v)
    {
        x = S_r32(v);
        y = S_r32(v);
        z = S_r32(v);
        w = S_r32(v);
    }
    
    S_Vec4(math::Vec4 vec)
    {
        x = S_r32(vec.x);
        y = S_r32(vec.y);
        z = S_r32(vec.z);
        w = S_r32(vec.w);
    }
    
    S_Vec4(math::Vec4 v1, math::Vec4 v2, math::Vec4 v3, math::Vec4 v4)
    {
        x = S_r32(v1.x, v2.x, v3.x, v4.x);
        y = S_r32(v1.y, v2.y, v3.y, v4.y);
        z = S_r32(v1.z, v2.z, v3.z, v4.z);
        w = S_r32(v1.w, v2.w, v3.w, v4.w);
    }
    
    S_Vec4& operator=(const S_Vec4& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        
        return *this;
    }
    
    S_Vec4 operator+(S_Vec4& v)
    {
        S_Vec4 res(0.0f);
        
        res.x = x + v.x;
        res.y = x + v.y;
        res.z = x + v.z;
        res.w = x + v.w;
        
        return res;
    }
    
    S_Vec4 operator+(S_r32 v)
    {
        S_Vec4 res(0.0f);
        
        res.x = x + v;
        res.y = x + v;
        res.z = x + v;
        res.w = x + v;
        
        return res;
    }
    
    S_Vec4 operator+(r32 v)
    {
        S_Vec4 res(0.0f);
        
        S_r32 _v = S_r32(v);
        
        res.x = x + _v;
        res.y = x + _v;
        res.z = x + _v;
        res.w = x + _v;
        
        return res;
    }
    
    
    S_Vec4 operator-(S_Vec4& v)
    {
        S_Vec4 res(0.0f);
        
        res.x = x - v.x;
        res.y = x - v.y;
        res.z = x - v.z;
        res.w = x - v.w;
        
        return res;
    }
    
    S_Vec4 operator-(S_r32 v)
    {
        S_Vec4 res(0.0f);
        
        res.x = x - v;
        res.y = x - v;
        res.z = x - v;
        res.w = x - v;
        
        return res;
    }
    
    S_Vec4 operator-(r32 v)
    {
        S_Vec4 res(0.0f);
        
        S_r32 _v = S_r32(v);
        
        res.x = x - _v;
        res.y = x - _v;
        res.z = x - _v;
        res.w = x - _v;
        
        return res;
    }
    
    S_Vec4 operator*(S_Vec4& v)
    {
        S_Vec4 res(0.0f);
        
        res.x = x * v.x;
        res.y = x * v.y;
        res.z = x * v.z;
        res.w = x * v.w;
        
        return res;
    }
    
    S_Vec4 operator*(S_r32 v)
    {
        S_Vec4 res(0.0f);
        
        res.x = x * v;
        res.y = x * v;
        res.z = x * v;
        res.w = x * v;
        
        return res;
    }
    
    S_Vec4 operator*(r32 v)
    {
        S_Vec4 res(0.0f);
        
        S_r32 _v = S_r32(v);
        
        res.x = x * _v;
        res.y = x * _v;
        res.z = x * _v;
        res.w = x * _v;
        
        return res;
    }
    
    S_Vec4 operator/(S_Vec4& v)
    {
        S_Vec4 res(0.0f);
        
        res.x = x / v.x;
        res.y = x / v.y;
        res.z = x / v.z;
        res.w = x / v.w;
        
        return res;
    }
    
    S_Vec4 operator/(S_r32 v)
    {
        S_Vec4 res(0.0f);
        
        res.x = x / v;
        res.y = x / v;
        res.z = x / v;
        res.w = x / v;
        
        return res;
    }
    
    S_Vec4 operator/(r32 v)
    {
        S_Vec4 res(0.0f);
        
        S_r32 _v = S_r32(v);
        
        res.x = x / _v;
        res.y = x / _v;
        res.z = x / _v;
        res.w = x / _v;
        
        return res;
    }
};

b32 all_zero(S_r32 v)
{
    return false;
}

using S_Rgba = S_Vec4;

// Extra math

S_Vec3 random_direction()
{
    S_Vec3 result(0.0f);
    
    result.x = _mm_set_ps((rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f);
    result.y = _mm_set_ps((rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f);
    result.z = _mm_set_ps((rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f);
    
    return result;
}

namespace math
{
    S_r32 lerp(S_r32 a, S_r64 t, S_r32 b)
    {
        S_r32 res(0.0f);
        
        S_r64 min = simd_min(1.0, t);
        S_r32 inverse_min = 1.0f - min;
        S_r32 a_times_inverse = inverse_min * a;
        
        res = a_times_inverse + (t * b);
        return res;
    }
    
    S_r64 lerp(S_r64 a, S_r64 t, S_r64 b)
    {
        S_r64 res(0.0);
        
        S_r64 min = simd_min(1.0, t);
        S_r64 inverse_min = 1.0 - min;
        S_r64 a_times_inverse = inverse_min * a;
        
        res = a_times_inverse + (t * b);
        return res;
    }
    
    S_Vec2 lerp(S_Vec2 a, S_r64 t, S_Vec2 b)
    {
        S_Vec2 res(0.0f);
        res.x = lerp(a.x, t, b.x);
        res.y = lerp(a.y, t, b.y);
        
        return res;
    }
    
    S_Vec3 lerp(S_Vec3 a, S_r64 t, S_Vec3 b)
    {
        S_Vec3 res(0.0f);
        res.x = lerp(a.x, t, b.x);
        res.y = lerp(a.y, t, b.y);
        res.z = lerp(a.z, t, b.z);
        
        return res;
    }
    
    S_Vec4 lerp(S_Vec4 a, S_r64 t, S_Vec4 b)
    {
        S_Vec4 res(0.0f);
        res.x = lerp(a.x, t, b.x);
        res.y = lerp(a.y, t, b.y);
        res.z = lerp(a.z, t, b.z);
        res.w = lerp(a.w, t, b.w);
        
        return res;
    }
    
    S_r32 r_sqrt(S_r32 v)
    {
        return _mm_rsqrt_ps(v.p);
    }
    
    S_Vec3 normalize(S_Vec3 v)
    {
        auto rec_sqrt = r_sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        
        S_Vec3 res(0.0f);
        
        res.x = rec_sqrt * v.x;
        res.y = rec_sqrt * v.y;
        res.z = rec_sqrt * v.z;
        
        return res;
    }
    
    r32 sqrt(r32 val)
    {
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(val)));
    }
}

math::Vec2 to_vec2(S_Vec2 vec, i32 index)
{
    assert(index >= 0 && index <= 3);
    math::Vec2 res(0.0f);
    
    res.x = vec.x.e[index];
    res.y = vec.y.e[index];
    
    return res;
}

math::Vec3 to_vec3(S_Vec3 vec, i32 index)
{
    assert(index >= 0 && index <= 3);
    math::Vec3 res(0.0f);
    
    res.x = vec.x.e[index];
    res.y = vec.y.e[index];
    res.z = vec.z.e[index];
    
    return res;
}

math::Vec4 to_vec4(S_Vec4 vec, i32 index)
{
    assert(index >= 0 && index <= 3);
    math::Vec4 res(0.0f);
    
    res.x = vec.x.e[index];
    res.y = vec.y.e[index];
    res.z = vec.z.e[index];
    res.w = vec.w.e[index];
    
    return res;
}

// Memory stuff for SIMD
// Generally using _mm_malloc and _mm_free is good, since it makes it possible to pass in alignment parameters
// We have alignment in MemoryArenas 

#endif
