#ifndef SIMD_H
#define SIMD_H

union S_i32
{
    S_i32(i32 _p) 
    {
        p = _mm_set1_epi32(_p);
    }
    
    S_i32(i32 _a, i32 _b, i32 _c, i32 _d)
    {
        p = _mm_set_epi32(_a, _b, _c, _d);
    }
    
    __m128i p;
    i32 e[4];
};

S_i32 operator+ (S_i32 a, S_i32 b)
{
    S_i32 res(0);
    
    res.p = _mm_add_epi32(a.p, b.p);
    
    return res;
}

S_i32 operator+= (S_i32 a, S_i32 b)
{
    return a + b;
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

union S_r32
{
    S_r32(r32 _p) 
    {
        p = _mm_set1_ps(_p);
    }
    
    S_r32(r32 _a, r32 _b, r32 _c, r32 _d)
    {
        p = _mm_set_ps(_a, _b, _c, _d);
    }
    
    __m128 p;
    r32 e[4];
    
};

S_r32 operator+ (S_r32 a, S_r32 b)
{
    S_r32 res(0.0f);
    
    res.p = _mm_add_ps(a.p, b.p);
    
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

// Used to store 4 doubles in one SIMD constructions
union S_r64
{
    struct
    {
        union
        {
            __m128d upper_bits;
            r64 u_e[2];
        };
        union
        {
            __m128d lower_bits;
            r64 l_e[2];
        };
    };
    
    S_r64(r64 v)
    {
        upper_bits = _mm_set1_pd(v);
        lower_bits = _mm_set1_pd(v);
    }
    
    S_r64(r64 v1, r64 v2, r64 v3, r64 v4)
    {
        upper_bits = _mm_set_pd(v1, v2);
        lower_bits = _mm_set_pd(v3, v4);
    }
    
    S_r64& operator=(const S_r64& v)
    {
        upper_bits = _mm_set_pd(v.u_e[0], v.u_e[1]);
        lower_bits = _mm_set_pd(v.l_e[2], v.l_e[3]);
        
        return *this;
    }
    
    S_r64& operator=(const r64& v)
    {
        upper_bits = _mm_set_pd(v, v);
        lower_bits = _mm_set_pd(v, v);
        
        return *this;
    }
};


S_r64 operator+ (S_r64 a, S_r64 b)
{
    S_r64 res(0.0);
    
    res.upper_bits = _mm_add_pd(a.upper_bits, b.upper_bits);
    res.lower_bits = _mm_add_pd(a.lower_bits, b.lower_bits);
    
    return res;
}

S_r64 operator+= (S_r64 a, S_r64 b)
{
    return a + b;
}

S_r64 operator- (S_r64 a, S_r64 b)
{
    S_r64 res(0.0);
    
    res.upper_bits = _mm_sub_pd(a.upper_bits, b.upper_bits);
    res.lower_bits = _mm_sub_pd(a.lower_bits, b.lower_bits);
    
    return res;
}

S_r64 operator-= (S_r64 a, S_r64 b)
{
    return a - b;
}

S_r64 operator* (S_r64 a, S_r64 b)
{
    S_r64 res(0.0);
    
    res.upper_bits = _mm_mul_pd(a.upper_bits, b.upper_bits);
    res.lower_bits = _mm_mul_pd(a.lower_bits, b.lower_bits);
    
    return res;
}

S_r64 operator*= (S_r64 a, S_r64 b)
{
    return a * b;
}

S_r64 operator/ (S_r64 a, S_r64 b)
{
    S_r64 res(0.0);
    
    res.upper_bits = _mm_div_pd(a.upper_bits, b.upper_bits);
    res.lower_bits = _mm_div_pd(a.lower_bits, b.lower_bits);
    
    return res;
}

S_r64 operator/= (S_r64 a, S_r64 b)
{
    return a / b;
}

union S_Vec2
{
    S_r32 x;
    S_r32 y;
    
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
    S_r32 x;
    S_r32 y;
    S_r32 z;
    
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
        
        
        res.x = x - a.x;
        res.y = y - a.y;
        res.z = z - a.z;
        
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


union S_Vec4
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
        w = S_r32(vec.z);
    }
    
    S_Vec4(math::Vec4 v1, math::Vec4 v2, math::Vec4 v3, math::Vec4 v4)
    {
        x = S_r32(v1.x, v2.x, v3.x, v4.x);
        y = S_r32(v1.y, v2.y, v3.y, v4.y);
        z = S_r32(v1.z, v2.z, v3.z, v4.z);
        w = S_r32(v1.w, v2.w, v3.w, v4.w);
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

b32 any_nz(S_r64 v)
{
    v.upper_bits = _mm_set1_pd(5.0);
    
    __m128d upper_cmp = _mm_cmp_pd(v.upper_bits, _mm_setzero_pd(), _CMP_EQ_OQ);
    __m128d lower_cmp = _mm_cmp_pd(v.lower_bits, _mm_setzero_pd(), _CMP_EQ_OQ);
    
    upper_cmp = _mm_cmp_pd(upper_cmp, _mm_setzero_pd(), _CMP_NLT_US);
    auto i_upper = _mm_srl_epi32(_mm_castpd_si128(upper_cmp), _mm_set1_epi32(31));
    
    i_upper = _mm_srl_epi32(_mm_castpd_si128(upper_cmp), _mm_set1_epi32(31));
    i_upper = _mm_xor_si128(i_upper, _mm_set1_epi32(1));
    
    
    lower_cmp = _mm_cmp_pd(lower_cmp, _mm_setzero_pd(), _CMP_NLT_US);
    auto i_lower = _mm_srl_epi32(_mm_castpd_si128(lower_cmp), _mm_set1_epi32(31));
    
    i_lower = _mm_srl_epi32(_mm_castpd_si128(lower_cmp), _mm_set1_epi32(31));
    i_lower = _mm_xor_si128(i_lower, _mm_set1_epi32(1));
    
    i32 upper_mask = (_mm_movemask_epi8(i_upper));
    i32 lower_mask = (_mm_movemask_epi8(i_lower));
    return upper_mask != 0xff && lower_mask != 0xff;
}

using S_Rgba = S_Vec4;

#endif
