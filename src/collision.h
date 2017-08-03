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
 
 struct collision_info
 {
     Collision_Side Side;
     entity* Other[NUM_OTHERS]; 
     i32 OtherCount = 0;
 };
 
 struct collision_AABB
 {
     math::v2 Center;
     math::v2 Offset;
     math::v2 Extents;
     math::v2 Min;
     math::v2 Max;
     math::v2 Size;
     b32 IsTrigger;
     b32 IsColliding;
 };
 
#endif