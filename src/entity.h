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
    glm::vec2 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale = glm::vec3(1, 1, 1);
    uint32 TextureHandle;
    uint32 ShaderIndex;
    union
    {
        struct
        {
            real32 WalkingSpeed;
        } player;
        struct
        {
        } palm_tree;
        struct
        {
        } enemy;
    };
};

#define NUM_ENTITIES 10

//TODO(daniel) Put this to use
struct entity_manager
{
    uint16 Count;
    entity Entities[NUM_ENTITIES];
};

#endif