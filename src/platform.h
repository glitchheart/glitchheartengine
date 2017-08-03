#ifndef PLATFORM_H
#define PLATFORM_H

#define NUM_ENTITIES 100

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0])) 


#if GLITCH_DEBUG
#define DEBUG_PRINT(format, ...) printf(format, __VA_ARGS__)
#else
#define DEBUG_PRINT(format, ...)
#endif

#if GLITCH_DEBUG
#define Assert(Expression) if(!(Expression)) {DEBUG_PRINT("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); exit(EXIT_FAILURE);}
#else
#define Assert(Expression)
#endif

#define Min(A,B) ((A < B) ? (A) : (B))
#define Max(A,B) ((A > B) ? (A) : (B))
#define Abs(x) ((x) < 0 ? -(x) : (x))

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define PI 3.141592653589793f

#include <stdint.h>
#include <malloc.h>
#include <sys/types.h>  
#include <sys/stat.h>  

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef i32 b32;

typedef float r32;
typedef double r64;

#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

#include "gmap.h"
#include "gmap.cpp"
#include "keycontroller.h"

struct timer
{
    i32 TimerHandle = -1;
    r64 TimerMax = 0.0;
    char* Name;
};

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
        
        v2 operator* (v2 O)
        {
            v2 Res(*this);
            Res.X *= O.X;
            Res.Y *= O.Y;
            return Res;
        }
        
        v2 operator+ (v2 O)
        {
            v2 Res(*this);
            Res.X += O.X;
            Res.Y += O.Y;
            return Res;
        }
        
        void operator*= (v2 O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
        }
        
        void operator+= (v2 O)
        {
            this->X += O.X;
            this->Y += O.Y;
        }
        
        v2 operator+ (r32 S)
        {
            v2 Res(*this);
            Res.X += S;
            Res.Y += S;
            return Res;
        }
        
        v2 operator* (r32 S)
        {
            v2 Res(*this);
            Res.X *= S;
            Res.Y *= S;
            return Res;
        }
        
        v2 operator/ (r32 S)
        {
            v2 Res(*this);
            Res.X /= S;
            Res.Y /= S;
            return Res;
        }
        
        void operator+= (r32 S)
        {
            this->X += S;
            this->Y += S;
        }
        
        void operator*= (r32 S)
        {
            this->X *= S;
            this->Y *= S;
        }
        
        void operator/= (r32 S)
        {
            this->X /= S;
            this->Y /= S;
        }
        
        void operator-= (r32 S)
        {
            this->X -= S;
            this->Y -= S;
        }
        
        v2 operator- (v2 O)
        {
            v2 Res(*this);
            Res.X -= O.X;
            Res.Y -= O.Y;
            return Res;
        }
        
        void operator-= (v2 O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
        }
        
        v2 operator- (r32 S)
        {
            v2 Res(*this);
            Res.X -= S;
            Res.Y -= S;
            return Res;
        }
        
        v2 operator/ (v2 O)
        {
            v2 Res(*this);
            Res.X /= O.X;
            Res.Y /= O.Y;
            return Res;
        }
        
        void operator/= (v2 O)
        {
            this->X /= O.X;
            this->Y /= O.Y;
        }
    };
    
    union v3
    {
        struct
        {
            r32 X, Y, Z;
        };
        struct
        {
            r32 x, y, z;
        };
        struct
        {
            r32 R, G, B;
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
        
        v3 operator* (v3 O)
        {
            v3 Res(*this);
            Res.X *= O.X;
            Res.Y *= O.Y;
            Res.Z *= O.Z;
            return Res;
        }
        
        v3 operator+ (v3 O)
        {
            v3 Res(*this);
            Res.X += O.X;
            Res.Y += O.Y;
            Res.Z += O.Z;
            return Res;
        }
        
        void operator*= (v3 O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
            this->Z *= O.Z;
        }
        
        void operator+= (v3 O)
        {
            this->X += O.X;
            this->Y += O.Y;
            this->Z += O.Z;
        }
        
        v3 operator+ (r32 S)
        {
            v3 Res(*this);
            Res.X += S;
            Res.Y += S;
            Res.Z += S;
            return Res;
        }
        
        v3 operator* (r32 S)
        {
            v3 Res(*this);
            Res.X *= S;
            Res.Y *= S;
            Res.Z *= S;
            return Res;
        }
        
        v3 operator/ (r32 S)
        {
            v3 Res(*this);
            Res.X /= S;
            Res.Y /= S;
            Res.Z /= S;
            return Res;
        }
        
        void operator+= (r32 S)
        {
            this->X += S;
            this->Y += S;
            this->Z += S;
        }
        
        void operator*= (r32 S)
        {
            this->X *= S;
            this->Y *= S;
            this->Z *= S;
        }
        
        void operator/= (r32 S)
        {
            this->X /= S;
            this->Y /= S;
            this->Z /= S;
        }
        
        void operator-= (r32 S)
        {
            this->X -= S;
            this->Y -= S;
            this->Z -= S;
        }
        
        v3 operator- (v3 O)
        {
            v3 Res(*this);
            Res.X -= O.X;
            Res.Y -= O.Y;
            Res.Z -= O.Z;
            return Res;
        }
        
        void operator-= (v3 O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
            this->Z -= O.Z;
        }
        
        
        v3 operator- (r32 S)
        {
            v3 Res(*this);
            Res.X -= S;
            Res.Y -= S;
            Res.Z -= S;
            return Res;
        }
        
        v3 operator/ (v3 O)
        {
            v3 Res(*this);
            Res.X /= O.X;
            Res.Y /= O.Y;
            Res.Z /= O.Z;
            return Res;
        }
        
        void operator/= (v3 O)
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
            r32 X, Y, Z, W;
        };
        struct
        {
            r32 x, y, z, w;
        };
        struct
        {
            r32 R, G, B, A;
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
        
        
        v4 operator* (v4 O)
        {
            v4 Res(*this);
            Res.X *= O.X;
            Res.Y *= O.Y;
            Res.Z *= O.Z;
            Res.W *= O.W;
            return Res;
        }
        
        v4 operator+ (v4 O)
        {
            v4 Res(*this);
            Res.X += O.X;
            Res.Y += O.Y;
            Res.Z += O.Z;
            Res.W += O.W;
            return Res;
        }
        
        void operator*= (v4 O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
            this->Z *= O.Z;
            this->W *= O.W;
        }
        
        void operator+= (v4 O)
        {
            this->X += O.X;
            this->Y += O.Y;
            this->Z += O.Z;
            this->W += O.W;
        }
        
        v4 operator+ (r32 S)
        {
            v4 Res(*this);
            Res.X += S;
            Res.Y += S;
            Res.Z += S;
            Res.W += S;
            return Res;
        }
        
        v4 operator* (r32 S)
        {
            v4 Res(*this);
            Res.X *= S;
            Res.Y *= S;
            Res.Z *= S;
            Res.W *= S;
            return Res;
        }
        
        v4 operator/ (r32 S)
        {
            v4 Res(*this);
            Res.X /= S;
            Res.Y /= S;
            Res.Z /= S;
            Res.W /= S;
            return Res;
        }
        
        void operator+= (r32 S)
        {
            this->X += S;
            this->Y += S;
            this->Z += S;
            this->W += S;
        }
        
        void operator*= (r32 S)
        {
            this->X *= S;
            this->Y *= S;
            this->Z *= S;
            this->W *= S;
        }
        
        void operator/= (r32 S)
        {
            this->X /= S;
            this->Y /= S;
            this->Z /= S;
            this->W /= S;
        }
        
        void operator-= (r32 S)
        {
            this->X -= S;
            this->Y -= S;
            this->Z -= S;
            this->W -= S;
        }
        
        v4 operator- (v4 O)
        {
            v4 Res(*this);
            Res.X -= O.X;
            Res.Y -= O.Y;
            Res.Z -= O.Z;
            Res.W -= O.W;
            return Res;
        }
        
        void operator-= (v4 O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
            this->Z -= O.Z;
            this->W -= O.W;
        }
        
        v4 operator- (r32 S)
        {
            v4 Res(*this);
            Res.X -= S;
            Res.Y -= S;
            Res.Z -= S;
            Res.W -= S;
            return Res;
        }
        
        v4 operator/ (v4 O)
        {
            v4 Res(*this);
            Res.X /= O.X;
            Res.Y /= O.Y;
            Res.Z /= O.Z;
            Res.W /= O.W;
            return Res;
        }
        
        void operator/= (v4 O)
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
        
        v2i operator* (v2i O)
        {
            v2i Res(*this);
            Res.X *= O.X;
            Res.Y *= O.Y;
            return Res;
        }
        
        v2i operator+ (v2i O)
        {
            v2i Res(*this);
            Res.X += O.X;
            Res.Y += O.Y;
            return Res;
        }
        
        void operator*= (v2i O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
        }
        
        void operator+= (v2i O)
        {
            this->X += O.X;
            this->Y += O.Y;
        }
        
        v2i operator+ (i32 S)
        {
            v2i Res(*this);
            Res.X += S;
            Res.Y += S;
            return Res;
        }
        
        v2i operator* (i32 S)
        {
            v2i Res(*this);
            Res.X *= S;
            Res.Y *= S;
            return Res;
        }
        
        v2i operator/ (i32 S)
        {
            v2i Res(*this);
            Res.X /= S;
            Res.Y /= S;
            return Res;
        }
        
        void operator+= (i32 S)
        {
            this->X += S;
            this->Y += S;
        }
        
        void operator*= (i32 S)
        {
            this->X *= S;
            this->Y *= S;
        }
        
        void operator/= (i32 S)
        {
            this->X /= S;
            this->Y /= S;
        }
        
        void operator-= (i32 S)
        {
            this->X -= S;
            this->Y -= S;
        }
        
        v2i operator- (v2i O)
        {
            v2i Res(*this);
            Res.X -= O.X;
            Res.Y -= O.Y;
            return Res;
        }
        
        void operator-= (v2i O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
        }
        
        v2i operator- (i32 S)
        {
            v2i Res(*this);
            Res.X -= S;
            Res.Y -= S;
            return Res;
        }
        
        v2i operator/ (v2i O)
        {
            v2i Res(*this);
            Res.X /= O.X;
            Res.Y /= O.Y;
            return Res;
        }
        
        void operator/= (v2i O)
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
        
        v3i operator* (v3i O)
        {
            v3i Res(*this);
            Res.X *= O.X;
            Res.Y *= O.Y;
            return Res;
        }
        
        v3i operator+ (v3i O)
        {
            v3i Res(*this);
            Res.X += O.X;
            Res.Y += O.Y;
            return Res;
        }
        
        void operator*= (v3i O)
        {
            this->X *= O.X;
            this->Y *= O.Y;
        }
        
        void operator+= (v3i O)
        {
            this->X += O.X;
            this->Y += O.Y;
        }
        
        v3i operator+ (i32 S)
        {
            v3i Res(*this);
            Res.X += S;
            Res.Y += S;
            return Res;
        }
        
        v3i operator* (i32 S)
        {
            v3i Res(*this);
            Res.X *= S;
            Res.Y *= S;
            return Res;
        }
        
        v3i operator/ (i32 S)
        {
            v3i Res(*this);
            Res.X /= S;
            Res.Y /= S;
            return Res;
        }
        
        void operator+= (i32 S)
        {
            this->X += S;
            this->Y += S;
        }
        
        void operator*= (i32 S)
        {
            this->X *= S;
            this->Y *= S;
        }
        
        void operator/= (i32 S)
        {
            this->X /= S;
            this->Y /= S;
        }
        
        void operator-= (i32 S)
        {
            this->X -= S;
            this->Y -= S;
            this->Z -= S;
        }
        
        v3i operator- (v3i O)
        {
            v3i Res(*this);
            Res.X -= O.X;
            Res.Y -= O.Y;
            Res.Z -= O.Z;
            return Res;
        }
        
        void operator-= (v3i O)
        {
            this->X -= O.X;
            this->Y -= O.Y;
            this->Z -= O.Z;
        }
        
        v3i operator- (i32 S)
        {
            v3i Res(*this);
            Res.X -= S;
            Res.Y -= S;
            Res.Z -= S;
            return Res;
        }
        
        v3i operator/ (v3i O)
        {
            v3i Res(*this);
            Res.X /= O.X;
            Res.Y /= O.Y;
            Res.Z /= O.Z;
            return Res;
        }
        
        void operator/= (v3i O)
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
            m4 Res(*this);
            for(i32 Outer = 0; Outer < 4; Outer++)
            {
                for(i32 Inner = 0; Inner < 4; Inner++)
                {
                    r32 Sum = 0;
                    for(i32 Shared = 0; Shared < 4; Shared++)
                    {
                        Sum += this->V[Inner][Shared] * Other.V[Shared][Outer];
                    }
                    Res.V[Inner][Outer] = Sum;
                }
            }
            
            return Res;
        }
        /*
        m4 operator*(m4 Other)
        {
            m4 Res(*this);
            Res.M11 = 
                this->M11 * Other.M11 + this->M12 * Other.M21 + this->M13 * Other.M31 + this->M14 * Other.M41;
            Res.M12 = this->M11 * Other.M12 + this->M12 * Other.M22 + this->M13 * Other.M32 + this->M14 * Other.M42;
            Res.M13 = this->M11 * Other.M13 + this->M12 * Other.M23+ this->M13 * Other.M33 + this->M14 * Other.M43;
            Res.M14 = this->M11 * Other.M14 + this->M12 * Other.M24 + this->M13 * Other.M34 + this->M14 * Other.M44;
            Res.M21 = this->M21 * Other.M11 + this->M22 * Other.M21 + this->M23 * Other.M31 + this->M24 * Other.M41;
            Res.M22 = this->M21 * Other.M12 + this->M22 * Other.M22 + this->M23 * Other.M32 + this->M24 * Other.M42;
            Res.M23 = this->M21 * Other.M13 + this->M22 * Other.M23 + this->M23 * Other.M33 + this->M24 * Other.M43;
            Res.M24 = this->M21 * Other.M14 + this->M22 * Other.M24 + this->M23 * Other.M34 + this->M24 * Other.M44;
            Res.M31 = this->M31 * Other.M11 + this->M32 * Other.M21 + this->M33 * Other.M31 + this->M34 * Other.M41;
            Res.M32 = this->M31 * Other.M12 + this->M32 * Other.M22 + this->M33 * Other.M32 + this->M34 * Other.M42;
            Res.M33 = this->M31 * Other.M13 + this->M32 * Other.M23 + this->M33 * Other.M33 + this->M34 * Other.M43;
            Res.M34 = this->M31 * Other.M14 + this->M32 * Other.M24 + this->M33 * Other.M34 + this->M34 * Other.M44;
            Res.M41 = this->M41 * Other.M11 + this->M42 * Other.M21 + this->M43 * Other.M31 + this->M44 * Other.M41;
            Res.M42 = this->M41 * Other.M12 + this->M42 * Other.M22 + this->M43 * Other.M32 + this->M44 * Other.M42;
            Res.M43 = this->M41 * Other.M13 + this->M42 * Other.M23 + this->M43 * Other.M33 + this->M44 * Other.M43;
            Res.M44 = this->M41 * Other.M14 + this->M42 * Other.M24 + this->M43 * Other.M34 + this->M44 * Other.M44;
            return Res;
        }*/
        
        v3 operator*(v3& Vec)
        {
            v3 Res(0.0f);
            Res.X = this->A * Vec.X + this->B * Vec.Y + this->C * Vec.Z + this->D * 1.0f;
            Res.Y = this->E * Vec.X + this->F * Vec.Y + this->G * Vec.Z + this->H * 1.0f;
            Res.Z = this->I * Vec.X + this->J * Vec.Y + this->K * Vec.Z + this->L * 1.0f;
            return Res;
        }
        
        v4 operator*(v4 Vec)
        {
            v4 Res(0.0f);
            Res.X = this->A * Vec.X + this->B * Vec.Y + this->C * Vec.Z + this->D * Vec.W;
            Res.Y = this->E * Vec.X + this->F * Vec.Y + this->G * Vec.Z + this->H * Vec.W;
            Res.Z = this->I * Vec.X + this->J * Vec.Y + this->K * Vec.Z + this->L * Vec.W;
            Res.W = this->M * Vec.X + this->N * Vec.Y + this->O * Vec.Z + this->P * Vec.W;
            return Res;
        }
    };
    
    
    v4 operator*(const v4& V, const m4& M)
    {
        v4 Res(0.0f);
        Res.X = M.A * V.X + M.B * V.Y + M.C * V.Z + M.D * V.W;
        Res.Y = M.E * V.X + M.F * V.Y + M.G * V.Z + M.H * V.W;
        Res.Z = M.I * V.X + M.J * V.Y + M.K * V.Z + M.L * V.W;
        Res.W = M.M * V.X + M.N * V.Y + M.O * V.Z + M.P * V.W;
        return Res;
    }
    
    m4 Inverse(const m4& In)
    {
        m4 Res(0.0f);
        Res.M11 =
            In.M22 * In.M33 * In.M44 + In.M23 * In.M34 * In.M42 + In.M24* In.M32* In.M43
            -In.M22 * In.M34 * In.M43 - In.M23 * In.M32 * In.M44 - In.M24 * In.M33 * In.M42;
        Res.M12 =
            In.M12 * In.M34 * In.M43 + In.M13 * In.M32 * In.M44 + In.M14 * In.M33 * In.M42
            -In.M12 * In.M33 * In.M44 - In.M13 * In.M34 * In.M42 - In.M14 * In.M32 - In.M43;
        Res.M13 =
            In.M12 * In.M23 * In.M44 + In.M13 * In.M24 * In.M42 + In.M14 * In.M22 * In.M43
            -In.M12 * In.M24 * In.M43 - In.M13 * In.M22 * In.M44 - In.M14 * In.M23 * In.M42;
        Res.M14 = 
            In.M12 * In.M24 * In.M33 + In.M13 * In.M22 * In.M34 + In.M14 * In.M23 * In.M32
            -In.M12 * In.M23 * In.M34 - In.M13 * In.M24 * In.M32 - In.M14 * In.M22 * In.M33;
        Res.M21 = 
            In.M21 * In.M34 * In.M43 + In.M23 * In.M31 * In.M44 + In.M24 * In.M33 * In.M41 
            -In.M21 * In.M33 * In.M44 - In.M23 * In.M34 * In.M41 - In.M24 * In.M31 * In.M43;
        Res.M22 = 
            In.M11 * In.M33 * In.M44 + In.M13 * In.M34 * In.M41 + In.M41 * In.M31 * In.M43
            -In.M11 * In.M34 * In.M43 - In.M13 * In.M31 * In.M44 - In.M14 * In.M33 * In.M41;
        Res.M23 =
            In.M11 * In.M24 * In.M43 + In.M13 * In.M21 * In.M44 + In.M14 * In.M23 * In.M41
            -In.M11 * In.M23 * In.M44 - In.M13 * In.M24 * In.M41- In.M14 * In.M21 * In.M43;
        Res.M24 = 
            In.M11 * In.M23 * In.M34 + In.M13 * In.M24 * In.M31 + In.M14 * In.M21 * In.M33 
            -In.M11 * In.M24 * In.M33 - In.M13 * In.M21 * In.M34 - In.M14 * In.M23 * In.M31;
        Res.M31 = 
            In.M21 * In.M32 * In.M44 + In.M22 * In.M34 * In.M41 + In.M24 * In.M31 * In.M42 
            - In.M21 * In.M34 * In.M42 - In.M22 * In.M31 * In.M44 - In.M24 * In.M32 * In.M41;
        Res.M32 = 
            In.M11 * In.M34 * In.M42 + In.M12 * In.M31 * In.M44 + In.M14 * In.M32 * In.M41 
            -In.M11 * In.M32 * In.M44 - In.M12 * In.M34 * In.M41 - In.M14 * In.M31 * In.M42;
        Res.M33 = 
            In.M11 * In.M22 * In.M44 + In.M12 * In.M24 * In.M41 + In.M14 * In.M21 * In.M42 
            - In.M11 * In.M24 * In.M42 - In.M12 * In.M21 * In.M44 - In.M14 * In.M22 * In.M41;
        Res.M34 = 
            In.M11 * In.M24 * In.M32 + In.M12 * In.M21 * In.M34 + In.M14 * In.M22 * In.M31 
            - In.M11 * In.M22 * In.M34 - In.M12 * In.M24 * In.M31 - In.M14 * In.M21 * In.M32;
        Res.M41 = 
            In.M21 * In.M33 * In.M42 + In.M22 * In.M31 * In.M43 + In.M23 * In.M32 * In.M41 
            - In.M21 * In.M32 * In.M43 - In.M22 * In.M33 * In.M41 - In.M23 * In.M31 * In.M42;
        Res.M42 = 
            In.M11 * In.M32 * In.M43 + In.M12 * In.M33 * In.M41 + In.M13 * In.M31 * In.M42 
            - In.M11 * In.M33 * In.M42 - In.M12 * In.M31 * In.M43 - In.M13 * In.M32 * In.M41;
        Res.M43 = 
            In.M11 * In.M23 * In.M42 + In.M12 * In.M21 * In.M43 + In.M13 * In.M22 * In.M41 
            - In.M11 * In.M22 * In.M43 - In.M12 * In.M23 * In.M41 - In.M13 * In.M21 * In.M42;
        Res.M44 = 
            In.M11 * In.M22 * In.M33 + In.M12 * In.M23 * In.M31 + In.M13 * In.M21 * In.M32 
            - In.M11 * In.M23 * In.M32 - In.M12 * In.M21 * In.M33 - In.M13 * In.M22 * In.M31;
        
        return Res;
    }
    
    m4 Transpose(m4 In)
    {
        m4 Res(In);
        Res.M11 = In.M11;
        Res.M12 = In.M21;
        Res.M13 = In.M31;
        Res.M14 = In.M41;
        Res.M21 = In.M12;
        Res.M22 = In.M22;
        Res.M23 = In.M32;
        Res.M24 = In.M42;
        Res.M31 = In.M13;
        Res.M32 = In.M23;
        Res.M33 = In.M33;
        Res.M34 = In.M43;
        Res.M41 = In.M14;
        Res.M42 = In.M24;
        Res.M43 = In.M34;
        Res.M44 = In.M44;
        return Res;
    }
    
    r32 Dot(v2 V1, v2 V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y;
    }
    
    r32 Dot(v3 V1, v3 V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y + V1.Z * V2.Z;
    }
    
    r32 Dot(v4 V1, v4 V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y + V1.Z * V2.Z + V1.W * V2.W;
    }
    
    i32 Dot(v2i V1, v2i V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y;
    }
    
    
    i32 Dot(v3i V1, v3i V2)
    {
        return V1.X * V2.X + V1.Y + V2.Y + V1.Z + V2.Z;
    }
    
    r32 Distance(v2 V1, v2 V2)
    {
        return sqrt(pow(V1.X - V2.X, 2.0f) + pow(V1.Y - V2.Y, 2.0f));
    }
    
    r32 Distance(v3 V1, v3 V2)
    {
        return sqrt(pow(V1.X - V2.X, 2.0f) + pow(V1.Y - V2.Y, 2.0f) + pow(V1.Z - V2.Z, 2.0f));
    }
    
    r32 Distance(v4 V1, v4 V2)
    {
        return sqrt(pow(V1.X - V2.X, 2.0f) + pow(V1.Y - V2.Y, 2.0f) + pow(V1.Z - V2.Z, 2.0f) + pow(V1.W - V2.W,2.0f));
    }
    
    i32 Distance(v2i V1, v2i V2)
    {
        return (i32)sqrt(pow(V1.X - V2.X, 2) + pow(V1.Y - V2.Y, 2));
    }
    
    i32 Distance(v3i V1, v3i V2)
    {
        return (i32)sqrt(pow(V1.X - V2.X, 2) + pow(V1.Y - V2.Y, 2) + pow(V1.Z - V2.Z, 2));
    }
    
    i32 Floor(r32 V)
    {
        return (i32)floor(V);
    }
    
    v2 Floor(v2 V)
    {
        v2 Res(V);
        Res.X = (r32)Floor(V.X);
        Res.Y = (r32)Floor(V.Y);
        return Res;
    }
    
    i32 Ceil(r32 V)
    {
        return (i32)ceil(V);
    }
    
    v2 Ceil(v2 V)
    {
        v2 Res(V);
        Res.X = (r32)Ceil(V.X);
        Res.Y = (r32)Ceil(V.Y);
        return Res;
    }
    
    r32 Length(v2 V)
    {
        return sqrt(pow(V.X,2) + pow(V.Y,2));
    }
    
    r32 Length(v3 V)
    {
        return sqrt(pow(V.X,2) + pow(V.Y,2) + pow(V.Z,2));
    }
    
    r32 Length(v4 V)
    {
        return sqrt(pow(V.X,2) + pow(V.Y,2) + pow(V.Z,2) + pow(V.W,2));
    }
    
    v2 Normalize(v2 V)
    {
        v2 Res;
        auto L = Length(V);
        Res /= L;
        return Res;
    }
    
    v3 Normalize(v3 V)
    {
        v3 Res;
        auto L = Length(V);
        Res /= L;
        return Res;
    }
    
    v4 Normalize(v4 V)
    {
        v4 Res(V);
        auto L = Length(V);
        Res /= L;
        return Res;
    }
    
    m4 Scale(m4 In, v3 Scale)
    {
        m4 Res(In);
        m4 S(Scale.X,0,      0,      0,
             0,      Scale.Y,0,      0,
             0,      0,      Scale.Z,0,
             0,      0,      0,      1);
        
        Res = S * Res;
        
        return Res;
    }
    
    m4 Translate(m4 In, v3 Translate)
    {
        m4 Res(In);
        m4 T(1.0f);
        T.M14 = Translate.X;
        T.M24 = Translate.Y;
        T.M34 = Translate.Z;
        
        Res = T * Res;
        
        return Res;
    }
    
    v3 Project(v3 In, m4 M, m4 P, v4 Viewport)
    {
        v3 Res(1.0f);
        
        m4 V = Translate(m4(1.0f),v3(Viewport.X,Viewport.Y,Viewport.Z));
        auto Temp = P * V * M;
        Res = v3(Temp.M14,Temp.M24,Temp.M34);
        
        return Res;
    }
    
    m4 Ortho(r32 Left, r32 Right, r32 Bottom, r32 Top, r32 Near, r32 Far)
    {
        m4 Res(1.0f);
        Res.M11 = 2.0f/(Right - Left);
        Res.M22 = 2.0f/(Top - Bottom);
        Res.M33 = (-2.0f)/(Far - Near);
        Res.M34 = -((Far + Near)/(Far - Near));
        Res.M14 = -((Right + Left)/(Right - Left));
        Res.M24 = -((Top + Bottom)/(Top - Bottom));
        //Res.M44 = 1.0f;
        
        return Res;
    }
    
    v3 UnProject(v3 In, m4 Model, m4 Projection, v4 Viewport)
    {
        v3 Res;
        //(Viewport * Projection * View * Model)^-1 * projected;
        m4 VP(1.0f);
        VP = Ortho(Viewport.X,Viewport.Y,Viewport.Z,Viewport.W,-1.0f,1.0f);
        auto IV = Inverse(VP * Projection * Model);
        Res = IV * In;
        return Res;
    }
}

struct entity_file_reload_data
{
    b32 ReloadPlayerFile;
    b32 ReloadSkeletonFile;
    b32 ReloadMinotaurFile;
    b32 ReloadWraithFile;
    b32 ReloadBonfireFile;
};

struct config_data
{
    char* Title;
    char* Version;
    char* StartingLevelFilePath;
    u32 ScreenWidth;
    u32 ScreenHeight;
    b32 Fullscreen;
    r32 Contrast;
    r32 Brightness;
    b32 Muted;
    r32 SFXVolume;
    r32 MusicVolume;
    r32 Zoom;
};

struct game_memory
{
    b32 IsInitialized;
    b32 ShouldReload;
    config_data ConfigData;
    
    u64 PermanentStorageSize;
    void* PermanentStorage;
};

struct input_controller;
struct sound_queue;
struct sound_effects;

struct game_update_return
{
    math::v2 EntityPositions[NUM_ENTITIES];
    i32 EntityCount;
};

#define UPDATE(name)void name(r64 DeltaTime, game_memory* GameMemory, input_controller* InputController, sound_queue* SoundQueue, game_update_return* GameUpdateStruct, sound_effects* SoundEffects)
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

inline void HandleError(char const *File, i32 LineNum, char const *msg)
{
    fprintf(stderr, "Error on in file %s on line %d\n", File, LineNum);
    fprintf(stderr, "%s\n", msg);
}

#define ERR(msg) HandleError(__FILE__,__LINE__,msg)

struct CompareCStrings 
{
    bool operator()(const char* lhs, const char* rhs) const
    {
        return strcmp(lhs, rhs) < 0;
    }
};

//Remember to free string after usage
inline char* Concat(const char *s1, const char *s2)
{
    char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

inline b32 StartsWith(const char *A, const char *B)
{
    if(strncmp(A, B, strlen(B)) == 0) return 1;
    return 0;
}

inline void LoadConfig(const char* FilePath, config_data* ConfigData)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    ConfigData->Title = (char*)malloc(sizeof(char) * 40);
    ConfigData->Version = (char*)malloc(sizeof(char) * 40);
    
    if(File)
    {
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(LineBuffer, "title"))
            {
                sscanf(LineBuffer, "title %s", ConfigData->Title);
            }
            else if(StartsWith(LineBuffer, "version"))
            {
                sscanf(LineBuffer, "version %s", ConfigData->Version);
            }
            else if(StartsWith(LineBuffer, "screen_width"))
            {
                sscanf(LineBuffer, "screen_width %d", &ConfigData->ScreenWidth);
            }
            else if(StartsWith(LineBuffer, "screen_height"))
            {
                sscanf(LineBuffer, "screen_height %d", &ConfigData->ScreenHeight);
            }
            else if(StartsWith(LineBuffer, "screen_height"))
            {
                sscanf(LineBuffer, "screen_height %d", &ConfigData->ScreenHeight);
            }
            else if(StartsWith(LineBuffer, "contrast"))
            {
                sscanf(LineBuffer, "contrast %f", &ConfigData->Contrast);
            }
            else if(StartsWith(LineBuffer, "brightness"))
            {
                sscanf(LineBuffer, "brightness %f", &ConfigData->Brightness);
            }
            else if(StartsWith(LineBuffer, "fullscreen"))
            {
                sscanf(LineBuffer, "fullscreen %d", &ConfigData->Fullscreen);
            } 
            else if(StartsWith(LineBuffer, "muted"))
            {
                sscanf(LineBuffer, "muted %d", &ConfigData->Muted);
            }
            else if(StartsWith(LineBuffer, "sfx_volume"))
            {
                sscanf(LineBuffer, "sfx_volume %f", &ConfigData->SFXVolume);
            }
            else if(StartsWith(LineBuffer, "music_volume"))
            {
                sscanf(LineBuffer, "music_volume %f", &ConfigData->MusicVolume);
            }
            else if(StartsWith(LineBuffer, "starting_level_path"))
            {
                ConfigData->StartingLevelFilePath = (char*)malloc(40 * sizeof(char));
                sscanf(LineBuffer, "starting_level_path %s", ConfigData->StartingLevelFilePath);
            }
            else if(StartsWith(LineBuffer, "zoom"))
            {
                sscanf(LineBuffer, "zoom %f", &ConfigData->Zoom);
            }
        }
        
        fclose(File);
    }
}

struct directory_data
{
    char** FilePaths;
    char** FileNames;
    i32 FilesLength = 0;
};

inline void FindFilesWithExtensions(const char* DirectoryPath, const char* Extension, directory_data* DirectoryData, b32 WithSubDirectories = false)
{
    if(DirectoryData->FilesLength == 0)
    {
        DirectoryData->FileNames = (char**)malloc(128 * sizeof(char*));
        DirectoryData->FilePaths = (char**)malloc(128 * sizeof(char*));
    }
    
    WIN32_FIND_DATA FindFile;
    HANDLE hFind = NULL;
    
    char Path[2048];
    
    sprintf(Path, "%s\\*.%s", DirectoryPath, Extension);
    
    if((hFind = FindFirstFile(Path, &FindFile)) == INVALID_HANDLE_VALUE)
    {
        DEBUG_PRINT("Path not found: %s\n", DirectoryPath);
        return;
    }
    
    if(FindFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
    {
        if(WithSubDirectories)
            FindFilesWithExtensions(Concat(DirectoryPath, FindFile.cFileName), Extension, DirectoryData, true);
    }
    else
    {
        char* ConcatStr = Concat(DirectoryPath, FindFile.cFileName);
        char* FileName = strtok(FindFile.cFileName, ".");
        
        DirectoryData->FilePaths[DirectoryData->FilesLength] = (char*)malloc((strlen(ConcatStr) + 1) * sizeof(char));
        DirectoryData->FileNames[DirectoryData->FilesLength] = (char*)malloc((strlen(FileName) + 1) * sizeof(char));
        strcpy(DirectoryData->FilePaths[DirectoryData->FilesLength], ConcatStr);
        strcpy(DirectoryData->FileNames[DirectoryData->FilesLength], FileName);
        DirectoryData->FilesLength++;
    }
    
    while(FindNextFile(hFind, &FindFile))
    {
        if(strcmp(FindFile.cFileName, ".") != 0
           && strcmp(FindFile.cFileName, "..") != 0)
        {
            if(FindFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                if(WithSubDirectories)
                    FindFilesWithExtensions(Path, Extension, DirectoryData, true);
            }
            else
            {
                char* ConcatStr = Concat(DirectoryPath, FindFile.cFileName);
                char* FileName = strtok(FindFile.cFileName, ".");
                
                DirectoryData->FilePaths[DirectoryData->FilesLength] = (char*)malloc((strlen(ConcatStr) + 1) * sizeof(char));
                DirectoryData->FileNames[DirectoryData->FilesLength] = (char*)malloc((strlen(FileName)  + 1) * sizeof(char));
                strcpy(DirectoryData->FilePaths[DirectoryData->FilesLength], ConcatStr);
                strcpy(DirectoryData->FileNames[DirectoryData->FilesLength], FileName);
                DirectoryData->FilesLength++;
            }
        }
    }
}

inline void DebugPrintVec2(math::v2 Vec2, const char* Msg = "")
{
    DEBUG_PRINT(Concat(Msg, " (%f,%f)\n"),Vec2.x,Vec2.y);
}

inline b32 FileExists(char* FilePath)
{
    struct stat Buffer;
    return (stat(FilePath,&Buffer) == 0);
}

inline math::v2 ToCartesian(math::v2 Position)
{
    math::v2 TempPt;
    TempPt.x = (2 * Position.y + Position.x) / 2;
    TempPt.y = (2 * Position.y - Position.x) / 2;
    return TempPt;
}

inline math::v2 ToIsometric(math::v2 Position)
{
    Position.x *= 0.5f;
    Position.y *= 0.5f;
    
    math::v2 TempPt;
    TempPt.x = Position.x - Position.y;
    TempPt.y = (Position.x + Position.y) / 2.0f;
    return TempPt;
}

inline r32 Sign(math::v2 P1, math::v2 P2, math::v2 P3)
{
    return (P1.x - P3.x) * (P2.y - P3.y) - (P2.x - P3.x) * (P1.y - P3.y);
}

inline b32 PointInTriangle(math::v2 Pt, math::v2 V1, math::v2 V2, math::v2 V3)
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


#endif