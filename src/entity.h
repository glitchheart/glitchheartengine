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

enum Entity_Type
{
    Entity_Player,
    Entity_Enemy,
    Entity_Tile,
    Entity_Weapon,
    Entity_Barrel,
    Entity_RenderItem,
    Entity_Max
};

enum Enemy_Type
{
    Enemy_Blob,
    Enemy_Skeleton,
    Enemy_Wraith
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
    Entity_Type Type;
    char* Name;
    uint32 EntityIndex;
    glm::vec2 Position;
    glm::vec2 Center = glm::vec2(0.5, 0.5);
    glm::vec3 Rotation;;
    glm::vec3 Scale = glm::vec3(1, 1, 1);
    bool32 IsFlipped;
    
    Look_Direction LookDirection = Down;
    Entity_Layer Layer;
    Entity_Layer IgnoreLayers;
    
    animation_info AnimationInfo;
    animation* CurrentAnimation;
    
    uint32 RenderEntityHandle;
    
    bool32 Active;
    bool32 Dead;
    collision_AABB CollisionAABB;
    bool32 IsKinematic;
    bool32 IsColliding;
    bool32 IsStatic; // For stuff that can't be moved by collision
    bool32 IsPickup;
    bool32 HasHitTrigger;
    collision_AABB HitTrigger;
    
    bool32 Hit = false;
    
    int32 HitFlickerFramesLeft = 0;
    int32 HitFlickerFrameMax = 6;
    timer* HitFlickerTimer;
    
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
            timer* LastAttackTimer;
            timer* DashTimer;
            timer* DashCooldownTimer;
            timer* PickupCooldownTimer;
            
            real32 AttackMoveSpeed;
            timer* AttackMoveTimer;
            real64 CurrentAttackCooldownTime;
            real64 AttackCooldown;
            
            bool32 IsDashing;
            real32 DashCounterDivider;
            real64 CurrentDashTime;
            real64 MaxDashTime;
            
            real32 DashDirectionX;
            real32 DashDirectionY;
            real32 DashSpeed;
            uint32 DashCount;
            
            real64 CurrentDashCooldownTime;
            real64 DashCooldown;
            
            real32 WalkingSpeed;
            entity* Pickup;
            bool32 RenderCrosshair;
            real32 CrosshairRadius;
            real32 CrosshairPositionX;
            real32 CrosshairPositionY;
            
            real64 PickupCooldown;
            real32 ThrowingSpeed;
            real32 LastKnownDirectionX;
            real32 LastKnownDirectionY;
            real32 TargetingDistance;
            int32 TargetedEnemyHandle;
            int32 DustCloudHandle;
        } Player;
        struct
        {
            Enemy_Type EnemyType;
            entity_healthbar* Healthbar;
            AI_State AIState;
            astar_path AStarPath;
            bool32 IsTargeted;
            real32 TargetingPositionX;
            real32 TargetingPositionY;
            real32 MinDistanceToPlayer;
            real32 MaxAlertDistance;
            real32 MaxFollowDistance;
            real32 WalkingSpeed;
            
            union
            {
                struct
                {
                    real32 ExplosionCollisionExtentsX;
                    real32 ExplosionCollisionExtentsY;
                    timer* ExplodeStartTimer;
                    timer* ExplodeCountdownTimer;
                    
                    bool32 InPickupMode;
                    timer* PickupThrowTimer;
                } Blob;
                struct
                {
                    bool32 IsAttacking;
                    timer* AttackCooldownTimer;
                    timer* ChargingTimer;
                    
                } Skeleton;
                struct
                {
                    bool32 IsAttacking;
                    timer* AttackCooldownTimer;
                    timer* ChargingTimer;
                } Wraith;
            };
        } Enemy;
        struct
        {
            timer* PickupThrowTimer;
        } Pickup;
    };
    
    bool32 RenderButtonHint;
    glm::vec2 Velocity;
};

#define NUM_ENTITIES 100

#endif