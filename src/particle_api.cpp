
static ParticleSystemInfo * get_particle_system_info(ParticleSystemHandle handle, Renderer& renderer)
{
    if(handle.handle >= 0 && handle.handle < renderer.particle_system_count)
    {
        return &renderer.particle_systems[handle.handle];
    }
    return 0;
}

static void start_particle_system(ParticleSystemInfo &system, b32 one_shot = false)
{
    system.attributes.one_shot = one_shot;
    system.running = true;
    system.emitting = true;
    system.total_emitted = 0;
    system.particle_count = 0;
    
    for(i32 index = 0; index < system.max_particles; index++)
    {
        system.particles[index].life = 0.0f;
    }
}

static void start_particle_system(ParticleSystemHandle handle, Renderer &renderer, b32 one_shot = false)
{
    ParticleSystemInfo &system = renderer.particle_systems[handle.handle];
    start_particle_system(system, one_shot);
}

static void stop_particle_system(ParticleSystemHandle handle, Renderer &renderer)
{
    ParticleSystemInfo &system = renderer.particle_systems[handle.handle];
    system.running = false;
}

static ParticleSystemAttributes get_default_particle_system_attributes()
{
    ParticleSystemAttributes attributes = {};
    attributes.one_shot = false;
    attributes.particle_space = PS_WORLD;
    attributes.color = math::Rgba(1.0f);
    attributes.size = math::Vec2(1.0f);
    attributes.direction = math::Vec3(0.0f, 1.0f, 0.0f);
    attributes.life_time = 1.0;
    attributes.speed_multiplier = 1.0f;
    attributes.spread = 1.0f;
    attributes.particles_per_second = 100;
    attributes.texture_handle = 0;
    return attributes;
}

static void create_particle_system(Renderer &renderer, ParticleSystemHandle &handle, i32 max_particles,  MemoryArena *memory_arena)
{
    ParticleSystemInfo &system_info = renderer.particle_systems[renderer.particle_system_count++];
    system_info.running = false;
    
    system_info.attributes = get_default_particle_system_attributes();
    
    system_info.particle_count = 0;
    system_info.last_used_particle = 0;
    system_info.max_particles = max_particles;
    
    system_info.particles = push_array(memory_arena, system_info.max_particles, Particle);
    
    system_info.offsets = push_array(memory_arena, system_info.max_particles, math::Vec3);
    
    system_info.colors = push_array(memory_arena, system_info.max_particles, math::Vec4);
    
    system_info.sizes = push_array(memory_arena, system_info.max_particles, math::Vec2);
    
    system_info.color_over_lifetime.value_count = 0;
    system_info.size_over_lifetime.value_count = 0;
    system_info.speed_over_lifetime.value_count = 0;
    system_info.color_over_lifetime.values = nullptr;
    system_info.size_over_lifetime.values = nullptr;
    system_info.speed_over_lifetime.values = nullptr;
    system_info.color_over_lifetime.keys = nullptr;
    system_info.size_over_lifetime.keys = nullptr;
    system_info.speed_over_lifetime.keys = nullptr;
    
    BufferData offset_data = {};
    offset_data.for_instancing = true;
    offset_data.instance_buffer_size = sizeof(math::Vec3) * system_info.max_particles;
    renderer.buffers[renderer.buffer_count] = offset_data;
    system_info.offset_buffer_handle = renderer.buffer_count++;
    
    BufferData color_data = {};
    color_data.for_instancing = true;
    color_data.instance_buffer_size = sizeof(math::Vec4) * system_info.max_particles;
    renderer.buffers[renderer.buffer_count] = color_data;
    system_info.color_buffer_handle = renderer.buffer_count++;
    
    BufferData size_data = {};
    size_data.for_instancing = true;
    size_data.instance_buffer_size = sizeof(math::Vec2) * system_info.max_particles;
    renderer.buffers[renderer.buffer_count] = size_data;
    system_info.size_buffer_handle = renderer.buffer_count++;
    
    handle.handle = renderer.particle_system_count - 1;
}
/*
static void create_particle_system(Renderer &renderer, ParticleSystemHandle &handle, math::Vec2 size, math::Rgba color, r64 life_time, r32 speed_multiplier, r32 spread, i32 particles_per_second, i32 max_particles, i32 texture_handle, MemoryArena *memory_arena)
{
    //assert((i32)(particles_per_second * life_time) <= max_particles);
    ParticleSystemInfo &system_info = renderer.particle_systems[renderer.particle_system_count++];
    system_info.running = false;
    system_info.one_shot = false;
    system_info.particle_space = PS_WORLD;
    system_info.color = color;
    system_info.size = size;
    system_info.direction = math::Vec3(0.0f, 1.0f, 0.0f);
    system_info.life_time = life_time;
    system_info.speed_multiplier = speed_multiplier;
    system_info.spread = spread;
    system_info.particles_per_second = particles_per_second;
    system_info.max_particles = max_particles;
    system_info.particle_count = 0;
    system_info.last_used_particle = 0;
    
    system_info.particles = push_array(memory_arena, system_info.max_particles, Particle);
    
    system_info.offsets = push_array(memory_arena, system_info.max_particles, math::Vec3);
    
    system_info.colors = push_array(memory_arena, system_info.max_particles, math::Vec4);
    
    system_info.sizes = push_array(memory_arena, system_info.max_particles, math::Vec2);
    
    system_info.texture_handle = texture_handle;
    system_info.color_over_lifetime.value_count = 0;
    system_info.size_over_lifetime.value_count = 0;
    system_info.speed_over_lifetime.value_count = 0;
    system_info.color_over_lifetime.values = nullptr;
    system_info.size_over_lifetime.values = nullptr;
    system_info.speed_over_lifetime.values = nullptr;
    system_info.color_over_lifetime.keys = nullptr;
    system_info.size_over_lifetime.keys = nullptr;
    system_info.speed_over_lifetime.keys = nullptr;
    
    BufferData offset_data = {};
    offset_data.for_instancing = true;
    offset_data.instance_buffer_size = sizeof(math::Vec3) * system_info.max_particles;
    renderer.buffers[renderer.buffer_count] = offset_data;
    system_info.offset_buffer_handle = renderer.buffer_count++;
    
    BufferData color_data = {};
    color_data.for_instancing = true;
    color_data.instance_buffer_size = sizeof(math::Vec4) * system_info.max_particles;
    renderer.buffers[renderer.buffer_count] = color_data;
    system_info.color_buffer_handle = renderer.buffer_count++;
    
    BufferData size_data = {};
    size_data.for_instancing = true;
    size_data.instance_buffer_size = sizeof(math::Vec2) * system_info.max_particles;
    renderer.buffers[renderer.buffer_count] = size_data;
    system_info.size_buffer_handle = renderer.buffer_count++;
    
    handle.handle = renderer.particle_system_count - 1;
}
*/
static void add_color_key(MemoryArena *memory_arena, ParticleSystemInfo &particle_system, r64 key_time, math::Rgba value)
{
    if(particle_system.color_over_lifetime.value_count == 0)
    {
        particle_system.color_over_lifetime.values = push_array(memory_arena, MAX_LIFETIME_VALUES, math::Rgba);
        particle_system.color_over_lifetime.keys = push_array(memory_arena, MAX_LIFETIME_VALUES, r64);
    }
    
    assert(particle_system.color_over_lifetime.value_count + 1 < MAX_LIFETIME_VALUES);
    auto &values = particle_system.color_over_lifetime.values;
    auto &keys = particle_system.color_over_lifetime.keys;
    
    values[particle_system.color_over_lifetime.value_count] = value;
    keys[particle_system.color_over_lifetime.value_count] = key_time;
    particle_system.color_over_lifetime.value_count++;
}

static void add_size_key(MemoryArena *memory_arena, ParticleSystemInfo &particle_system, r64 key_time, math::Vec2 value)
{
    if(particle_system.size_over_lifetime.value_count == 0)
    {
        particle_system.size_over_lifetime.values = push_array(memory_arena, MAX_LIFETIME_VALUES, math::Vec2);
        particle_system.size_over_lifetime.keys = push_array(memory_arena, MAX_LIFETIME_VALUES, r64);
    }
    
    assert(particle_system.size_over_lifetime.value_count + 1 < MAX_LIFETIME_VALUES);
    auto &values = particle_system.size_over_lifetime.values;
    auto &keys = particle_system.size_over_lifetime.keys;
    
    values[particle_system.size_over_lifetime.value_count] = value;
    keys[particle_system.size_over_lifetime.value_count] = key_time;
    particle_system.size_over_lifetime.value_count++;
}

static void add_speed_key(MemoryArena *memory_arena, ParticleSystemInfo &particle_system, r64 key_time, r32 value)
{
    if(particle_system.speed_over_lifetime.value_count == 0)
    {
        particle_system.speed_over_lifetime.values = push_array(memory_arena, MAX_LIFETIME_VALUES, r32);
        particle_system.speed_over_lifetime.keys = push_array(memory_arena, MAX_LIFETIME_VALUES, r64);
    }
    
    assert(particle_system.speed_over_lifetime.value_count + 1 < MAX_LIFETIME_VALUES);
    auto &values = particle_system.speed_over_lifetime.values;
    auto &keys = particle_system.speed_over_lifetime.keys;
    
    values[particle_system.speed_over_lifetime.value_count] = value;
    keys[particle_system.speed_over_lifetime.value_count] = key_time;
    particle_system.speed_over_lifetime.value_count++;
}


