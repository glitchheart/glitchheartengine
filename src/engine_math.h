#ifndef MAtH_H
#define MAtH_H

#include <cmath>

//#define GLM
#ifdef GLM
#include <glm/gtc/matrix_transform.hpp>
namespace math
{
    using v2 = glm::vec2;
    using v3 = glm::vec3;
    using v4 = glm::vec4;
    using m4 = glm::mat4;
    using v2i = glm::i32vec2;
    using v3i = glm::i32vec3;
    using rgb = v3;
    using rgba = v4;
    
    r32 Dot(v2 v1, v2 v2)
    {
        return glm::dot(v1,v2);
    }
    
    r32 Dot(v3 v1, v3 v2)
    {
        return glm::dot(v1,v2);
    }
    
    r32 Dot(v4 v1, v4 v2)
    {
        return glm::dot(v1,v2);
    }
    
    r32 Distance(v2 v1, v2 v2)
    {
        return glm::distance(v1,v2);
    }
    
    m4 translate(m4 M, v3 V)
    {
        return glm::translate(M,V);
    }
    
    m4 Rotate(m4 M, r32 Angle, v3 V)
    {
        return glm::rotate(M, Angle, V);
    }
    
    m4 Inverse(m4 M)
    {
        return glm::inverse(M);
    }
    
    m4 transpose(m4 M)
    {
        return glm::transpose(M);
    }
    
    v3 Project(v3 V, m4 M, m4 P, v4 Vp)
    {
        return glm::project(V,M,P,Vp);
    }
    
    v3 UnProject(v3 V, m4 M, m4 P, v4 Vp)
    {
        return glm::unProject(V,M,P,Vp);
    }
    
    m4 Scale(m4 M, v3 S)
    {
        return glm::scale(M,S);
    }
    
    m4 Ortho(r32 L, r32 R, r32 B, r32 t, r32 N, r32 F)
    {
        return glm::ortho(L,R,B,t,N,F);
    }
    
    v2 Normalize(v2 V)
    {
        return glm::normalize(V);
    }
    
    v3 Normalize(v3 V)
    {
        return glm::normalize(V);
    }
    
    v4 Normalize(v4 V)
    {
        return glm::normalize(V);
    }
    
    inline i32 Round(r32 V)
    {
        return glm::round(V);
    }
    
    r32 Floor(r32 V)
    {
        return glm::floor(V);
    }
    
    v2 Floor(v2 V)
    {
        return glm::floor(V);
    }
    
    v3 Floor(v3 V)
    {
        return glm::floor(V);
    }
    
    r32 Ceil(r32 V)
    {
        return glm::ceil(V);
    }
    
    v2 Ceil(v2 V)
    {
        return glm::ceil(V);
    }
    
    r32 Length(v2 V)
    {
        return glm::length(V);
    }
    
    r32 Length(v3 V)
    {
        return glm::length(V);
    }
    
    r32 Length(v4 V)
    {
        return glm::length(V);
    }
}
#else
namespace math
{
    
    inline i32 clamp(i32 Minimum, i32 Value, i32 Maximum)
    {
        i32 result = Max(Minimum, Min(Value,Maximum));
        return result;
    }
    
    inline r32 clamp(r32 Minimum, r32 Value, r32 Maximum)
    {
        return Max(Minimum, Min(Value,Maximum));
    }
    
    inline r64 clamp(r64 Minimum, r64 Value, r64 Maximum)
    {
        return Max(Minimum, Min(Value,Maximum));
    }
    
    union v2
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
        v2(r32 x, r32 y) : x(x), y(y){}
        v2() : x(0.0f), y(0.0f) {}
        v2(r32 i) : e{i,i} {}
        v2(r32 i[2]) : e{i[0],i[1]} {}
        v2(const v2& o) = default;
        v2(i32 x, i32 y) : x((r32)x), y((r32)y) {}
        v2(r64 x, r64 y) : x((r32)x), y((r32)y) {}
        v2(r32 x, r64 y) : x(x), y((r32)y) {}
        v2(r32 x, i32 y) : x(x), y((r32)y) {}
        v2(i32 x, r32 y) : x((r32)x), y(y) {}
        v2(i32 x, r64 y) : x((r32)x), y((r32)y) {}
        v2(r64 x, i32 y) : x((r32)x), y((r32)y) {}
        v2(r64 x, r32 y) : x((r32)x), y(y) {}
        
        v2& operator=(const v2& v) = default;
        
        r32 operator[](i32 i)
        {
            return this->e[i];
        }
        
        inline v2 operator* (v2 o)
        {
            v2 result(*this);
            result.x *= o.x;
            result.y *= o.y;
            return result;
        }
        
        inline v2 operator+ (v2 o)
        {
            v2 result(*this);
            result.x += o.x;
            result.y += o.y;
            return result;
        }
        
        inline void operator*= (v2 o)
        {
            this->x *= o.x;
            this->y *= o.y;
        }
        
        inline void operator+= (v2 o)
        {
            this->x += o.x;
            this->y += o.y;
        }
        
        inline v2 operator+ (r32 s)
        {
            v2 result(*this);
            result.x += s;
            result.y += s;
            return result;
        }
        
        inline v2 operator* (r32 s)
        {
            v2 result(*this);
            result.x *= s;
            result.y *= s;
            return result;
        }
        
        inline v2 operator/ (r32 s)
        {
            v2 result(*this);
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
        
        inline v2 operator- (v2 o)
        {
            v2 result(*this);
            result.x -= o.x;
            result.y -= o.y;
            return result;
        }
        
        inline void operator-= (v2 o)
        {
            this->x -= o.x;
            this->y -= o.y;
        }
        
        inline v2 operator- (r32 s)
        {
            v2 result(*this);
            result.x -= s;
            result.y -= s;
            return result;
        }
        
        inline v2 operator/ (v2 o)
        {
            v2 result(*this);
            result.x /= o.x;
            result.y /= o.y;
            return result;
        }
        
        inline void operator/= (v2 o)
        {
            this->x /= o.x;
            this->y /= o.y;
        }
    };
    
    union v3
    {
        struct 
        {
            union
            {
                v2 xy;
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
                v2 rg;
                struct
                {
                    r32 r, g;
                };
            };
            
            r32 b;
        };
        
        r32 e[3];
        v3(r32 x, r32 y, r32 z) : x(x), y(y), z(z) {}
        v3() : x(0.0f), y(0.0f), z(0.0f) {}
        v3(r32 i) : e{i,i,i} {}
        v3(r32 i[3]) : e{i[0],i[1], i[2]} {}
        v3(const v3& o) : e{o.x, o.y, o.z} {}
        v3(r64 x, r64 y, r64 z) : x((r32)x), y((r32)y), z((r32)z) {}
        v3(r64 x, i32 y, r64 z) : x((r32)x), y((r32)y), z((r32)z) {}
        v3(i32 x, i32 y, i32 z) : x((r32)x), y((r32)y), z((r32)z) {}
        v3(i32 x, r32 y, i32 z) : x((r32)x), y(y), z((r32)z) {}
        v3(r64 x, r64 y, i32 z) : x((r32)x), y((r32)y), z((r32)z) {}
        v3(r32 x, r32 y, i32 z) : x(x), y(y), z((r32)z) {}
        v3(r32 x, i32 y, i32 z) : x(x), y((r32)y), z((r32)z) {}
        v3(i32 x, i32 y, r32 z) : x((r32)x), y((r32)y), z(z) {}
        v3(r32 x, r32 y, r64 z) : x(x), y(y), z((r32)z) {}
        v3(r32 x, i32 y, r32 z) : x(x), y((r32)y), z(z) {}
        v3(r64 x, r32 y, r64 z) : x((r32)x), y(y), z((r32)z) {}
        v3(r64 x, r32 y, r32 z) : x((r32)x), y(y), z(z) {}
        v3(v2 v, r32 z) : x(v.x), y(v.y), z(z) {}
        
        v3& operator=(const v3& v) = default;
        
        r32 operator[](i32 i)
        {
            return this->e[i];
        }
        
        inline v3 operator= (v2 o)
        {
            return v3(o.x, o.y, 0);
        }
        
        inline v3 operator-()
        {
            v3 result(1.0f);
            result.x = -this->x;
            result.y = -this->y;
            result.z = -this->z;
            return result;
        }
        
        inline v3 operator* (v3 o)
        {
            v3 result(*this);
            result.x *= o.x;
            result.y *= o.y;
            result.z *= o.z;
            return result;
        }
        
        inline v3 operator+ (v3 o)
        {
            v3 result(*this);
            result.x += o.x;
            result.y += o.y;
            result.z += o.z;
            return result;
        }
        
        inline void operator*= (v3 o)
        {
            this->x *= o.x;
            this->y *= o.y;
            this->z *= o.z;
        }
        
        inline void operator+= (v3 o)
        {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;
        }
        
        inline v3 operator+ (r32 s)
        {
            v3 result(*this);
            result.x += s;
            result.y += s;
            result.z += s;
            return result;
        }
        
        inline v3 operator* (r32 s)
        {
            v3 result(*this);
            result.x *= s;
            result.y *= s;
            result.z *= s;
            return result;
        }
        
        inline v3 operator/ (r32 s)
        {
            v3 result(*this);
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
        
        inline v3 operator- (v3 o)
        {
            v3 result(*this);
            result.x -= o.x;
            result.y -= o.y;
            result.z -= o.z;
            return result;
        }
        
        inline void operator-= (v3 o)
        {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;
        }
        
        inline v3 operator- (r32 s)
        {
            v3 result(*this);
            result.x -= s;
            result.y -= s;
            result.z -= s;
            return result;
        }
        
        inline v3 operator/ (v3 o)
        {
            v3 result(*this);
            result.x /= o.x;
            result.y /= o.y;
            result.z /= o.z;
            return result;
        }
        
        inline void operator/= (v3 o)
        {
            this->x /= o.x;
            this->y /= o.y;
            this->z /= o.z;
        }
    };
    
    union v4
    {
        struct 
        {
            union
            {
                v3 xyz;
                struct
                {
                    r32 x, y, z;
                };
                struct
                {
                    
                    v2 xy;
                };
            };
            r32 w;
            
        };
        struct 
        {
            union
            {
                v3 rgb;
                struct
                {
                    r32 r, g, b;
                };
            };
            r32 a;
        };
        r32 e[4];
        
        v4(r32 x, r32 y, r32 z, r32 w) : x(x), y(y), z(z), w(w) {}
        v4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        v4(r32 i) : e{i,i,i,i} {}
        v4(r32 i[4]) : e{i[0], i[1], i[2], i[3]} {}
        v4(const v4& o) : x(o.x), y(o.y), z(o.z), w(o.w) {}
        
        v4(i32 x, i32 y, i32 z, i32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        v4(r32 x, r32 y, r32 z, i32 w) : 
        x(x), y(y), z(z), w((r32)w) {}
        
        v4(r64 x, r64 y, r64 z, r64 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        v4(r64 x, r64 y, r64 z, i32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        v4(r32 x, i32 y, r32 z, i32 w) : 
        x(x), y((r32)y), z(z), w((r32)w) {}
        
        v4(i32 x, r64 y, i32 z, i32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        v4(r64 x, i32 y, i32 z, i32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        v4(i32 x, i32 y, i32 z, r64 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        v4(r32 x, i32 y, i32 z, i32 w) : 
        x(x), y((r32)y), z((r32)z), w((r32)w) {}
        
        v4(i32 x, i32 y, i32 z, r32 w) : 
        x((r32)x), y((r32)y), z((r32)z), w(w) {}
        
        v4(r64 x, r64 y, i32 z, r64 w) : 
        x((r32)x), y((r32)y), z((r32)z), w((r32)w) {}
        
        v4(v3 o, r32 w) : x(o.x), y(o.y), z(o.z), w(w) {}
        
        v4(v2 v, r32 z, r32 w) : x(v.x), y(v.y), z(z), w(w) {} 
        
        v4& operator=(const v4& v) = default;
        
        r32 operator[](i32 i)
        {
            return this->e[i];
        }
        
        inline v4 operator* (v4 o)
        {
            v4 result(*this);
            result.x *= o.x;
            result.y *= o.y;
            result.z *= o.z;
            result.w *= o.w;
            return result;
        }
        
        inline v4 operator+ (v4 o)
        {
            v4 result(*this);
            result.x += o.x;
            result.y += o.y;
            result.z += o.z;
            result.w += o.w;
            return result;
        }
        
        inline void operator*= (v4 o)
        {
            this->x *= o.x;
            this->y *= o.y;
            this->z *= o.z;
            this->w *= o.w;
        }
        
        inline void operator+= (v4 o)
        {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;
            this->w += o.w;
        }
        
        inline v4 operator+ (r32 s)
        {
            v4 result(*this);
            result.x += s;
            result.y += s;
            result.z += s;
            result.w += s;
            return result;
        }
        
        inline v4 operator* (r32 s)
        {
            v4 result(*this);
            result.x *= s;
            result.y *= s;
            result.z *= s;
            result.w *= s;
            return result;
        }
        
        inline v4 operator/ (r32 s)
        {
            v4 result(*this);
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
        
        inline v4 operator- (v4 o)
        {
            v4 result(*this);
            result.x -= o.x;
            result.y -= o.y;
            result.z -= o.z;
            result.w -= o.w;
            return result;
        }
        
        inline void operator-= (v4 o)
        {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;
            this->w -= o.w;
        }
        
        inline v4 operator- (r32 s)
        {
            v4 result(*this);
            result.x -= s;
            result.y -= s;
            result.z -= s;
            result.w -= s;
            return result;
        }
        
        inline v4 operator/ (v4 o)
        {
            v4 result(*this);
            result.x /= o.x;
            result.y /= o.y;
            result.z /= o.z;
            result.w /= o.w;
            return result;
        }
        
        inline void operator/= (v4 o)
        {
            this->x /= o.x;
            this->y /= o.y;
            this->z /= o.z;
            this->w /= o.w;
        }
    };
    
    union v2i
    {
        struct
        {
            i32 x,y;
        };
        i32 e[2];
        v2i(i32 x, i32 y) : x(x), y(y){}
        v2i(r32 x, r32 y) : x((i32)x), y((i32)y){}
        v2i() : x(0),y(0) {}
        v2i(i32 i) : x(i), y(i) {}
        v2i(i32 i[2]) : e{i[0],i[1]} {}
        
        inline v2i operator* (v2i o)
        {
            v2i result(*this);
            result.x *= o.x;
            result.y *= o.y;
            return result;
        }
        
        inline v2i operator+ (v2i o)
        {
            v2i result(*this);
            result.x += o.x;
            result.y += o.y;
            return result;
        }
        
        inline void operator*= (v2i o)
        {
            this->x *= o.x;
            this->y *= o.y;
        }
        
        inline void operator+= (v2i o)
        {
            this->x += o.x;
            this->y += o.y;
        }
        
        inline v2i operator+ (i32 s)
        {
            v2i result(*this);
            result.x += s;
            result.y += s;
            return result;
        }
        
        inline v2i operator* (i32 s)
        {
            v2i result(*this);
            result.x *= s;
            result.y *= s;
            return result;
        }
        
        inline v2i operator/ (i32 s)
        {
            v2i result(*this);
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
        
        inline v2i operator- (v2i o)
        {
            v2i result(*this);
            result.x -= o.x;
            result.y -= o.y;
            return result;
        }
        
        inline void operator-= (v2i o)
        {
            this->x -= o.x;
            this->y -= o.y;
        }
        
        inline v2i operator- (i32 s)
        {
            v2i result(*this);
            result.x -= s;
            result.y -= s;
            return result;
        }
        
        inline v2i operator/ (v2i o)
        {
            v2i result(*this);
            result.x /= o.x;
            result.y /= o.y;
            return result;
        }
        
        inline void operator/= (v2i o)
        {
            this->x /= o.x;
            this->y /= o.y;
        }
    };
    
    union v3i
    {
        struct
        {
            i32 x,y,z;
        };
        i32 e[3];
        v3i(i32 x, i32 y, i32 z) : x(x), y(y), z(z){}
        v3i() : x(0), y(0), z(0) {}
        v3i(i32 i) : x(i), y(i), z(i) {}
        v3i(i32 i[3]) : e{i[0], i[1], i[2]} {}
        v3i(v3 v) : e{(i32)v.e[0], (i32)v.e[1], (i32)v.e[2]} {}
        
        inline v3i operator* (v3i o)
        {
            v3i result(*this);
            result.x *= o.x;
            result.y *= o.y;
            result.z *= o.z;
            return result;
        }
        
        inline v3i operator+ (v3i o)
        {
            v3i result(*this);
            result.x += o.x;
            result.y += o.y;
            result.z += o.z;
            return result;
        }
        
        inline void operator*= (v3i o)
        {
            this->x *= o.x;
            this->y *= o.y;
            this->z *= o.z;
        }
        
        inline void operator+= (v3i o)
        {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;
        }
        
        inline v3i operator+ (i32 s)
        {
            v3i result(*this);
            result.x += s;
            result.y += s;
            result.z += s;
            return result;
        }
        
        inline v3i operator* (i32 s)
        {
            v3i result(*this);
            result.x *= s;
            result.y *= s;
            result.z *= s;
            return result;
        }
        
        inline v3i operator/ (i32 s)
        {
            v3i result(*this);
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
        
        inline v3i operator- (v3i o)
        {
            v3i result(*this);
            result.x -= o.x;
            result.y -= o.y;
            result.z -= o.z;
            return result;
        }
        
        inline void operator-= (v3i o)
        {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;
        }
        
        inline v3i operator- (i32 s)
        {
            v3i result(*this);
            result.x -= s;
            result.y -= s;
            result.z -= s;
            return result;
        }
        
        inline v3i operator/ (v3i o)
        {
            v3i result(*this);
            result.x /= o.x;
            result.y /= o.y;
            result.z /= o.z;
            return result;
        }
        
        inline void operator/= (v3i o)
        {
            this->x /= o.x;
            this->y /= o.y;
            this->z /= o.z;
        }
        
    };
    
    
    union v4i
    {
        struct
        {
            i32 x, y, z, w;
        };
        i32 e[4];
        v4i(i32 x, i32 y, i32 z, i32 w) : x(x), y(y), z(z), w(w){}
        v4i() : x(0), y(0), z(0), w(0) {}
        v4i(i32 i) : x(i), y(i), z(i), w(i) {}
        v4i(i32 i[4]) : e{i[0], i[1], i[2], i[3]} {}
        
        inline v4i operator* (v4i o)
        {
            v4i result(*this);
            result.x *= o.x;
            result.y *= o.y;
            result.z *= o.z;
            result.w *= o.w;
            return result;
        }
        
        inline v4i operator+ (v4i o)
        {
            v4i result(*this);
            result.x += o.x;
            result.y += o.y;
            result.z += o.z;
            result.w += o.w;
            return result;
        }
        
        inline void operator*= (v4i o)
        {
            this->x *= o.x;
            this->y *= o.y;
            this->z *= o.z;
            this->w *= o.w;
        }
        
        inline void operator+= (v4i o)
        {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;
            this->w += o.w;
        }
        
        inline v4i operator+ (i32 s)
        {
            v4i result(*this);
            result.x += s;
            result.y += s;
            result.z += s;
            result.w += s;
            return result;
        }
        
        inline v4i operator* (i32 s)
        {
            v4i result(*this);
            result.x *= s;
            result.y *= s;
            result.z *= s;
            result.w *= s;
            return result;
        }
        
        inline v4i operator/ (i32 s)
        {
            v4i result(*this);
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
        
        inline v4i operator- (v4i o)
        {
            v4i result(*this);
            result.x -= o.x;
            result.y -= o.y;
            result.z -= o.z;
            result.w -= o.w;
            return result;
        }
        
        inline void operator-= (v4i o)
        {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;
            this->w -= o.w;
        }
        
        inline v4i operator- (i32 s)
        {
            v4i result(*this);
            result.x -= s;
            result.y -= s;
            result.z -= s;
            result.w -= s;
            return result;
        }
        
        inline v4i operator/ (v4i o)
        {
            v4i result(*this);
            result.x /= o.x;
            result.y /= o.y;
            result.z /= o.z;
            result.w /= o.w;
            return result;
        }
        
        inline void operator/= (v4i o)
        {
            this->x /= o.x;
            this->y /= o.y;
            this->z /= o.z;
            this->w /= o.w;
        }
        
    };
    
    
    union m4
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
        r32 v[4][4];
        r32 q[16];
        
        inline r32* operator[](i32 idx)
        {
            return this->v[idx];
        }
        
        m4() : v{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}} {}
        m4(r32 m11, r32 m12, r32 m13, r32 m14, 
           r32 m21, r32 m22, r32 m23, r32 m24,
           r32 m31, r32 m32, r32 m33, r32 m34,
           r32 m41, r32 m42, r32 m43, r32 m44) : 
        m11(m11), m12(m12), m13(m13), m14(m14),
        m21(m21), m22(m22), m23(m23), m24(m24),
        m31(m31), m32(m32), m33(m33), m34(m34),
        m41(m41), m42(m42), m43(m43), m44(m44) {}
        
        m4(r32 m0[4], r32 m1[4], r32 m2[4], r32 m3[4]) : 
        m0 {m0[0],m0[1],m0[2],m0[3]}, 
        m1 {m1[0],m1[1],m1[2],m1[3]}, 
        m2 {m2[0],m2[1],m2[2],m2[3]}, 
        m3 {m3[0],m3[1],m3[2],m3[3]} {}
        
        m4(r32 i[4][4]) : 
        v{ {i[0][0],i[0][1],i[0][2],i[0][3]}, 
            {i[1][0],i[1][1],i[1][2],i[1][3]}, 
            {i[2][0],i[2][1],i[2][2],i[2][3]}, 
            {i[3][0],i[3][1],i[3][2],i[3][3]}}
        {}
        
        m4(r32 i) : v {{i,0,0,0},{0,i,0,0},{0,0,i,0},{0,0,0,i}} {}
        
        m4(const m4& o) : v{ {o.v[0][0],o.v[0][1],o.v[0][2],o.v[0][3]}, 
            {o.v[1][0],o.v[1][1],o.v[1][2],o.v[1][3]}, 
            {o.v[2][0],o.v[2][1],o.v[2][2],o.v[2][3]}, 
            {o.v[3][0],o.v[3][1],o.v[3][2],o.v[3][3]}} {}
        
        
        m4& operator=(const m4& m) = default;
        
        m4 operator*(m4 other)
        {
            m4 result(*this);
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
        void operator *= (m4 other)
        {
            memcpy(this->v,(other * (*this)).v, sizeof(r32) * 4 * 4);
        }
        
        inline m4 operator*(r32 s)
        {
            m4 result(*this);
            result.m11 *= s;
            result.m12 *= s;
            result.m13 *= s;
            result.m14 *= s;
            result.m21 *= s;
            result.m22 *= s;
            result.m23 *= s;
            result.m24 *= s;
            result.m31 *= s;
            result.m32 *= s;
            result.m33 *= s;
            result.m34 *= s;
            result.m41 *= s;
            result.m42 *= s;
            result.m43 *= s;
            result.m44 *= s;
            return result;
        }
        
    };
    
    inline m4 operator*(r32 s, m4 m)
    {
        m4 result(m);
        result.m11 *= s;
        result.m12 *= s;
        result.m13 *= s;
        result.m14 *= s;
        result.m21 *= s;
        result.m22 *= s;
        result.m23 *= s;
        result.m24 *= s;
        result.m31 *= s;
        result.m32 *= s;
        result.m33 *= s;
        result.m34 *= s;
        result.m41 *= s;
        result.m42 *= s;
        result.m43 *= s;
        result.m44 *= s;
        return result;
    }
    
    union quat
    {
        struct
        {
            r32 x, y, z, w;
        };
        struct
        {
            v4 axis_angle;
        };
        struct
        {
            v3 axis;
            r32 angle;
        };
        
        // Identity quaternion
        quat() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        quat(r32 x, r32 y, r32 z, r32 angle) : Axis(v3(x * (r32)sin(angle / 2.0f), y * (r32)sin(angle / 2.0f), z * (r32)sin(angle / 2.0f))), Angle((r32)cos(angle / 2.0f)) {}
        quat(v3 axis, r32 angle) : 
        Axis(v3(axis.x * (r32)sin(angle / 2.0f), axis.y * (r32)sin(angle / 2.0f), axis.z * (r32)sin(angle / 2.0f))),
        Angle((r32)cos(angle / 2.0f)) {}
        quat(const quat& o) : Axis(o.axis), Angle(o.angle) {}
        
        quat& operator=(const quat& q) = default;
        
        inline quat operator-()
        {
            quat result(*this);
            result.x = -this->x;
            result.y = -this->y;
            result.z = -this->z;
            result.w = -this->w;
            return result;
        }
        
        inline quat operator+ (quat q)
        {
            quat result(*this);
            result.x += q.x;
            result.y += q.y;
            result.z += q.z;
            result.w += q.w;
            return result;
        }
        
        inline void operator+= (quat q)
        {
            this->x += q.x;
            this->y += q.y;
            this->z += q.z;
            this->w += q.w;
        }
        
        inline quat operator- (quat q)
        {
            quat result(*this);
            result.x -= q.x;
            result.y -= q.y;
            result.z -= q.z;
            result.w -= q.w;
            return result;
        }
        
        inline quat operator* (quat q)
        {
            quat result(*this);
            result.w = this->w * q.w - (this->x * q.x + this->y * q.y + this->z * q.z);
            result.x = this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y; 
            result.y = this->w * q.y - this->x * q.z + this->y * q.w + this->z * q.z;
            result.z = this->w * q.z + this->x * q.y - this->y * q.x + this->z * q.w;
            return result;
        }
        
        inline void operator*= (quat q)
        {
            auto result = *this * q;
            this->x = result.x;
            this->y = result.y;
            this->z = result.z;
            this->w = result.w;
        }
        
        inline quat operator* (r32 v)
        {
            quat result(*this);
            result.w *= v;
            result.x *= v;
            result.y *= v;
            result.z *= v;
            return result;
        }
        
        inline quat operator/ (r32 v)
        {
            quat result(*this);
            
            result.w /= v;
            result.x /= v;
            result.y /= v;
            result.z /= v;
            
            return result;
        }
    };
    
    b32 is_identity(quat q);
    r32 dot(quat q1, quat q2);
    r32 dot(v2 v1, v2 v2);
    r32 dot(v3 v1, v3 v2);
    r32 dot(v4 v1, v4 v2);
    i32 dot(v2i v1, v2i v2);
    i32 dot(v3i v1, v3i v2);
    r32 distance(v2 v1, v2 v2);
    r32 distance(v3 v1, v3 v2);
    r32 distance(v4 v1, v4 v2);
    i32 distance(v2i v1, v2i v2);
    i32 distance(v3i v1, v3i v2);
    r32 distance(v3i v1, v3 v2);
    i32 floor(r32 v);
    v2 floor(v2 v);
    v3 floor(v3 v);
    i32 ceil(r32 v);
    v2 ceil(v2 v);
    v3 ceil(v3 v);
    r32 sin(r32 v);
    r32 cos(r32 v);
    r32 a_cos(r32 v);
    r32 absolute(r32 v);
    v2 absolute(v2 v);
    v3 absolute(v3 v);
    v4 absolute(v4 v);
    i32 round(r32 v);
    r32 square(r32 v);
    r32 sqrt(r32 v);
    r32 pow(r32 v, i32 e);
    r32 sin(r32 v);
    r32 cos(r32 v);
    r32 length(v2 v);
    r32 length(v3 v);
    r32 length(v4 v);
    v2 normalize(v2 v);
    v3 normalize(v3 v);
    v4 normalize(v4 v);
    quat normalize(quat q);
    m4 scale(m4 in, v3 scale);
    m4 translate(m4 in, v3 translate);
    m4 x_rotate(r32 angle);
    m4 y_rotate(r32 angle);
    m4 z_rotate(r32 angle);
    m4 create_rotation(r32 x_angle, r32 y_angle, r32 z_angle);
    
    quat rotate(quat in, r32 a, v3 axis);
    m4 rotate(m4 m, r32 a, v3 axis);
    quat conjugate(quat q);
    r32 magnitude(quat q);
    r32 get_angle_in_radians(quat q);
    v3 get_axis(quat q);
    v3 right(m4 m);
    v3 up(m4 m);
    v3 forward(m4 m);
    v3 translation(m4 m);
    v3 scale(m4 m);
    v3 project(v3 in, m4 m, m4 p, v4 viewport);
    v3 cross(v3 a, v3 b);
    m4 ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far);
    m4 look_at(v3 p, v3 t);
    m4 perspective(r32 aspect_width_over_height, r32 focal_length, r32 near, r32 far);
    m4 frustum(r32 bottom, r32 top, r32 left, r32 right,
               r32 near, r32 far);
    v3 mult_point_matrix(v3 in, m4 m);
    v3 un_project(v3 in, m4 model, m4 projection, v4i viewport);
    
    r32 random_float(r32 from, r32 to);
    v3 cast_ray(r32 mouse_x, r32 mouse_y, r32 width, r32 height, m4 p, m4 v, r32 near);
    
    quat slerp(quat q0, quat q1, r32 t);
    r32 lerp(r32 a, r32 t, r32 b);
    v2 lerp(v2 a, r32 t, v2 b);
    v3 lerp(v3 a, r32 t, v3 b);
    v4 lerp(v4 a, r32 t, v4 b);
    quat lerp(quat q0, quat q1, r32 t);
    quat nlerp(quat q0, quat q1, r32 t);
    quat interpolate(quat q0, quat q1, r32 f);
    m4 transpose(m4 in);
    m4 to_matrix(quat q);
    v4 transform(m4& m, const v4& v);
    r32 determinant(const m4& in);
    m4 inverse(m4 m);
    
    inline b32 is_identity(quat q)
    {
        return q.x == 0.0f && q.y == 0.0f && q.z == 0.0f && q.w == 1.0f;
    }
    
    inline quat operator* (r32 v, quat q)
    {
        quat result(q);
        result.w *= v;
        result.x *= v;
        result.y *= v;
        result.z *= v;
        return result;
    }
    
    inline r32 dot(quat q1, quat q2)
    {
        r32 result;
        result = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
        return result;
    }
    
    inline quat conjugate(quat q)
    {
        quat result(-q.Axis, q.w);
        return result;
    }
    
    inline r32 magnitude(quat q)
    {
        r32 result = 0.0f;
        result = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
        result = sqrt(result);
        return result;
    }
    
    inline quat normalize(quat q)
    {
        return q / magnitude(q);
    }
    
    //@Incomplete JBlow, CaseyM, ShawnM say don't use this
    inline quat slerp(quat q0, quat q1, r32 t)
    {
        q0 = normalize(q0);
        q1 = normalize(q1);
        
        auto dot_p = dot(q0, q1);
        
        const r64 dot_threshold = 0.9995;
        if(dot_p > dot_threshold)
        {
            quat result = q0 + t * (q1 - q0);
            result = normalize(result);
            return result;
        }
        
        if(dot_p < 0.0f)
        {
            q1 = -q1;
            dot_p = -dot_p;
        }
        
        clamp(dot_p, -1.0f, 1.0f);
        auto theta_0 = a_cos(dot_p);
        auto theta = theta_0 * t;
        
        auto q2 = q1 - q0 * dot_p;
        q2 = normalize(q2);
        
        auto result = q0 * cos(theta) + q2 * sin(theta);
        result = normalize(result);
        return result;
    }
    
    inline quat lerp(quat q0, quat q1, r32 t)
    {
        return (1.0f - t) * q0 + t * q1;
    }
    
    inline quat nlerp(quat q0, quat q1, r32 t)
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
    
    inline quat interpolate(quat q0, quat q1, r32 f)
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
            omega = a_cos(cosom); // extract theta from dot product's cos theta
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
        
        quat out;
        
        out.x = sclp * q0.x + sclq * end.x;
        out.y = sclp * q0.y + sclq * end.y;
        out.z = sclp * q0.z + sclq * end.z;
        out.w = sclp * q0.w + sclq * end.w;
        return out;
    }
    
    inline m4 transpose(m4 in)
    {
        m4 result(in);
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
    
    inline m4 to_matrix(quat q)
    {
        m4 result(1.0f);
        
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
    
    inline v4 transform(m4& m, const v4& v)
    {
        v4 r(0.0f);
        
        r.x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3];
        r.y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3];
        r.z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3];
        r.w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3];
        
        return r;
    }
    
    inline v3 operator*(m4 m, const v3& v)
    {
        v3 r = transform(m,v4(v,1.0f)).xyz;
        return r;
    }
    
    inline v4 operator*(m4 m, const v4& v)
    {
        v4 r = transform(m,v);
        return r;
    }
    
    void print_matrix(m4 In)
    {
        Debug("%f %f %f %f\n", In[0][0],In[0][1],In[0][2],In[0][3]);
        Debug("%f %f %f %f\n", In[1][0],In[1][1],In[1][2],In[1][3]);
        Debug("%f %f %f %f\n", In[2][0],In[2][1],In[2][2],In[2][3]);
        Debug("%f %f %f %f\n", In[3][0],In[3][1],In[3][2],In[3][3]);
    }
    
    void print_quat(quat Q)
    {
        Debug("(%f, %f, %f, %f)\n", Q.x, Q.y, Q.z, Q.w);
    }
    
    inline v4 operator*(const v4& v, const m4& m)
    {
        v4 result(0.0f);
        result.x = m.A * v.x + m.B * v.y + m.C * v.z + m.D * v.w;
        result.y = m.e * v.x + m.F * v.y + m.G * v.z + m.H * v.w;
        result.z = m.I * v.x + m.J * v.y + m.K * v.z + m.L * v.w;
        result.w = m.m * v.x + m.N * v.y + m.O * v.z + m.P * v.w;
        return result;
    }
    
    inline v3 operator*(const v3& v, const m4& m)
    {
        v3 result(0.0f);
        result.x = m.A * v.x + m.B * v.y + m.C * v.z + m.D * 1.0f;
        result.y = m.e * v.x + m.F * v.y + m.G * v.z + m.H * 1.0f;
        result.z = m.I * v.x + m.J * v.y + m.K * v.z + m.L * 1.0f;
        return result;
    }
    
    inline r32 determinant(const m4& in)
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
    inline m4 inverse(m4 m)
    {
        m4 result(0.0f);
        
        auto e = m.Q;
        
        result.Q[0] = 
            e[5]  *   e[10] * e[15] - 
            e[5]  *   e[11] * e[14] -
            e[9]  *   e[6]  * e[15] +
            e[9]  *   e[7]  * e[14] +
            e[13] *   e[6]  * e[11] -
            e[13] *   e[7]  * e[10];
        
        // DONe
        
        result.Q[4] = 
            -e[4] *   e[10] * e[15] +
            e[4]  *   e[11] * e[14] +
            e[8]  *   e[6]  * e[15] -
            e[8]  *   e[7]  * e[14] -
            e[12] *   e[6]  * e[11] +
            e[12] *   e[7]  * e[10];
        
        // DONE
        
        result.Q[8] = 
            e[4]  *   e[9]  * e[15] - 
            e[4]  *   e[11] * e[13] -
            e[8]  *   e[5]  * e[15] +
            e[8]  *   e[7]  * e[13] +
            e[12] *   e[5]  * e[11] -
            e[12] *   e[7]  * e[9];
        //DONE
        
        result.Q[12] = 
            -e[4] *   e[9]  * e[14] +
            e[4]  *   e[10] * e[13] +
            e[8]  *   e[5]  * e[14] -
            e[8]  *   e[6]  * e[13] -
            e[12] *   e[5]  * e[10] +
            e[12] *   e[6]  * e[9];
        //DONE
        
        result.Q[1] = 
            -e[1] *   e[10] * e[15] +
            e[1]  *   e[11] * e[14] +
            e[9]  *   e[2]  * e[15] -
            e[9]  *   e[3]  * e[14] -
            e[13] *   e[2]  * e[11] +
            e[13] *   e[3]  * e[10];
        //DONE
        
        result.Q[5] = 
            e[0]  *   e[10] * e[15] - 
            e[0]  *   e[11] * e[14] -
            e[8]  *   e[2]  * e[15] +
            e[8]  *   e[3]  * e[14] +
            e[12] *   e[2]  * e[11] -
            e[12] *   e[3]  * e[10];
        //DONE
        
        result.Q[9] = 
            -e[0] *   e[9]  * e[15] + 
            e[0]  *   e[11] * e[13] +
            e[8]  *   e[1]  * e[15] -
            e[8]  *   e[3]  * e[13] -
            e[12] *   e[1]  * e[11] +
            e[12] *   e[3]  * e[9];
        //DONE
        
        result.Q[13] = 
            e[0]  *   e[9]  * e[14] - 
            e[0]  *   e[10] * e[13] -
            e[8]  *   e[1]  * e[14] +
            e[8]  *   e[2]  * e[13] +
            e[12] *   e[1]  * e[10] -
            e[12] *   e[2]  * e[9];
        //DONE
        
        result.Q[2] = 
            e[1]  *   e[6]  * e[15] - 
            e[1]  *   e[7]  * e[14] -
            e[5]  *   e[2]  * e[15] +
            e[5]  *   e[3]  * e[14] +
            e[13] *   e[2]  * e[7]  -
            e[13] *   e[3]  * e[6];
        //DONE
        
        result.Q[6] = 
            -e[0] *   e[6]  * e[15] + 
            e[0]  *   e[7]  * e[14] +
            e[4]  *   e[2]  * e[15] -
            e[4]  *   e[3]  * e[14] -
            e[12] *   e[2]  * e[7]  +
            e[12] *   e[3]  * e[6];
        //DONE
        
        result.Q[10] = 
            e[0]  *   e[5]  * e[15] - 
            e[0]  *   e[7]  * e[13] -
            e[4]  *   e[1]  * e[15] +
            e[4]  *   e[3]  * e[13] +
            e[12] *   e[1]  * e[7]  -
            e[12] *   e[3]  * e[5];
        //DONE
        
        result.Q[14] = 
            -e[0] *   e[5]  * e[14] +
            e[0]  *   e[6]  * e[13] +
            e[4]  *   e[1]  * e[14] -
            e[4]  *   e[2]  * e[13] -
            e[12] *   e[1]  * e[6]  +
            e[12] *   e[2]  * e[5];
        //DONE
        
        result.Q[3] = 
            -e[1]  *   e[6]  * e[11] + 
            e[1]   *   e[7]  * e[10] +
            e[5]   *   e[2]  * e[11] -
            e[5]   *   e[3]  * e[10] -
            e[9]   *   e[2]  * e[7]  +
            e[9]   *   e[3]  * e[6];
        //DONE
        
        result.Q[7] = 
            e[0]  *   e[6]  * e[11] - 
            e[0]  *   e[7]  * e[10] -
            e[4]  *   e[2]  * e[11] +
            e[4]  *   e[3]  * e[10] +
            e[8]  *   e[2]  * e[7]  -
            e[8]  *   e[3]  * e[6];
        //DONE
        
        result.Q[11] =  
            -e[0]  *   e[5]  * e[11] +
            e[0]   *   e[7]  * e[9]  +
            e[4]   *   e[1]  * e[11] -
            e[4]   *   e[3]  * e[9]  -
            e[8]   *   e[1]  * e[7]  +
            e[8]   *   e[3]  * e[5];
        //DONE
        
        result.Q[15] = 
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
    
    inline r32 dot(v2 v1, v2 v2)
    {
        return v1.x * v2.x + v1.y + v2.y;
    }
    
    inline r32 dot(v3 v1, v3 v2)
    {
        return v1.x * v2.x + v1.y + v2.y + v1.z * v2.z;
    }
    
    inline r32 dot(v4 v1, v4 v2)
    {
        return v1.x * v2.x + v1.y + v2.y + v1.z * v2.z + v1.w * v2.w;
    }
    
    inline i32 dot(v2i v1, v2i v2)
    {
        return v1.x * v2.x + v1.y + v2.y;
    }
    
    inline i32 dot(v3i v1, v3i v2)
    {
        return v1.x * v2.x + v1.y + v2.y + v1.z + v2.z;
    }
    
    inline r32 distance(v2 v1, v2 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
    }
    
    inline r32 distance(v3 v1, v3 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
    }
    
    inline r32 distance(v4 v1, v4 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2) + pow(v1.w - v2.w,2));
    }
    
    inline i32 distance(v2i v1, v2i v2)
    {
        return (i32)(sqrt(pow((r32)v1.x - (r32)v2.x, 2) + pow((r32)v1.y - (r32)v2.y, 2)));
    }
    
    inline i32 distance(v3i v1, v3i v2)
    {
        return (i32)sqrt(pow((r32)v1.x - v2.x, 2) + pow((r32)v1.y - v2.y, 2) + pow((r32)v1.z - v2.z, 2));
    }
    
    inline r32 distance(v3i v1, v3 v2)
    {
        return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
    }
    
    inline i32 floor(r32 v)
    {
        return (i32)floor(v);
    }
    
    inline v2 floor(v2 v)
    {
        v2 result(v);
        result.x = (r32)floor(v.x);
        result.y = (r32)floor(v.y);
        return result;
    }
    
    inline v3 floor(v3 v)
    {
        v3 result(v);
        result.x = (r32)floor(v.x);
        result.y = (r32)floor(v.y);
        result.z = (r32)floor(v.z);
        return result;
    }
    
    inline i32 ceil(r32 v)
    {
        return (i32)ceil(v);
    }
    
    inline v2 ceil(v2 v)
    {
        v2 result(v);
        result.x = (r32)ceil(v.x);
        result.y = (r32)ceil(v.y);
        return result;
    }
    
    inline v3 ceil(v3 v)
    {
        v3 result(v);
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
        return Abs(v);
    }
    
    v2 absolute(v2 v)
    {
        return math::v2(Abs(v.x), Abs(v.y));
    }
    
    v3 absolute(v3 v)
    {
        return math::v3(Abs(v.x), Abs(v.y), Abs(v.z));
    }
    
    v4 absolute(v4 v)
    {
        return math::v4((r32)Abs(v.x), (r32)Abs(v.y), (r32)Abs(v.z), (r32)Abs(v.w));
    }
    
    inline r32 square(r32 v)
    {
        return v * v;
    }
    
    inline r32 sqrt(r32 v)
    {
        return (r32)sqrt(v);
    }
    
    inline r32 pow(r32 v, i32 e)
    {
        return (r32)pow(v, e);
    }
    
    inline r32 sin(r32 v)
    {
        return (r32)sin(v);
    }
    
    inline r32 cos(r32 v)
    {
        return (r32)cos(v);
    }
    
    inline r32 a_cos(r32 v)
    {
        return (r32)acos(v);
    }
    
    inline r32 length(v2 v)
    {
        return sqrt(pow(v.x,2) + pow(v.y,2));
    }
    
    inline r32 length(v3 v)
    {
        return sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));
    }
    
    inline r32 length(v4 v)
    {
        return sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2) + pow(v.w,2));
    }
    
    inline r32 length(quat q)
    {
        return sqrt(pow(q.x, 2) + pow(q.y, 2) + pow(q.z, 2) + pow(q.w, 2));
    }
    
    inline v2 normalize(v2 v)
    {
        v2 result(v);
        auto l = length(v);
        if(l == 0.0f)
        {
            return result;
        }
        result /= l;
        return result;
    }
    
    inline v3 normalize(v3 v)
    {
        v3 result(v);
        auto l = length(v);
        if(l == 0.0f)
        {
            return result;
        }
        result /= l;
        return result;
    }
    
    inline v4 normalize(v4 v)
    {
        v4 result(v);
        auto l = length(v);
        if(l == 0.0f)
        {
            return result;
        }
        result /= l;
        return result;
    }
    
    inline r32 get_angle_in_radians(quat q)
    {
        return a_cos(q.w) * 2.0f;
    }
    
    inline v3 get_axis(quat q)
    {
        r32 angle = get_angle_in_radians(q);
        v3 result;
        result.x = q.x / sin(angle / 2.0f);
        result.y = q.y / sin(angle / 2.0f); 
        result.z = q.z / sin(angle / 2.0f); 
        return result;
    }
    
    inline m4 scale(m4 in, v3 scale)
    {
        m4 result(in);
        result.m11 = scale.x * result.m11;
        result.m22 = scale.y * result.m22;
        result.m33 = scale.z * result.m33;
        
        return result;
    }
    
    inline m4 translate(m4 in, v3 translate)
    {
        m4 result(in);
        result.m14 += translate.x;
        result.m24 += translate.y;
        result.m34 += translate.z;
        return result;
    }
    
    inline m4 x_rotate(r32 angle)
    {
        angle *= DEGREE_IN_RADIANS;
        
        r32 c = cos(angle);
        r32 s = sin(angle);
        
        m4 r(1,0, 0,0,
             0,c,-s,0,
             0,s, c,0,
             0,0, 0,1);
        
        return r;
    }
    
    inline m4 y_rotate(r32 angle)
    {
        angle *= DEGREE_IN_RADIANS;
        
        r32 c = cos(angle);
        r32 s = sin(angle);
        
        m4 r(c, 0,s,0,
             0, 1,0,0,
             -s,0,c,0,
             0, 0,0,1);
        
        return r;
    }
    
    inline m4 z_rotate(r32 angle)
    {
        angle *= DEGREE_IN_RADIANS;
        
        r32 c = cos(angle);
        r32 s = sin(angle);
        
        m4 r(c,-s,0,0,
             s,c,0,0,
             0,0,1,0,
             0,0,0,1);
        
        return r;
    }
    
    inline m4 create_rotation(r32 x_angle, r32 y_angle, r32 z_angle)
    {
        m4 result(1.0f);
        result = y_rotate(y_angle) * x_rotate(x_angle) * z_rotate(z_angle) * result;
        return result;
    }
    
    inline quat rotate(quat in, r32 a, v3 axis)
    {
        quat result(in);
        auto q = math::quat(axis.x, axis.y, axis.z, DEGREE_IN_RADIANS * a);
        result = in * q;
        result = normalize(result);
        return result;
    }
    
    // https://gamedev.stackexchange.com/a/50545
    inline v3 rotate(v3 in, quat q)
    {
        math::v3 u(q.x, q.y, q.z);
        
        r32 s = q.w;
        
        auto result = 2.0f * dot(u, in) * u 
            + (s * s - dot(u,u)) * in 
            + 2.0f * s * cross(u, in);
        return result;
    }
    
    inline m4 rotate(m4 m, quat r)
    {
        m4 result(1.0f);
        result = to_matrix(r) * m;
        return result;
    }
    
    inline v3 right(m4 m)
    {
        return normalize(math::v3(m[0][0],
                                  m[1][0],
                                  m[2][0]));
    }
    
    inline v3 up(m4 m)
    {
        return normalize(math::v3(m[0][1],
                                  m[1][1],
                                  m[2][1]));
    }
    
    inline v3 forward(m4 m)
    {
        return normalize(math::v3(m[0][2],
                                  m[1][2],
                                  m[2][2]));
    }
    
    inline v3 translation(m4 m)
    {
        return math::v3(m[0][3],
                        m[1][3],
                        m[2][3]);
    }
    
    inline v3 scale(m4 m)
    {
        math::v3 result;
        result.x = length(right(m));
        result.y = length(up(m));
        result.z = length(forward(m));
        return result;
    }
    
    inline v3 project(v3 in, m4 m, m4 p, v4 viewport)
    {
        v3 result(1.0f);
        auto tmp = v4(in, 1.0f);
        tmp = m * tmp;
        tmp = p * tmp;
        tmp /= tmp.w;
        
        tmp = tmp * 0.5f + 0.5f;
        tmp.x = tmp.x * viewport.z + viewport.x;
        tmp.y = tmp.y * viewport.w + viewport.y;
        
        return v3(tmp.x,tmp.y,tmp.z);
    }
    
    inline v3 cross(v3 a, v3 b)
    {
        v3 result;
        
        result.x = a.y*b.z - a.z*b.y;
        result.y = a.z*b.x - a.x*b.z;
        result.z = a.x*b.y - a.y*b.x;
        
        return result;
    }
    
    inline m4 ortho(r32 left, r32 right, r32 bottom, r32 top, r32 near, r32 far)
    {
        m4 result(1.0f);
        result.m11 = 2.0f/(right - left);
        result.m22 = 2.0f/(top - bottom);
        result.m33 = (-2.0f)/(far - near);
        result.m34 = -((far + near)/(far - near));
        result.m14 = -((right + left)/(right - left));
        result.m24 = -((top + bottom)/(top - bottom));
        result.m44 = 1.0f;
        
        return result;
    }
    
    inline m4 look_at(v3 p, v3 t)
    {
        auto f = normalize(p - t);
        auto u = v3(0.0f, 1.0f, 0.0f);
        auto r = normalize(cross(u, f));
        u = normalize(cross(f, r));
        
        m4 result(
            r.x, r.y, r.z, 0,
            u.x, u.y, u.z, 0,
            f.x, f.y, f.z, 0,
            0,   0,   0,   1
            );
        
        result = translate(result, -p);
        
        return result;
    }
    
    inline m4 perspective(r32 aspect_width_over_height, r32 focal_length, r32 near, r32 far)
    {
        r32 a = 1.0f;
        r32 b = aspect_width_over_height;
        r32 c = focal_length;
        
        r32 n = near;
        r32 f = far;
        
        r32 d = (n + f) / (n - f);
        r32 e = (2 * f * n) / (n - f);
        
        m4 result(
            a * c, 0.0f,  0.0f, 0.0f,
            0,     b * c, 0.0f, 0.0f,
            0.0f,  0.0f,  d,    e,
            0.0f,  0.0f, -1.0f, 0.0f
            );
        
        return result;
    }
    
    inline m4 frustum(r32 bottom, r32 top, r32 left, r32 right,
                      r32 near, r32 far)
    {
        
        auto a = 2 * near / (right - left);
        auto b = 2 * near / (top - bottom);
        auto c = (right + left) / (right - left);
        auto d = (top + bottom) / (top - bottom);
        auto e = -(far + near) / (far - near);
        auto f = -2 * far * near / (far - near);
        
        m4 result(
            a,    0.0f, c,     0.0f,
            0.0f, b,    d,     0.0f,
            0.0f, 0.0f, e,     f,
            0.0f, 0.0f, -1.0f, 0.0f
            );
        
        return result;
    }
    
    inline v3 mult_point_matrix(v3 in, m4 m)
    {
        math::v3 result(0.0f);
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
    
    
    inline v3 un_project(v3 in, m4 model, m4 projection, v4i viewport)
    {
        auto inv = inverse(projection * model);
        
        auto tmp = v4(in,1.0f);
        tmp.x = (tmp.x - viewport.x) / viewport.z;
        tmp.y = (tmp.y - viewport.y) / viewport.w;
        tmp = tmp * 2 - 1;
        
        auto obj = inv * tmp;
        obj /= obj.w;
        
        return v3(obj.x,obj.y,obj.z);
    }
    
    inline r32 lerp(r32 a, r32 t, r32 b)
    {
        Assert(t <= 1.0f);
        r32 result = (1.0f - t) * a + t * b;
        return result;
    }
    
    inline v2 lerp(v2 a, r32 t, v2 b)
    {
        Assert(t <= 1.0f);
        v2 result(0.0f);
        result.x = lerp(a.x,t,b.x);
        result.y = lerp(a.y,t,b.y);
        return result;
    }
    
    inline v3 lerp(v3 a, r32 t, v3 b)
    {
        Assert(t <= 1.0f);
        v3 result(0.0f);
        result.x = lerp(a.x,t,b.x);
        result.y = lerp(a.y,t,b.y);
        result.z = lerp(a.z,t,b.z);
        return result;
    }
    
    inline v4 lerp(v4 a, r32 t, v4 b)
    {
        Assert(t <= 1.0f);
        v4 result(0.0f);
        result.x = lerp(a.x,t,b.x);
        result.y = lerp(a.y,t,b.y);
        result.z = lerp(a.z,t,b.z);
        result.w = lerp(a.w,t,b.w);
        return result;
    }
    
    inline v2 rotate_by_angle(v2 in, r32 angle)
    {
        math::v2 result;
        result.x = in.x * cos(angle) - in.y * sin(angle);
        result.y = in.x * sin(angle) + in.y * cos(angle);
        return result;
    }
    
    inline r32 angle_from_direction(v2 in)
    {
        return (r32)atan2(in.x, in.y);
    }
    
    inline void seed_random(u32 seed)
    {
        srand(seed);
    }
    
    // @Incomplete:(Niels): Doesn't work (I think)
    inline r32 random_float(r32 from, r32 to)
    {
        r32 rand = Min(Max(From, ((r32)rand()/(r32)(RAND_MAx)) * to),to);
        return rand;
    }
    
    inline i32 random_int(i32 from, i32 to)
    {
        return rand() % to + from;
    }
    
    struct Ray
    {
        v3 origin;
        v3 target;
        v3 ray;
    };
    
    inline Ray cast_picking_ray(r32 mouse_x, r32 mouse_y, m4 p, m4 v, r32 width, r32 height)
    {
        auto mx = (2.0f * mouse_x) / width - 1.0f;
        auto my = 1.0f - (2.0f * mouse_y / height);
        
        // 1.0f is the far plane in NDC
        auto mouse = inverse(p) * math::v3(mx, my, 1.0f);
        mouse.z = -1.0f;
        mouse = inverse(v) * mouse;
        
        // -1.0f is the near plane in NDC
        auto origin = inverse(p) * math::v3(mx, my, -1.0f);
        origin.z = 1.0f;
        origin = inverse(v) * origin;
        
        auto temp_ray = math::v4(mouse - origin, 0.0f);
        temp_ray = normalize(temp_ray);
        Ray ray;
        ray.Origin = origin;
        ray.target = mouse;
        ray.Ray = temp_ray.xyz;
        return ray;
    }
    
    inline Ray cast_ray(v3 origin, v3 target)
    {
        Ray ray;
        ray.Origin = origin;
        ray.target = target;
        ray.Ray = normalize(math::v4(target - origin, 0.0f)).xyz;
        return ray;
    }
    
    using rgb = v3;
    using rgba = v4;
}

#endif

struct Rect
{
    union
    {
        struct
        {
            r32 x;
            r32 y;
        };
        math::v2 position;
    };
    union
    {
        struct
        {
            r32 width;
            r32 height;
        };
        math::v2 size;
    };
    
    
    Rect() {}
    Rect(r32 x, r32 y, r32 width, r32 height) : x(x), y(y), width(width), height(height) {}
    Rect(i32 x, i32 y, i32 width, i32 height) : x((r32)x), y((r32)y), width((r32)width), height((r32)height) {}
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
        math::v2i position;
    };
    union
    {
        struct
        {
            i32 width;
            i32 height;
        };
        math::v2i size;
    };
    
    
    Recti() {}
    Recti(i32 x, i32 y, i32 width, i32 height) : x(x), y(y), width(width), height(height) {}
};

inline r32 sign(math::v2 p1, math::v2 p2, math::v2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

inline math::v2 to_cartesian(math::v2 position)
{
    // @Cleanup: Move these to a global variable or similar
    r32 tile_width_half = 0.5f;
    r32 tile_height_half = 0.25f;
    
    math::v2 temp_pt;
    
    temp_pt.x = (position.x / tile_width_half + position.y / tile_height_half) / 2.0f;
    temp_pt.y = (position.y / tile_height_half - position.x / tile_width_half) / 2.0f;
    return temp_pt;
}

inline math::v2 to_isometric(math::v2 position)
{
    // @Cleanup: Move these to a global variable or similar
    r32 tile_width_half = 0.5f;
    r32 tile_height_half = 0.25f;
    
    math::v2 temp_pt;
    temp_pt.x = (position.x - position.y) * tile_width_half;
    temp_pt.y = (position.x + position.y) * tile_height_half;
    //return tempPt;
    return position;
}

inline b32 point_in_triangle(math::v2 pt, math::v2 v1, math::v2 v2, math::v2 v3)
{
    bool b1, b2, b3;
    
    b1 = sign(pt, v1, v2) < 0.0f;
    b2 = sign(pt, v2, v3) < 0.0f;
    b3 = sign(pt, v3, v1) < 0.0f;
    
    return ((b1 == b2) && (b2 == b3));
}

#endif
