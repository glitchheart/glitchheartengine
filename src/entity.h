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
    AI_Defending,
    AI_Attacking,
    AI_Hit,
    AI_Dying,
    AI_Wandering,
    AI_Count
};

#define AIPAIR(name) {AI_ ## name, "AI_" "" #name}

const static struct
{
    AI_State Val;
    char* Str;
    
} AIConversion [] =
{
    AIPAIR(Idle),
    AIPAIR(Alerted),
    AIPAIR(Following),
    AIPAIR(Charging),
    AIPAIR(Defending),
    AIPAIR(Attacking),
    AIPAIR(Hit),
    AIPAIR(Dying),
    AIPAIR(Wandering)
};

char* AIEnumToStr(AI_State State)
{
    for(i32 Index = 0; Index < AI_Count; Index++)
    {
        if(State == AIConversion[Index].Val)
        {
            return AIConversion[Index].Str;
        }
    }
    Assert(false);
}

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
Entity->Enemy.Defending = & ## entityname ## Defending; \
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
    b32 HasCheckpoint;
};


enum Loot_Type
{
    Loot_Health,
    Loot_Checkpoint
};

struct loot
{
    glm::vec2 Position;
    Loot_Type Type;
    b32 RenderButtonHint;
    i32 Handle = -1;
    i32 OwnerHandle;
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
            r32 ConstantAtten;
            r32 LinearAtten;
            r32 ExponentialAtten;
        } Pointlight;
        struct
        {
            r32 Intensity;
        } Ambient;
    };
    
    light_source(){}
};

enum Object_Type
{
    Object_Shadow,
    Object_Rock
};

struct object_entity
{
    b32 Active;
    Object_Type Type;
    glm::vec2 Position;
    r32 Scale;
    glm::vec2 Center;
    b32 IsFlipped;
    i32 RenderEntityHandle;
    i32 LightSourceHandle;
    
    animation* CurrentAnimation;
    animation_info AnimationInfo;
    
    union
    {
        struct
        {
            b32 IsKinematic;
            collision_AABB Collider;
            glm::vec2 Velocity;
        } Moving;
    };
    
    object_entity(){}
};

struct entity
{
    Entity_Type Type;
    char* Name;
    u32 EntityIndex;
    glm::vec2 Position;
    glm::vec2 Center;
    glm::vec3 Rotation;
    r32 Scale;
    b32 IsFlipped;
    b32 IsTemporary;
    
    Look_Direction LookDirection;
    Entity_Layer Layer;
    Entity_Layer IgnoreLayers;
    
    animation_info AnimationInfo;
    animation* CurrentAnimation;
    
    u32 RenderEntityHandle;
    
    b32 Active;
    b32 Dead;
    collision_AABB CollisionAABB;
    b32 IsKinematic;
    b32 IsColliding;
    b32 IsStatic; // For stuff that can't be moved by collision
    b32 IsPickup;
    b32 HasHitTrigger;
    collision_AABB HitTrigger;
    
    i32 LightSourceHandle;
    
    glm::vec2 Velocity;
    
    b32 Hit;
    
    i16 HitFlickerFramesLeft;
    i16 HitFlickerFrameMax;
    timer HitFlickerTimer;
    
    timer HitAttackCountIdResetTimer;
    
    b32 Invincible;
    i16 FullHealth;
    i16 Health;
    i16 HealthLost;
    timer HealthDecreaseTimer;
    
    i32 AttackCount;
    i32 HitAttackCountId;
    
    timer RecoilTimer;
    timer StaggerCooldownTimer;
    r32 HitRecoilSpeed;
    glm::vec2 HitRecoilDirection;
    
    r32 AttackMoveSpeed;
    timer AttackMoveTimer;
    i32 AttackLowFrameIndex;
    i32 AttackHighFrameIndex;
    
    b32 HasWeapon;
    entity_weapon Weapon;
    weapon_collider_info WeaponColliderInfo;
    
    union
    {
        struct
        {
            i32 Level;
            i32 LastMilestone;
            i32 Experience;
            b32 IsAttacking;
            b32 IsChargingCheckpoint;
            
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
            r64 StaminaGainTimerFast;
            r64 StaminaGainTimerSlow;
            
            timer CheckpointPlacementTimer;
            timer CheckpointPlacementCooldownTimer;
            
            timer AttackCooldownTimer;
            timer LastAttackTimer;
            timer DashTimer;
            timer DashCooldownTimer;
            timer PickupCooldownTimer;
            
            r64 CurrentAttackCooldownTime;
            r64 AttackCooldown;
            
            b32 IsDashing;
            r32 DashCounterDivider;
            r64 CurrentDashTime;
            r64 MaxDashTime;
            
            r32 DashDirectionX;
            r32 DashDirectionY;
            r32 DashSpeed;
            u32 DashCount;
            
            b32 IsDefending;
            
            r64 CurrentDashCooldownTime;
            r64 DashCooldown;
            
            r32 WalkingSpeed;
            entity* Pickup;
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
            entity_healthbar* Healthbar;
            
            i32 Experience;
            i32 HealthCountIndex;
            enemy_health_count HealthCounts[10];
            glm::vec2 HealthCountStart;
            
            i32 TimesHit;
            
            b32 HasLoot;
            loot Loot;
            
            i32 AttackMode;
            AI_State AIState;
            astar_path AStarPath;
            b32 IsTargeted;
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
            AIFunction Defending;
            AIFunction Attacking;
            AIFunction Hit;
            AIFunction Dying;
            AIFunction Wandering;
            
            i32 WaypointCount;
            v2i Waypoints[10];
            i32 WaypointIndex;
            b32 WanderingForward;
            
            timer DefendingTimer;
            glm::vec2 LastAttackMoveDirection;
            
            union
            {
                struct
                {
                    r32 ExplosionCollisionExtentsX;
                    r32 ExplosionCollisionExtentsY;
                    timer ExplodeStartTimer;
                    timer ExplodeCountdownTimer;
                    
                    b32 InPickupMode;
                    timer PickupThrowTimer;
                } Blob;
                struct
                {
                    b32 IsAttacking;
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                    timer AlertedTimer;
                } Skeleton;
                struct
                {
                    b32 IsAttacking;
                    i32 MaxAttackStreak;
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                    timer AlertedTimer;
                    timer JumpAttackTimer;
                    timer JumpAttackImpactTimer;
                    glm::vec2 ImpactCollisionExtents;
                    glm::vec2 OldCollisionExtents;
                    i32 ShadowHandle;
                } Minotaur;
                struct
                {
                    b32 IsAttacking;
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
    
    glm::vec2 RenderButtonOffset;
    b32 RenderButtonHint;
    
    entity(){}
};

#define NUM_ENTITIES 100

#endif