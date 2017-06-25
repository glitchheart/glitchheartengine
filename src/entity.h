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
    AI_Charging,
    AI_Attacking,
    AI_Hit,
    AI_Dying
};

enum Entity_Enum
{
    Entity_Player,
    Entity_Tile,
    Entity_Skeleton,
    Entity_Blob,
    Entity_Weapon,
    Entity_Barrel,
    
    Entity_Max
};

enum Look_Direction
{
    Up, Down, Left, Right
};

struct entity_weapon
{
    collision_AABB CollisionAABB;
    glm::vec2 Center = glm::vec2(0.5, 0.5);
    glm::vec3 Rotation;
    glm::vec3 Scale = glm::vec3(1, 1, 1);
    bool32 IsFlipped;
    Entity_Layer Layer;
    Entity_Layer IgnoreLayers;
};

struct entity_healthbar
{
    glm::vec2 Offset;
    glm::vec3 Scale;
    ui_render_info RenderInfo;
    int32 CurrentFrame;
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
    
    Look_Direction LookDirection = Down;
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
    bool32 Hit = false;
    
    int32 Health = -1;
    
    int32 AttackCount;
    int32 HitAttackCountId = -1;
    
    timer* RecoilTimer;
    timer* HitCooldownTimer;
    real32 HitRecoilSpeed;
    glm::vec2 HitRecoilDirection;
    
    bool32 HasWeapon;
    entity_weapon Weapon;
    
    union
    {
        struct
        {
            bool32 IsAttacking;
            
            timer* AttackCooldownTimer;
            timer* DashTimer;
            timer* DashCooldownTimer;
            timer* PickupCooldownTimer;
            
            real32 AttackMoveSpeed;
            timer* AttackMoveTimer;
            real64 CurrentAttackCooldownTime;
            real64 AttackCooldown;
            
            bool32 IsDashing;
            
            real64 CurrentDashTime;
            real64 MaxDashTime;
            
            real32 DashSpeed;
            uint32 DashCount;
            
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
            AI_State AIState;
            timer* ExplodeStartTimer;
            timer* ExplodeCountdownTimer;
            astar_path AStarPath;
        } Blob;
        struct
        {
            entity_healthbar* Healthbar;
            
            bool32 IsAttacking;
            real32 WalkingSpeed;
            real32 MaxAlertDistance;
            real32 MaxFollowDistance;
            real32 MinDistance;
            
            timer* AttackCooldownTimer;
            timer* ChargingTimer;
            
            AI_State AIState;
            astar_path AStarPath;
            
        } Skeleton;
    };
    
    glm::vec2 Velocity;
};

#define NUM_ENTITIES 100

#endif