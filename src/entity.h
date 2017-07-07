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
    AI_Idle,
    AI_Alerted,
    AI_Following,
    AI_Charging,
    AI_Attacking,
    AI_Hit,
    AI_Dying,
    AI_Wandering
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
    int32 Damage;
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

struct game_state;

typedef void (*AIFunction)(entity*, game_state*, real64);
#define AI_FUNC(name) void name(entity* Entity, game_state* GameState, real64 DeltaTime)

#define AI_FUNCS(entityname) Entity->Enemy.Idle = & ## entityname ## Idle; \
Entity->Enemy.Alerted = & ## entityname ## Alerted; \
Entity->Enemy.Following = & ## entityname ## Following; \
Entity->Enemy.Charging = & ## entityname ## Charging; \
Entity->Enemy.Attacking = & ## entityname ## Attacking; \
Entity->Enemy.Hit = & ## entityname ## Hit; \
Entity->Enemy.Wandering = & ## entityname ## Wandering; \
Entity->Enemy.Dying = & ## entityname ## Dying; 

struct weapon_collider_info
{
    glm::vec2 OffsetUp;
    glm::vec2 ExtentsUp;
    glm::vec2 OffsetDown;
    glm::vec2 ExtentsDown;
    glm::vec2 OffsetLeft;
    glm::vec2 ExtentsLeft;
    glm::vec2 OffsetRight;
    glm::vec2 ExtentsRight;
    
    weapon_collider_info() {}
};

struct enemy_health_count
{
    bool32 Visible = false;
    char Count[20];
    glm::vec2 Position = glm::vec2(0, 0);
};

struct entity
{
    Entity_Type Type;
    char* Name;
    uint32 EntityIndex;
    glm::vec2 Position;
    glm::vec2 Center = glm::vec2(0.5, 0.5);
    glm::vec3 Rotation;;
    real32 Scale;
    bool32 IsFlipped;
    
    Look_Direction LookDirection = Down;
    Entity_Layer Layer;
    Entity_Layer IgnoreLayers;
    
    animation_info AnimationInfo;
    animation* CurrentAnimation = 0;
    
    uint32 RenderEntityHandle;
    
    bool32 Active = true;
    bool32 Dead = false;
    collision_AABB CollisionAABB;
    bool32 IsKinematic = false;
    bool32 IsColliding = false;
    bool32 IsStatic = false; // For stuff that can't be moved by collision
    bool32 IsPickup = false;
    bool32 HasHitTrigger = false;
    collision_AABB HitTrigger;
    
    glm::vec2 Velocity;
    
    bool32 Hit = false;
    
    int32 HitFlickerFramesLeft = 0;
    int32 HitFlickerFrameMax = 6;
    timer HitFlickerTimer;
    
    timer HitAttackCountIdResetTimer;
    
    int32 FullHealth;
    int32 Health = -1;
    int32 HealthLost;
    timer HealthDecreaseTimer;
    
    int32 AttackCount;
    int32 HitAttackCountId = -1;
    
    timer RecoilTimer;
    timer HitCooldownTimer;
    real32 HitRecoilSpeed;
    glm::vec2 HitRecoilDirection;
    
    real32 AttackMoveSpeed;
    timer AttackMoveTimer;
    
    bool32 HasWeapon = false;
    entity_weapon Weapon;
    weapon_collider_info WeaponColliderInfo;
    
    union
    {
        struct
        {
            bool32 IsAttacking = false;
            
            int32 FullStamina;
            int32 Stamina;
            int32 StaminaLost;
            int32 HitStaminaCost;
            int32 RollStaminaCost;
            int32 AttackStaminaCost;
            
            timer StaminaGainTimer;
            timer StaminaDecreaseTimer;
            
            timer AttackCooldownTimer;
            timer LastAttackTimer;
            timer DashTimer;
            timer DashCooldownTimer;
            timer PickupCooldownTimer;
            
            real64 CurrentAttackCooldownTime;
            real64 AttackCooldown;
            
            bool32 IsDashing = false;
            real32 DashCounterDivider;
            real64 CurrentDashTime;
            real64 MaxDashTime;
            
            real32 DashDirectionX;
            real32 DashDirectionY;
            real32 DashSpeed;
            uint32 DashCount;
            
            bool32 IsDefending = false;
            
            real64 CurrentDashCooldownTime;
            real64 DashCooldown;
            
            real32 WalkingSpeed;
            entity* Pickup = 0;
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
            entity_healthbar* Healthbar = 0;
            
            int32 HealthCountIndex = 0;
            enemy_health_count HealthCounts[10];
            glm::vec2 HealthCountStart;
            
            AI_State AIState;
            astar_path AStarPath;
            bool32 IsTargeted = false;
            real32 TargetingPositionX;
            real32 TargetingPositionY;
            real32 MinDistanceToPlayer;
            real32 MaxAlertDistance;
            real32 SlowdownDistance;
            real32 MaxFollowDistance;
            real32 AttackDistance;
            real32 WalkingSpeed;
            real32 CloseToPlayerSpeed;
            AIFunction Idle;
            AIFunction Alerted;
            AIFunction Following;
            AIFunction Charging;
            AIFunction Attacking;
            AIFunction Hit;
            AIFunction Dying;
            AIFunction Wandering;
            
            union
            {
                struct
                {
                    real32 ExplosionCollisionExtentsX;
                    real32 ExplosionCollisionExtentsY;
                    timer ExplodeStartTimer;
                    timer ExplodeCountdownTimer;
                    
                    bool32 InPickupMode = false;
                    timer PickupThrowTimer;
                } Blob;
                struct
                {
                    bool32 IsAttacking = false;
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                } Skeleton;
                struct
                {
                    bool32 IsAttacking = false;
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                } Wraith;
            };
        } Enemy;
        struct
        {
            timer PickupThrowTimer;
        } Pickup;
    };
    
    bool32 RenderButtonHint = false;
    
    entity(){}
};

#define NUM_ENTITIES 100

#endif