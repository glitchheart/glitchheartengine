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
 
 struct collision_AABB;
 
 struct collision_info
 {
     Collision_Side Side;
     collision_AABB* Other[NUM_OTHERS]; 
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
 
 struct sat_collision_info
 {
     math::v3 PV;
     r32 Overlap;
     b32 Colliding;
 };
 
 struct collision_volume
 {
     union
     {
         math::v3 Center;
         r32 c[3];
     };
     
     union
     {
         math::v3 Extents;
         r32 e[3];
     };
     
     math::v3 u[3];
     
     math::quat Orientation; // Need this to get Up/Right/Forward
     b32 Colliding;
     b32 Static;
 };
 
#endif