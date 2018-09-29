
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
    
    for(i32 index = 0; index < system.max_particles / 4; index++)
    {
        system.particles.life[index] = 0.0;
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

static void remove_all_particle_systems(Renderer &renderer)
{
    renderer.particle_system_count = 0;
}

static b32 particle_system_is_running(ParticleSystemHandle handle, Renderer &renderer)
{
    ParticleSystemInfo &system = renderer.particle_systems[handle.handle];
    return(system.running);
}

static void set_rate_over_distanace(ParticleSystemInfo &particle_system, r32 rate_over_distance)
{
    particle_system.attributes.emission_module.rate_over_distance = rate_over_distance;
}

static ParticleSystemAttributes get_default_particle_system_attributes()
{
    ParticleSystemAttributes attributes = {};
    attributes.one_shot = false;
    attributes.particle_space = PS_WORLD;
    attributes.start_color = math::Rgba(1.0f);
    attributes.start_size = math::Vec2(1.0f);
    attributes.start_size_type = SST_CONSTANT;
    attributes.direction = math::Vec3(0.0f, 1.0f, 0.0f);
    attributes.life_time = 1.0;
    attributes.start_speed = 1.0f;
    attributes.spread = 1.0f;
    attributes.particles_per_second = 100;
    attributes.texture_handle = 0;
    
    attributes.emission_module.rate_over_distance = 0.0f;
    attributes.emission_module.burst_over_lifetime.value_count = 0;
    attributes.emission_module.burst_over_lifetime.current_index = 0;
    attributes.emission_module.burst_over_lifetime.values = nullptr;
    
    return attributes;
}

static void create_particle_system(Renderer &renderer, ParticleSystemHandle &handle, i32 max_particles,  MemoryArena *memory_arena)
{
    ParticleSystemInfo &system_info = renderer.particle_systems[renderer.particle_system_count++];
    system_info.running = false;
    
    system_info.attributes = get_default_particle_system_attributes();
    
    system_info.particle_count = 0;
    system_info.last_used_particle = 0;
    
    //@Note: For SIMD
    system_info.max_particles = math::multiple_of_number(max_particles, 4);
    auto max_over_four = system_info.max_particles / 4;
    
    system_info.unused_particles = push_array(memory_arena, system_info.max_particles, i32);
    system_info.alive_particles = push_array(memory_arena, system_info.max_particles, i32);
    system_info.next_alive_particles = push_array(memory_arena, system_info.max_particles, i32);
    system_info.dead_particles = push_array(memory_arena, system_info.max_particles, i32);
    system_info.use_next = false;
    
    system_info.alive_particle_count = 0;
    system_info.next_alive_particle_count = 0;
    system_info.dead_particle_count = 0;
    
    system_info.particles.position = push_array_simd(memory_arena, max_over_four, S_Vec3);
    system_info.particles.direction = push_array_simd(memory_arena, max_over_four, S_Vec3);
    system_info.particles.color = push_array_simd(memory_arena, max_over_four, S_Rgba);
    
    system_info.particles.size = push_array_simd(memory_arena, max_over_four, S_Vec2);
    system_info.particles.relative_position = push_array_simd(memory_arena, max_over_four, S_Vec3);
    
    system_info.particles.life = push_array_simd(memory_arena, max_over_four, S_r64);
    system_info.particles.texture_handle = push_array(memory_arena, system_info.max_particles, i32);
    
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

static void add_burst_over_time_key(MemoryArena *memory_arena, ParticleSystemInfo &particle_system, r64 key_time, i32 count, i32 cycle_count = 0, i32 max_count = 0, i32 min_count = 0)
{
    if(particle_system.attributes.emission_module.burst_over_lifetime.value_count == 0)
    {
        particle_system.attributes.emission_module.burst_over_lifetime.values = push_array(memory_arena, MAX_LIFETIME_VALUES, Burst);
    }
    
    assert(particle_system.attributes.emission_module.burst_over_lifetime.value_count + 1 < MAX_LIFETIME_VALUES);
    auto &values = particle_system.attributes.emission_module.burst_over_lifetime.values;
    
    Burst new_burst = {};
    new_burst.count = count;
    new_burst.repeat_interval = key_time;
    new_burst.cycle_count = cycle_count;
    new_burst.max_count = 0;
    new_burst.min_count = 0;
    
    values[particle_system.attributes.emission_module.burst_over_lifetime.value_count] = new_burst;
    particle_system.attributes.emission_module.burst_over_lifetime.value_count++;
}

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
    
    b32 replaced = false;
    
    for(i32 i = 0; i < particle_system.color_over_lifetime.value_count; i++)
    {
        if(keys[i] > key_time)
        {
            memmove(&keys[i + 1], &keys[i], sizeof(r64) * (particle_system.color_over_lifetime.value_count - i));
            memmove(&values[i + 1], &values[i], sizeof(math::Rgba) * (particle_system.color_over_lifetime.value_count - i));
            keys[i] = key_time;
            values[i] = value;
            replaced = true;
            break;
        }
    }
    
    if(!replaced)
    {
        values[particle_system.color_over_lifetime.value_count] = value;
        keys[particle_system.color_over_lifetime.value_count] = key_time;
    }
    
    particle_system.color_over_lifetime.value_count++;
}

static void remove_color_key(ParticleSystemInfo &particle_system, r64 key_time)
{
    auto& values = particle_system.color_over_lifetime.values;
    auto& keys = particle_system.color_over_lifetime.keys;
    
    for(i32 i = 0; i < particle_system.color_over_lifetime.value_count; i++)
    {
        if(keys[i] == key_time)
        {
            memmove(&keys[i], &keys[i + 1], sizeof(math::Rgba) * (particle_system.color_over_lifetime.value_count - i));
            memmove(&values[i], &values[i + 1], sizeof(r64) * (particle_system.color_over_lifetime.value_count - i));
            particle_system.color_over_lifetime.value_count--;
            break;
        }
    }
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
    
    b32 replaced = false;
    
    for(i32 i = 0; i < particle_system.size_over_lifetime.value_count; i++)
    {
        if(keys[i] > key_time)
        {
            memmove(&keys[i + 1], &keys[i], sizeof(r64) * (particle_system.size_over_lifetime.value_count - i));
            memmove(&values[i + 1], &values[i], sizeof(math::Vec2) * (particle_system.size_over_lifetime.value_count - i));
            keys[i] = key_time;
            values[i] = value;
            replaced = true;
            break;
        }
    }
    
    if(!replaced)
    {
        values[particle_system.size_over_lifetime.value_count] = value;
        keys[particle_system.size_over_lifetime.value_count] = key_time;
    }
    
    particle_system.size_over_lifetime.value_count++;
}


static void remove_size_key(ParticleSystemInfo &particle_system, r64 key_time)
{
    auto& values = particle_system.size_over_lifetime.values;
    auto& keys = particle_system.size_over_lifetime.keys;
    
    for(i32 i = 0; i < particle_system.size_over_lifetime.value_count; i++)
    {
        if(keys[i] == key_time)
        {
            memmove(&keys[i], &keys[i + 1], sizeof(math::Vec2) * (particle_system.size_over_lifetime.value_count - i));
            memmove(&values[i], &values[i + 1], sizeof(r64) * (particle_system.size_over_lifetime.value_count - i));
            particle_system.size_over_lifetime.value_count--;
            break;
        }
    }
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
    
    b32 replaced = false;
    
    for(i32 i = 0; i < particle_system.speed_over_lifetime.value_count; i++)
    {
        if(keys[i] > key_time)
        {
            memmove(&values[i + 1], &values[i], sizeof(r32) * (particle_system.speed_over_lifetime.value_count - i));
            memmove(&keys[i + 1], &keys[i], sizeof(r64) * (particle_system.speed_over_lifetime.value_count - i));
            keys[i] = key_time;
            values[i] = value;
            replaced = true;
            break;
        }
    }
    
    if(!replaced)
    {
        values[particle_system.speed_over_lifetime.value_count] = value;
        keys[particle_system.speed_over_lifetime.value_count] = key_time;
    }
    
    
    particle_system.speed_over_lifetime.value_count++;
}


static void remove_speed_key(ParticleSystemInfo &particle_system, r64 key_time)
{
    auto& values = particle_system.speed_over_lifetime.values;
    auto& keys = particle_system.speed_over_lifetime.keys;
    
    for(i32 i = 0; i < particle_system.speed_over_lifetime.value_count; i++)
    {
        if(keys[i] == key_time)
        {
            memmove(&keys[i], &keys[i + 1], sizeof(r32) * (particle_system.speed_over_lifetime.value_count - i));
            memmove(&values[i], &values[i + 1], sizeof(r64) * (particle_system.speed_over_lifetime.value_count - i));
            particle_system.speed_over_lifetime.value_count--;
            break;
        }
    }
}

