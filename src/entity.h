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
    return 0;
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
    North, 
    NorthEast, 
    East, 
    SouthEast, 
    South, 
    SouthWest, 
    West, 
    NorthWest,
    
    LookDirection_Count
};

struct entity_weapon
{
    i32 Damage;
    collision_AABB CollisionAABB;
    math::v2 Center = math::v2(0.5, 0.5);
    math::v3 Rotation;
    math::v3 Scale = math::v3(1, 1, 1);
    b32 IsFlipped;
    Entity_Layer Layer;
    Entity_Layer IgnoreLayers;
};

struct entity_healthbar
{
    math::v2 Offset;
    math::v3 Scale;
    ui_render_info RenderInfo;
    i32 CurrentFrame;
};

struct game_state;

using AIFunction = void(*)(entity*, game_state*, sound_queue*, r64);
#define AI_FUNC(name) void name(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)

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
    math::v2 OffsetUp;
    math::v2 ExtentsUp;
    math::v2 OffsetDown;
    math::v2 ExtentsDown;
    math::v2 OffsetLeft;
    math::v2 ExtentsLeft;
    math::v2 OffsetRight;
    math::v2 ExtentsRight;
    
    weapon_collider_info() {}
};

struct enemy_health_count
{
    b32 Visible = false;
    char Count[20];
    math::v2 Position = math::v2(0, 0);
};

struct player_inventory
{
    i32 HealthPotionCount;
    b32 HasCheckpoint;
};

enum Loot_Type
{
    Loot_Nothing,
    Loot_Health,
    Loot_Checkpoint,
    Loot_LevelItem
};

struct loot
{
    Loot_Type Type;
    b32 RenderButtonHint;
    i32 Handle = -1;
    i32 OwnerHandle;
};


struct light_source
{
    Light_Type Type;
    b32 Active;
    math::v4 Color;
    union
    {
        struct
        {
            r32 Intensity;
            math::v3 Position;
            math::v3 RenderPosition;
            math::v3 Offset;
            r32 ConstantAtten;
            r32 LinearAtten;
            r32 ExponentialAtten;
            timer GlowTimer;
            r32 GlowIncrease;
            b32 IncreasingGlow;
            r32 EmissionIntensity;
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
    Object_Rock,
    Object_Loot,
    Object_Will,
    Object_Tree
};

enum Render_Type
{
    Render_Type_Entity,
    Render_Type_Object
};

struct object_entity
{
    b32 Active;
    Object_Type Type;
    b32 UsesTransparency;
    math::v2 Position;
    r32 Scale;
    math::v2 Center;
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
            math::v2 Velocity;
        } Moving;
        struct
        {
            
        } Inanimate;
    };
    
    object_entity(){}
};

struct render_entity
{
    Render_Type RenderType = Render_Type_Entity;
    
    union
    {
        entity* Entity;
        object_entity* Object;
    };
    
    b32 Rendered = true;
    b32 Background = false;
    
    char* TextureName;
    Shader_Type Shader;
    math::v4 Color = math::v4(1, 1, 1, 1);
};

struct hit_tile_extents
{
    i32 StartX;
    i32 EndX;
    i32 StartY;
    i32 EndY;
};

struct entity
{
    Entity_Type Type;
    char* Name;
    u32 EntityIndex;
    
    math::v3 Position;
    math::v2i CurrentTile;
    math::v2 CurrentDestination;
    i32 FramesForDestination;
    math::v3 Center;
    math::v3 Rotation;
    
    r32 Scale;
    b32 IsFlipped;
    b32 IsTemporary;
    b32 ShowAttackTiles;
    
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
    b32 HasHitTrigger;
    collision_AABB HitTrigger;
    
    i32 LightSourceHandle;
    
    math::v3 Velocity;
    
    b32 Hit;
    
    i16 HitFlickerFramesLeft;
    i16 HitFlickerFrameMax;
    timer HitFlickerTimer;
    
    timer HitAttackCountIdResetTimer;
    
    b32 IsAttacking;
    b32 Invincible;
    i32 FullHealth;
    i32 Health;
    i32 HealthLost;
    timer HealthDecreaseTimer;
    
    i32 AttackCount;
    i32 HitAttackCountId;
    
    timer RecoilTimer;
    timer StaggerCooldownTimer;
    r32 HitRecoilSpeed;
    math::v3 HitRecoilDirection;
    
    r32 AttackMoveSpeed;
    timer AttackMoveTimer;
    i32 AttackLowFrameIndex;
    i32 AttackHighFrameIndex;
    
    b32 HasWeapon;
    entity_weapon Weapon;
    weapon_collider_info WeaponColliderInfo;
    
    hit_tile_extents HitExtents[LookDirection_Count];
    
    union
    {
        struct
        {
            i32 Level;
            i32 Will;
            b32 IsChargingCheckpoint;
            
            i32 FullStamina;
            i32 Stamina;
            i32 StaminaLost;
            i32 HitStaminaCost;
            i32 RollStaminaCost;
            i32 AttackStaminaCost;
            i32 MinDiffStamina;
            
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
            timer HealthPotionTimer;
            
            b32 TakingHealthPotion;
            
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
            b32 RenderCrosshair;
            r32 CrosshairRadius;
            r32 CrosshairPositionX;
            r32 CrosshairPositionY;
            
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
            collision_AABB EnemyCollider;
            
            entity_healthbar* Healthbar;
            
            i32 Will;
            i32 HealthCountIndex;
            enemy_health_count HealthCounts[10];
            math::v2 HealthCountStart;
            
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
            math::v3i Waypoints[10];
            i32 WaypointIndex;
            b32 WanderingForward;
            
            timer DefendingTimer;
            math::v2 LastAttackMoveDirection;
            
            union
            {
                struct
                {
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                    timer AlertedTimer;
                } Skeleton;
                struct
                {
                    i32 MaxAttackStreak;
                    timer AttackCooldownTimer;
                    timer ChargingTimer;
                    timer AlertedTimer;
                    timer JumpAttackTimer;
                    timer JumpAttackImpactTimer;
                    math::v2 ImpactCollisionExtents;
                    math::v2 OldCollisionExtents;
                    i32 ShadowHandle;
                } Minotaur;
            };
        } Enemy;
        struct
        {
        } Bonfire;
    };
    
    math::v2 RenderButtonOffset;
    b32 RenderButtonHint;
    
    entity(){}
};

#endif