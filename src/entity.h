#ifndef ENTITY_H
#define ENTITY_H

enum Entity_Enum
{
    Entity_Player,
    Entity_Crosshair,
    Entity_Tile,
    Entity_PalmTree,
    Entity_Enemy,
    Entity_Barrel,
    
    Entity_Max
};

struct entity
{
    Entity_Enum Type;
    char* Name;
    uint32 EntityIndex;
    glm::vec2 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale = glm::vec3(1, 1, 1);
    
    char* CurrentAnimation;
    
    std::map<char*, animation, CompareCStrings> Animations;
    render_entity RenderEntity;
    
    collision_AABB CollisionAABB;
    bool32 IsKinematic;
    bool32 IsColliding;
    bool32 IsStatic; // For stuff that can't be moved by collision
    union
    {
        struct
        {
            bool32 IsAttacking;
            real32 WalkingSpeed;
        } Player;
        struct
        {
        } PalmTree;
        struct
        {
        } Enemy;
    };
    glm::vec2 Velocity;
    
};

#define NUM_ENTITIES 2048

#endif