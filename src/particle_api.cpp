static ParticleSystemInfo* get_particle_system_info(ParticleSystemHandle handle, Renderer& renderer)
{
    i32 _internal_handle = renderer.particles._internal_handles[handle.handle - 1];
    if(_internal_handle >= 0 && _internal_handle < renderer.particles.particle_system_count)
    {
        return &renderer.particles.particle_systems[_internal_handle];
    }
    return 0;
}

static void start_particle_system(ParticleSystemInfo &system)
{
    system.running = true;
    system.emitting = true;
    system.total_emitted = 0;
    system.particle_count = 0;
    
    for(i32 index = 0; index < system.max_particles / 4; index++)
    {
        system.particles.life[index] = 0.0;
    }
}

static void start_particle_system(ParticleSystemHandle handle, Renderer &renderer)
{
    i32 _internal_handle = renderer.particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer.particles.particle_system_count);
    ParticleSystemInfo &system = renderer.particles.particle_systems[_internal_handle];
    start_particle_system(system);
}

static void stop_particle_system(ParticleSystemHandle handle, Renderer &renderer)
{
    i32 _internal_handle = renderer.particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer.particles.particle_system_count);
    ParticleSystemInfo &system = renderer.particles.particle_systems[_internal_handle];
    system.running = false;
}

static void remove_all_particle_systems(Renderer &renderer)
{
    renderer.particles.particle_system_count = 0;
}

static b32 particle_system_is_running(ParticleSystemHandle handle, Renderer &renderer)
{
    i32 _internal_handle = renderer.particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer.particles.particle_system_count);
    ParticleSystemInfo &system = renderer.particles.particle_systems[_internal_handle];
    return(system.running);
}

static void set_rate_over_distanace(ParticleSystemInfo &particle_system, r32 rate_over_distance)
{
    particle_system.attributes.emission_module.rate_over_distance = rate_over_distance;
}

EMITTER_FUNC(emit_dir)
{
    ParticleSpawnInfo info;
    
    info.position = Vec3_4x(0.0f);
    info.direction = Vec3_4x(0.0f);
    
    return info;
}

EMITTER_FUNC(emit_random_dir)
{
    ParticleSpawnInfo info;
    
    info.position = Vec3_4x(0.0f);
    info.direction = random_direction_4x(series);
    
    return info;
}

EMITTER_FUNC(emit_from_2D_square)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_rect_4x(series, min, max);
    
    info.position = r;
    
    info.direction = Vec3_4x(0.0f, 1.0f, 0.0f);
    
    return info;
}

EMITTER_FUNC(emit_from_2D_square_random)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_rect_4x(series, min, max);
    
    info.position = r;
    
    info.direction = random_direction_4x(series);
    
    return info;
}

EMITTER_FUNC(emit_from_square)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_outer_rect_4x(series, min, max, min, max);
    
    info.position = r;
    
    info.direction = Vec3_4x(0.0f, 1.0f, 0.0f);
    
    return info;
}

EMITTER_FUNC(emit_from_square_random)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_outer_rect_4x(series, min, max, min, max);
    
    info.position = r;
    
    info.direction = random_direction_4x(series);
    
    return info;
}

EMITTER_FUNC(emit_from_disc)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_disc_4x(series, (max - min) / 2.0f);
    
    info.position = r;
    
    info.direction = Vec3_4x(0.0f, 1.0f, 0.0f);
    
    return info;
}

EMITTER_FUNC(emit_from_circle)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_circle_4x(series, (max - min) / 2.0f);
    
    info.position = r;
    
    info.direction = Vec3_4x(0.0f, 1.0f, 0.0f);
    
    return info;
}

EMITTER_FUNC(emit_from_disc_random)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_disc_4x(series, (max - min) / 2.0f);
    
    info.position = r;
    
    info.direction = random_direction_4x(series);
    
    return info;
}

EMITTER_FUNC(emit_from_circle_random)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_circle_4x(series, (max - min) / 2.0f);
    
    info.position = r;
    
    info.direction = random_direction_4x(series);
    
    return info;
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
    attributes.emission_module.emitter_func = emit_from_circle;
    
    return attributes;
}

static void _allocate_particle_system(Renderer& renderer, ParticleSystemInfo& system_info, i32 max_particles)
{
    MemoryArena* memory_arena = &system_info.arena;
    clear(memory_arena);
    
    //@Note: For SIMD
    system_info.max_particles = math::multiple_of_number(max_particles, 4);
    i32 max_over_four = system_info.max_particles / 4;
    
    system_info.alive0_particles = push_array(memory_arena, system_info.max_particles, i32);
    system_info.alive1_particles = push_array(memory_arena, system_info.max_particles, i32);
    system_info.alive0_active = true;
    system_info.dead_particle_count = max_over_four;
    system_info.dead_particles = push_array(memory_arena, system_info.dead_particle_count, i32);
    
    for(i32 dead_index = 0; dead_index < system_info.dead_particle_count; dead_index++)
    {
        system_info.dead_particles[dead_index] = dead_index;
    }
    
    system_info.alive0_particle_count = 0;
    system_info.alive1_particle_count = 0;
    
    system_info.particles.position = push_array_simd(memory_arena, max_over_four, Vec3_4x);
    system_info.particles.direction = push_array_simd(memory_arena, max_over_four, Vec3_4x);
    system_info.particles.color = push_array_simd(memory_arena, max_over_four, Rgba_4x);
    
    system_info.particles.size = push_array_simd(memory_arena, max_over_four, Vec2_4x);
    system_info.particles.relative_position = push_array_simd(memory_arena, max_over_four, Vec3_4x);
    
    system_info.particles.life = push_array_simd(memory_arena, max_over_four, r64_4x);
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
    
    if(system_info.offset_buffer_handle == 0)
    {
        register_instance_buffer(renderer, sizeof(math::Vec3) * system_info.max_particles, &system_info.offset_buffer_handle);
    }
    else
    {
        update_instanced_buffer(renderer, sizeof(math::Vec3) * system_info.max_particles, system_info.offset_buffer_handle);
    }
    
    if(system_info.color_buffer_handle == 0)
    {
        register_instance_buffer(renderer, sizeof(math::Vec4) * system_info.max_particles, &system_info.color_buffer_handle);
    }
    else
    {
        update_instanced_buffer(renderer, sizeof(math::Vec4) * system_info.max_particles, system_info.color_buffer_handle);
    }
    
    if(system_info.size_buffer_handle == 0)
    {
        register_instance_buffer(renderer, sizeof(math::Vec2) * system_info.max_particles, &system_info.size_buffer_handle);
    }
    else
    {
        update_instanced_buffer(renderer, sizeof(math::Vec2) * system_info.max_particles, system_info.size_buffer_handle);
    }
}

i32 _find_unused_particle_system(Renderer& renderer)
{
    for(i32 index = renderer.particles._current_internal_handle; index < renderer.particles._max_particle_system_count; index++)
    {
        if(renderer.particles._internal_handles[index] == -1)
        {
            renderer.particles._current_internal_handle = index;
            return index;
        }
    }
    
    for(i32 index = 0; index < renderer.particles._current_internal_handle; index++)
    {
        if(renderer.particles._internal_handles[index] == -1)
        {
            renderer.particles._current_internal_handle = index;
            return index;
        }
    }
    
    assert(false);
    
    return -1;
}

static ParticleSystemHandle create_particle_system(Renderer &renderer, i32 max_particles)
{
    i32 unused_handle = _find_unused_particle_system(renderer) + 1;
    
    ParticleSystemHandle handle = { unused_handle };
    renderer.particles._internal_handles[unused_handle - 1] = renderer.particles.particle_system_count++;
    
    ParticleSystemInfo &system_info = renderer.particles.particle_systems[renderer.particles._internal_handles[unused_handle - 1]];
    system_info.running = false;
    
    system_info.attributes = get_default_particle_system_attributes();
    system_info.transform = {};
    
    system_info.particle_count = 0;
    system_info.last_used_particle = 0;
    
    _allocate_particle_system(renderer, system_info, max_particles);
    
    return handle;
}

static void remove_particle_system(Renderer& renderer, ParticleSystemHandle &handle)
{
    if(handle.handle == 0)
        return;
    
    i32 removed_handle = handle.handle;
    
    renderer.particles._tagged_removed[renderer.particles._tagged_removed_count++] = removed_handle;
    
    if(renderer.particles.particle_system_count == 1)
    {
        unregister_buffer(renderer, renderer.particles.particle_systems[0].offset_buffer_handle);
        unregister_buffer(renderer, renderer.particles.particle_systems[0].size_buffer_handle);
        unregister_buffer(renderer, renderer.particles.particle_systems[0].color_buffer_handle);
        
        renderer.particles.particle_system_count = 0;
        renderer.particles._current_internal_handle = 0;
        renderer.particles._internal_handles[removed_handle - 1] = -1;
		clear(&renderer.particles.particle_systems[0].arena);
        renderer.particles.particle_systems[0] = {};
        renderer.particles.particle_systems[0].running = false;
    }
    else
    {
        i32 real_handle = renderer.particles._internal_handles[removed_handle - 1];
        ParticleSystemInfo& info = renderer.particles.particle_systems[real_handle];
        
        unregister_buffer(renderer, info.offset_buffer_handle);
        unregister_buffer(renderer, info.size_buffer_handle);
        unregister_buffer(renderer, info.color_buffer_handle);
        
        // Swap system infos
	    clear(&renderer.particles.particle_systems[real_handle].arena);
        
        renderer.particles.particle_systems[real_handle] = renderer.particles.particle_systems[renderer.particles.particle_system_count - 1];
        
        copy_arena(&renderer.particles.particle_systems[renderer.particles.particle_system_count - 1].arena, &renderer.particles.particle_systems[real_handle].arena);
        
        clear(&renderer.particles.particle_systems[renderer.particles.particle_system_count - 1].arena);

        renderer.particles.particle_systems[renderer.particles.particle_system_count - 1] = {};
        
        renderer.particles.particle_systems[renderer.particles.particle_system_count - 1].running = false;
        
        renderer.particles._internal_handles[removed_handle - 1] = -1;
        
        // Find the internal handle corresponding to this particle system
        for(i32 index = 0; index < renderer.particles._max_particle_system_count; index++)
        {
            if(renderer.particles._internal_handles[index] == renderer.particles.particle_system_count - 1)
            {
                renderer.particles._internal_handles[index] = real_handle;
                break;
            }
        }
        
        renderer.particles.particle_system_count--;
    }
}

static void update_particle_system(Renderer& renderer, ParticleSystemHandle handle, i32 max_particles)
{
    i32 _internal_handle = renderer.particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer.particles.particle_system_count);
    ParticleSystemInfo &system_info = renderer.particles.particle_systems[_internal_handle];
    _allocate_particle_system(renderer, system_info, max_particles);
}

static void add_burst_over_time_key(ParticleSystemInfo &particle_system, r64 key_time, i32 count, i32 cycle_count = 0, i32 max_count = 0, i32 min_count = 0)
{
    if(particle_system.attributes.emission_module.burst_over_lifetime.value_count == 0)
    {
        particle_system.attributes.emission_module.burst_over_lifetime.values = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, Burst);
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

static void add_color_key(ParticleSystemInfo &particle_system, r64 key_time, math::Rgba value)
{
    if(particle_system.color_over_lifetime.value_count == 0)
    {
        particle_system.color_over_lifetime.values = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, math::Rgba);
        particle_system.color_over_lifetime.keys = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, r64);
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

static void add_size_key(ParticleSystemInfo &particle_system, r64 key_time, math::Vec2 value)
{
    if(particle_system.size_over_lifetime.value_count == 0)
    {
        particle_system.size_over_lifetime.values = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, math::Vec2);
        particle_system.size_over_lifetime.keys = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, r64);
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

static void add_speed_key(ParticleSystemInfo &particle_system, r64 key_time, r32 value)
{
    if(particle_system.speed_over_lifetime.value_count == 0)
    {
        particle_system.speed_over_lifetime.values = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, r32);
        particle_system.speed_over_lifetime.keys = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, r64);
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

