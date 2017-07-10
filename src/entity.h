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
    Entity_Bonfire,
    Entity_RenderItem,
    Entity_Max
};

enum Enemy_Type
{
    Enemy_Blob,
    Enemy_Skeleton,
    Enemy_Wraith,
    Enemy_Minotaur
};


enum Light_Type
{
    Light_Pointlight, Light_Ambient, LightCount
};


enum Look_Direction
{
    Up, Down, Left, Right
};

struct entity_weapon
{
    i32 Damage;
    collision_AABB CollisionAABB;
    glm::vec2 Center = glm::vec2(0.5, 0.5);
    glm::vec3 Rotation;
    glm::vec3 Scale = glm::vec3(1, 1, 1);
    b32 IsFlipped;
    Entity_Layer Layer;
    Entity_Layer IgnoreLayers;
};

struct entity_healthbar
{
    glm::vec2 Offset;
    glm::vec3 Scale;
    ui_render_info RenderInfo;
    i32 CurrentFrame;
};

struct game_state;

typedef void (*AIFunction)(entity*, game_state*, r64);
#define AI_FUNC(name) void name(entity* Entity, game_state* GameState, r64 DeltaTime)

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
    b32 Visible = false;
    char Count[20];
    glm::vec2 Position = glm::vec2(0, 0);
};

struct player_inventory
{
    i32 HealthPotionCount = 0;
};

struct loot
{
    i32 HealthPotions = 0;
};

struct light_source
{
    Light_Type Type;
    b32 Active = false;
    glm::vec4 Color;
    union
    {
        struct
        {
            r32 Radius;
            r32 Intensity;
            glm::vec2 Position;
        } Pointlight;
        struct
        {
            r32 Intensity;
        } Ambient;
    };
    
    light_source(){}
};

struct entity
{
    Entity_Type Type;
    char* Name;
    u32 EntityIndex;
    glm::vec2 Position;
    glm::vec2 Center = glm::vec2(0.5, 0.5);
    glm::vec3 Rotation;
    r32 Scale;
    b32 IsFlipped;
    
    Look_Direction LookDirection = Down;
    Entity_Layer Layer;
    Entity_Layer IgnoreLayers;
    
    animation_info AnimationInfo;
    animation* CurrentAnimation = 0;
    
    u32 RenderEntityHandle;
    
    b32 Active = true;
    b32 Dead = false;
    collision_AABB CollisionAABB;
    b32 IsKinematic = false;
    b32 IsColliding = false;
    b32 IsStatic = false; // For stuff that can't be moved by collision
    b32 IsPickup = false;
    b32 HasHitTrigger = false;
    collision_AABB HitTrigger;
    
    i32 LightSourceHandle = -1;
    
    glm::vec2 Velocity;
    
    b32 Hit = false;
    
    i16 HitFlickerFramesLeft = 0;
    i16 HitFlickerFrameMax = 6;
    timer HitFlickerTimer;
    
    timer HitAttackCountIdResetTimer;
    
    i16 FullHealth;
    i16 Health = -1;
    i16 HealthLost;
    timer HealthDecreaseTimer;
    
    i32 AttackCount;
    i32 HitAttackCountId = -1;
    
    timer RecoilTimer;
    timer HitCooldownTimer;
    r32 HitRecoilSpeed;
    glm::vec2 HitRecoilDirection;
    
    r32 AttackMoveSpeed;
    timer AttackMoveTimer;
    i32 AttackLowFrameIndex;
    i32 AttackHighFrameIndex;
    
    b32 HasWeapon = false;
    entity_weapon Weapon;
    weapon_collider_info WeaponColliderInfo;
    
    union
    {
        struct
        {
            b32 IsAttacking = false;
            
            i16 FullStamina;
            i16 Stamina;
            i16 StaminaLost;
            i16 HitStaminaCost;
            i16 RollStaminaCost;
            i16 AttackStaminaCost;
            i16 MinDiffStamina;
            
            player_inventory Inventory;
            
            timer StaminaGainCooldownTimer;
            timer StaminaGainTimer;
            timer StaminaDecreaseTimer;
            
            timer AttackCooldownTimer;
            timer LastAttackTimer;
            timer DashTimer;
            timer DashCooldownTimer;
            timer PickupCooldownTimer;
            
            r64 CurrentAttackCooldownTime;
            r64 AttackCooldown;
            
            b32 IsDashing = false;
            r32 DashCounterDivider;
            r64 CurrentDashTime;
            r64 MaxDashTime;
            
            r32 DashDirectionX;
            r32 DashDirectionY;
            r32 DashSpeed;
            u32 DashCount;
            
            b32 IsDefending = false;
            
            r64 CurrentDashCooldownTime;
            r64 DashCooldown;
            
            r32 WalkingSpeed;
            entity* Pickup = 0;
            b32 RenderCrosshair;
            r32 CrosshairRadius;
            r32 CrosshairPositionX;
            r32 CrosshairPositionY;
            
            r64 PickupCooldown;
            r32 ThrowingSpeed;
            r32 LastKnownDirectionX;
            r32 LastKnownDirectionY;
            r32 TargetingDistance;
            i32 TargetedEnemyHandle;
            i32 DustCloudHandle;
            
        } Player;
        struct
        {
            Enemy_Type EnemyType;
            entity_healthbar* Healthbar = 0;
            
            i32 HealthCountIndex = 0;
            enemy_health_count HealthCounts[10];
            glm::vec2 HealthCountStart;
            
            b32 HasLoot = false;
            loot Loot;
            
            AI_State AIState;
            astar_path AStarPath;
            b32 IsTargeted = false;
            r32 TargetingPositionX;
            r32 TargetingPositionY;
            r32 MinDistanceToPlayer;
            r32 MaxAlertDistance;
            r32 SlowdownDistance;
            r32 MaxFollowDistance;
            r32 AttackDistance;
            r32 WalkingSpeed;
            r32 WanderingSpeed;
            r32 CloseToPlayerSpeed;
            AIFunction Idle;
            AIFunction Alerted;
            AIFunction Following;
            AIFunction Charging;
            AIFunction Attacking;
            AIFunction Hit;
            AIFunction Dying;
            AIFunction Wandering;
            
            i32 WaypointCount = 0;
            v2i Waypoints[10];
            i32 WaypointIndex = 0;
            b32 WanderingForward = true;
            
            union
            {
                struct
                {
                    r32 ExplosionCollisionExtentsX;
                    r32 ExplosionCollisionExtentsY;
                    timer ExplodeStartTimer;
                    timer ExplodeCountdownTimer;
                    
                    b32 InPickupMode = false;
                    timer PickupThrowTimer;
                } Blob;
                struct
                {
                    b32 IsAttacking = false;
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                    timer AlertedTimer;
                } Skeleton;
                struct
                {
                    b32 IsAttacking = false;
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                    timer AlertedTimer;
                } Minotaur;
                struct
                {
                    b32 IsAttacking = false;
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                    timer AlertedTimer;
                } Wraith;
            };
        } Enemy;
        struct
        {
            timer PickupThrowTimer;
        } Pickup;
    };
    
    glm::vec2 RenderButtonOffset = glm::vec2(0.5f, 1.5f);
    b32 RenderButtonHint = false;
    
    entity(){}
};

#define NUM_ENTITIES 100

#endif