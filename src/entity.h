#ifndef ENTITY_H
#define ENTITY_H

enum Entity_Enum
{
    Entity_Player,
    Entity_Crosshair,
    Entity_Tile,
    Entity_PalmTree,
    Entity_Enemy,
    
    Entity_Max
};

struct entity
{
    Entity_Enum Type;
    uint32 EntityIndex;
    glm::vec2 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale = glm::vec3(1, 1, 1);
    
    char* CurrentAnimation;
    
    std::map<char*, animation, CompareCStrings> Animations;
    render_entity RenderEntity;
    collision_rect CollisionRect;
    collision_AABB CollisionAABB;
    bool32 IsKinematic;
    bool32 IsColliding;
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
    
    
};

#define NUM_ENTITIES 16

//TODO(daniel) Put this to use

#endif