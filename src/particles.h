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
    r64 duration;

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
            r64 l0;
            r64 l1;
        } random_between_two_constants;
        struct
        {
            r64 life_time;            
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
    Rgba_4x *color;
    Vec2_4x *size;
    r32_4x *angle;
    
    Vec3_4x *relative_position;
    r32 *relative_size;

    Vec2_4x* start_size;
    r32_4x* start_speed;
    r64_4x* start_life;
    r32_4x* start_angle;
    
    r64_4x *life;
    
    rendering::TextureHandle *texture_handle;
};

struct ParticleSystemHandle
{
    i32 handle;
};

#define MAX_LIFETIME_VALUES 16

struct ParticleSystemInfo
{
    b32 simulating;
    b32 paused;
    
    b32 emitting;
    
    b32 prewarmed;

    r64 time_spent;

    ParticleSystemAttributes attributes;
    rendering::MaterialInstanceHandle material_handle;

    r32 particles_cumulative;

    struct
    {
        r32 *values;
        r64 *keys;
        i32 value_count;
    } angle_over_lifetime;
    
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
    
    rendering::InstanceBufferHandle offset_buffer_handle;
    rendering::InstanceBufferHandle color_buffer_handle;
    rendering::InstanceBufferHandle size_buffer_handle;
    rendering::InstanceBufferHandle angle_buffer_handle;
    rendering::Transform transform;
    
    Particles particles;
    i32* emitted_for_this_index;
    
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
    
    r64 current_emission_time;
    
    MemoryArena arena;
};

#endif

