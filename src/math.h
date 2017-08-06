
#include <cmath>

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
    
    r32 Floor(r32 V)
    {
        return glm::floor(V);
    }
    
    v2 Floor(v2 V)
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
                r32 R, G, B;
                r32 r, g, b;
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
        /*
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
        }*/
        
        inline m4 operator*(m4 Other)
        {
            m4 Result(*this);
            Result.M11 = 
                this->M11 * Other.M11 + this->M12 * Other.M21 + this->M13 * Other.M31 + this->M14 * Other.M41;
            Result.M12 = this->M11 * Other.M12 + this->M12 * Other.M22 + this->M13 * Other.M32 + this->M14 * Other.M42;
            Result.M13 = this->M11 * Other.M13 + this->M12 * Other.M23+ this->M13 * Other.M33 + this->M14 * Other.M43;
            Result.M14 = this->M11 * Other.M14 + this->M12 * Other.M24 + this->M13 * Other.M34 + this->M14 * Other.M44;
            Result.M21 = this->M21 * Other.M11 + this->M22 * Other.M21 + this->M23 * Other.M31 + this->M24 * Other.M41;
            Result.M22 = this->M21 * Other.M12 + this->M22 * Other.M22 + this->M23 * Other.M32 + this->M24 * Other.M42;
            Result.M23 = this->M21 * Other.M13 + this->M22 * Other.M23 + this->M23 * Other.M33 + this->M24 * Other.M43;
            Result.M24 = this->M21 * Other.M14 + this->M22 * Other.M24 + this->M23 * Other.M34 + this->M24 * Other.M44;
            Result.M31 = this->M31 * Other.M11 + this->M32 * Other.M21 + this->M33 * Other.M31 + this->M34 * Other.M41;
            Result.M32 = this->M31 * Other.M12 + this->M32 * Other.M22 + this->M33 * Other.M32 + this->M34 * Other.M42;
            Result.M33 = this->M31 * Other.M13 + this->M32 * Other.M23 + this->M33 * Other.M33 + this->M34 * Other.M43;
            Result.M34 = this->M31 * Other.M14 + this->M32 * Other.M24 + this->M33 * Other.M34 + this->M34 * Other.M44;
            Result.M41 = this->M41 * Other.M11 + this->M42 * Other.M21 + this->M43 * Other.M31 + this->M44 * Other.M41;
            Result.M42 = this->M41 * Other.M12 + this->M42 * Other.M22 + this->M43 * Other.M32 + this->M44 * Other.M42;
            Result.M43 = this->M41 * Other.M13 + this->M42 * Other.M23 + this->M43 * Other.M33 + this->M44 * Other.M43;
            Result.M44 = this->M41 * Other.M14 + this->M42 * Other.M24 + this->M43 * Other.M34 + this->M44 * Other.M44;
            return Result;
        }
        
        inline v3 operator*(v3& Vec)
        {
            v3 Result(0.0f);
            Result.X = this->A * Vec.X + this->B * Vec.Y + this->C * Vec.Z + this->D * 1.0f;
            Result.Y = this->E * Vec.X + this->F * Vec.Y + this->G * Vec.Z + this->H * 1.0f;
            Result.Z = this->I * Vec.X + this->J * Vec.Y + this->K * Vec.Z + this->L * 1.0f;
            return Result;
        }
        
        inline v4 operator*(v4 Vec)
        {
            v4 Result(0.0f);
            Result.X = this->A * Vec.X + this->B * Vec.Y + this->C * Vec.Z + this->D * Vec.W;
            Result.Y = this->E * Vec.X + this->F * Vec.Y + this->G * Vec.Z + this->H * Vec.W;
            Result.Z = this->I * Vec.X + this->J * Vec.Y + this->K * Vec.Z + this->L * Vec.W;
            Result.W = this->M * Vec.X + this->N * Vec.Y + this->O * Vec.Z + this->P * Vec.W;
            return Result;
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
        m4 T(1.0f);
        T.M14 = Translate.X;
        T.M24 = Translate.Y;
        T.M34 = Translate.Z;
        Result = T * Result;
        return Result;
    }
    
    inline m4 XRotate(r32 Angle)
    {
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
        r32 C = Cos(Angle);
        r32 S = Sin(Angle);
        
        m4 R(C,-S,0,0,
             S,C,0,0,
             0,0,1,0,
             0,0,0,1);
        
        return R;
    }
    
    inline m4 CreateRotate(r32 XAngle, r32 YAngle, r32 ZAngle)
    {
        m4 R = YRotate(YAngle) * XRotate(XAngle) * ZRotate(ZAngle);
        return R;
    }
    
    inline m4 Rotate(m4 In, v3 Rotation)
    {
        m4 Result(In);
        auto R = CreateRotate(Rotation.X,Rotation.Y,Rotation.Z);
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
    
    
    inline v2 ToCartesian(v2 Position)
    {
        v2 TempPt;
        TempPt.x = (2 * Position.y + Position.x);
        TempPt.y = (2 * Position.y - Position.x) / 2;
        return TempPt;
    }
    
    inline v2 ToIsometric(v2 Position)
    {
        // @Cleanup: Move these to a global variable or similar
        r32 TileWidthHalf = 0.5f;
        r32 TileHeightHalf = 0.25f;
        
        v2 TempPt;
        TempPt.x = (Position.x - Position.y) * TileWidthHalf;
        TempPt.y = (Position.x + Position.y) * TileHeightHalf;
        return TempPt;
    }
    
    inline r32 Sign(v2 P1, v2 P2, v2 P3)
    {
        return (P1.x - P3.x) * (P2.y - P3.y) - (P2.x - P3.x) * (P1.y - P3.y);
    }
    
    inline b32 PointInTriangle(v2 Pt, v2 V1, v2 V2, v2 V3)
    {
        bool B1, B2, B3;
        
        B1 = Sign(Pt, V1, V2) < 0.0f;
        B2 = Sign(Pt, V2, V3) < 0.0f;
        B3 = Sign(Pt, V3, V1) < 0.0f;
        
        return ((B1 == B2) && (B2 == B3));
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


