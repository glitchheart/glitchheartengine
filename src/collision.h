#ifndef COLLISION_H
#define COLLISION_H

enum Collision_Side
{
    Side_None, Side_Top, Side_Bottom, Side_Left, Side_Right
};

struct collision_info
{
    Collision_Side Side;
    char* Other;
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