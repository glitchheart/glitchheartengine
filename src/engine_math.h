#ifndef MATH_H
#define MATH_H

#ifdef __linux
#include <math.h>
#else
#include <cmath>
#endif

namespace math
{
    inline i32 clamp(i32 minimum, i32 value, i32 maximum)
    {
        i32 result = MAX(minimum, MIN(value,maximum));
        return result;
    }
    
    inline r32 clamp(r32 minimum, r32 value, r32 maximum)
    {
        return MAX(minimum, MIN(value,maximum));
    }
    
    inline r64 clamp(r64 minimum, r64 value, r64 maximum)
    {
        return MAX(minimum, MIN(value,maximum));
    }

    union Vec2
    {
	struct
	{
	    r32 x;
	    r32 y;
	};
	struct
	{
	    r32 u;
	    r32 v;
	};
	r32 e[2];
        
    Vec2(r32 x, r32 y) : x(x), y(y) {}
	Vec2() : x(0.0f), y(0.0f) {}
    Vec2(r32 i) : e{i,i} {}
    Vec2(r32 i[2]) : e{i[0],i[1]} {}
        Vec2(const Vec2& o) = default;
    Vec2(i32 x, i32 y) : x((r32)x), y((r32)y) {}
    Vec2(r64 x, r64 y) : x((r32)x), y((r32)y) {}
    Vec2(r32 x, r64 y) : x(x), y((r32)y) {}
    Vec2(r32 x, i32 y) : x(x), y((r32)y) {}
    Vec2(i32 x, r32 y) : x((r32)x), y(y) {}
    Vec2(i32 x, r64 y) : x((r32)x), y((r32)y) {}
    Vec2(r64 x, i32 y) : x((r32)x), y((r32)y) {}
    Vec2(r64 x, r32 y) : x((r32)x), y(y) {}
        
        Vec2& operator=(const Vec2& v) = default;
        
        r32 operator[](i32 i)
        {
            return this->e[i];
        }
        
        inline Vec2 operator* (Vec2 o)
        {
            Vec2 result(*this);
            result.x *= o.x;
            result.y *= o.y;
            return result;
        }
        
        inline Vec2 operator+ (Vec2 o)
        {
            Vec2 result(*this);
            result.x += o.x;
            result.y += o.y;
            return result;
        }
        
        inline void operator*= (Vec2 o)
        {
            this->x *= o.x;
            this->y *= o.y;
        }
        
        inline void operator+= (Vec2 o)
        {
            this->x += o.x;
            this->y += o.y;
        }
        
        inline Vec2 operator+ (r32 s)
        {
            Vec2 result(*this);
            result.x += s;
            result.y += s;
            return result;
        }
        
        inline Vec2 operator* (r32 s)
        {
            Vec2 result(*this);
            result.x *= s;
            result.y *= s;
            return result;
        }
        
        inline Vec2 operator/ (r32 s)
        {
            Vec2 result(*this);
            result.x /= s;
            result.y /= s;
            return result;
        }
        
        inline void operator+= (r32 s)
        {
            this->x += s;
            this->y += s;
        }
        
        inline void operator*= (r32 s)
        {
            this->x *= s;
            this->y *= s;
        }
        
        inline void operator/= (r32 s)
        {
            this->x /= s;
            this->y /= s;
        }
        
        inline void operator-= (r32 s)
        {
            this->x -= s;
            this->y -= s;
        }
        
        inline Vec2 operator- (Vec2 o)
        {
            Vec2 result(*this);
            result.x -= o.x;
            result.y -= o.y;
            return result;
        }
        
        inline void operator-= (Vec2 o)
        {
            this->x -= o.x;
            this->y -= o.y;
        }
        
        inline Vec2 operator- (r32 s)
        {
            Vec2 result(*this);
            result.x -= s;
            result.y -= s;
            return result;
        }
        
        inline Vec2 operator/ (Vec2 o)
        {
            Vec2 result(*this);
            result.x /= o.x;
            result.y /= o.y;
            return result;
        }
        
        inline void operator/= (Vec2 o)
        {
            this->x /= o.x;
            this->y /= o.y;
        }
    };
    
    union Vec3
    {
        struct 
        {
            union
            {
                Vec2 xy;
                struct
                {
                    r32 x, y;
                };
            };
            r32 z;
        };
        struct 
        {
            union
            {
                Vec2 rg;
                struct
                {
                    r32 r, g;
                };
            };
            r32 b;
        };
        
        r32 e[3];
        Vec3(r32 x, r32 y, r32 z) : x(x), y(y), z(z) {}
        Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vec3(r32 i) : e{i,i,i} {}
        Vec3(r32 i[3]) : e{i[0],i[1], i[2]} {}
        Vec3(const Vec3& o) = default;
        Vec3(r64 x, r64 y, r64 z) : x((r32)x), y((r32)y), z((r32)z) {}
        Vec3(r64 x, i32 y, r64 z) : x((r32)x), y((r32)y), z((r32)z) {}
        Vec3(i32 x, i32 y, i32 z) : x((r32)x), y((r32)y), z((r32)z) {}
        Vec3(i32 x, r32 y, i32 z) : x((r32)x), y(y), z((r32)z) {}
        Vec3(r64 x, r64 y, i32 z) : x((r32)x), y((r32)y), z((r32)z) {}
        Vec3(r32 x, r32 y, i32 z) : x(x), y(y), z((r32)z) {}
        Vec3(r32 x, i32 y, i32 z) : x(x), y((r32)y), z((r32)z) {}
        Vec3(i32 x, i32 y, r32 z) : x((r32)x), y((r32)y), z(z) {}
        Vec3(r32 x, r32 y, r64 z) : x(x), y(y), z((r32)z) {}
        Vec3(r32 x, i32 y, r32 z) : x(x), y((r32)y), z(z) {}
        Vec3(r64 x, r32 y, r64 z) : x((r32)x), y(y), z((r32)z) {}
        Vec3(r64 x, r32 y, r32 z) : x((r32)x), y(y), z(z) {}
        Vec3(Vec2 v, r32 z) : x(v.x), y(v.y), z(z) {}
        
        Vec3& operator=(const Vec3& v) = default;
        
        r32& operator[](i32 i)
        {
            return this->e[i];
        }
        
        inline Vec3& operator= (Vec2 o)
        {
            x = o.x;
            y = o.y;
            z = 0.0f;
            return *this;
        }
        
        inline Vec3 operator-()
        {
            Vec3 result(1.0f);
            result.x = -this->x;
            result.y = -this->y;
            result.z = -this->z;
            return result;
        }
        
        inline Vec3 operator* (Vec3 o)
        {
            Vec3 result(*this);
            result.x *= o.x;
            result.y *= o.y;
            result.z *= o.z;
            return result;
        }
        
        inline Vec3 operator+ (Vec3 o)
        {
            Vec3 result(*this);
            result.x += o.x;
            result.y += o.y;
            result.z += o.z;
            return result;
        }
        
        inline void operator*= (Vec3 o)
        {
            this->x *= o.x;
            this->y *= o.y;
            this->z *= o.z;
        }
        
        inline void operator+= (Vec3 o)
        {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;
        }
        
        inline Vec3 operator+ (r32 s)
        {
            Vec3 result(*this);
            result.x += s;
            result.y += s;
            result.z += s;
            return result;
        }
        
        inline Vec3 operator* (r32 s)
        {
            Vec3 result(*this);
            result.x *= s;
            result.y *= s;
            result.z *= s;
            return result;
        }
        
        inline Vec3 operator/ (r32 s)
        {
            Vec3 result(*this);
            result.x /= s;
            result.y /= s;
            result.z /= s;
            return result;
        }
        
        inline void operator+= (r32 s)
        {
            this->x += s;
            this->y += s;
            this->z += s;
        }
        
        inline void operator*= (r32 s)
        {
            this->x *= s;
            this->y *= s;
            this->z *= s;
        }
        
        inline void operator/= (r32 s)
        {
            this->x /= s;
            this->y /= s;
            this->z /= s;
        }
        
        inline void operator-= (r32 s)
        {
            this->x -= s;
            this->y -= s;
            this->z -= s;
        }
        
        inline Vec3 operator- (Vec3 o)
        {
            Vec3 result(*this);
            result.x -= o.x;
            result.y -= o.y;
            result.z -= o.z;
            return result;
        }
        
        inline void operator-= (Vec3 o)
        {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;
        }
        
        inline Vec3 operator- (r32 s)
        {
            Vec3 result(*this);
            result.x -= s;
            result.y -= s;
            result.z -= s;
            return result;
        }
        
        inline Vec3 operator/ (Vec3 o)
        {
            Vec3 result(*this);
            result.x /= o.x;
            result.y /= o.y;
            result.z /= o.z;
            return result;
        }
        
        inline void operator/= (Vec3 o)
        {
            this->x /= o.x;
            this->y /= o.y;
            this->z /= o.z;
        }
    };
    
    union Vec4
    {
        struct 
        {
            union
            {
                Vec3 xyz;
                struct
                {
                    r32 x, y, z;
                };
                struct
                {
                    
                    Vec2 xy;
                };
            };
            r32 w;
            
        };
        struct 
        {
            union
            {
                Vec3 rgb;
                struct
                {
                    r32 r, g, b;
                };
            };
            r32 a;
        };
        r32 e[4];
        
        Vec4(r32 x, r32 y, r32 z, r32 w) : x(x), y(y), z(z), w(w) {}
        Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        Vec4(r32 i) : e{i,i,i,i} {}
        Vec4(r32 i[4]) : e{i[0], i[1], i[2], i[3]} {}
        Vec4(const Vec4& o) = default;
        
        Vec4(i32 x, i32 y, i32 z, i32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        Vec4(r32 x, r32 y, r32 z, i32 w) : 
        x(x), y(y), z(z), w((r32)w) {}
        
        Vec4(r64 x, r64 y, r64 z, r64 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        Vec4(r64 x, r64 y, r64 z, i32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        Vec4(r32 x, i32 y, r32 z, i32 w) : 
        x(x), y((r32)y), z(z), w((r32)w) {}
        
        Vec4(i32 x, r64 y, i32 z, i32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        Vec4(r64 x, i32 y, i32 z, i32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        Vec4(i32 x, i32 y, i32 z, r64 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        Vec4(r32 x, i32 y, i32 z, i32 w) : 
        x(x), y((r32)y), z((r32)z), w((r32)w) {}
        
        Vec4(i32 x, i32 y, i32 z, r32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w(w) {}
        
        Vec4(r64 x, r64 y, i32 z, r64 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        Vec4(Vec3 o, r32 w) : x(o.x), y(o.y), z(o.z), w(w) {}
        
        Vec4(Vec2 v, r32 z, r32 w) : x(v.x), y(v.y), z(z), w(w) {} 
        
        Vec4& operator=(const Vec4& v) = default;
        
        r32 operator[](i32 i)
        {
            return this->e[i];
        }
        
        inline Vec4 operator* (Vec4 o)
        {
            Vec4 result(*this);
            result.x *= o.x;
            result.y *= o.y;
            result.z *= o.z;
            result.w *= o.w;
            return result;
        }
        
        inline Vec4 operator+ (Vec4 o)
        {
            Vec4 result(*this);
            result.x += o.x;
            result.y += o.y;
            result.z += o.z;
            result.w += o.w;
            return result;
        }
        
        inline void operator*= (Vec4 o)
        {
            this->x *= o.x;
            this->y *= o.y;
            this->z *= o.z;
            this->w *= o.w;
        }
        
        inline void operator+= (Vec4 o)
        {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;
            this->w += o.w;
        }
        
        inline Vec4 operator+ (r32 s)
        {
            Vec4 result(*this);
            result.x += s;
            result.y += s;
            result.z += s;
            result.w += s;
            return result;
        }
        
        inline Vec4 operator* (r32 s)
        {
            Vec4 result(*this);
            result.x *= s;
            result.y *= s;
            result.z *= s;
            result.w *= s;
            return result;
        }
        
        inline Vec4 operator/ (r32 s)
        {
            Vec4 result(*this);
            result.x /= s;
            result.y /= s;
            result.z /= s;
            result.w /= s;
            return result;
        }
        
        inline void operator+= (r32 s)
        {
            this->x += s;
            this->y += s;
            this->z += s;
            this->w += s;
        }
        
        inline void operator*= (r32 s)
        {
            this->x *= s;
            this->y *= s;
            this->z *= s;
            this->w *= s;
        }
        
        inline void operator/= (r32 s)
        {
            this->x /= s;
            this->y /= s;
            this->z /= s;
            this->w /= s;
        }
        
        inline void operator-= (r32 s)
        {
            this->x -= s;
            this->y -= s;
            this->z -= s;
            this->w -= s;
        }
        
        inline Vec4 operator- (Vec4 o)
        {
            Vec4 result(*this);
            result.x -= o.x;
            result.y -= o.y;
            result.z -= o.z;
            result.w -= o.w;
            return result;
        }
        
        inline void operator-= (Vec4 o)
        {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;
            this->w -= o.w;
        }
        
        inline Vec4 operator- (r32 s)
        {
            Vec4 result(*this);
            result.x -= s;
            result.y -= s;
            result.z -= s;
            result.w -= s;
            return result;
        }
        
        inline Vec4 operator/ (Vec4 o)
        {
            Vec4 result(*this);
            result.x /= o.x;
            result.y /= o.y;
            result.z /= o.z;
            result.w /= o.w;
            return result;
        }
        
        inline void operator/= (Vec4 o)
        {
            this->x /= o.x;
            this->y /= o.y;
            this->z /= o.z;
            this->w /= o.w;
        }
    };
    
    
    using Rgb = Vec3;
    using Rgba = Vec4;
    
    union Vec2i
    {
        struct
        {
            i32 x,y;
        };
        i32 e[2];
        Vec2i(i32 x, i32 y) : x(x), y(y){}
        Vec2i(r32 x, r32 y) : x((i32)x), y((i32)y){}
        Vec2i() : x(0),y(0) {}
        Vec2i(i32 i) : x(i), y(i) {}
        Vec2i(i32 i[2]) : e{i[0],i[1]} {}
        Vec2i(Vec2 v) : x((i32)v.x), y((i32)v.y) {}
        
        inline b32 operator== (Vec2i o)
        {
            return this->x == o.x && this->y == o.y;
        }
        
        inline Vec2i operator* (Vec2i o)
        {
            Vec2i result(*this);
            result.x *= o.x;
            result.y *= o.y;
            return result;
        }
        
        inline Vec2i operator+ (Vec2i o)
        {
            Vec2i result(*this);
            result.x += o.x;
            result.y += o.y;
            return result;
        }
        
        inline void operator*= (Vec2i o)
        {
            this->x *= o.x;
            this->y *= o.y;
        }
        
        inline void operator+= (Vec2i o)
        {
            this->x += o.x;
            this->y += o.y;
        }
        
        inline Vec2i operator+ (i32 s)
        {
            Vec2i result(*this);
            result.x += s;
            result.y += s;
            return result;
        }
        
        inline Vec2i operator* (i32 s)
        {
            Vec2i result(*this);
            result.x *= s;
            result.y *= s;
            return result;
        }
        
        inline Vec2i operator/ (i32 s)
        {
            Vec2i result(*this);
            result.x /= s;
            result.y /= s;
            return result;
        }
        
        inline void operator+= (i32 s)
        {
            this->x += s;
            this->y += s;
        }
        
        inline void operator*= (i32 s)
        {
            this->x *= s;
            this->y *= s;
        }
        
        inline void operator/= (i32 s)
        {
            this->x /= s;
            this->y /= s;
        }
        
        inline void operator-= (i32 s)
        {
            this->x -= s;
            this->y -= s;
        }
        
        inline Vec2i operator- (Vec2i o)
        {
            Vec2i result(*this);
            result.x -= o.x;
            result.y -= o.y;
            return result;
        }

        inline Vec2i operator- ()
        {
            Vec2i result(*this);
            result.x = -result.x;
            result.y = -result.y;
            return result;
        }
        
        inline void operator-= (Vec2i o)
        {
            this->x -= o.x;
            this->y -= o.y;
        }
        
        inline Vec2i operator- (i32 s)
        {
            Vec2i result(*this);
            result.x -= s;
            result.y -= s;
            return result;
        }
        
        inline Vec2i operator/ (Vec2i o)
        {
            Vec2i result(*this);
            result.x /= o.x;
            result.y /= o.y;
            return result;
        }
        
        inline void operator/= (Vec2i o)
        {
            this->x /= o.x;
            this->y /= o.y;
        }
    };
    
    union Vec3i
    {
        struct
        {
            i32 x,y,z;
        };
        i32 e[3];
        Vec3i(i32 x, i32 y, i32 z) : x(x), y(y), z(z){}
        Vec3i() : x(0), y(0), z(0) {}
        Vec3i(i32 i) : x(i), y(i), z(i) {}
        Vec3i(i32 i[3]) : e{i[0], i[1], i[2]} {}
        Vec3i(Vec3 v) : e{(i32)v.e[0], (i32)v.e[1], (i32)v.e[2]} {}
        
        inline Vec3i operator* (Vec3i o)
        {
            Vec3i result(*this);
            result.x *= o.x;
            result.y *= o.y;
            result.z *= o.z;
            return result;
        }
        
        inline Vec3i operator+ (Vec3i o)
        {
            Vec3i result(*this);
            result.x += o.x;
            result.y += o.y;
            result.z += o.z;
            return result;
        }
        
        inline void operator*= (Vec3i o)
        {
            this->x *= o.x;
            this->y *= o.y;
            this->z *= o.z;
        }
        
        inline void operator+= (Vec3i o)
        {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;
        }
        
        inline Vec3i operator+ (i32 s)
        {
            Vec3i result(*this);
            result.x += s;
            result.y += s;
            result.z += s;
            return result;
        }
        
        inline Vec3i operator* (i32 s)
        {
            Vec3i result(*this);
            result.x *= s;
            result.y *= s;
            result.z *= s;
            return result;
        }
        
        inline Vec3i operator/ (i32 s)
        {
            Vec3i result(*this);
            result.x /= s;
            result.y /= s;
            result.z /= s;
            return result;
        }
        
        inline void operator+= (i32 s)
        {
            this->x += s;
            this->y += s;
            this->z += s;
        }
        
        inline void operator*= (i32 s)
        {
            this->x *= s;
            this->y *= s;
            this->z *= s;
        }
        
        inline void operator/= (i32 s)
        {
            this->x /= s;
            this->y /= s;
            this->z /= s;
        }
        
        inline void operator-= (i32 s)
        {
            this->x -= s;
            this->y -= s;
            this->z -= s;
        }
        
        inline Vec3i operator- (Vec3i o)
        {
            Vec3i result(*this);
            result.x -= o.x;
            result.y -= o.y;
            result.z -= o.z;
            return result;
        }
        
        inline void operator-= (Vec3i o)
        {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;
        }
        
        inline Vec3i operator- (i32 s)
        {
            Vec3i result(*this);
            result.x -= s;
            result.y -= s;
            result.z -= s;
            return result;
        }
        
        inline Vec3i operator/ (Vec3i o)
        {
            Vec3i result(*this);
            result.x /= o.x;
            result.y /= o.y;
            result.z /= o.z;
            return result;
        }
        
        inline void operator/= (Vec3i o)
        {
            this->x /= o.x;
            this->y /= o.y;
            this->z /= o.z;
        }
        
    };
    
    
    union Vec4i
    {
        struct
        {
            i32 x, y, z, w;
        };
        i32 e[4];
        Vec4i(i32 x, i32 y, i32 z, i32 w) : x(x), y(y), z(z), w(w){}
        Vec4i() : x(0), y(0), z(0), w(0) {}
        Vec4i(i32 i) : x(i), y(i), z(i), w(i) {}
        Vec4i(i32 i[4]) : e{i[0], i[1], i[2], i[3]} {}
        Vec4i(Vec4 v) : x((i32)v.x), y((i32)v.y), z((i32)v.z), w((i32)v.w) {}
        
        inline Vec4i operator* (Vec4i o)
        {
            Vec4i result(*this);
            result.x *= o.x;
            result.y *= o.y;
            result.z *= o.z;
            result.w *= o.w;
            return result;
        }
        
        inline Vec4i operator+ (Vec4i o)
        {
            Vec4i result(*this);
            result.x += o.x;
            result.y += o.y;
            result.z += o.z;
            result.w += o.w;
            return result;
        }
        
        inline void operator*= (Vec4i o)
        {
            this->x *= o.x;
            this->y *= o.y;
            this->z *= o.z;
            this->w *= o.w;
        }
        
        inline void operator+= (Vec4i o)
        {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;
            this->w += o.w;
        }
        
        inline Vec4i operator+ (i32 s)
        {
            Vec4i result(*this);
            result.x += s;
            result.y += s;
            result.z += s;
            result.w += s;
            return result;
        }
        
        inline Vec4i operator* (i32 s)
        {
            Vec4i result(*this);
            result.x *= s;
            result.y *= s;
            result.z *= s;
            result.w *= s;
            return result;
        }
        
        inline Vec4i operator/ (i32 s)
        {
            Vec4i result(*this);
            result.x /= s;
            result.y /= s;
            result.z /= s;
            result.w /= s;
            return result;
        }
        
        inline void operator+= (i32 s)
        {
            this->x += s;
            this->y += s;
            this->z += s;
            this->w += s;
        }
        
        inline void operator*= (i32 s)
        {
            this->x *= s;
            this->y *= s;
            this->z *= s;
            this->w *= s;
        }
        
        inline void operator/= (i32 s)
        {
            this->x /= s;
            this->y /= s;
            this->z /= s;
            this->w /= s;
        }
        
        inline void operator-= (i32 s)
        {
            this->x -= s;
            this->y -= s;
            this->z -= s;
            this->w -= s;
        }
        
        inline Vec4i operator- (Vec4i o)
        {
            Vec4i result(*this);
            result.x -= o.x;
            result.y -= o.y;
            result.z -= o.z;
            result.w -= o.w;
            return result;
        }
        
        inline void operator-= (Vec4i o)
        {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;
            this->w -= o.w;
        }
        
        inline Vec4i operator- (i32 s)
        {
            Vec4i result(*this);
            result.x -= s;
            result.y -= s;
            result.z -= s;
            result.w -= s;
            return result;
        }
        
        inline Vec4i operator/ (Vec4i o)
        {
            Vec4i result(*this);
            result.x /= o.x;
            result.y /= o.y;
            result.z /= o.z;
            result.w /= o.w;
            return result;
        }
        
        inline void operator/= (Vec4i o)
        {
            this->x /= o.x;
            this->y /= o.y;
            this->z /= o.z;
            this->w /= o.w;
        }
        
    };
    
    
    union Mat4
    {
        struct
        {
            r32 m11,m12,m13,m14;
            r32 m21,m22,m23,m24;
            r32 m31,m32,m33,m34;
            r32 m41,m42,m43,m44;
        };
        struct
        {
            r32 a, b, c, d;
            r32 e, f, g, h;
            r32 i, j, k, l;
            r32 m, n, o, p;
        };
        struct
        {
            r32 m0[4];
            r32 m1[4];
            r32 m2[4];
            r32 m3[4];
        };
        struct
        {
            Vec4 v1;
            Vec4 v2;
            Vec4 v3;
            Vec4 v4;
        };
        r32 v[4][4];
        r32 q[16];
        
        inline r32* operator[](i32 idx)
        {
            return this->v[idx];
        }
        
    Mat4() : Mat4(1.0f) {}
        Mat4(r32 m11, r32 m12, r32 m13, r32 m14, 
             r32 m21, r32 m22, r32 m23, r32 m24,
             r32 m31, r32 m32, r32 m33, r32 m34,
             r32 m41, r32 m42, r32 m43, r32 m44) : 
        m11(m11), m12(m12), m13(m13), m14(m14),
        m21(m21), m22(m22), m23(m23), m24(m24),
        m31(m31), m32(m32), m33(m33), m34(m34),
        m41(m41), m42(m42), m43(m43), m44(m44) {}

    Mat4(Vec4 v1, Vec4 v2, Vec4 v3, Vec4 v4) :
            m11(v1.x), m12(v1.y), m13(v1.z), m14(v1.w),
            m21(v2.x), m22(v2.y), m23(v2.z), m24(v2.w),
            m31(v3.x), m32(v3.y), m33(v3.z), m34(v3.w),
            m41(v4.x), m42(v4.y), m43(v4.z), m44(v4.w) {}
        
        Mat4(r32 m0[4], r32 m1[4], r32 m2[4], r32 m3[4]) : 
        m0 {m0[0],m0[1],m0[2],m0[3]}, 
        m1 {m1[0],m1[1],m1[2],m1[3]}, 
        m2 {m2[0],m2[1],m2[2],m2[3]}, 
        m3 {m3[0],m3[1],m3[2],m3[3]} {}
        
        Mat4(r32 i[4][4]) : 
        v{ {i[0][0],i[0][1],i[0][2],i[0][3]}, 
            {i[1][0],i[1][1],i[1][2],i[1][3]}, 
            {i[2][0],i[2][1],i[2][2],i[2][3]}, 
            {i[3][0],i[3][1],i[3][2],i[3][3]}}
        {}
        
        Mat4(r32 i) : v {{i,0,0,0},{0,i,0,0},{0,0,i,0},{0,0,0,i}} {}
        
        Mat4(const Mat4& o) = default;
        
        
        Mat4& operator=(const Mat4& m) = default;

        Mat4 operator*(Mat4 other)
        {
            Mat4 result(*this);
            for(i32 outer = 0; outer < 4; outer++)
            {
                for(i32 inner = 0; inner < 4; inner++)
                {
                    r32 sum = 0;
                    for(i32 shared = 0; shared < 4; shared++)
                    {
                        sum += this->v[inner][shared] * other.v[shared][outer];
                    }
                    result.v[inner][outer] = sum;
                }
            }
            return result;
        }
        
        // Only __absolute__ convenience: __always__ better to control order yourself
        void operator *= (Mat4 other)
        {
            memcpy(this->v,(other * (*this)).v, sizeof(r32) * 4 * 4);
        }
        
        inline Mat4 operator*(r32 s)
        {
            Mat4 result(*this);
            for(i32 i = 0; i < 16; i++)
            {
                result.q[i] *= s;
            }
            return result;
        }
        
    };
    
    inline Mat4 operator*(r32 s, Mat4 m)
    {
        Mat4 result(m);
        for(i32 i = 0; i < 16; i++)
        {
            result.q[i] *= s;
        }
        return result;
    }
    
    union Quat
    {
        struct
        {
            r32 x, y, z, w;
        };
        struct
        {
            Vec4 axis_angle;
        };
        struct
        {
            Vec3 axis;
            r32 angle;
        };
        
        // Identity quaternion
        Quat() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        Quat(r32 x, r32 y, r32 z, r32 angle) : axis(Vec3(x * (r32)sin(angle / 2.0f), y * (r32)sin(angle / 2.0f), z * (r32)sin(angle / 2.0f))), angle((r32)cos(angle / 2.0f)) {}
        Quat(Vec3 axis, r32 angle) : 
        axis(Vec3(axis.x * (r32)sin(angle / 2.0f), axis.y * (r32)sin(angle / 2.0f), axis.z * (r32)sin(angle / 2.0f))),
        angle((r32)cos(angle / 2.0f)) {}
        Quat(const Quat& o) : axis(o.axis), angle(o.angle) {}
        
        Quat& operator=(const Quat& q) = default;
        
        inline Quat operator-()
        {
            Quat result(*this);
            result.x = -this->x;
            result.y = -this->y;
            result.z = -this->z;
            result.w = -this->w;
            return result;
        }
        
        inline Quat operator+ (Quat q)
        {
            Quat result(*this);
            result.x += q.x;
            result.y += q.y;
            result.z += q.z;
            result.w += q.w;
            return result;
        }
        
        inline void operator+= (Quat q)
        {
            this->x += q.x;
            this->y += q.y;
            this->z += q.z;
            this->w += q.w;
        }
        
        inline Quat operator- (Quat q)
        {
            Quat result(*this);
            result.x -= q.x;
            result.y -= q.y;
            result.z -= q.z;
            result.w -= q.w;
            return result;
        }
        
        inline Quat operator* (Quat q)
        {
            Quat result(*this);
            result.w = this->w * q.w - (this->x * q.x + this->y * q.y + this->z * q.z);
            result.x = this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y; 
            result.y = this->w * q.y - this->x * q.z + this->y * q.w + this->z * q.z;
            result.z = this->w * q.z + this->x * q.y - this->y * q.x + this->z * q.w;
            return result;
        }
        
        inline void operator*= (Quat q)
        {
            auto result = *this * q;
            this->x = result.x;
            this->y = result.y;
            this->z = result.z;
            this->w = result.w;
        }
        
        inline Quat operator* (r32 v)
        {
            Quat result(*this);
            result.w *= v;
            result.x *= v;
            result.y *= v;
            result.z *= v;
            return result;
        }
        
        inline Quat operator/ (r32 v)
        {
            Quat result(*this);
            
            result.w /= v;
            result.x /= v;
            result.y /= v;
            result.z /= v;
            
            return result;
        }
    };
    
    b32 is_identity(Quat q);
    r32 dot(Quat q1, Quat q2);
    r32 dot(Vec2 v1, Vec2 v2);
    r32 dot(Vec3 v1, Vec3 v2);
    r32 dot(Vec4 v1, Vec4 v2);
    i32 dot(Vec2i v1, Vec2i v2);
    i32 dot(Vec3i v1, Vec3i v2);
    r32 distance(Vec2 v1, Vec2 v2);
    r32 distance(Vec3 v1, Vec3 v2);
    r32 distance(Vec4 v1, Vec4 v2);
    i32 distance(Vec2i v1, Vec2i v2);
    i32 distance(Vec3i v1, Vec3i v2);
    r32 distance(Vec3i v1, Vec3 v2);
    i32 floor(r32 v);
    Vec2 floor(Vec2 v);
    Vec3 floor(Vec3 v);
    i32 ceil(r32 v);
    Vec2 ceil(Vec2 v);
    Vec3 ceil(Vec3 v);
    r32 ceilf(r32 v);
    r32 sin(r32 v);
    r32 asin(r32 v);
    r32 cos(r32 v);
    r32 acos(r32 v);
    r32 absolute(r32 v);
    Vec2 absolute(Vec2 v);
    Vec3 absolute(Vec3 v);
    Vec4 absolute(Vec4 v);
    i32 round(r32 v);
    r32 square(r32 v);
    r32 sqrt(r32 v);
    r32 pow(r32 v, i32 e);
    r32 sin(r32 v);
    r32 cos(r32 v);
    r32 length(Vec2 v);
    r32 length(Vec3 v);
    r32 length(Vec4 v);
    i32 length(Vec2i v);
    Vec2 normalize(Vec2 v);
    Vec3 normalize(Vec3 v);
    Vec4 normalize(Vec4 v);
    Vec2i normalize(Vec2i v);
    Quat normalize(Quat q);
    r32 norm(Vec3 v);
    Mat4 scale(Mat4 in, Vec3 scale);
    Mat4 translate(Mat4 in, Vec3 translate);
    Mat4 x_rotate(r32 angle);
    Mat4 y_rotate(r32 angle);
    Mat4 z_rotate(r32 angle);
    Mat4 create_rotation(r32 x_angle, r32 y_angle, r32 z_angle);
    
    Quat rotate(Quat in, r32 a, Vec3 axis);
    Mat4 rotate(Mat4 m, r32 a, Vec3 axis);
    Mat4 rotate(Mat4 m, Quat r);
    Quat conjugate(Quat q);
    r32 magnitude(Quat q);
    r32 get_angle_in_radians(Quat q);
    Vec3 get_axis(Quat q);
    Vec3 right(Mat4 m);
    Vec3 up(Mat4 m);
    Vec3 forward(Mat4 m);

    Vec3 translation(Mat4 m);
    Vec3 scale(Mat4 m);
    Vec3 euler_angles(Mat4 m);
    
    Vec3 project(Vec3 in, Mat4 m, Mat4 p, Vec4 viewport);
    Vec3 cross(Vec3 a, Vec3 b);
    Mat4 ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far);
    Mat4 look_at(Vec3 p, Vec3 t);
    Mat4 perspective(r32 aspect_width_over_height, r32 focal_length, r32 near, r32 far);
    Mat4 frustum(r32 bottom, r32 top, r32 left, r32 right,
                 r32 near, r32 far);
    Vec3 mult_point_matrix(Vec3 in, Mat4 m);
    Vec3 un_project(Vec3 in, Mat4 model, Mat4 projection, Vec4i viewport);
    
    r32 random_float(r32 from, r32 to);
    Rgba random_color();
    Vec3 cast_ray(r32 mouse_x, r32 mouse_y, r32 width, r32 height, Mat4 p, Mat4 v, r32 near);
    
    Quat slerp(Quat q0, Quat q1, r32 t);
    r32 lerp(r32 a, r32 t, r32 b);
    Vec2 lerp(Vec2 a, r32 t, Vec2 b);
    Vec3 lerp(Vec3 a, r32 t, Vec3 b);
    Vec4 lerp(Vec4 a, r32 t, Vec4 b);
    Quat lerp(Quat q0, Quat q1, r32 t);
    Quat nlerp(Quat q0, Quat q1, r32 t);
    Quat interpolate(Quat q0, Quat q1, r32 f);
    Mat4 transpose(Mat4 in);
    Mat4 to_matrix(Quat q);
    Vec4 transform(Mat4& m, const Vec4& v);
    r32 determinant(const Mat4& in);
    Mat4 inverse(Mat4 m);
    
    inline b32 is_identity(Quat q)
    {
        return q.x == 0.0f && q.y == 0.0f && q.z == 0.0f && q.w == 1.0f;
    }
    
    inline Quat operator* (r32 v, Quat q)
    {
        Quat result(q);
        result.w *= v;
        result.x *= v;
        result.y *= v;
        result.z *= v;
        return result;
    }
    
    inline r32 dot(Quat q1, Quat q2)
    {
        r32 result;
        result = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
        return result;
    }
    
    inline Quat conjugate(Quat q)
    {
        Quat result(-q.axis, q.w);
        return result;
    }
    
    inline r32 magnitude(Quat q)
    {
        r32 result = 0.0f;
        result = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
        result = sqrt(result);
        return result;
    }
    
    inline Quat normalize(Quat q)
    {
        return q / magnitude(q);
    }

    inline r32 norm(Vec3 v)
    {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }
    
    //@Incomplete JBlow, CaseyM, ShawnM say don't use this
    inline Quat slerp(Quat q0, Quat q1, r32 t)
    {
        q0 = normalize(q0);
        q1 = normalize(q1);
        
        auto dot_p = dot(q0, q1);
        
        const r64 dot_threshold = 0.9995;
        if(dot_p > dot_threshold)
        {
            Quat result = q0 + t * (q1 - q0);
            result = normalize(result);
            return result;
        }
        
        if(dot_p < 0.0f)
        {
            q1 = -q1;
            dot_p = -dot_p;
        }
        
        clamp(dot_p, -1.0f, 1.0f);
        auto theta_0 = acos(dot_p);
        auto theta = theta_0 * t;
        
        auto q2 = q1 - q0 * dot_p;
        q2 = normalize(q2);
        
        auto result = q0 * cos(theta) + q2 * sin(theta);
        result = normalize(result);
        return result;
    }
    
    inline Quat lerp(Quat q0, Quat q1, r32 t)
    {
        return (1.0f - MIN(1.0f,t)) * q0 + t * q1;
    }
    
    inline Quat nlerp(Quat q0, Quat q1, r32 t)
    {
        q0 = normalize(q0);
        q1 = normalize(q1);
        
        auto dot_p = dot(q0, q1);
        
        if(dot_p < 0.0f)
        {
            q1 = -q1;
        }
        
        return normalize(lerp(q0, q1, t));
    }
    
    inline Quat interpolate(Quat q0, Quat q1, r32 f)
    {
        r32 cosom = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
        auto end = q1;
        
        if(cosom < 0.0f)
        {
            cosom = -cosom;
            end.x = -end.x;   // Reverse all signs
            end.y = -end.y;
            end.z = -end.z;
            end.w = -end.w;
        }
        
        // Calculate coefficients
        r32 sclp, sclq;
        
        if((1.0f - cosom) > 0.0001f) // 0.0001 -> some epsillon
        {
            // Standard case (slerp)
            r32 omega, sinom;
            omega = acos(cosom); // extract theta from dot product's cos theta
            sinom = sin(omega);
            sclp = sin((1.0f - f) * omega) / sinom;
            sclq = sin(f * omega) / sinom;
        } 
        else
        {
            // Very close, do linear interp (because it's faster)
            sclp = 1.0f - f;
            sclq = f;
        }
        
        Quat out;
        
        out.x = sclp * q0.x + sclq * end.x;
        out.y = sclp * q0.y + sclq * end.y;
        out.z = sclp * q0.z + sclq * end.z;
        out.w = sclp * q0.w + sclq * end.w;
        return out;
    }
    
    b32 mat4_equals(const Mat4 &first, const Mat4 &second)
    {
        for(i32 index = 0; index < 16; index++)
        {
            if(first.q[index] != second.q[index])
                return false;
        }
            
        return true;
    }
        
    inline Mat4 transpose(Mat4 in)
    {
        Mat4 result(in);
        result.m11 = in.m11;
        result.m12 = in.m21;
        result.m13 = in.m31;
        result.m14 = in.m41;
        result.m21 = in.m12;
        result.m22 = in.m22;
        result.m23 = in.m32;
        result.m24 = in.m42;
        result.m31 = in.m13;
        result.m32 = in.m23;
        result.m33 = in.m33;
        result.m34 = in.m43;
        result.m41 = in.m14;
        result.m42 = in.m24;
        result.m43 = in.m34;
        result.m44 = in.m44;
        return result;
    }
    
    inline Mat4 to_matrix(Quat q)
    {
        Mat4 result(1.0f);
        
        result[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
        result[0][1] = 2.0f * q.x * q.y + 2.0f * q.z * q.w;
        result[0][2] = 2.0f * q.x * q.z - 2.0f * q.y * q.w;
        result[1][0] = 2.0f * q.x * q.y - 2.0f * q.z * q.w;
        result[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
        result[1][2] = 2.0f * q.y * q.z + 2.0f * q.x * q.w;
        result[2][0] = 2.0f * q.x * q.z + 2.0f * q.y * q.w;
        result[2][1] = 2.0f * q.y * q.z - 2.0f * q.x * q.w;
        result[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
        
        return result;
    }
    
    inline Vec4 transform(Mat4& m, const Vec4& v)
    {
        Vec4 r(0.0f);

        r.x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3];
        r.y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3];
        r.z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3];
        r.w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3];
        
        return r;
    }
    
    inline Vec3 operator*(Mat4 m, const Vec3& v)
    {
        Vec3 r = transform(m,Vec4(v,1.0f)).xyz;
        return r;
    }
    
    inline Vec4 operator*(Mat4 m, const Vec4& v)
    {
        Vec4 r = transform(m,v);
        return r;
    }
    
    void print_matrix(Mat4 In)
    {
        debug("%f %f %f %f\n", In[0][0],In[0][1],In[0][2],In[0][3]);
        debug("%f %f %f %f\n", In[1][0],In[1][1],In[1][2],In[1][3]);
        debug("%f %f %f %f\n", In[2][0],In[2][1],In[2][2],In[2][3]);
        debug("%f %f %f %f\n", In[3][0],In[3][1],In[3][2],In[3][3]);
    }
    
    void print_quat(Quat Q)
    {
        debug("(%f, %f, %f, %f)\n", Q.x, Q.y, Q.z, Q.w);
    }
    
    inline Vec4 operator*(const Vec4& v, const Mat4& m)
    {
        Vec4 result(0.0f);
        result.x = m.a * v.x + m.b * v.y + m.c * v.z + m.d * v.w;
        result.y = m.e * v.x + m.f * v.y + m.g * v.z + m.h * v.w;
        result.z = m.i * v.x + m.j * v.y + m.k * v.z + m.l * v.w;
        result.w = m.m * v.x + m.n * v.y + m.o * v.z + m.p * v.w;
        return result;
    }
    
    inline Vec3 operator*(const Vec3& v, const Mat4& m)
    {
        Vec3 result(0.0f);
        result.x = m.a * v.x + m.b * v.y + m.c * v.z;// + m.d * 1.0f;
        result.y = m.e * v.x + m.f * v.y + m.g * v.z;// + m.h * 1.0f;
        result.z = m.i * v.x + m.j * v.y + m.k * v.z;// + m.l * 1.0f;
        return result;
    }
    
    inline r32 determinant(const Mat4& in)
    {
        return in.m11 * in.m22 * in.m33 * in.m44 + in.m11 * in.m23 * in.m34 * in.m42 + in.m11 * in.m24 * in.m32 * in.m43 + 
            in.m12 * in.m21 * in.m34 * in.m43 + in.m12 * in.m23 * in.m31 * in.m44 + in.m12 * in.m24 * in.m33 * in.m41 +
            in.m13 * in.m21 * in.m32 * in.m44 + in.m13 * in.m22 * in.m34 * in.m41 + in.m13 * in.m24 * in.m31 * in.m42 +
            in.m14 * in.m21 * in.m33 * in.m42 + in.m14 * in.m22 * in.m31 * in.m43 + in.m14 * in.m23 * in.m32 * in.m41 -
            in.m11 * in.m22 * in.m34 * in.m43 - in.m11 * in.m23 * in.m32 * in.m44 - in.m11 * in.m24 * in.m33 * in.m42 -
            in.m12 * in.m21 * in.m33 * in.m44 - in.m12 * in.m23 * in.m34 * in.m41 - in.m12 * in.m24 * in.m31 * in.m43 -
            in.m13 * in.m21 * in.m34 * in.m42 - in.m13 * in.m22 * in.m31 * in.m44 - in.m13 * in.m24 * in.m32 * in.m41 -
            in.m14 * in.m21 * in.m32 * in.m43 - in.m14 * in.m22 * in.m33 * in.m41 - in.m14 * in.m23 * in.m31 * in.m42;
        
    }
    
    /*
    * I got this shit from stackoverflow and it took long enough to do
    * Has been tested by doing: Matrix * Inverse(Matrix) = I checks and seems to 
    * consistently work! 
    * Link: https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
    */
    inline Mat4 inverse(Mat4 m)
    {
        Mat4 result(0.0f);
        
        auto e = m.q;
        
        result.q[0] = 
            e[5]  *   e[10] * e[15] - 
            e[5]  *   e[11] * e[14] -
            e[9]  *   e[6]  * e[15] +
            e[9]  *   e[7]  * e[14] +
            e[13] *   e[6]  * e[11] -
            e[13] *   e[7]  * e[10];
        
        // DONe
        
        result.q[4] = 
            -e[4] *   e[10] * e[15] +
            e[4]  *   e[11] * e[14] +
            e[8]  *   e[6]  * e[15] -
            e[8]  *   e[7]  * e[14] -
            e[12] *   e[6]  * e[11] +
            e[12] *   e[7]  * e[10];
        
        // DONE
        
        result.q[8] = 
            e[4]  *   e[9]  * e[15] - 
            e[4]  *   e[11] * e[13] -
            e[8]  *   e[5]  * e[15] +
            e[8]  *   e[7]  * e[13] +
            e[12] *   e[5]  * e[11] -
            e[12] *   e[7]  * e[9];
        //DONE
        
        result.q[12] = 
            -e[4] *   e[9]  * e[14] +
            e[4]  *   e[10] * e[13] +
            e[8]  *   e[5]  * e[14] -
            e[8]  *   e[6]  * e[13] -
            e[12] *   e[5]  * e[10] +
            e[12] *   e[6]  * e[9];
        //DONE
        
        result.q[1] = 
            -e[1] *   e[10] * e[15] +
            e[1]  *   e[11] * e[14] +
            e[9]  *   e[2]  * e[15] -
            e[9]  *   e[3]  * e[14] -
            e[13] *   e[2]  * e[11] +
            e[13] *   e[3]  * e[10];
        //DONE
        
        result.q[5] = 
            e[0]  *   e[10] * e[15] - 
            e[0]  *   e[11] * e[14] -
            e[8]  *   e[2]  * e[15] +
            e[8]  *   e[3]  * e[14] +
            e[12] *   e[2]  * e[11] -
            e[12] *   e[3]  * e[10];
        //DONE
        
        result.q[9] = 
            -e[0] *   e[9]  * e[15] + 
            e[0]  *   e[11] * e[13] +
            e[8]  *   e[1]  * e[15] -
            e[8]  *   e[3]  * e[13] -
            e[12] *   e[1]  * e[11] +
            e[12] *   e[3]  * e[9];
        //DONE
        
        result.q[13] = 
            e[0]  *   e[9]  * e[14] - 
            e[0]  *   e[10] * e[13] -
            e[8]  *   e[1]  * e[14] +
            e[8]  *   e[2]  * e[13] +
            e[12] *   e[1]  * e[10] -
            e[12] *   e[2]  * e[9];
        //DONE
        
        result.q[2] = 
            e[1]  *   e[6]  * e[15] - 
            e[1]  *   e[7]  * e[14] -
            e[5]  *   e[2]  * e[15] +
            e[5]  *   e[3]  * e[14] +
            e[13] *   e[2]  * e[7]  -
            e[13] *   e[3]  * e[6];
        //DONE
        
        result.q[6] = 
            -e[0] *   e[6]  * e[15] + 
            e[0]  *   e[7]  * e[14] +
            e[4]  *   e[2]  * e[15] -
            e[4]  *   e[3]  * e[14] -
            e[12] *   e[2]  * e[7]  +
            e[12] *   e[3]  * e[6];
        //DONE
        
        result.q[10] = 
            e[0]  *   e[5]  * e[15] - 
            e[0]  *   e[7]  * e[13] -
            e[4]  *   e[1]  * e[15] +
            e[4]  *   e[3]  * e[13] +
            e[12] *   e[1]  * e[7]  -
            e[12] *   e[3]  * e[5];
        //DONE
        
        result.q[14] = 
            -e[0] *   e[5]  * e[14] +
            e[0]  *   e[6]  * e[13] +
            e[4]  *   e[1]  * e[14] -
            e[4]  *   e[2]  * e[13] -
            e[12] *   e[1]  * e[6]  +
            e[12] *   e[2]  * e[5];
        //DONE
        
        result.q[3] = 
            -e[1]  *   e[6]  * e[11] + 
            e[1]   *   e[7]  * e[10] +
            e[5]   *   e[2]  * e[11] -
            e[5]   *   e[3]  * e[10] -
            e[9]   *   e[2]  * e[7]  +
            e[9]   *   e[3]  * e[6];
        //DONE
        
        result.q[7] = 
            e[0]  *   e[6]  * e[11] - 
            e[0]  *   e[7]  * e[10] -
            e[4]  *   e[2]  * e[11] +
            e[4]  *   e[3]  * e[10] +
            e[8]  *   e[2]  * e[7]  -
            e[8]  *   e[3]  * e[6];
        //DONE
        
        result.q[11] =  
            -e[0]  *   e[5]  * e[11] +
            e[0]   *   e[7]  * e[9]  +
            e[4]   *   e[1]  * e[11] -
            e[4]   *   e[3]  * e[9]  -
            e[8]   *   e[1]  * e[7]  +
            e[8]   *   e[3]  * e[5];
        //DONE
        
        result.q[15] = 
            e[0]  *   e[5]  * e[10] - 
            e[0]  *   e[6]  * e[9]  -
            e[4]  *   e[1]  * e[10] +
            e[4]  *   e[2]  * e[9]  +
            e[8]  *   e[1]  * e[6]  -
            e[8]  *   e[2]  * e[5];
        
        auto det = determinant(m);
        det = 1.0f / det;
        
        result = result * det;
        
        return result;
    }
    
    inline r32 dot(Vec2 v1, Vec2 v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }
    
    inline r32 dot(Vec3 v1, Vec3 v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    
    inline r32 dot(Vec4 v1, Vec4 v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }
    
    inline i32 dot(Vec2i v1, Vec2i v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }
    
    inline i32 dot(Vec3i v1, Vec3i v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z + v2.z;
    }
    
    inline r32 distance(Vec2 v1, Vec2 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
    }
    
    inline r32 distance(Vec3 v1, Vec3 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
    }
    
    inline r32 distance(Vec4 v1, Vec4 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2) + pow(v1.w - v2.w,2));
    }
    
    inline i32 distance(Vec2i v1, Vec2i v2)
    {
        return (i32)(sqrt(pow((r32)v1.x - (r32)v2.x, 2) + pow((r32)v1.y - (r32)v2.y, 2)));
    }
    
    inline i32 distance(Vec3i v1, Vec3i v2)
    {
        return (i32)sqrt(pow((r32)v1.x - v2.x, 2) + pow((r32)v1.y - v2.y, 2) + pow((r32)v1.z - v2.z, 2));
    }
    
    inline r32 distance(Vec3i v1, Vec3 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
    }
    
    
    inline i32 floor(r32 v)
    {
        return (i32)::floor(v);
    }
    
    
    inline Vec2 floor(Vec2 v)
    {
        Vec2 result(v);
        result.x = (r32)floor(v.x);
        result.y = (r32)floor(v.y);
        return result;
    }
    
    inline Vec3 floor(Vec3 v)
    {
        Vec3 result(v);
        result.x = (r32)floor(v.x);
        result.y = (r32)floor(v.y);
        result.z = (r32)floor(v.z);
        return result;
    }
    
    inline i32 ceil(r32 v)
    {
        return (i32)::ceil(v);
    }
    
    inline r32 ceilf(r32 v)
    {
        return (r32)::ceil(v);
    }
    
    inline Vec2 ceil(Vec2 v)
    {
        Vec2 result(v);
        result.x = (r32)ceil(v.x);
        result.y = (r32)ceil(v.y);
        return result;
    }
    
    inline Vec3 ceil(Vec3 v)
    {
        Vec3 result(v);
        result.x = (r32)ceil(v.x);
        result.y = (r32)ceil(v.y);
        result.z = (r32)ceil(v.z);
        return result;
    }
    
    inline i32 round(r32 v)
    {
        r32 half_ceil = ((r32)ceil(v))/2.0f;
        if(v >= half_ceil)
        {
            return ceil(v);
        }
        else 
        {
            return floor(v);
        }
    }
    
    r32 absolute(r32 v)
    {
        return ABS(v);
    }
    
    Vec2 absolute(Vec2 v)
    {
        return math::Vec2(ABS(v.x), ABS(v.y));
    }
    
    Vec3 absolute(Vec3 v)
    {
        return math::Vec3(ABS(v.x), ABS(v.y), ABS(v.z));
    }
    
    Vec4 absolute(Vec4 v)
    {
        return math::Vec4((r32)ABS(v.x), (r32)ABS(v.y), (r32)ABS(v.z), (r32)ABS(v.w));
    }
    
    inline r32 square(r32 v)
    {
        return v * v;
    }
    
    /*inline r32 sqrt(r32 v)
    {
        return (r32)std::sqrt(v);
    }*/
    
    inline r32 pow(r32 v, i32 e)
    {
        return (r32)::pow(v, e);
    }
    
    inline r32 sin(r32 v)
    {
        return (r32)::sin(v);
    }

    inline r32 asin(r32 v)
    {
        return (r32)::asin(v);
    }
    
    inline r32 cos(r32 v)
    {
        return (r32)::cos(v);
    }
    
    inline r32 acos(r32 v)
    {
        return (r32)::acos(v);
    }

    /* inline r32 tan(r32 v) */
    /* { */
    /*     return(r32)::tan(v); */
    /* } */

    inline r32 atan(r32 v)
    {
        return(r32)::atan(v);
    }
    
    inline i32 multiple_of_number(i32 n, i32 mul)
    {
        return mul * (ceil(((r32)n / (r32)mul)));
    }

    inline u32 multiple_of_number_uint(u32 n, u32 mul)
    {
        return mul * (ceil(((r32)n / (r32)mul)));
    }

    inline size_t multiple_of_number(size_t n, size_t mul)
    {
        return (size_t)(mul * (ceil(((r32)n / (r32)mul))));
    }

    inline i32 next_power_of_two(i32 n)
    {
        i32 v = n;
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        assert(v == (i32)pow(2, ceil((r32)log2(n))));
        return v;
    }
    
    inline r32 length(Vec2 v)
    {
        return sqrt(pow(v.x,2) + pow(v.y,2));
    }
    
    inline r32 length(Vec3 v)
    {
        return sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));
    }
    
    inline r32 length(Vec4 v)
    {
        return sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2) + pow(v.w,2));
    }

    inline i32 length(Vec2i v)
    {
        return (i32) (sqrt(pow((r32)v.x,2) + pow((r32)v.y,2)));
    }

    inline r32 length(Quat q)
    {
        return sqrt(pow(q.x, 2) + pow(q.y, 2) + pow(q.z, 2) + pow(q.w, 2));
    }
    
    inline Vec2 normalize(Vec2 v)
    {
        Vec2 result(v);
        auto l = length(v);
        if(l == 0.0f)
        {
            return result;
        }
        result /= l;
        return result;
    }

    inline Vec2i normalize(Vec2i v)
    {
        Vec2i result(v);
        auto l = length(v);
        if(l == 0.0f)
        {
            return result;
        }
        result /= l;
        return result;
    }
    
    inline Vec3 normalize(Vec3 v)
    {
        Vec3 result(v);
        auto l = length(v);
        if(l == 0.0f)
        {
            return result;
        }
        result /= l;
        return result;
    }
    
    inline Vec4 normalize(Vec4 v)
    {
        Vec4 result(v);
        auto l = length(v);
        if(l == 0.0f)
        {
            return result;
        }
        result /= l;
        return result;
    }
    
    inline r32 get_angle_in_radians(Quat q)
    {
        return acos(q.w) * 2.0f;
    }
    
    inline Vec3 get_axis(Quat q)
    {
        r32 angle = get_angle_in_radians(q);
        Vec3 result;
        result.x = q.x / sin(angle / 2.0f);
        result.y = q.y / sin(angle / 2.0f); 
        result.z = q.z / sin(angle / 2.0f); 
        return result;
    }

    inline Vec3 to_euler_angles(Quat q)
    {
        math::Vec3 result(0.0f);
        // roll (x-axis rotation)
        r32 sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
        r32 cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
        result.x = (r32)atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        r32 sinp = 2.0f * (q.w * q.y - q.z * q.x);
        if (fabs(sinp) >= 1)
        {
            result.y = (r32)copysign(PI / 2, sinp); // use 90 degrees if out of range
        }
        else
        {
            result.y = asin(sinp);
        }

        // yaw (z-axis rotation)
        r32 siny_cosp = +2.0f * (q.w * q.z + q.x * q.y);
        r32 cosy_cosp = +1.0f - 2.0f * (q.y * q.y + q.z * q.z);  
        result.z = (r32)atan2(siny_cosp, cosy_cosp);

        return result;
    }
    
    inline Mat4 scale(Mat4 in, Vec3 scale)
    {
        Mat4 result(in);
        result.m11 = scale.x * result.m11;
        result.m22 = scale.y * result.m22;
        result.m33 = scale.z * result.m33;
        return result;
    }
    
    inline Mat4 translate(Mat4 in, Vec3 translation)
    {
        Mat4 result(1.0f);
        result[0][3] += translation.x;
        result[1][3] += translation.y;
        result[2][3] += translation.z;
        return(result * in);
    }
    
    inline Mat4 x_rotate(r32 angle)
    {
        angle *= DEGREE_IN_RADIANS;
        
        r32 c = cos(angle);
        r32 s = sin(angle);
        
        Mat4 r(1,0, 0,0,
               0,c,-s,0,
               0,s, c,0,
               0,0, 0,1);
        
        return r;
    }
    
    inline Mat4 y_rotate(r32 angle)
    {
        angle *= DEGREE_IN_RADIANS;
        
        r32 c = cos(angle);
        r32 s = sin(angle);
        
        Mat4 r(c, 0,s,0,
               0, 1,0,0,
               -s,0,c,0,
               0, 0,0,1);
        
        return r;
    }
    
    inline Mat4 z_rotate(r32 angle)
    {
        angle *= DEGREE_IN_RADIANS;
        
        r32 c = cos(angle);
        r32 s = sin(angle);
        
        Mat4 r(c,-s,0,0,
               s,c,0,0,
               0,0,1,0,
               0,0,0,1);
        
        return r;
    }
    
    inline Mat4 create_rotation(r32 x_angle, r32 y_angle, r32 z_angle)
    {
        Mat4 result(1.0f);
        result = y_rotate(y_angle) * x_rotate(x_angle) * z_rotate(z_angle) * result;
        return result;
    }

    Mat4 rotate(Mat4 m, r32 a, Vec3 axis)
    {
        Quat r = rotate(Quat(), a / DEGREE_IN_RADIANS, axis);
        Mat4 result = rotate(m, r);
        return result;
    }

    inline Quat rotate(Quat in, r32 a, Vec3 axis)
    {
        Quat result(in);
        Quat q = math::Quat(axis.x, axis.y, axis.z, DEGREE_IN_RADIANS * a);
        result = in * q;
        result = normalize(result);
        return result;
    }

    inline Quat rotate(Quat in, Quat rotation)
    {
        Quat result(in);
        result = in * rotation;
        result = normalize(result);
        return result;
    }

    inline Quat rotate(Quat in, Vec3 rotation)
    {
        r32 x_axis = ABS(rotation.x) > 0.0f ? 1.0f : 0.0f;
        r32 y_axis = ABS(rotation.y) > 0.0f ? 1.0f : 0.0f;
        r32 z_axis = ABS(rotation.z) > 0.0f ? 1.0f : 0.0f;
        
        in = math::rotate(in, rotation.x, math::Vec3(x_axis, 0.0f, 0.0f));
        in = math::rotate(in, rotation.y, math::Vec3(0.0f, y_axis, 0.0f));
        in = math::rotate(in, rotation.z, math::Vec3(0.0f, 0.0f, z_axis));

        return in;
    }

    // https://gamedev.stackexchange.com/a/50545
    inline Vec3 rotate(Vec3 in, Quat q)
    {
        math::Vec3 u(q.x, q.y, q.z);
        
        r32 s = q.w;
        
        auto result = 2.0f * dot(u, in) * u 
            + (s * s - dot(u,u)) * in 
            + 2.0f * s * cross(u, in);
        return result;
    }
    
    inline Mat4 rotate(Mat4 m, Quat r)
    {
        Mat4 result(1.0f);
        result = to_matrix(r) * m;
        return result;
    }
    
    inline Vec3 right(Mat4 m)
    {
        return math::Vec3(m[0][0],
                          m[1][0],
                          m[2][0]);
    }
    
    inline Vec3 up(Mat4 m)
    {
        return math::Vec3(m[0][1],
                          m[1][1],
                          m[2][1]);
    }
    
    inline Vec3 forward(Mat4 m)
    {
        return math::Vec3(m[0][2],
                          m[1][2],
                          m[2][2]);
    }
    
    inline Vec3 translation(Mat4 m)
    {
        return math::Vec3(m[0][3],
                          m[1][3],
                          m[2][3]);
    }
    
    inline Vec3 scale(Mat4 m)
    {
        math::Vec3 result;
        result.x = length(right(m));
        result.y = length(up(m));
        result.z = length(forward(m));
        return result;
    }

    inline Vec3 euler_angles(Mat4 m)
    {
        math::Vec3 result;

        if(m[0][0] == 1.0f)
        {
            result.y = atan2f(m[0][2], m[2][3]);
            result.x = 0;
            result.z = 0;
        }
        else if (m[0][0] == -1.0f)
        {
            result.y = atan2f(m[0][2], m[2][3]);
            result.x = 0;
            result.z = 0;
        }
        else 
        {

            result.y = atan2(-m[2][0], m[0][0]);
            result.x = asin(m[1][0]);
            result.z = atan2(-m[1][2], m[1][1]);
        }
        return result;
    }
    
    inline Vec3 project(Vec3 in, Mat4 m, Mat4 p, Vec4 viewport)
    {
        Vec3 result(1.0f);
        auto tmp = Vec4(in, 1.0f);
        tmp = m * tmp;
        tmp = p * tmp;
        tmp /= tmp.w;
        
        tmp = tmp * 0.5f + 0.5f;
        tmp.x = tmp.x * viewport.z + viewport.x;
        tmp.y = tmp.y * viewport.w + viewport.y;
        
        return Vec3(tmp.x,tmp.y,tmp.z);
    }
    
    inline Vec3 cross(Vec3 a, Vec3 b)
    {
        Vec3 result;
        
        result.x = a.y*b.z - a.z*b.y;
        result.y = a.z*b.x - a.x*b.z;
        result.z = a.x*b.y - a.y*b.x;
        
        return result;
    }
    
    inline Mat4 ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far)
    {
        Mat4 result(1.0f);
        result.m11 = 2.0f/(right - left);
        result.m22 = 2.0f/(top - bottom);
        result.m33 = -2.0f/(far - near);
        result.m34 = -((far + near)/(far - near));
        result.m14 = -((right + left)/(right - left));
        result.m24 = -((top + bottom)/(top - bottom));
        result.m44 = 1.0f;
        
        return result;
    }
    
    inline Mat4 look_at(Vec3 forward, Vec3 eye)
    {
        Vec3 right = normalize(cross(Vec3(0, 1, 0), forward));
        Vec3 up = normalize(cross(forward, right));
        
        Mat4 result(right.x,    right.y,     right.z,    0,
                    up.x,       up.y,        up.z,       0,
                    -forward.x, -forward.y,  -forward.z, 0,
                    0,          0,           0,          1);
        
        Vec4 translation = result * Vec4(-eye, 1.0f);
        result[0][3] = translation.x;
        result[1][3] = translation.y;
        result[2][3] = translation.z;
        
        return(result);
    }
    
    inline Mat4 look_at_with_target(Vec3 eye, Vec3 target)
    {
        Vec3 forward = normalize(target - eye);
        return look_at(forward, eye);
    }
    
    inline Mat4 perspective(r32 aspect, r32 focal_length, r32 near, r32 far)
    {
        r32 a = 1.0f;
        r32 b = aspect;
        r32 c = focal_length;
        
        r32 n = near;
        r32 f = far;
        
        r32 d = (n + f) / (n - f);
        r32 e = (2 * f * n) / (n - f);
        
        Mat4 result =
        {
            a * c,    0,  0,  0,
            0,  b * c,  0,  0,
            0,    0,  d,  e,
            0,    0, -1,  0
        };
        
        return(result);
    }
    
    inline Vec3 mult_point_matrix(Vec3 in, Mat4 m)
    {
        math::Vec3 result(0.0f);
        result.x = in.x * m[0][0] + in.y * m[0][1] + in.z * m[0][2] + m[0][3];
        result.y = in.x * m[1][0] + in.y * m[1][1] + in.z * m[1][2] + m[1][3];
        result.z = in.x * m[2][0] + in.y * m[2][1] + in.z * m[2][2] + m[2][3];
        r32 w = in.x * m[3][0] + in.y * m[3][1] + in.z * m[3][2] + m[3][3];
        
        if(w != 1)
        {
            result.x /= w;
            result.y /= w;
            result.z /= w;
        }
        return result;
    }
    
    inline Vec3 un_project(Vec3 in, Mat4 model, Mat4 projection, Vec4i viewport)
    {
        auto inv = inverse(projection * model);
        
        auto tmp = Vec4(in,1.0f);
        tmp.x = (tmp.x - viewport.x) / viewport.z;
        tmp.y = (tmp.y - viewport.y) / viewport.w;
        tmp = tmp * 2 - 1;
        
        auto obj = inv * tmp;
        obj /= obj.w;
        
        return Vec3(obj.x,obj.y,obj.z);
    }
    
    inline r32 lerp(r32 a, r32 t, r32 b)
    {
        r32 result = (1.0f - t) * a + t * b;
        return result;
    }

    inline r32 linear_tween(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * t + b;
    }

    inline r32 ease_in_quad(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * t * t + b;
    }
    
    inline r32 ease_out_quad(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return -c * t * (t - 2) + b;
    }

    inline r32 ease_in_out_quad(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        t *= 2.0f;
        if(t < 1.0f)
        {
            return (c / 2.0f) * (t * t) + b;
        }

        t--;
        return -c/2.0f * (t *( t - 2.0f) - 1.0f) + b;
    }

    inline r32 ease_in_cubic(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * t * t * t + b;
    }

    inline r32 ease_out_cubic(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        t--;
        return c * (t * t * t + 1.0f) + b;
    }

    inline r32 ease_in_out_cubic(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        t *= 2.0f;
        if(t < 1.0f)
        {
            return (c / 2.0f) * (t * t * t) + b;
        }

        t -= 2;
        return c/2.0f * (t * t * t + 2.0f) + b;
    }

    inline r32 ease_in_quart(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * t * t * t * t + b;
    }

    inline r32 ease_out_quart(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        t--;
        return -c * (t * t * t * t - 1.0f) + b;;
    }

    inline r32 ease_in_out_quart(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        t *= 2.0f;
        if(t < 1.0f)
        {
            return (c / 2.0f) * (t * t * t * t) + b;
        }
        t -= 2;
        return -c/2.0f * (t * t * t * t - 2.0f) + b;
    }

    inline r32 ease_in_quint(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * t * t * t * t * t + b;
    }

    inline r32 ease_out_quint(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        t--;
        return c * (t * t * t * t * t + 1.0f) + b;
    }

    inline r32 ease_in_out_quint(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        t *= 2.0f;
        if(t < 1.0f)
        {
            return (c / 2.0f) * (t * t * t * t * t) + b;
        }
        t -= 2;
        return c/2.0f * (t * t * t * t * t + 2.0f) + b;
    }

    inline r32 ease_in_sin(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return -c * cos(t * (PI/2.0f)) + c + b;
    }

    inline r32 ease_out_sin(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * sin(t * (PI/2.0f)) + b;
    }

    inline r32 ease_in_out_sin(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return -c/2 * (cos(t * PI) - 1.0f) + b;
    }

    inline r32 ease_in_exponential(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * (pow(2.0f, 10.0f * (t - 1.0f))) + b;
    }

    inline r32 ease_out_exponential(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * (-pow(2.0f, -10.0f * t) + 1.0f) + b;
    }

    inline r32 ease_in_out_exponential(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        if(t < 1.0f)
        {
            return c/2.0f * pow(2, 10 * (t - 1.0f)) + b;
        }
        t--;
        return c/2.0f * (-pow(2, -10 * t) + 2) + b;
    }

    inline r32 ease_in_circular(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return -c * (sqrt(1 - t * t) - 1.0f) + b;
    }

    inline r32 ease_out_circular(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        return c * sqrt(1 - t * t) + b;
    }

    inline r32 ease_in_out_circular(r32 b, r32 t, r32 _c)
    {
        r32 c = _c - b;
        t *= 2.0f;
        if(t < 1.0f)
        {
            return -c/2.0f * (sqrt(1.0f - t * t) - 1.0f) + b;
        }
        t--;
        return c/2.0f * (sqrt(1.0f - t * t) + 1.0f) + b;
    }

    using easing_function = r32 (*)(r32, r32, r32);
    easing_function easing_table[] =
        {
            ease_in_quad, ease_out_quad, ease_in_out_quad,
            ease_in_cubic, ease_out_cubic, ease_in_out_cubic,
            ease_in_quart, ease_out_quart, ease_in_out_quart,
            ease_in_quint, ease_out_quint, ease_in_out_quint,
            ease_in_sin, ease_out_sin, ease_in_out_sin,
            ease_in_exponential, ease_out_exponential, ease_in_out_exponential,
            ease_in_circular, ease_out_circular, ease_in_out_circular
        };

    inline Vec3 linear_tween(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(linear_tween(b.x, t, _c.x), linear_tween(b.y, t, _c.y), linear_tween(b.z, t, _c.z));
    }

    inline Vec3 ease_in_quad(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_quad(b.x, t, _c.x), ease_in_quad(b.y, t, _c.y), ease_in_quad(b.z, t, _c.z));
    }

    inline Vec3 ease_out_quad(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_out_quad(b.x, t, _c.x), ease_out_quad(b.y, t, _c.y), ease_out_quad(b.z, t, _c.z));
    }

    inline Vec3 ease_in_out_quad(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_out_quad(b.x, t, _c.x), ease_in_out_quad(b.y, t, _c.y), ease_in_out_quad(b.z, t, _c.z));
    }

    inline Vec3 ease_in_cubic(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_cubic(b.x, t, _c.x), ease_in_cubic(b.y, t, _c.y), ease_in_cubic(b.z, t, _c.z));
    }

    inline Vec3 ease_out_cubic(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_out_cubic(b.x, t, _c.x), ease_out_cubic(b.y, t, _c.y), ease_out_cubic(b.z, t, _c.z));
    }

    inline Vec3 ease_in_out_cubic(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_out_cubic(b.x, t, _c.x), ease_in_out_cubic(b.y, t, _c.y), ease_in_out_cubic(b.z, t, _c.z));
    }

    inline Vec3 ease_in_quart(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_quart(b.x, t, _c.x), ease_in_quart(b.y, t, _c.y), ease_in_quart(b.z, t, _c.z));
    }

    inline Vec3 ease_out_quart(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_out_quart(b.x, t, _c.x), ease_out_quart(b.y, t, _c.y), ease_out_quart(b.z, t, _c.z));
    }

    inline Vec3 ease_in_out_quart(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_out_quart(b.x, t, _c.x), ease_in_out_quart(b.y, t, _c.y), ease_in_out_quart(b.z, t, _c.z));
    }

    inline Vec3 ease_in_quint(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_quint(b.x, t, _c.x), ease_in_quint(b.y, t, _c.y), ease_in_quint(b.z, t, _c.z));
    }

    inline Vec3 ease_out_quint(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_out_quint(b.x, t, _c.x), ease_out_quint(b.y, t, _c.y), ease_out_quint(b.z, t, _c.z));
    }

    inline Vec3 ease_in_out_quint(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_out_quint(b.x, t, _c.x), ease_in_out_quint(b.y, t, _c.y), ease_in_out_quint(b.z, t, _c.z));
    }

    inline Vec3 ease_in_sin(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_sin(b.x, t, _c.x), ease_in_sin(b.y, t, _c.y), ease_in_sin(b.z, t, _c.z));
    }

    inline Vec3 ease_out_sin(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_out_sin(b.x, t, _c.x), ease_out_sin(b.y, t, _c.y), ease_out_sin(b.z, t, _c.z));
    }

    inline Vec3 ease_in_out_sin(Vec3 b, r32 t, Vec3 _c)
    {
        return Vec3(ease_in_out_sin(b.x, t, _c.x), ease_in_out_sin(b.y, t, _c.y), ease_in_out_sin(b.z, t, _c.z));
    }

    inline Vec2 lerp(Vec2 a, r32 t, Vec2 b)
    {
        Vec2 result(0.0f);
        result.x = lerp(a.x,t,b.x);
        result.y = lerp(a.y,t,b.y);
        return result;
    }
    
    inline Vec3 lerp(Vec3 a, r32 t, Vec3 b)
    {
        Vec3 result(0.0f);
        result.x = lerp(a.x,t,b.x);
        result.y = lerp(a.y,t,b.y);
        result.z = lerp(a.z,t,b.z);
        return result;
    }
    
    inline Vec4 lerp(Vec4 a, r32 t, Vec4 b)
    {
        Vec4 result(0.0f);
        result.x = lerp(a.x,t,b.x);
        result.y = lerp(a.y,t,b.y);
        result.z = lerp(a.z,t,b.z);
        result.w = lerp(a.w,t,b.w);
        return result;
    }
    
    inline Vec2 clamp(Vec2 minimum, Vec2 value, Vec2 maximum)
    {
        return Vec2(clamp(minimum.x, value.x, maximum.x), clamp(minimum.y, value.y, maximum.y));
    }
    
    inline Vec3 clamp(Vec3 minimum, Vec3 value, Vec3 maximum)
    {
        return Vec3(clamp(minimum.x, value.x, maximum.x), clamp(minimum.y, value.y, maximum.y), clamp(minimum.z, value.z, maximum.z));
    }
    
    inline Vec4 clamp(Vec4 minimum, Vec4 value, Vec4 maximum)
    {
        return Vec4(clamp(minimum.x, value.x, maximum.x), clamp(minimum.y, value.y, maximum.y), clamp(minimum.z, value.z, maximum.z), clamp(minimum.w, value.w, maximum.w));
    }
    
    inline Vec2i clamp(Vec2i minimum, Vec2i value, Vec2i maximum)
    {
        return Vec2i(clamp(minimum.x, value.x, maximum.x), clamp(minimum.y, value.y, maximum.y));
    }
    
    inline Vec3i clamp(Vec3i minimum, Vec3i value, Vec3i maximum)
    {
        return Vec3i(clamp(minimum.x, value.x, maximum.x), clamp(minimum.y, value.y, maximum.y), clamp(minimum.z, value.z, maximum.z));
    }
    
    inline Vec4i clamp(Vec4i minimum, Vec4i value, Vec4i maximum)
    {
        return Vec4i(clamp(minimum.x, value.x, maximum.x), clamp(minimum.y, value.y, maximum.y), clamp(minimum.z, value.z, maximum.z), clamp(minimum.w, value.w, maximum.w));
    }
    
    
    inline Vec2 rotate_by_angle(Vec2 in, r32 angle)
    {
        Vec2 result(0.0f);
        result.x = in.x * cos(angle) - in.y * sin(angle);
        result.y = in.x * sin(angle) + in.y * cos(angle);
        return result;
    }
    
    inline r32 angle_from_direction(Vec2i in)
    {
        return (r32)atan2((r32)in.x, (r32)in.y);
    }
    
    inline r32 angle_from_direction(Vec2 in)
    {
        return (r32)atan2(in.x, in.y);
    }
    
    inline void seed_random(u32 seed)
    {
        srand(seed);
    }
    
    inline i32 random_int(i32 from, i32 to)
    {
	return rand() & (to + 1 - from) + from;
    }
    
    inline r32 random_float(r32 from, r32 to)
    {
        return from + rand() / ((RAND_MAX/(to - from)));
    }
    
    inline Rgba random_color()
    {
        Rgba res;
        
        res.a = 1.0f;
        
        res.r = random_float(0.0f, 1.0f);
        res.g = random_float(0.0f, 1.0f);
        res.b = random_float(0.0f, 1.0f);
        
        return res;
    }
    
    struct Ray
    {
        Vec3 origin;
        Vec3 end;
        Vec3 direction;
    };
    
    struct BoundingBox
    {
        Vec3 min;
        Vec3 max;
    };

    inline b32 line_plane_intersection(Ray ray, math::Vec3 normal, math::Vec3 coord, math::Vec3 *intersection_point)
    {
        r32 d = dot(normal, coord);

        if(dot(normal, ray.direction) == 0)
        {
            return false;
        }

        float x = (d - dot(normal, ray.origin)) / dot(normal, ray.direction);
        *intersection_point = ray.origin + normalize(ray.direction) * x;
        return true;
    }
    
    inline b32 aabb_ray_intersection(Ray r, BoundingBox b, Vec3* intersection_point)
    {
        r32 t = 0;
        Vec3 lb = b.min;
        Vec3 rt = b.max;
        // r.dir is unit direction vector of ray
        Vec3 dirfrac;
        dirfrac.x = 1.0f / r.direction.x;
        dirfrac.y = 1.0f / r.direction.y;
        dirfrac.z = 1.0f / r.direction.z;
        
        // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
        // r.org is origin of ray
        r32 t1 = (lb.x - r.origin.x) * dirfrac.x;
        r32 t2 = (rt.x - r.origin.x) * dirfrac.x;
        r32 t3 = (lb.y - r.origin.y) * dirfrac.y;
        r32 t4 = (rt.y - r.origin.y) * dirfrac.y;
        r32 t5 = (lb.z - r.origin.z) * dirfrac.z;
        r32 t6 = (rt.z - r.origin.z) * dirfrac.z;
        
        float tmin = MAX(MAX(MIN(t1, t2), MIN(t3, t4)), MIN(t5, t6));
        float tmax = MIN(MIN(MAX(t1, t2), MAX(t3, t4)), MAX(t5, t6));
        
        // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
        if (tmax < 0)
        {
            t = tmax;
            return false;
        }
        
        // if tmin > tmax, ray doesn't intersect AABB
        if (tmin > tmax)
        {
            t = tmax;
            return false;
        }
        
        t = tmin;
        *intersection_point = r.origin + r.direction * t;
        return true;
    }

    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    // Our savior intersection algorithm
    inline b32 triangle_ray_intersection(Ray r, Vec3 v0, Vec3 v1, Vec3 v2, Vec3* intersection_point)
    {
        const r32 EPSILON = 0.0000001f;
        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;

        Vec3 h = cross(r.direction, e2);
        r32 a = dot(e1, h);

        if( a > -EPSILON && a < EPSILON)
            return false;

        r32 f = 1.0f / a;
        Vec3 s = r.origin - v0;
        r32 u = f * dot(s, h);
        if(u < 0.0f || u > 1.0f)
            return false;

        Vec3 q = cross(s, e1);
        r32 v = f * dot(r.direction, q);
        if(v < 0.0f || u + v > 1.0f)
            return false;

        r32 t = f * dot(e2, q);
        if(t > EPSILON)
        {
            *intersection_point = r.origin + r.direction * t;
            return true;
        }
        return false;
    }
    
    inline b32 new_aabb_ray_intersection(Ray ray, BoundingBox b, Vec3* intersection_point)
    {
        auto ray_dir = ray.direction;
        auto ray_origin = ray.origin;
        
        auto temp = 0.0f;
        auto tx_min = (b.min.x - ray_origin.x) / ray_dir.x;
        auto tx_max = (b.max.x - ray_origin.x) / ray_dir.x;
        
        if(tx_max < tx_min)
        {
            temp = tx_max;
            tx_max = tx_min;
            tx_min = temp;
        }
        
        auto ty_min = (b.min.y - ray_origin.y) / ray_dir.y;
        auto ty_max = (b.max.y - ray_origin.y) / ray_dir.y;
        
        if(ty_max < ty_min)
        {
            temp = ty_max;
            ty_max = ty_min;
            ty_min = temp;
        }
        
        auto tz_min = (b.min.z - ray_origin.z) / ray_dir.z;
        auto tz_max = (b.max.z - ray_origin.z) / ray_dir.z;
        
        if(tz_max < tz_min)
        {
            temp = tz_max;
            tz_max = tz_min;
            tz_min = temp;
        }
        
        auto t_min = (tx_min > ty_min) ? tx_min : ty_min;
        auto t_max = (tx_max < ty_max) ? tx_max : ty_max;
        
        if(tx_min > ty_max || ty_min > tx_max) return false;
        if(t_min > tz_max || tz_min > t_max) return false;
        if(tz_min > t_min) t_min = tz_min;
        if(tz_max < t_max) t_max = tz_max;
        
        *intersection_point = ray_origin + ray_dir * temp;
        
        return true;
    }

    struct Plane
    {
        Vec3 normal;
        r32 d;
    };

    static Plane get_plane(Vec3 a, Vec3 b, Vec3 c)
    {
        Vec3 u = b - a;
        Vec3 v = c - a;

        Plane p = {};
        p.normal = normalize(cross(u, v));
        p.d = dot(p.normal, b);

        return p;
    }

#define COLOR_RED math::Rgba(1, 0, 0, 1)
#define COLOR_GREEN math::Rgba(0, 1, 0, 1)
#define COLOR_BLUE math::Rgba(0, 0, 1, 1)
#define COLOR_BLACK math::Rgba(0, 0, 0, 1)
#define COLOR_WHITE math::Rgba(1, 1, 1, 1)
    
    struct Rect
    {
        union
        {
            struct
            {
                r32 x;
                r32 y;
            };
            Vec2 position;
        };
        union
        {
            struct
            {
                r32 width;
                r32 height;
            };
            Vec2 size;
        };
        
        Rect(r32 x, r32 y, r32 width, r32 height) : x(x), y(y), width(width), height(height) {}
        Rect(i32 x, i32 y, i32 width, i32 height) : x((r32)x), y((r32)y), width((r32)width),
        height((r32)height) {}
        Rect(r32 v) : x(v), y(v), width(v), height(v) {}
        Rect() : Rect(0.0f) {}
    };
    
    struct Recti
    {
        union
        {
            struct
            {
                i32 x;
                i32 y;
            };
            Vec2i position;
        };
        union
        {
            struct
            {
                i32 width;
                i32 height;
            };
            Vec2i size;
        };
        
        
        Recti() {}
        Recti(i32 x, i32 y, i32 width, i32 height) : x(x), y(y), width(width), height(height) {}
    };
    
    inline b32 rects_overlap(Rect rect1, Rect rect2)
    {
        return rect1.x < rect2.x + rect2.width && rect1.x + rect1.width > rect2.x &&
            rect1.y + rect1.height > rect2.y && rect1.y < rect2.y + rect2.height;
    }
    
    inline b32 point_inside_rect(Vec2i point, Recti rect)
    {
        return point.x >= rect.x && rect.y >= rect.y && point.x < rect.x + rect.width && point.y < rect.y + rect.height;
    }
    
    inline r32 sign(Vec2 p1, Vec2 p2, Vec2 p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }
    
    inline Vec2 to_cartesian(Vec2 position)
    {
        // @Cleanup: Move these to a global variable or similar
        r32 tile_width_half = 0.5f;
        r32 tile_height_half = 0.25f;
        
        Vec2 temp_pt(0.0f);
        
        temp_pt.x = (position.x / tile_width_half + position.y / tile_height_half) / 2.0f;
        temp_pt.y = (position.y / tile_height_half - position.x / tile_width_half) / 2.0f;
        return temp_pt;
    }
    
    inline Vec2 to_isometric(Vec2 position)
    {
        // @Cleanup: Move these to a global variable or similar
        r32 tile_width_half = 0.5f;
        r32 tile_height_half = 0.25f;
        
        Vec2 temp_pt(0.0f);
        temp_pt.x = (position.x - position.y) * tile_width_half;
        temp_pt.y = (position.x + position.y) * tile_height_half;
        //return tempPt;
        return position;
    }
    
    inline b32 point_in_triangle(Vec2 pt, Vec2 v1, Vec2 v2, Vec2 v3)
    {
        bool b1, b2, b3;
        
        b1 = sign(pt, v1, v2) < 0.0f;
        b2 = sign(pt, v2, v3) < 0.0f;
        b3 = sign(pt, v3, v1) < 0.0f;
        
        return ((b1 == b2) && (b2 == b3));
    }
}

#endif
