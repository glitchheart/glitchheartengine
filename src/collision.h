#ifndef COLLISION_H
#define COLLISION_H

struct collision_rect
{
    real32 X;
    real32 Y;
    real32 Width;
    real32 Height;
    bool32 Top;
    bool32 Bottom;
    bool32 Left;
    bool32 Right;
};


struct collision_AABB
{
    glm::vec2 Center;
    glm::vec2 Extents;
    glm::vec2 Min;
    glm::vec2 Max;
    glm::vec2 Size;
    
};

#endif