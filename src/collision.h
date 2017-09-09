 #ifndef COLLISION_H
#define COLLISION_H
 
 enum Collision_Side
 {
     Side_None = 1 << 0, 
     Side_Top = 1 << 1, 
     Side_Bottom = 1 << 2, 
     Side_Left = 1 << 3, 
     Side_Right = 1 << 4
 };
 
 inline Collision_Side operator|(Collision_Side a, Collision_Side b)
 {
     return static_cast<Collision_Side>(static_cast<int>(a) | static_cast<int>(b));
 }
 
#define NUM_OTHERS 40 //@Improvement: We need a more dynamic way for this. This gives us bugs, when we try to write to more than the size allows.
 
 struct entity;
 
 struct collision_info
 {
     Collision_Side Side;
     entity* Other[NUM_OTHERS]; 
     i32 OtherCount = 0;
 };
 
 struct collision_AABB
 {
     math::v3 Center;
     math::v3 Offset;
     math::v3 Extents;
     math::v3 Min;
     math::v3 Max;
     math::v3 Size;
     b32 IsTrigger;
     b32 IsColliding;
 };
 
 struct collision_volume
 {
     math::v3 Center;
     math::v3 Extents;
     math::quat Orientation; // Need this to get Up/Right/Forward
     
     //@Incomplete: Volumes always cubes as of now
     math::v4 Vertices[8] =
     {
         math::v4(-0.5, -0.5, -0.5, 1.0),
         math::v4(0.5, -0.5, -0.5, 1.0),
         math::v4(0.5,  0.5, -0.5, 1.0),
         math::v4(-0.5,  0.5, -0.5, 1.0),
         math::v4(-0.5, -0.5,  0.5, 1.0),
         math::v4(0.5, -0.5,  0.5, 1.0),
         math::v4(0.5,  0.5,  0.5, 1.0),
         math::v4(-0.5,  0.5,  0.5, 1.0),
     };
     
     math::v3 Normals[6] = 
     {
         math::v3(1.0f, 0.0f, 0.0f),
         math::v3(0.0f, 1.0f, 0.0f),
         math::v3(0.0f, 0.0f, 1.0f),
         math::v3(-1.0f, 0.0f, 0.0f),
         math::v3(0.0f, -1.0f, 0.0f),
         math::v3(0.0f, 0.0f, -1.0f)
     };
 };
 
 inline void SAT(collision_volume& V1, collision_volume& V2)
 {
     
 }
 
 math::v2 Project(math::v3* Vertices, math::v3 Axis, i32 VertexCount)
 {
     if(VertexCount > 0)
     {
         r32 Min = math::Dot(Axis, Vertices[0]);
         r32 Max = Min;
         for(i32 I = 1; I < VertexCount; I++)
         {
             r32 P = Dot(Axis, Vertices[I]);
             if(P < Min)
             {
                 Min = P;
             }
             else if(P > Max)
             {
                 Max = P;
             }
         }
         return math::v2(Min, Max);
     }
     
     return math::v2();
 }
 
#endif