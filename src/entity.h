#ifndef ENTITY_H
#define ENTITY_H

enum Entity_Layer
{
    Layer_Player = 1 << 0,
    Layer_Environment = 1 << 1,
    Layer_Enemy = 1 << 2,
};

enum AI_State
{
    AI_Sleeping,
    AI_Idle,
    AI_Alerted,
    AI_Following,
    AI_Attacking,
    AI_Hit
};

enum Entity_Enum
{
    Entity_Player,
    Entity_PlayerWeapon,
    Entity_Tile,
    Entity_PalmTree,
    Entity_Enemy,
    Entity_EnemyWeapon,
    Entity_Barrel,
    
    Entity_Max
};

struct entity
{
    Entity_Enum Type;
    char* Name;
    uint32 EntityIndex;
    glm::vec2 Position;
    glm::vec2 Center = glm::vec2(0.5, 0.5);
    glm::vec3 Rotation;
    glm::vec3 Scale = glm::vec3(1, 1, 1);
    bool32 IsFlipped;
    
    Entity_Layer Layer;
    Entity_Layer IgnoreLayers;
    
    animation_info AnimationInfo;
    animation* CurrentAnimation;
    
    uint32 RenderEntityHandle;
    
    bool32 Active;
    collision_AABB CollisionAABB;
    bool32 IsKinematic;
    bool32 IsColliding;
    bool32 IsStatic; // For stuff that can't be moved by collision
    bool32 Pickup;
    collision_AABB* HitTrigger;
    
    int32 Health = -1;
    real64 HitCooldownLeft;
    real64 HitCooldownTime;
    union
    {
        struct
        {
            bool32 IsAttacking;
            real64 CurrentAttackCooldownTime;
            real64 AttackCooldown;
            bool32 IsDashing;
            real64 CurrentDashTime;
            real32 DashSpeed;
            real64 MaxDashTime;
            real64 CurrentDashCooldownTime;
            real64 DashCooldown;
            real32 WalkingSpeed;
            entity* Pickup;
            real64 PickupCooldown;
            real32 ThrowingSpeed;
            real32 LastKnownDirectionX;
            real32 LastKnownDirectionY;
        } Player;
        struct
        {
        } PalmTree;
        struct
        {
            bool32 IsAttacking;
            real32 WalkingSpeed;
            real32 MaxAlertDistance;
            real32 MinDistance;
            real64 AttackCooldown;
            real64 AttackCooldownCounter;
            AI_State AIState;
            real32 AStarCooldown;
            real32 AStarInterval;
            bool32 Path;
        } Enemy;
        struct
        {
            uint32 EntityHandle;
        } Weapon;
        
    };
    glm::vec2* AStarPath;
    uint32 AStarPathLength;
    uint32 PathIndex;
    glm::vec2 Velocity;
};

#define NUM_ENTITIES 20

#endif