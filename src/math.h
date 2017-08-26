#ifndef MATH_H
#define MATH_H

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
    
    /*PrintMatrix(m4 In)
    {
        DEBUG_PRINT("GLM: \n");
        DEBUG_PRINT("%f %f %f %f\n", In[0][0],In[1][0],In[2][0],In[3][0]);
        DEBUG_PRINT("%f %f %f %f\n", In[0][1],In[1][1],In[2][1],In[3][1]);
        DEBUG_PRINT("%f %f %f %f\n", In[0][2],In[1][2],In[2][2],In[3][2]);
        DEBUG_PRINT("%f %f %f %f\n", In[0][3],In[1][3],In[3][3],In[3][3]);
    }*/
    
    r32 Dot(v2 V1, v2 V2)
    {
        return glm::dot(V1,V2);
    }
    
    r32 Dot(v3 V1, v3 V2)
    {
        return glm::dot(V1,V2);
    }
    
    r32 Dot(v4 V1, v4 V2)
    {
        return glm::dot(V1,V2);
    }
    
    r32 Distance(v2 V1, v2 V2)
    {
        return glm::distance(V1,V2);
    }
    
    m4 Translate(m4 M, v3 V)
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
    
    m4 Transpose(m4 M)
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
    
    m4 Ortho(r32 L, r32 R, r32 B, r32 T, r32 N, r32 F)
    {
        return glm::ortho(L,R,B,T,N,F);
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
    
    inline i32 Clamp(i32 Minimum, i32 Value, i32 Maximum)
    {
        i32 Result = Max(Minimum, Min(Value,Maximum));
        return Result;
    }
    
    inline r32 Clamp(r32 Minimum, r32 Value, r32 Maximum)
    {
        return Max(Minimum, Min(Value,Maximum));
    }
    
    inline r64 Clamp(r64 Minimum, r64 Value, r64 Maximum)
    {
        return Max(Minimum, Min(Value,Maximum));
    }
    
    
    union v2
    {
        struct
        {
            r32 X;
            r32 Y;
        };
        struct
        {
            r32 x;
            r32 y;
        };
        struct
        {
            r32 U;
            r32 V;
        };
        r32 E[2];
        v2(r32 X, r32 Y) : X(X), Y(Y){}
        v2() : X(0.0f), Y(0.0f) {}
        v2(r32 I) : E{I,I} {}
        v2(r32 E[2]) : E{E[0],E[1]} {}
        v2(const v2& O) = default;
        v2(i32 X, i32 Y) : X((r32)X), Y((r32)Y) {}
        v2(r64 X, r64 Y) : X((r32)X), Y((r32)Y) {}
        v2(r32 X, r64 Y) : X(X), Y((r32)Y) {}
        v2(r32 X, i32 Y) : X(X), Y((r32)Y) {}
        v2(i32 X, r32 Y) : X((r32)X), Y(Y) {}
        v2(i32 X, r64 Y) : X((r32)X), Y((r32)Y) {}
        v2(r64 X, i32 Y) : X((r32)X), Y((r32)Y) {}
        v2(r64 X, r32 Y) : X((r32)X), Y(Y) {}
        
        r32 operator[](i32 I)
        {
            return this->E[I];
        }
        
        inline v2 operator* (v2 O)
        {
            v2 Result(*this);
            Result.X *= O.X;
            Result.Y *= O.Y;
            return Result;
        }
        
        inline v2 operator+ (v2 O)
        {
            v2 Result(*this);
            Result.X += O.X;
            Result.Y += O.Y;
            return Result;
        }
        
        inline void operator*= (v2 O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
        }
        
        inline void operator+= (v2 O)
        {
            this->X += O.X;
            this->Y += O.Y;
        }
        
        inline v2 operator+ (r32 S)
        {
            v2 Result(*this);
            Result.X += S;
            Result.Y += S;
            return Result;
        }
        
        inline v2 operator* (r32 S)
        {
            v2 Result(*this);
            Result.X *= S;
            Result.Y *= S;
            return Result;
        }
        
        inline v2 operator/ (r32 S)
        {
            v2 Result(*this);
            Result.X /= S;
            Result.Y /= S;
            return Result;
        }
        
        inline void operator+= (r32 S)
        {
            this->X += S;
            this->Y += S;
        }
        
        inline void operator*= (r32 S)
        {
            this->X *= S;
            this->Y *= S;
        }
        
        inline void operator/= (r32 S)
        {
            this->X /= S;
            this->Y /= S;
        }
        
        inline void operator-= (r32 S)
        {
            this->X -= S;
            this->Y -= S;
        }
        
        inline v2 operator- (v2 O)
        {
            v2 Result(*this);
            Result.X -= O.X;
            Result.Y -= O.Y;
            return Result;
        }
        
        inline void operator-= (v2 O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
        }
        
        inline v2 operator- (r32 S)
        {
            v2 Result(*this);
            Result.X -= S;
            Result.Y -= S;
            return Result;
        }
        
        inline v2 operator/ (v2 O)
        {
            v2 Result(*this);
            Result.X /= O.X;
            Result.Y /= O.Y;
            return Result;
        }
        
        inline void operator/= (v2 O)
        {
            this->X /= O.X;
            this->Y /= O.Y;
        }
    };
    
    union v3
    {
        struct 
        {
            union
            {
                v2 xy;
                v2 XY;
                struct
                {
                    r32 x, y;
                };
                struct 
                {
                    r32 X, Y;
                };
            };
            union
            {
                r32 z;
                r32 Z;
            };
            
        };
        struct 
        {
            union
            {
                v2 rg;
                v2 RG;
                struct
                {
                    r32 r, g;
                };
                struct 
                {
                    r32 R, G;
                };
            };
            union
            {
                r32 b;
                r32 B;
            };
            
        };
        
        r32 E[3];
        v3(r32 X, r32 Y, r32 Z) : X(X), Y(Y), Z(Z) {}
        v3() : X(0.0f), Y(0.0f), Z(0.0f) {}
        v3(r32 I) : E{I,I,I} {}
        v3(r32 E[3]) : E{E[0],E[1], E[2]} {}
        v3(const v3& O) : E{O.X, O.Y, O.Z} {}
        v3(r64 X, r64 Y, r64 Z) : X((r32)X), Y((r32)Y), Z((r32)Z) {}
        v3(r64 X, i32 Y, r64 Z) : X((r32)X), Y((r32)Y), Z((r32)Z) {}
        v3(i32 X, i32 Y, i32 Z) : X((r32)X), Y((r32)Y), Z((r32)Z) {}
        v3(i32 X, r32 Y, i32 Z) : X((r32)X), Y(Y), Z((r32)Z) {}
        v3(r64 X, r64 Y, i32 Z) : X((r32)X), Y((r32)Y), Z((r32)Z) {}
        v3(r32 X, r32 Y, i32 Z) : X(X), Y(Y), Z((r32)Z) {}
        v3(r32 X, i32 Y, i32 Z) : X(X), Y((r32)Y), Z((r32)Z) {}
        v3(i32 X, i32 Y, r32 Z) : X((r32)X), Y((r32)Y), Z(Z) {}
        v3(r32 X, r32 Y, r64 Z) : X(X), Y(Y), Z((r32)Z) {}
        v3(r32 X, i32 Y, r32 Z) : X(X), Y((r32)Y), Z(Z) {}
        v3(r64 X, r32 Y, r64 Z) : X((r32)X), Y(Y), Z((r32)Z) {}
        
        r32 operator[](i32 I)
        {
            return this->E[I];
        }
        
        inline v3 operator-()
        {
            v3 Result(1.0f);
            Result.X = -this->X;
            Result.Y = -this->Y;
            Result.Z = -this->Z;
            return Result;
        }
        
        inline v3 operator* (v3 O)
        {
            v3 Result(*this);
            Result.X *= O.X;
            Result.Y *= O.Y;
            Result.Z *= O.Z;
            return Result;
        }
        
        inline v3 operator+ (v3 O)
        {
            v3 Result(*this);
            Result.X += O.X;
            Result.Y += O.Y;
            Result.Z += O.Z;
            return Result;
        }
        
        inline void operator*= (v3 O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
            this->Z *= O.Z;
        }
        
        inline void operator+= (v3 O)
        {
            this->X += O.X;
            this->Y += O.Y;
            this->Z += O.Z;
        }
        
        inline v3 operator+ (r32 S)
        {
            v3 Result(*this);
            Result.X += S;
            Result.Y += S;
            Result.Z += S;
            return Result;
        }
        
        inline v3 operator* (r32 S)
        {
            v3 Result(*this);
            Result.X *= S;
            Result.Y *= S;
            Result.Z *= S;
            return Result;
        }
        
        inline v3 operator/ (r32 S)
        {
            v3 Result(*this);
            Result.X /= S;
            Result.Y /= S;
            Result.Z /= S;
            return Result;
        }
        
        inline void operator+= (r32 S)
        {
            this->X += S;
            this->Y += S;
            this->Z += S;
        }
        
        inline void operator*= (r32 S)
        {
            this->X *= S;
            this->Y *= S;
            this->Z *= S;
        }
        
        inline void operator/= (r32 S)
        {
            this->X /= S;
            this->Y /= S;
            this->Z /= S;
        }
        
        inline void operator-= (r32 S)
        {
            this->X -= S;
            this->Y -= S;
            this->Z -= S;
        }
        
        inline v3 operator- (v3 O)
        {
            v3 Result(*this);
            Result.X -= O.X;
            Result.Y -= O.Y;
            Result.Z -= O.Z;
            return Result;
        }
        
        inline void operator-= (v3 O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
            this->Z -= O.Z;
        }
        
        inline v3 operator- (r32 S)
        {
            v3 Result(*this);
            Result.X -= S;
            Result.Y -= S;
            Result.Z -= S;
            return Result;
        }
        
        inline v3 operator/ (v3 O)
        {
            v3 Result(*this);
            Result.X /= O.X;
            Result.Y /= O.Y;
            Result.Z /= O.Z;
            return Result;
        }
        
        inline void operator/= (v3 O)
        {
            this->X /= O.X;
            this->Y /= O.Y;
            this->Z /= O.Z;
        }
    };
    
    union v4
    {
        struct 
        {
            union
            {
                v3 xyz;
                v3 XYZ;
                struct
                {
                    r32 x, y, z;
                };
                struct 
                {
                    r32 X, Y, Z;
                };
            };
            union
            {
                r32 w;
                r32 W;
            };
            
        };
        struct 
        {
            union
            {
                v3 rgb;
                v3 RGB;
                struct
                {
                    r32 r, g, b;
                };
                struct
                {
                    r32 R, G, B;
                };
            };
            union
            {
                r32 a;
                r32 A;
            };
            
        };
        r32 E[4];
        
        v4(r32 X, r32 Y, r32 Z, r32 W) : X(X), Y(Y), Z(Z), W(W) {}
        v4() : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) {}
        v4(r32 I) : E{I,I,I,I} {}
        v4(r32 E[4]) : E{E[0],E[1], E[2], E[3]} {}
        v4(const v4& O) : X(O.X), Y(O.Y), Z(O.Z), W(O.W) {}
        
        v4(i32 X, i32 Y, i32 Z, i32 W) : 
        X((r32)X), Y((r32)Y), Z((r32)Z), W((r32)W) {}
        
        v4(r32 X, r32 Y, r32 Z, i32 W) : 
        X(X), Y(Y), Z(Z), W((r32)W) {}
        
        v4(r64 X, r64 Y, r64 Z, r64 W) : 
        X((r32)X), Y((r32)Y), Z((r32)Z), W((r32)W) {}
        
        v4(r64 X, r64 Y, r64 Z, i32 W) : 
        X((r32)X), Y((r32)Y), Z((r32)Z), W((r32)W) {}
        
        v4(r32 X, i32 Y, r32 Z, i32 W) : 
        X(X), Y((r32)Y), Z(Z), W((r32)W) {}
        
        v4(i32 X, r64 Y, i32 Z, i32 W) : 
        X((r32)X), Y((r32)Y), Z((r32)Z), W((r32)W) {}
        
        v4(r64 X, i32 Y, i32 Z, i32 W) : 
        X((r32)X), Y((r32)Y), Z((r32)Z), W((r32)W) {}
        
        v4(i32 X, i32 Y, i32 Z, r64 W) : 
        X((r32)X), Y((r32)Y), Z((r32)Z), W((r32)W) {}
        
        v4(r32 X, i32 Y, i32 Z, i32 W) : 
        X(X), Y((r32)Y), Z((r32)Z), W((r32)W) {}
        
        v4(i32 X, i32 Y, i32 Z, r32 W) : 
        X((r32)X), Y((r32)Y), Z((r32)Z), W(W) {}
        
        v4(r64 X, r64 Y, i32 Z, r64 W) : 
        X((r32)X), Y((r32)Y), Z((r32)Z), W((r32)W) {}
        
        v4(v3 O, r32 W) : X(O.X), Y(O.Y), Z(O.Z), W(W) {}
        
        r32 operator[](i32 I)
        {
            return this->E[I];
        }
        
        inline v4 operator* (v4 O)
        {
            v4 Result(*this);
            Result.X *= O.X;
            Result.Y *= O.Y;
            Result.Z *= O.Z;
            Result.W *= O.W;
            return Result;
        }
        
        inline v4 operator+ (v4 O)
        {
            v4 Result(*this);
            Result.X += O.X;
            Result.Y += O.Y;
            Result.Z += O.Z;
            Result.W += O.W;
            return Result;
        }
        
        inline void operator*= (v4 O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
            this->Z *= O.Z;
            this->W *= O.W;
        }
        
        inline void operator+= (v4 O)
        {
            this->X += O.X;
            this->Y += O.Y;
            this->Z += O.Z;
            this->W += O.W;
        }
        
        inline v4 operator+ (r32 S)
        {
            v4 Result(*this);
            Result.X += S;
            Result.Y += S;
            Result.Z += S;
            Result.W += S;
            return Result;
        }
        
        inline v4 operator* (r32 S)
        {
            v4 Result(*this);
            Result.X *= S;
            Result.Y *= S;
            Result.Z *= S;
            Result.W *= S;
            return Result;
        }
        
        inline v4 operator/ (r32 S)
        {
            v4 Result(*this);
            Result.X /= S;
            Result.Y /= S;
            Result.Z /= S;
            Result.W /= S;
            return Result;
        }
        
        inline void operator+= (r32 S)
        {
            this->X += S;
            this->Y += S;
            this->Z += S;
            this->W += S;
        }
        
        inline void operator*= (r32 S)
        {
            this->X *= S;
            this->Y *= S;
            this->Z *= S;
            this->W *= S;
        }
        
        inline void operator/= (r32 S)
        {
            this->X /= S;
            this->Y /= S;
            this->Z /= S;
            this->W /= S;
        }
        
        inline void operator-= (r32 S)
        {
            this->X -= S;
            this->Y -= S;
            this->Z -= S;
            this->W -= S;
        }
        
        inline v4 operator- (v4 O)
        {
            v4 Result(*this);
            Result.X -= O.X;
            Result.Y -= O.Y;
            Result.Z -= O.Z;
            Result.W -= O.W;
            return Result;
        }
        
        inline void operator-= (v4 O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
            this->Z -= O.Z;
            this->W -= O.W;
        }
        
        inline v4 operator- (r32 S)
        {
            v4 Result(*this);
            Result.X -= S;
            Result.Y -= S;
            Result.Z -= S;
            Result.W -= S;
            return Result;
        }
        
        inline v4 operator/ (v4 O)
        {
            v4 Result(*this);
            Result.X /= O.X;
            Result.Y /= O.Y;
            Result.Z /= O.Z;
            Result.W /= O.W;
            return Result;
        }
        
        inline void operator/= (v4 O)
        {
            this->X /= O.X;
            this->Y /= O.Y;
            this->Z /= O.Z;
            this->W /= O.W;
        }
    };
    
    union v2i
    {
        struct
        {
            i32 X,Y;
        };
        struct
        {
            i32 x,y;
        };
        i32 E[2];
        v2i(i32 X, i32 Y) : X(X), Y(Y){}
        v2i() : X(0),Y(0) {}
        v2i(i32 I) : X(I), Y(I) {}
        v2i(i32 E[2]) : E{E[0],E[1]} {}
        
        inline v2i operator* (v2i O)
        {
            v2i Result(*this);
            Result.X *= O.X;
            Result.Y *= O.Y;
            return Result;
        }
        
        inline v2i operator+ (v2i O)
        {
            v2i Result(*this);
            Result.X += O.X;
            Result.Y += O.Y;
            return Result;
        }
        
        inline void operator*= (v2i O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
        }
        
        inline void operator+= (v2i O)
        {
            this->X += O.X;
            this->Y += O.Y;
        }
        
        inline v2i operator+ (i32 S)
        {
            v2i Result(*this);
            Result.X += S;
            Result.Y += S;
            return Result;
        }
        
        inline v2i operator* (i32 S)
        {
            v2i Result(*this);
            Result.X *= S;
            Result.Y *= S;
            return Result;
        }
        
        inline v2i operator/ (i32 S)
        {
            v2i Result(*this);
            Result.X /= S;
            Result.Y /= S;
            return Result;
        }
        
        inline void operator+= (i32 S)
        {
            this->X += S;
            this->Y += S;
        }
        
        inline void operator*= (i32 S)
        {
            this->X *= S;
            this->Y *= S;
        }
        
        inline void operator/= (i32 S)
        {
            this->X /= S;
            this->Y /= S;
        }
        
        inline void operator-= (i32 S)
        {
            this->X -= S;
            this->Y -= S;
        }
        
        inline v2i operator- (v2i O)
        {
            v2i Result(*this);
            Result.X -= O.X;
            Result.Y -= O.Y;
            return Result;
        }
        
        inline void operator-= (v2i O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
        }
        
        inline v2i operator- (i32 S)
        {
            v2i Result(*this);
            Result.X -= S;
            Result.Y -= S;
            return Result;
        }
        
        inline v2i operator/ (v2i O)
        {
            v2i Result(*this);
            Result.X /= O.X;
            Result.Y /= O.Y;
            return Result;
        }
        
        inline void operator/= (v2i O)
        {
            this->X /= O.X;
            this->Y /= O.Y;
        }
    };
    
    union v3i
    {
        struct
        {
            i32 X,Y,Z;
        };
        struct
        {
            i32 x, y, z;
        };
        i32 E[3];
        v3i(i32 X, i32 Y, i32 Z) : X(X), Y(Y), Z(Z){}
        v3i() : X(0), Y(0), Z(0) {}
        v3i(i32 I) : X(I), Y(I), Z(I) {}
        v3i(i32 E[3]) : E{E[0],E[1], E[2]} {}
        
        inline v3i operator* (v3i O)
        {
            v3i Result(*this);
            Result.X *= O.X;
            Result.Y *= O.Y;
            return Result;
        }
        
        inline v3i operator+ (v3i O)
        {
            v3i Result(*this);
            Result.X += O.X;
            Result.Y += O.Y;
            return Result;
        }
        
        inline void operator*= (v3i O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
        }
        
        inline void operator+= (v3i O)
        {
            this->X += O.X;
            this->Y += O.Y;
        }
        
        inline v3i operator+ (i32 S)
        {
            v3i Result(*this);
            Result.X += S;
            Result.Y += S;
            return Result;
        }
        
        inline v3i operator* (i32 S)
        {
            v3i Result(*this);
            Result.X *= S;
            Result.Y *= S;
            return Result;
        }
        
        inline v3i operator/ (i32 S)
        {
            v3i Result(*this);
            Result.X /= S;
            Result.Y /= S;
            return Result;
        }
        
        inline void operator+= (i32 S)
        {
            this->X += S;
            this->Y += S;
        }
        
        inline void operator*= (i32 S)
        {
            this->X *= S;
            this->Y *= S;
        }
        
        inline void operator/= (i32 S)
        {
            this->X /= S;
            this->Y /= S;
        }
        
        inline void operator-= (i32 S)
        {
            this->X -= S;
            this->Y -= S;
            this->Z -= S;
        }
        
        inline v3i operator- (v3i O)
        {
            v3i Result(*this);
            Result.X -= O.X;
            Result.Y -= O.Y;
            Result.Z -= O.Z;
            return Result;
        }
        
        inline void operator-= (v3i O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
            this->Z -= O.Z;
        }
        
        inline v3i operator- (i32 S)
        {
            v3i Result(*this);
            Result.X -= S;
            Result.Y -= S;
            Result.Z -= S;
            return Result;
        }
        
        inline v3i operator/ (v3i O)
        {
            v3i Result(*this);
            Result.X /= O.X;
            Result.Y /= O.Y;
            Result.Z /= O.Z;
            return Result;
        }
        
        inline void operator/= (v3i O)
        {
            this->X /= O.X;
            this->Y /= O.Y;
            this->Z /= O.Z;
        }
        
    };
    
    union m4
    {
        struct
        {
            r32 M11,M12,M13,M14;
            r32 M21,M22,M23,M24;
            r32 M31,M32,M33,M34;
            r32 M41,M42,M43,M44;
        };
        struct
        {
            r32 A, B, C, D;
            r32 E, F, G, H;
            r32 I, J, K, L;
            r32 M, N, O, P;
        };
        struct
        {
            r32 M0[4];
            r32 M1[4];
            r32 M2[4];
            r32 M3[4];
        };
        r32 V[4][4];
        
        inline r32* operator[](i32 Idx)
        {
            return this->V[Idx];
        }
        
        m4() : V{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}} {}
        m4(r32 M11, r32 M12, r32 M13, r32 M14, 
           r32 M21, r32 M22, r32 M23, r32 M24,
           r32 M31, r32 M32, r32 M33, r32 M34,
           r32 M41, r32 M42, r32 M43, r32 M44) : 
        M11(M11), M12(M12), M13(M13), M14(M14),
        M21(M21), M22(M22), M23(M23), M24(M24),
        M31(M31), M32(M32), M33(M33), M34(M34),
        M41(M41), M42(M42), M43(M43), M44(M44) {}
        
        m4(r32 M0[4], r32 M1[4], r32 M2[4], r32 M3[4]) : 
        M0 {M0[0],M0[1],M0[2],M0[3]}, 
        M1 {M1[0],M1[1],M1[2],M1[3]}, 
        M2 {M2[0],M2[1],M2[2],M2[3]}, 
        M3 {M3[0],M3[1],M3[2],M3[3]} {}
        
        m4(r32 V[4][4]) : 
        V{ {V[0][0],V[0][1],V[0][2],V[0][3]}, 
            {V[1][0],V[1][1],V[1][2],V[1][3]}, 
            {V[2][0],V[2][1],V[2][2],V[2][3]}, 
            {V[3][0],V[3][1],V[3][2],V[3][3]}}
        {}
        
        m4(r32 I) : V {{I,0,0,0},{0,I,0,0},{0,0,I,0},{0,0,0,I}} {}
        
        m4(const m4& O) : V{ {O.V[0][0],O.V[0][1],O.V[0][2],O.V[0][3]}, 
            {O.V[1][0],O.V[1][1],O.V[1][2],O.V[1][3]}, 
            {O.V[2][0],O.V[2][1],O.V[2][2],O.V[2][3]}, 
            {O.V[3][0],O.V[3][1],O.V[3][2],O.V[3][3]}} {}
        
        m4 operator*(m4 Other)
        {
            m4 Result(*this);
            for(i32 Outer = 0; Outer < 4; Outer++)
            {
                for(i32 Inner = 0; Inner < 4; Inner++)
                {
                    r32 Sum = 0;
                    for(i32 Shared = 0; Shared < 4; Shared++)
                    {
                        Sum += this->V[Inner][Shared] * Other.V[Shared][Outer];
                    }
                    Result.V[Inner][Outer] = Sum;
                }
            }
            return Result;
        }
        
        // Only __absolute__ convenience: __always__ better to control order yourself
        void operator *= (m4 Other)
        {
            memcpy(this->V,(Other * (*this)).V, sizeof(r32) * 4 * 4);
        }
        
        inline m4 operator*(r32 S)
        {
            m4 Result(*this);
            Result.M11 *= S;
            Result.M12 *= S;
            Result.M13 *= S;
            Result.M14 *= S;
            Result.M21 *= S;
            Result.M22 *= S;
            Result.M23 *= S;
            Result.M24 *= S;
            Result.M31 *= S;
            Result.M32 *= S;
            Result.M33 *= S;
            Result.M34 *= S;
            Result.M41 *= S;
            Result.M42 *= S;
            Result.M43 *= S;
            Result.M44 *= S;
            return Result;
        }
        
    };
    
    union quat
    {
        struct
        {
            r32 x, y, z, w;
        };
        struct
        {
            r32 X, Y, Z, W;
        };
        struct
        {
            v4 AxisAngle;
        };
        struct
        {
            v3 Axis;
            r32 Angle;
        };
        
        // Identity quaternion
        quat() : X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) {}
        quat(r32 X, r32 Y, r32 Z, r32 Angle) : Axis(v3(X * sin(Angle / 2.0f), Y * sin(Angle / 2.0f), Z * sin(Angle / 2.0f))), Angle(cos(Angle / 2.0f)) {}
        quat(v3 Axis, r32 Angle) : 
        Axis(v3(Axis.X * sin(Angle / 2.0f), Axis.Y * sin(Angle / 2.0f), Axis.Z * sin(Angle / 2.0f))),
        Angle(cos(Angle / 2.0f)) {}
        quat(const quat& O) : Axis(O.Axis), Angle(O.Angle) {}
        
        inline quat operator+ (quat In)
        {
            quat Result(*this);
            Result.x += In.X;
            Result.y += In.Y;
            Result.z += In.Z;
            Result.w += In.W;
            return Result;
        }
        
        inline void operator+= (quat In)
        {
            this->X += In.X;
            this->Y += In.Y;
            this->Z += In.Z;
            this->W += In.W;
        }
        
        inline quat operator- (quat In)
        {
            quat Result(*this);
            Result.x -= In.X;
            Result.y -= In.Y;
            Result.z -= In.Z;
            Result.w -= In.W;
            return Result;
        }
        
        inline quat operator* (quat In)
        {
            quat Result(*this);
            Result.w = this->w * In.w - this->x * In.x - this->y * In.y - this->z * In.z;
            Result.x = this->w * In.x + this->x * In.w + this->y * In.z - this->z * In.y;
            Result.y = this->w * In.y - this->x * In.z + this->y * In.w + this->z * In.z;
            Result.z = this->w * In.z + this->x * In.y - this->y * In.x + this->z * In.z;
            return Result;
        }
        
        inline void operator*= (quat In)
        {
            auto Result = *this * In;
            this->X = Result.X;
            this->Y = Result.Y;
            this->Z = Result.Z;
            this->W = Result.W;
        }
        
        inline quat operator* (r32 V)
        {
            quat Result(*this);
            Result.w *= V;
            Result.x *= V;
            Result.y *= V;
            Result.z *= V;
            return Result;
        }
        
        inline quat operator/ (r32 V)
        {
            quat Result(*this);
            
            Result.W /= V;
            Result.X /= V;
            Result.Y /= V;
            Result.Z /= V;
            
            return Result;
        }
    };
    
    inline quat operator* (r32 V, quat Q)
    {
        quat Result(Q);
        Result.w *= V;
        Result.x *= V;
        Result.y *= V;
        Result.z *= V;
        return Result;
    }
    
    inline r32 Dot(quat Q1, quat Q2)
    {
        r32 Result;
        Result = Q1.w * Q2.w + Q1.x * Q2.x + Q1.y * Q2.y + Q1.z * Q2.z;
        return Result;
    }
    
    inline quat Conjugate(quat In)
    {
        quat Result(-In.Axis, In.w);
        return Result;
    }
    
    inline r32 Magnitude(quat In)
    {
        r32 Result = 0.0f;
        Result = In.W * In.W + In.X * In.X + In.Y * In.Y + In.Z * In.Z;
        Result = sqrt(Result);
        return Result;
    }
    
    inline quat Normalize(quat In)
    {
        return In / Magnitude(In);
    }
    
    //@Incomplete JBlow, CaseyM, ShawnM say don't use this
    inline quat Slerp(quat Q0, quat Q1, r32 T)
    {
        Q0 = Normalize(Q0);
        Q1 = Normalize(Q1);
        
        auto DotP = Dot(Q0, Q1);
        
        const r64 DOT_THRESHOLD = 0.9995;
        if(DotP > DOT_THRESHOLD)
        {
            quat Result = Q0 + (1.0f - T) * (Q1 - Q0);
            Result = Normalize(Result);
            return Result;
        }
        
        Clamp(DotP, -1.0f, 1.0f);
        auto Theta_0 = acos(DotP);
        auto Theta = Theta_0 * T;
        
        auto Q2 = Q1 - Q0 * DotP;
        Q2 = Normalize(Q2);
        
        auto Result = Q0 * cos(Theta) + Q2 * sin(Theta);
        Result = Normalize(Result);
        return Result;
    }
    
    inline quat Lerp(quat Q0, r32 T, quat Q1)
    {
        return (1.0f - T) * Q0 + T * Q1;
    }
    
    inline quat NLerp(quat Q0, quat Q1, r32 T)
    {
        Q0 = Normalize(Q0);
        Q1 = Normalize(Q1);
        return Normalize(Lerp(Q0, T, Q1));
    }
    
    inline m4 Transpose(m4 In)
    {
        m4 Result(In);
        Result.M11 = In.M11;
        Result.M12 = In.M21;
        Result.M13 = In.M31;
        Result.M14 = In.M41;
        Result.M21 = In.M12;
        Result.M22 = In.M22;
        Result.M23 = In.M32;
        Result.M24 = In.M42;
        Result.M31 = In.M13;
        Result.M32 = In.M23;
        Result.M33 = In.M33;
        Result.M34 = In.M43;
        Result.M41 = In.M14;
        Result.M42 = In.M24;
        Result.M43 = In.M34;
        Result.M44 = In.M44;
        return Result;
    }
    
    inline m4 ToMatrix(quat In)
    {
        /*m4 Result(1.0f);
        Result[0][0] = In.W * In.W + In.X * In.X - In.Y * In.Y - In.Z * In.Z;
        Result[1][0] = 2 * In.X * In.Y + 2 * In.W * In.Z;
        Result[2][0] = 2 * In.X * In.Z - 2 * In.W * In.Y;
        Result[3][0] = 0.0f;
        Result[0][1] = 2 * In.X * In.Y - 2 * In.W * In.Z;
        Result[1][1] = In.W * In.W - In.X * In.X + In.Y * In.Y - In.Z * In.Z;
        Result[2][1] = 2 * In.Y * In.Z - 2 * In.W * In.X;
        Result[3][1] = 2 * In.Y * In.Z + 2 * In.W * In.X;
        Result[0][2] = 2 * In.X * In.Z + 2 * In.W * In.Y;
        Result[1][2] = 2 * In.Y * In.Z + 2 * In.W * In.X;
        Result[2][2] = In.W * In.W - In.X * In.X + In.Y * In.Y + In.Z * In.Z;
        Result[3][2] = 0.0f;
        Result[0][3] = 0.0f;
        Result[1][3] = 0.0f;
        Result[2][3] = 0.0f;
        Result[3][3] = 1.0f;
        
        Result = Transpose(Result);
        */
        
        m4 Result(1.0f);
        Result[0][0] = 1.0f - 2.0f * In.Y * In.Y - 2.0f * In.Z * In.Z;
        Result[1][0] = 2.0f * In.X * In.Y + 2.0f * In.Z * In.W;
        Result[2][0] = 2.0f * In.X * In.Z - 2.0f * In.Y * In.W;
        Result[3][0] = 0.0f;
        Result[0][1] = 2.0f * In.X * In.Y - 2.0f * In.Z * In.W;
        Result[1][1] = 1.0f - 2.0f * In.X * In.X - 2.0f * In.Z * In.Z;
        Result[2][1] = 2.0f * In.Y * In.Z + 2.0f * In.X * In.W;
        Result[3][1] = 0.0f;
        Result[0][2] = 2.0f * In.X * In.Z + 2.0f * In.Y * In.W;
        Result[1][2] = 2.0f * In.Y * In.Z - 2.0f * In.X * In.W;
        Result[2][2] = 1.0f - 2.0f * In.X * In.X - 2.0f * In.Y * In.Y;
        Result[3][2] = 0.0f;
        Result[0][3] = 0.0f;
        Result[1][3] = 0.0f;
        Result[2][3] = 0.0f;
        Result[3][3] = 1.0f;
        
        Result = Transpose(Result);
        
        return Result;
    }
    
    inline v4 Transform(m4& M, const v4& V)
    {
        v4 R(0.0f);
        
        R.x = V.x * M[0][0] + V.y * M[0][1] + V.z * M[0][2] + V.w * M[0][3];
        R.y = V.x * M[1][0] + V.y * M[1][1] + V.z * M[1][2] + V.w * M[1][3];
        R.z = V.x * M[2][0] + V.y * M[2][1] + V.z * M[2][2] + V.w * M[2][3];
        R.w = V.x * M[3][0] + V.y * M[3][1] + V.z * M[3][2] + V.w * M[3][3];
        
        return R;
    }
    
    inline v3 operator*(m4 M, const v3& V)
    {
        v3 R = Transform(M,v4(V,1.0f)).xyz;
        return R;
    }
    
    inline v4 operator*(m4 M, const v4& V)
    {
        v4 R = Transform(M,V);
        return R;
    }
    
    void PrintMatrix(m4 In)
    {
        DEBUG_PRINT("Math: \n");
        DEBUG_PRINT("%f %f %f %f\n", In[0][0],In[0][1],In[0][2],In[0][3]);
        DEBUG_PRINT("%f %f %f %f\n", In[1][0],In[1][1],In[1][2],In[1][3]);
        DEBUG_PRINT("%f %f %f %f\n", In[2][0],In[2][1],In[2][2],In[2][3]);
        DEBUG_PRINT("%f %f %f %f\n", In[3][0],In[3][1],In[3][2],In[3][3]);
    }
    
    inline v4 operator*(const v4& V, const m4& M)
    {
        v4 Result(0.0f);
        Result.X = M.A * V.X + M.B * V.Y + M.C * V.Z + M.D * V.W;
        Result.Y = M.E * V.X + M.F * V.Y + M.G * V.Z + M.H * V.W;
        Result.Z = M.I * V.X + M.J * V.Y + M.K * V.Z + M.L * V.W;
        Result.W = M.M * V.X + M.N * V.Y + M.O * V.Z + M.P * V.W;
        return Result;
    }
    
    inline v3 operator*(const v3& V, const m4& M)
    {
        v3 Result(0.0f);
        Result.X = M.A * V.X + M.B * V.Y + M.C * V.Z + M.D * 1.0f;
        Result.Y = M.E * V.X + M.F * V.Y + M.G * V.Z + M.H * 1.0f;
        Result.Z = M.I * V.X + M.J * V.Y + M.K * V.Z + M.L * 1.0f;
        return Result;
    }
    
    inline r32 Determinant(const m4& In)
    {
        return In.M11 * In.M22 * In.M33 * In.M44 + In.M11 * In.M23 * In.M34 * In.M42 + In.M11 * In.M24 * In.M32 * In.M43 + 
            In.M12 * In.M21 * In.M34 * In.M43 + In.M12 * In.M23 * In.M31 * In.M44 + In.M12 * In.M24 * In.M33 * In.M41 +
            In.M13 * In.M21 * In.M32 * In.M44 + In.M13 * In.M22 * In.M34 * In.M41 + In.M13 * In.M24 * In.M31 * In.M42 +
            In.M14 * In.M21 * In.M33 * In.M42 + In.M14 * In.M22 * In.M31 * In.M43 + In.M14 * In.M23 * In.M32 * In.M41 -
            In.M11 * In.M22 * In.M34 * In.M43 - In.M11 * In.M23 * In.M32 * In.M44 - In.M11 * In.M24 * In.M33 * In.M42 -
            In.M12 * In.M21 * In.M33 * In.M44 - In.M12 * In.M23 * In.M34 * In.M41 - In.M12 * In.M24 * In.M31 * In.M43 -
            In.M13 * In.M21 * In.M34 * In.M42 - In.M13 * In.M22 * In.M31 * In.M44 - In.M13 * In.M24 * In.M32 * In.M41 -
            In.M14 * In.M21 * In.M32 * In.M43 - In.M14 * In.M22 * In.M33 * In.M41 - In.M14 * In.M23 * In.M31 * In.M42;
        
    }
    
    // Inverse is tested against glm::inverse and seems to work
    inline m4 Inverse(const m4& In)
    {
        m4 Result(0.0f);
        Result.M11 =
            In.M22 * In.M33 * In.M44 + In.M23 * In.M34 * In.M42 + In.M24* In.M32 * In.M43
            -In.M22 * In.M34 * In.M43 - In.M23 * In.M32 * In.M44 - In.M24 * In.M33 * In.M42;
        Result.M12 =
            In.M12 * In.M34 * In.M43 + In.M13 * In.M32 * In.M44 + In.M14 * In.M33 * In.M42
            -In.M12 * In.M33 * In.M44 - In.M13 * In.M34 * In.M42 - In.M14 * In.M32 - In.M43;
        Result.M13 =
            In.M12 * In.M23 * In.M44 + In.M13 * In.M24 * In.M42 + In.M14 * In.M22 * In.M43
            -In.M12 * In.M24 * In.M43 - In.M13 * In.M22 * In.M44 - In.M14 * In.M23 * In.M42;
        Result.M14 = 
            In.M12 * In.M24 * In.M33 + In.M13 * In.M22 * In.M34 + In.M14 * In.M23 * In.M32
            -In.M12 * In.M23 * In.M34 - In.M13 * In.M24 * In.M32 - In.M14 * In.M22 * In.M33;
        Result.M21 = 
            In.M21 * In.M34 * In.M43 + In.M23 * In.M31 * In.M44 + In.M24 * In.M33 * In.M41 
            -In.M21 * In.M33 * In.M44 - In.M23 * In.M34 * In.M41 - In.M24 * In.M31 * In.M43;
        Result.M22 = 
            In.M11 * In.M33 * In.M44 + In.M13 * In.M34 * In.M41 + In.M14 * In.M31 * In.M43
            -In.M11 * In.M34 * In.M43 - In.M13 * In.M31 * In.M44 - In.M14 * In.M33 * In.M41;
        Result.M23 =
            In.M11 * In.M24 * In.M43 + In.M13 * In.M21 * In.M44 + In.M14 * In.M23 * In.M41
            -In.M11 * In.M23 * In.M44 - In.M13 * In.M24 * In.M41- In.M14 * In.M21 * In.M43;
        Result.M24 = 
            In.M11 * In.M23 * In.M34 + In.M13 * In.M24 * In.M31 + In.M14 * In.M21 * In.M33 
            -In.M11 * In.M24 * In.M33 - In.M13 * In.M21 * In.M34 - In.M14 * In.M23 * In.M31;
        Result.M31 = 
            In.M21 * In.M32 * In.M44 + In.M22 * In.M34 * In.M41 + In.M24 * In.M31 * In.M42 
            - In.M21 * In.M34 * In.M42 - In.M22 * In.M31 * In.M44 - In.M24 * In.M32 * In.M41;
        Result.M32 = 
            In.M11 * In.M34 * In.M42 + In.M12 * In.M31 * In.M44 + In.M14 * In.M32 * In.M41 
            -In.M11 * In.M32 * In.M44 - In.M12 * In.M34 * In.M41 - In.M14 * In.M31 * In.M42;
        Result.M33 = 
            In.M11 * In.M22 * In.M44 + In.M12 * In.M24 * In.M41 + In.M14 * In.M21 * In.M42 
            - In.M11 * In.M24 * In.M42 - In.M12 * In.M21 * In.M44 - In.M14 * In.M22 * In.M41;
        Result.M34 = 
            In.M11 * In.M24 * In.M32 + In.M12 * In.M21 * In.M34 + In.M14 * In.M22 * In.M31 
            - In.M11 * In.M22 * In.M34 - In.M12 * In.M24 * In.M31 - In.M14 * In.M21 * In.M32;
        Result.M41 = 
            In.M21 * In.M33 * In.M42 + In.M22 * In.M31 * In.M43 + In.M23 * In.M32 * In.M41 
            - In.M21 * In.M32 * In.M43 - In.M22 * In.M33 * In.M41 - In.M23 * In.M31 * In.M42;
        Result.M42 = 
            In.M11 * In.M32 * In.M43 + In.M12 * In.M33 * In.M41 + In.M13 * In.M31 * In.M42 
            - In.M11 * In.M33 * In.M42 - In.M12 * In.M31 * In.M43 - In.M13 * In.M32 * In.M41;
        Result.M43 = 
            In.M11 * In.M23 * In.M42 + In.M12 * In.M21 * In.M43 + In.M13 * In.M22 * In.M41 
            - In.M11 * In.M22 * In.M43 - In.M12 * In.M23 * In.M41 - In.M13 * In.M21 * In.M42;
        Result.M44 = 
            In.M11 * In.M22 * In.M33 + In.M12 * In.M23 * In.M31 + In.M13 * In.M21 * In.M32 
            - In.M11 * In.M23 * In.M32 - In.M12 * In.M21 * In.M33 - In.M13 * In.M22 * In.M31;
        
        auto Det = Determinant(In);
        auto InvDet = 1.0f/Det;
        Result = Result * InvDet;
        return Result;
    }
    
    inline r32 Dot(v2 V1, v2 V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y;
    }
    
    inline r32 Dot(v3 V1, v3 V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y + V1.Z * V2.Z;
    }
    
    inline r32 Dot(v4 V1, v4 V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y + V1.Z * V2.Z + V1.W * V2.W;
    }
    
    inline i32 Dot(v2i V1, v2i V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y;
    }
    
    inline i32 Dot(v3i V1, v3i V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y + V1.Z + V2.Z;
    }
    
    inline r32 Distance(v2 V1, v2 V2)
    {
        return sqrt(pow(V1.X - V2.X, 2.0f) + pow(V1.Y - V2.Y, 2.0f));
    }
    
    inline r32 Distance(v3 V1, v3 V2)
    {
        return sqrt(pow(V1.X - V2.X, 2.0f) + pow(V1.Y - V2.Y, 2.0f) + pow(V1.Z - V2.Z, 2.0f));
    }
    
    inline r32 Distance(v4 V1, v4 V2)
    {
        return sqrt(pow(V1.X - V2.X, 2.0f) + pow(V1.Y - V2.Y, 2.0f) + pow(V1.Z - V2.Z, 2.0f) + pow(V1.W - V2.W,2.0f));
    }
    
    inline i32 Distance(v2i V1, v2i V2)
    {
        return (i32)sqrt(pow(V1.X - V2.X, 2) + pow(V1.Y - V2.Y, 2));
    }
    
    inline i32 Distance(v3i V1, v3i V2)
    {
        return (i32)sqrt(pow(V1.X - V2.X, 2) + pow(V1.Y - V2.Y, 2) + pow(V1.Z - V2.Z, 2));
    }
    
    inline i32 Floor(r32 V)
    {
        return (i32)floor(V);
    }
    
    inline v2 Floor(v2 V)
    {
        v2 Result(V);
        Result.X = (r32)Floor(V.X);
        Result.Y = (r32)Floor(V.Y);
        return Result;
    }
    
    inline v3 Floor(v3 V)
    {
        v3 Result(V);
        Result.X = (r32)Floor(V.X);
        Result.Y = (r32)Floor(V.Y);
        Result.Z = (r32)Floor(V.Z);
        return Result;
    }
    
    inline i32 Ceil(r32 V)
    {
        return (i32)ceil(V);
    }
    
    inline v2 Ceil(v2 V)
    {
        v2 Result(V);
        Result.X = (r32)Ceil(V.X);
        Result.Y = (r32)Ceil(V.Y);
        return Result;
    }
    
    inline v3 Ceil(v3 V)
    {
        v3 Result(V);
        Result.X = (r32)Ceil(V.X);
        Result.Y = (r32)Ceil(V.Y);
        Result.Z = (r32)Ceil(V.Z);
        return Result;
    }
    
    inline i32 Round(r32 V)
    {
        r32 HalfCeil = ((r32)Ceil(V))/2.0f;
        if(V >= HalfCeil)
        {
            return Ceil(V);
        }
        else 
        {
            return Floor(V);
        }
    }
    
    inline r32 Square(r32 V)
    {
        return V * V;
    }
    
    inline r32 Sin(r32 V)
    {
        return sin(V);
    }
    
    inline r32 Cos(r32 V)
    {
        return cos(V);
    }
    
    inline r32 Length(v2 V)
    {
        return sqrt(pow(V.X,2) + pow(V.Y,2));
    }
    
    inline r32 Length(v3 V)
    {
        return sqrt(pow(V.X,2) + pow(V.Y,2) + pow(V.Z,2));
    }
    
    inline r32 Length(v4 V)
    {
        return sqrt(pow(V.X,2) + pow(V.Y,2) + pow(V.Z,2) + pow(V.W,2));
    }
    
    inline v2 Normalize(v2 V)
    {
        v2 Result(V);
        auto L = Length(V);
        Result /= L;
        return Result;
    }
    
    inline v3 Normalize(v3 V)
    {
        v3 Result(V);
        auto L = Length(V);
        Result /= L;
        return Result;
    }
    
    inline v4 Normalize(v4 V)
    {
        v4 Result(V);
        auto L = Length(V);
        Result /= L;
        return Result;
    }
    
    inline m4 Scale(m4 In, v3 Scale)
    {
        m4 Result(In);
        Result.M11 = Scale.X * Result.M11;
        Result.M22 = Scale.Y * Result.M22;
        Result.M33 = Scale.Z * Result.M33;
        
        return Result;
    }
    
    inline m4 Translate(m4 In, v3 Translate)
    {
        m4 Result(In);
        Result.M14 += Translate.X;
        Result.M24 += Translate.Y;
        Result.M34 += Translate.Z;
        return Result;
    }
    
    inline m4 XRotate(r32 Angle)
    {
        Angle *= DEGREE_IN_RADIANS;
        
        r32 C = Cos(Angle);
        r32 S = Sin(Angle);
        
        m4 R(1,0, 0,0,
             0,C,-S,0,
             0,S, C,0,
             0,0, 0,1);
        
        return R;
    }
    
    inline m4 YRotate(r32 Angle)
    {
        Angle *= DEGREE_IN_RADIANS;
        
        r32 C = Cos(Angle);
        r32 S = Sin(Angle);
        
        m4 R(C, 0,S,0,
             0, 1,0,0,
             -S,0,C,0,
             0, 0,0,1);
        
        return R;
    }
    
    inline m4 ZRotate(r32 Angle)
    {
        Angle *= DEGREE_IN_RADIANS;
        
        r32 C = Cos(Angle);
        r32 S = Sin(Angle);
        
        m4 R(C,-S,0,0,
             S,C,0,0,
             0,0,1,0,
             0,0,0,1);
        
        return R;
    }
    
    inline m4 CreateRotation(r32 XAngle, r32 YAngle, r32 ZAngle)
    {
        m4 Result(1.0f);
        Result = YRotate(YAngle) * XRotate(XAngle) * ZRotate(ZAngle) * Result;
        return Result;
    }
    
    inline m4 Rotate(m4 In, r32 A, v3 Axis)
    {
        m4 Result(In);
        auto R = CreateRotation(A * Axis.X, A * Axis.Y, A * Axis.Z);
        Result = R * Result;
        return Result;
    }
    
    inline v3 Project(v3 In, m4 M, m4 P, v4 Viewport)
    {
        v3 Result(1.0f);
        auto Tmp = v4(In, 1.0f);
        Tmp = M * Tmp;
        Tmp = P * Tmp;
        Tmp /= Tmp.W;
        
        Tmp = Tmp * 0.5f + 0.5f;
        Tmp.X = Tmp.X * Viewport.Z + Viewport.X;
        Tmp.Y = Tmp.Y * Viewport.W + Viewport.Y;
        
        return v3(Tmp.X,Tmp.Y,Tmp.Z);
    }
    
    inline v3 Cross(v3 A, v3 B)
    {
        v3 Result;
        
        Result.x = A.y*B.z - A.z*B.y;
        Result.y = A.z*B.x - A.x*B.z;
        Result.z = A.x*B.y - A.y*B.x;
        
        return Result;
    }
    
    inline m4 Ortho(r32 Left, r32 Right, r32 Bottom, r32 Top, r32 Near, r32 Far)
    {
        m4 Result(1.0f);
        Result.M11 = 2.0f/(Right - Left);
        Result.M22 = 2.0f/(Top - Bottom);
        Result.M33 = (-2.0f)/(Far - Near);
        Result.M34 = -((Far + Near)/(Far - Near));
        Result.M14 = -((Right + Left)/(Right - Left));
        Result.M24 = -((Top + Bottom)/(Top - Bottom));
        Result.M44 = 1.0f;
        
        return Result;
    }
    
    inline m4 LookAt(v3 P, v3 T)
    {
        auto F = Normalize(P - T);
        auto U = v3(0.0f,1.0f,0.0f);
        auto R = Cross(U, F);
        U = Cross(F, R);
        
        m4 Result(
            R.x, R.y, R.z, 0,
            U.x, U.y, U.z, 0,
            F.x, F.y, F.z, 0,
            0,   0,   0,   1
            );
        
        m4 Tr = Translate(m4(1.0f), -P);
        Result = Result * Tr;
        
        return Result;
    }
    
    inline m4 Perspective(r32 AspectWidthOverHeight, r32 FocalLength, r32 Near, r32 Far)
    {
        r32 A = 1.0f;
        r32 B = AspectWidthOverHeight;
        r32 C = FocalLength;
        
        r32 N = Near;
        r32 F = Far;
        
        r32 D = (N + F) / (N - F);
        r32 E = (2 * F * N) / (N - F);
        
        m4 Result(
            A * C, 0.0f,  0.0f, 0.0f,
            0,     B * C, 0.0f, 0.0f,
            0.0f,  0.0f,  D,    E,
            0.0f,  0.0f, -1.0f, 0.0f
            );
        
        return Result;
    }
    /*
    inline void Perspective(r32& AngleOfView, r32 AspectRatio, r32 Near,
    r32 Far, r32& Bottom, r32& Top, r32& Left, r32& Right)
    {
    r32 Scale = tan(AngleOfView * 0.5f * PI / 180) * Near;
    Right = AspectRatio  * Scale;
    Left = -Right;
    Top = Scale;
    Bottom = -Top;
    }
    */
    inline m4 Frustum(r32 Bottom, r32 Top, r32 Left, r32 Right,
                      r32 Near, r32 Far)
    {
        
        auto A = 2 * Near / (Right - Left);
        auto B = 2 * Near / (Top - Bottom);
        auto C = (Right + Left) / (Right - Left);
        auto D = (Top + Bottom) / (Top - Bottom);
        auto E = -(Far + Near) / (Far - Near);
        auto F = -2 * Far * Near / (Far - Near);
        
        m4 Result(
            A,    0.0f, C,     0.0f,
            0.0f, B,    D,     0.0f,
            0.0f, 0.0f, E,     F,
            0.0f, 0.0f, -1.0f, 0.0f
            );
        
        return Result;
    }
    
    inline v3 MultPointMatrix(v3 In, m4 M)
    {
        math::v3 Result(0.0f);
        Result.X = In.x * M[0][0] + In.y * M[0][1] + In.z * M[0][2] + M[0][3];
        Result.Y = In.x * M[1][0] + In.y * M[1][1] + In.z * M[1][2] + M[1][3];
        Result.Z = In.x * M[2][0] + In.y * M[2][1] + In.z * M[2][2] + M[2][3];
        r32 W = In.x * M[3][0] + In.y * M[3][1] + In.z * M[3][2] + M[3][3];
        
        if(W != 1)
        {
            Result.X /= W;
            Result.Y /= W;
            Result.Z /= W;
        }
        
        return Result;
    }
    
    
    inline v3 UnProject(v3 In, m4 Model, m4 Projection, v4 Viewport)
    {
        auto Inv = Inverse(Projection * Model);
        
        auto Tmp = v4(In,1.0f);
        Tmp.X = (Tmp.X - Viewport.X) / Viewport.Z;
        Tmp.Y = (Tmp.Y - Viewport.Y) / Viewport.W;
        Tmp = Tmp * 2 - 1;
        
        auto Obj = Inv * Tmp;
        Obj /= Obj.W;
        
        return v3(Obj.X,Obj.Y,Obj.Z);
    }
    
    inline r32 Lerp(r32 A, r32 T, r32 B)
    {
        Assert(T <= 1.0f);
        r32 Result = (1.0f - T) * A + T * B;
        return Result;
    }
    
    inline v2 Lerp(v2 A, r32 T, v2 B)
    {
        Assert(T <= 1.0f);
        v2 Result(0.0f);
        Result.X = Lerp(A.X,T,B.X);
        Result.Y = Lerp(A.Y,T,B.Y);
        return Result;
    }
    
    inline v3 Lerp(v3 A, r32 T, v3 B)
    {
        Assert(T <= 1.0f);
        v3 Result(0.0f);
        Result.X = Lerp(A.X,T,B.X);
        Result.Y = Lerp(A.Y,T,B.Y);
        Result.Z = Lerp(A.Z,T,B.Z);
        return Result;
    }
    
    inline v4 Lerp(v4 A, r32 T, v4 B)
    {
        Assert(T <= 1.0f);
        v4 Result(0.0f);
        Result.X = Lerp(A.X,T,B.X);
        Result.Y = Lerp(A.Y,T,B.Y);
        Result.Z = Lerp(A.Z,T,B.Z);
        Result.W = Lerp(A.W,T,B.W);
        return Result;
    }
    
    inline r32 RandomFloat(r32 From, r32 To)
    {
        r32 Rand = Min(Max(From, ((r32)rand()/(r32)(RAND_MAX)) * To),To);
        return Rand;
    }
    
    using rgb = v3;
    using rgba = v4;
}

#endif

inline r32 Sign(math::v2 P1, math::v2 P2, math::v2 P3)
{
    return (P1.x - P3.x) * (P2.y - P3.y) - (P2.x - P3.x) * (P1.y - P3.y);
}

inline math::v2 ToCartesian(math::v2 Position)
{
    // @Cleanup: Move these to a global variable or similar
    r32 TileWidthHalf = 0.5f;
    r32 TileHeightHalf = 0.25f;
    
    math::v2 TempPt;
    
    TempPt.x = (Position.x / TileWidthHalf + Position.y / TileHeightHalf) / 2.0f;
    TempPt.y = (Position.y / TileHeightHalf - Position.x / TileWidthHalf) / 2.0f;
    return TempPt;
}

inline math::v2 ToIsometric(math::v2 Position)
{
    // @Cleanup: Move these to a global variable or similar
    r32 TileWidthHalf = 0.5f;
    r32 TileHeightHalf = 0.25f;
    
    math::v2 TempPt;
    TempPt.x = (Position.x - Position.y) * TileWidthHalf;
    TempPt.y = (Position.x + Position.y) * TileHeightHalf;
    //return TempPt;
    return Position;
}

inline b32 PointInTriangle(math::v2 Pt, math::v2 V1, math::v2 V2, math::v2 V3)
{
    bool B1, B2, B3;
    
    B1 = Sign(Pt, V1, V2) < 0.0f;
    B2 = Sign(Pt, V2, V3) < 0.0f;
    B3 = Sign(Pt, V3, V1) < 0.0f;
    
    return ((B1 == B2) && (B2 == B3));
}

#endif