#ifndef PARTICLES_H
#define PARTICLES_H

enum ParticleSpace
{
    PS_WORLD,
    PS_OBJECT
};

enum StartParameterType
{
    CONSTANT,
    RANDOM_BETWEEN_TWO_CONSTANTS
};

struct Burst
{
    i32 count;
    i32 cycle_count;
    i32 max_count;
    i32 min_count;
    r64 repeat_interval;
};

enum EmissionType
{
    ET_OVER_TIME,
    ET_OVER_DISTANCE
};

struct ParticleSpawnInfo
{
    Vec3_4x position;
    Vec3_4x direction;
};

#define EMITTER_FUNC(name) ParticleSpawnInfo name(RandomSeries& series, r32 min, r32 max)
typedef EMITTER_FUNC(EmitterFunc);

enum EmissionFuncType
{
    DIRECTION,
    RANDOM_DIRECTION,
    SQUARE_2D,
    SQUARE_2D_RANDOM,
    SQUARE,
    SQUARE_RANDOM,
    DISC,
    DISC_RANDOM,
    CIRCLE,
    CIRCLE_RANDOM
};

struct EmissionModule
{
    EmissionType type;
    
    struct
    {
        Burst *values;
        i32 value_count;
        i32 current_index;
    } burst_over_lifetime;
    
    r32 min;
    r32 max;
    r32 rate_over_distance;

    EmissionFuncType emitter_func_type;
};

struct ParticleSystemAttributes
{
    b32 looping;
    r32 duration;
    b32 play_on_awake;

    b32 prewarm;

    ParticleSpace particle_space;
    rendering::BufferHandle buffer;

    math::Rgba start_color;
    
    StartParameterType start_size_type;
    union
    {
        struct
        {
            // @Note: Right now, one value makes more sense than x,y
            r32 s0;
            r32 s1;
        } random_between_two_constants;
        struct
        {
            r32 start_size;            
        } constant;
    } size;

    StartParameterType start_angle_type;
    union
    {
        struct
        {
            r32 a0;
            r32 a1;
        } random_between_two_constants;
        struct
        {
            r32 start_angle;            
        } constant;
    } angle;
    
    math::Vec3 direction;
    math::Vec3 base_position;
    
    StartParameterType start_life_time_type;
    union
    {
        struct
        {
            r32 l0;
            r32 l1;
        } random_between_two_constants;
        struct
        {
            r32 life_time;            
        } constant;
    } life;

    StartParameterType start_speed_type;
    union
    {
        struct
        {
            r32 s0;
            r32 s1;
        } random_between_two_constants;
        struct
        {
            r32 start_speed;            
        } constant;
    } speed;

    r32 spread;
    i32 particles_per_second;
    rendering::TextureHandle texture_handle;
    r32 gravity;
    
    EmissionModule emission_module;
};

struct Particles
{
    Vec3_4x *position;
    Vec3_4x *direction;

    Vec3_4x *relative_position;

    Vec2_4x* start_size;
    r32_4x* start_speed;
    r32_4x* start_life;
    r32_4x* start_angle;

    i32 **current_index;
    
    r32_4x *life;
};

struct ParticleSystemHandle
{
    i32 handle;
};

#define MAX_LIFETIME_VALUES 8

struct WorkQueue;
struct ThreadInfo;
struct ParticleWorkData;

struct UpdateParticleSystemWorkData
{
    ParticleSystemInfo *info;
    Renderer *renderer;
    r32 delta_time;
};

template<typename T>
struct OverLifetime
{
    T *values;
    r32 *keys;
    r32 *recip_keys;
    i32 count;
};

struct ParticleSystemInfo
{
    b32 simulating;
    b32 paused;
    
    b32 emitting;
    b32 prewarmed;

    WorkQueue *work_queue;
    ThreadInfo *thread_infos;
    i32 thread_info_count;
    ParticleWorkData *work_datas;
    i32 work_data_count;
    UpdateParticleSystemWorkData update_work_data;

    r32 time_spent;

    ParticleSystemAttributes attributes;
    rendering::MaterialInstanceHandle material_handle;

    r32 particles_cumulative;

    OverLifetime<r32> angle_over_lifetime;
    OverLifetime<math::Rgba> color_over_lifetime;
    OverLifetime<math::Vec2> size_over_lifetime;
    OverLifetime<r32> speed_over_lifetime;
    
    rendering::InstanceBufferHandle offset_buffer_handle;
    rendering::InstanceBufferHandle color_buffer_handle;
    rendering::InstanceBufferHandle size_buffer_handle;
    rendering::InstanceBufferHandle angle_buffer_handle;
    rendering::Transform transform;
    
    Particles particles;
    
    i32 *alive0_particles;
    i32 alive0_particle_count;
    
    i32 *alive1_particles;
    i32 alive1_particle_count;
    
    b32 alive0_active;
    
    i32 *dead_particles;
    i32 dead_particle_count;
    i32 last_used_particle_index;
    
    i32 particle_count;
    i32 total_emitted;
    i32 last_used_particle;
    i32 max_particles;
    
    r32 current_emission_time;
    
    MemoryArena arena;
    MemoryArena simd_arena;
};

#define PARTICLE_DATA_SIZE 2048
struct ParticleWorkData
{
    Renderer *renderer;
    ParticleSystemInfo *info;
    r32 delta_time;
    i32 particle_count;
    
    r32 angle_buffer[PARTICLE_DATA_SIZE * 4];
    math::Vec3 offset_buffer[PARTICLE_DATA_SIZE * 4];
    math::Vec2 size_buffer[PARTICLE_DATA_SIZE * 4];
    math::Rgba color_buffer[PARTICLE_DATA_SIZE * 4];

    i32 emitted_buffer[PARTICLE_DATA_SIZE];
    i32 emitted_this_frame;

    i32 next_frame_buffer[PARTICLE_DATA_SIZE];
    i32 next_frame_count;

    i32 dead_particle_indices[PARTICLE_DATA_SIZE];
    i32 dead_particle_count;
};

typedef ParticleSystemInfo* (*GetParticleSystemInfo)(ParticleSystemHandle handle, Renderer *renderer);
typedef void (*StartParticleSystem)(ParticleSystemHandle, Renderer *renderer);
typedef void (*StopParticleSystem)(ParticleSystemHandle handle, Renderer *renderer);
typedef void (*PauseParticleSystem)(ParticleSystemHandle handle, Renderer *renderer, b32 paused);
typedef void (*RemoveAllParticleSystems)(Renderer *renderer);
typedef b32 (*ParticleSystemIsRunning)(ParticleSystemHandle handle, Renderer *renderer);

typedef ParticleSystemAttributes (*GetDefaultAttributes)();
typedef ParticleSystemHandle (*CreateParticleSystem)(Renderer *renderer, i32 max_particles, rendering::MaterialHandle material, rendering::BufferHandle buffer);
typedef void (*RemoveParticleSystem)(Renderer *renderer, ParticleSystemHandle &handle);
typedef void (*UpdateParticleSystem)(Renderer *renderer, ParticleSystemHandle handle, i32 max_particles);

typedef void (*AddBurstOverTimeKey)(ParticleSystemInfo &particle_system, r32 key_time, i32 count, i32 cycle_count, i32 max_count, i32 min_count);
typedef i32 (*AddAngleKey)(OverLifetime<r32> &over_lifetime, r32 key_time, r32 value, MemoryArena *arena);
typedef void (*RemoveAngleKey)(OverLifetime<r32> &over_lifetime, r32 key_time);

typedef i32 (*AddColorKey)(OverLifetime<math::Rgba> &over_lifetime, r32 key_time, math::Rgba value, MemoryArena *arena);
typedef void (*RemoveColorKey)(OverLifetime<math::Rgba> &over_lifetime, r32 key_time);

typedef i32 (*AddSizeKey)(OverLifetime<math::Vec2> &over_lifetime, r32 key_time, math::Vec2 value, MemoryArena *arena);
typedef void (*RemoveSizeKey)(OverLifetime<math::Vec2> &over_lifetime, r32 key_time);

typedef i32 (*AddSpeedKey)(OverLifetime<r32> &over_lifetime, r32 key_time, r32 value, MemoryArena *arena);
typedef void (*RemoveSpeedKey)(OverLifetime<r32> &over_lifetime, r32 key_time);

struct ParticleApi
{
    GetParticleSystemInfo get_particle_system_info;
    StartParticleSystem start_particle_system;
    StopParticleSystem stop_particle_system;
    PauseParticleSystem pause_particle_system;
    RemoveAllParticleSystems remove_all_particle_systems;
    ParticleSystemIsRunning particle_system_is_running;

    GetDefaultAttributes get_default_attributes;
    CreateParticleSystem create_particle_system;
    RemoveParticleSystem remove_particle_system;
    UpdateParticleSystem update_particle_system;

    AddBurstOverTimeKey add_burst_over_time_key;
    AddAngleKey add_angle_key;
    RemoveAngleKey remove_angle_key;

    AddColorKey add_color_key;
    RemoveColorKey remove_color_key;

    AddSizeKey add_size_key;
    RemoveSizeKey remove_size_key;
    AddSpeedKey add_speed_key;
    RemoveSpeedKey remove_speed_key;
};

#endif

