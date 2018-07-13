#ifndef SIMD_H
#define SIMD_H


using S_i32 = __m128i;


S_i32 operator+ (S_i32 a, S_i32 b)
{
    return _mm_add_epi32(a, b);
}

S_i32 operator+= (S_i32 a, S_i32 b)
{
    return a + b;
}

S_i32 operator- (S_i32 a, S_i32 b)
{
    return _mm_sub_epi32(a, b);
}

S_i32 operator-= (S_i32 a, S_i32 b)
{
    return a - b;
}

S_i32 operator* (S_i32 a, S_i32 b)
{
    return _mm_mul_epi32(a, b);
}

S_i32 operator*= (S_i32 a, S_i32 b)
{
    return a * b;
}

S_i32 operator/ (S_i32 a, S_i32 b)
{
    assert(false);
    return {};
}

S_i32 operator/= (S_i32 a, S_i32 b)
{
    assert(false);
    return {};
}

using S_r32 = __m128;

S_r32 operator+ (S_r32 a, S_r32 b)
{
    return _mm_add_ps(a, b);
}

S_r32 operator- (S_r32 a, S_r32 b)
{
    return _mm_sub_ps(a, b);
}

S_r32 operator* (S_r32 a, S_r32 b)
{
    return _mm_mul_ps(a, b);
}

S_r32 operator/ (S_r32 a, S_r32 b)
{
    return _mm_div_ps(a, b);
}

// Used to store 4 doubles in one SIMD constructions
union S_r64
{
    __m128d upper_bits;
    __m128d lower_bits;
    
    S_r64(r64 v)
    {
        upper_bits = _mm_set1_pd(v);
        lower_bits = _mm_set1_pd(v);
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
        x = _mm_set1_ps(_x);
        y = _mm_set1_ps(_y);
    }
    
    S_Vec2(r32 v)
    {
        x = _mm_set1_ps(v);
        y = _mm_set1_ps(v);
    }
    
    S_Vec2(math::Vec2 vec)
    {
        x = _mm_set1_ps(vec.x);
        y = _mm_set1_ps(vec.y);
    }
    
    S_Vec2 operator+(S_Vec2& v)
    {
        S_Vec2 res(0.0f);
        
        res.x = _mm_add_ps(x, v.x);
        res.y = _mm_add_ps(y, v.y);
        
        return res;
    }
    
    S_Vec2 operator+(S_r32 v)
    {
        S_Vec2 res(0.0f);
        
        res.x = _mm_add_ps(x, v);
        res.y = _mm_add_ps(y, v);
        
        return res;
    }
    
    S_Vec2 operator+(r32 v)
    {
        S_Vec2 res(0.0f);
        
        S_r32 _v = _mm_set1_ps(v);
        
        res.x = _mm_add_ps(x, _v);
        res.y = _mm_add_ps(y, _v);
        
        return res;
    }
    
    S_Vec2 operator-(S_Vec2& v)
    {
        S_Vec2 res(0.0f);
        
        res.x = _mm_sub_ps(x, v.x);
        res.y = _mm_sub_ps(y, v.y);
        
        return res;
    }
    
    S_Vec2 operator-(S_r32 v)
    {
        S_Vec2 res(0.0f);
        
        res.x = _mm_sub_ps(x, v);
        res.y = _mm_sub_ps(y, v);
        
        return res;
    }
    
    S_Vec2 operator-(r32 v)
    {
        S_Vec2 res(0.0f);
        
        S_r32 _v = _mm_set1_ps(v);
        
        res.x = _mm_sub_ps(x, _v);
        res.y = _mm_sub_ps(y, _v);
        
        return res;
    }
    
    
    S_Vec2 operator*(S_Vec2& v)
    {
        S_Vec2 res(0.0f);
        
        res.x = _mm_mul_ps(x, v.x);
        res.y = _mm_mul_ps(y, v.y);
        
        return res;
    }
    
    S_Vec2 operator*(S_r32 v)
    {
        S_Vec2 res(0.0f);
        
        res.x = _mm_mul_ps(x, v);
        res.y = _mm_mul_ps(y, v);
        
        return res;
    }
    
    S_Vec2 operator*(r32 v)
    {
        S_Vec2 res(0.0f);
        
        S_r32 _v = _mm_set1_ps(v);
        
        res.x = _mm_mul_ps(x, _v);
        res.y = _mm_mul_ps(y, _v);
        
        return res;
    }
    
    S_Vec2 operator/(S_Vec2& v)
    {
        S_Vec2 res(0.0f);
        
        res.x = _mm_div_ps(x, v.x);
        res.y = _mm_div_ps(y, v.y);
        
        return res;
    }
    
    S_Vec2 operator/(S_r32 v)
    {
        S_Vec2 res(0.0f);
        
        res.x = _mm_div_ps(x, v);
        res.y = _mm_div_ps(y, v);
        
        return res;
    }
    
    S_Vec2 operator/(r32 v)
    {
        S_Vec2 res(0.0f);
        
        S_r32 _v = _mm_set1_ps(v);
        
        res.x = _mm_div_ps(x, _v);
        res.y = _mm_div_ps(y, _v);
        
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
        x = _mm_set1_ps(_x);
        y = _mm_set1_ps(_y);
        z = _mm_set1_ps(_z);
    }
    
    S_Vec3(r32 v)
    {
        x = _mm_set1_ps(v);
        y = _mm_set1_ps(v);
        z = _mm_set1_ps(v);
    }
    
    S_Vec3(math::Vec3 vec)
    {
        x = _mm_set1_ps(vec.x);
        y = _mm_set1_ps(vec.y);
        z = _mm_set1_ps(vec.z);
    }
    
    S_Vec3 operator+(S_Vec3& v)
    {
        S_Vec3 res(0.0f);
        
        res.x = _mm_add_ps(x, v.x);
        res.y = _mm_add_ps(y, v.y);
        res.z = _mm_add_ps(z, v.z);
        
        return res;
    }
    
    S_Vec3 operator+(S_r32 a)
    {
        S_Vec3 res(0.0f);
        
        res.x = _mm_add_ps(x, a);
        res.y = _mm_add_ps(y, a);
        res.z = _mm_add_ps(z, a);
        
        return res;
    }
    
    S_Vec3 operator+(r32 a)
    {
        S_Vec3 res(0.0f);
        
        S_r32 _a = _mm_set1_ps(a);
        
        res.x = _mm_add_ps(x, _a);
        res.y = _mm_add_ps(y, _a);
        res.z = _mm_add_ps(z, _a);
        
        return res;
    }
    
    
    S_Vec3 operator-(S_Vec3& a)
    {
        S_Vec3 res(0.0f);
        
        res.x = _mm_sub_ps(x, a.x);
        res.y = _mm_sub_ps(y, a.y);
        res.z = _mm_sub_ps(z, a.z);
        
        return res;
    }
    
    
    S_Vec3 operator-(S_r32 a)
    {
        S_Vec3 res(0.0f);
        
        res.x = _mm_sub_ps(x, a);
        res.y = _mm_sub_ps(y, a);
        res.z = _mm_sub_ps(z, a);
        
        return res;
    }
    
    S_Vec3 operator-(r32 a)
    {
        S_Vec3 res(0.0f);
        
        S_r32 _a = _mm_set1_ps(a);
        
        res.x = _mm_sub_ps(x, _a);
        res.y = _mm_sub_ps(y, _a);
        res.z = _mm_sub_ps(z, _a);
        
        return res;
    }
    
    
    S_Vec3 operator*(S_Vec3& a)
    {
        S_Vec3 res(0.0f);
        
        res.x = _mm_mul_ps(x, a.x);
        res.y = _mm_mul_ps(y, a.y);
        res.z = _mm_mul_ps(z, a.z);
        
        return res;
    }
    
    S_Vec3 operator*(S_r32 a)
    {
        S_Vec3 res(0.0f);
        
        res.x = _mm_mul_ps(x, a);
        res.y = _mm_mul_ps(y, a);
        res.z = _mm_mul_ps(z, a);
        
        return res;
    }
    
    
    S_Vec3 operator*(r32 a)
    {
        S_Vec3 res(0.0f);
        
        S_r32 _a = _mm_set1_ps(a);
        
        res.x = _mm_mul_ps(x, _a);
        res.y = _mm_mul_ps(y, _a);
        res.z = _mm_mul_ps(z, _a);
        
        return res;
    }
    
    S_Vec3 operator/(S_Vec3& a)
    {
        S_Vec3 res(0.0f);
        
        res.x = _mm_div_ps(x, a.x);
        res.y = _mm_div_ps(y, a.y);
        res.z = _mm_div_ps(z, a.z);
        
        return res;
    }
    
    S_Vec3 operator/(S_r32 a)
    {
        S_Vec3 res(0.0f);
        
        res.x = _mm_div_ps(x, a);
        res.y = _mm_div_ps(y, a);
        res.z = _mm_div_ps(z, a);
        
        return res;
    }
    
    S_Vec3 operator/(r32 a)
    {
        S_Vec3 res(0.0f);
        
        S_r32 _a = _mm_set1_ps(a);
        
        res.x = _mm_div_ps(x, _a);
        res.y = _mm_div_ps(y, _a);
        res.z = _mm_div_ps(z, _a);
        
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
        x = _mm_set1_ps(_x);
        y = _mm_set1_ps(_y);
        z = _mm_set1_ps(_z);
        w = _mm_set1_ps(_w);
    }
    
    S_Vec4(r32 v)
    {
        x = _mm_set1_ps(v);
        y = _mm_set1_ps(v);
        z = _mm_set1_ps(v);
        w = _mm_set1_ps(v);
    }
    
    S_Vec4(math::Vec4 vec)
    {
        x = _mm_set1_ps(vec.x);
        y = _mm_set1_ps(vec.y);
        z = _mm_set1_ps(vec.z);
        w = _mm_set1_ps(vec.w);
    }
    
    S_Vec4 operator+(S_Vec4& v)
    {
        S_Vec4 res(0.0f);
        
        res.x = _mm_add_ps(x, v.x);
        res.y = _mm_add_ps(y, v.y);
        res.z = _mm_add_ps(z, v.z);
        res.w = _mm_add_ps(w, v.w);
        
        return res;
    }
    
    S_Vec4 operator+(S_r32 v)
    {
        S_Vec4 res(0.0f);
        
        res.x = _mm_add_ps(x, v);
        res.y = _mm_add_ps(y, v);
        res.z = _mm_add_ps(z, v);
        res.w = _mm_add_ps(w, v);
        
        return res;
    }
    
    S_Vec4 operator+(r32 v)
    {
        S_Vec4 res(0.0f);
        
        S_r32 _v = _mm_set1_ps(v);
        
        res.x = _mm_add_ps(x, _v);
        res.y = _mm_add_ps(y, _v);
        res.z = _mm_add_ps(z, _v);
        res.w = _mm_add_ps(w, _v);
        
        return res;
    }
    
    
    S_Vec4 operator-(S_Vec4& v)
    {
        S_Vec4 res(0.0f);
        
        res.x = _mm_sub_ps(x, v.x);
        res.y = _mm_sub_ps(y, v.y);
        res.z = _mm_sub_ps(z, v.z);
        res.w = _mm_sub_ps(w, v.w);
        
        return res;
    }
    
    S_Vec4 operator-(S_r32 v)
    {
        S_Vec4 res(0.0f);
        
        res.x = _mm_sub_ps(x, v);
        res.y = _mm_sub_ps(y, v);
        res.z = _mm_sub_ps(z, v);
        res.w = _mm_sub_ps(w, v);
        
        return res;
    }
    
    S_Vec4 operator-(r32 v)
    {
        S_Vec4 res(0.0f);
        
        S_r32 _v = _mm_set1_ps(v);
        
        res.x = _mm_sub_ps(x, _v);
        res.y = _mm_sub_ps(y, _v);
        res.z = _mm_sub_ps(z, _v);
        res.w = _mm_sub_ps(w, _v);
        
        return res;
    }
    
    S_Vec4 operator*(S_Vec4& v)
    {
        S_Vec4 res(0.0f);
        
        res.x = _mm_mul_ps(x, v.x);
        res.y = _mm_mul_ps(y, v.y);
        res.z = _mm_mul_ps(z, v.z);
        res.w = _mm_mul_ps(w, v.w);
        
        return res;
    }
    
    S_Vec4 operator*(S_r32 v)
    {
        S_Vec4 res(0.0f);
        
        res.x = _mm_mul_ps(x, v);
        res.y = _mm_mul_ps(y, v);
        res.z = _mm_mul_ps(z, v);
        res.w = _mm_mul_ps(w, v);
        
        return res;
    }
    
    S_Vec4 operator*(r32 v)
    {
        S_Vec4 res(0.0f);
        
        S_r32 _v = _mm_set1_ps(v);
        
        res.x = _mm_mul_ps(x, _v);
        res.y = _mm_mul_ps(y, _v);
        res.z = _mm_mul_ps(z, _v);
        res.w = _mm_mul_ps(w, _v);
        
        return res;
    }
    
    S_Vec4 operator/(S_Vec4& v)
    {
        S_Vec4 res(0.0f);
        
        res.x = _mm_div_ps(x, v.x);
        res.y = _mm_div_ps(y, v.y);
        res.z = _mm_div_ps(z, v.z);
        res.w = _mm_div_ps(w, v.w);
        
        return res;
    }
    
    S_Vec4 operator/(S_r32 v)
    {
        S_Vec4 res(0.0f);
        
        res.x = _mm_div_ps(x, v);
        res.y = _mm_div_ps(y, v);
        res.z = _mm_div_ps(z, v);
        res.w = _mm_div_ps(w, v);
        
        return res;
    }
    
    S_Vec4 operator/(r32 v)
    {
        S_Vec4 res(0.0f);
        
        S_r32 _v = _mm_set1_ps(v);
        
        res.x = _mm_div_ps(x, _v);
        res.y = _mm_div_ps(y, _v);
        res.z = _mm_div_ps(z, _v);
        res.w = _mm_div_ps(w, _v);
        
        return res;
    }
};

using S_Rgba = S_Vec4;


#endif

