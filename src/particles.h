#ifndef PARTICLES_H
#define PARTICLES_H

enum ParticleSpace
{
    PS_WORLD,
    PS_OBJECT
};

enum StartSizeType
{
    SST_CONSTANT,
    SST_BETWEEN_TWO_CONSTANTS
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
    S_Vec3 position;
    S_Vec3 direction;
};

#define EMITTER_FUNC(name) ParticleSpawnInfo name(RandomSeries& series)
typedef EMITTER_FUNC(EmitterFunc);

struct EmissionModule
{
    EmissionType type;
    
    struct
    {
        Burst *values;
        i32 value_count;
        i32 current_index;
    } burst_over_lifetime;
    
    r32 rate_over_distance;
    
    EmitterFunc* emitter_func;
};

struct ParticleSystemAttributes
{
    b32 one_shot;
    ParticleSpace particle_space;
    
    math::Rgba start_color;
    
    StartSizeType start_size_type;
    math::Vec2 start_size;
    
    math::Vec3 direction;
    r64 life_time;
    r32 start_speed;
    r32 spread;
    i32 particles_per_second;
    i32 texture_handle;
    
    EmissionModule emission_module;
};

struct Particles
{
    S_Vec3 *position;
    S_Vec3 *direction;
    S_Rgba *color;
    S_Vec2 *size;
    
    S_Vec3 *relative_position;
    r32 *relative_size;
    
    S_r64 *life;
    
    i32 *texture_handle;
};

struct ParticleSystemHandle
{
    i32 handle;
};

#define MAX_LIFETIME_VALUES 16

struct ParticleSystemInfo
{
    b32 running;
    b32 emitting;
    ParticleSystemAttributes attributes;
    
    r64 time_spent;
    
    struct
    {
        math::Rgba *values;
        r64 *keys;
        i32 value_count;
    } color_over_lifetime;
    
    struct 
    {
        math::Vec2 *values;
        r64 *keys;
        i32 value_count;
    } size_over_lifetime;
    
    struct
    {
        r32 *values;
        r64 *keys;
        i32 value_count;
    } speed_over_lifetime;
    
    i32 offset_buffer_handle;
    i32 color_buffer_handle;
    i32 size_buffer_handle;
    TransformInfo transform;
    Material material;
    
    Particles particles;
    i32 *unused_particles;
    i32 unused_particle_count;
    
    i32 *alive_particles;
    i32 alive_particle_count;
    
    i32 *next_alive_particles;
    i32 next_alive_particle_count;
    
    i32 *dead_particles;
    i32 dead_particle_count;
    
    b32 use_next;
    
    i32 particle_count;
    i32 total_emitted;
    i32 last_used_particle;
    i32 max_particles;
    
    math::Vec3 *offsets;
    math::Vec4 *colors;
    math::Vec2 *sizes;
    
    r64 current_emission_time;
    
    RandomSeries entropy;
    MemoryArena arena;
};

#endif

