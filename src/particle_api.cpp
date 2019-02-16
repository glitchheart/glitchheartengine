static ParticleSystemInfo* get_particle_system_info(ParticleSystemHandle handle, Renderer *renderer)
{
    i32 _internal_handle = renderer->particles._internal_handles[handle.handle - 1];
    if(_internal_handle >= 0 && _internal_handle < renderer->particles.particle_system_count)
    {
        return &renderer->particles.particle_systems[_internal_handle];
    }
    return 0;
}

static void start_particle_system(ParticleSystemInfo &system)
{
    system.simulating = true;
    system.emitting = true;
    system.total_emitted = 0;
    system.particle_count = 0;
    
    for(i32 index = 0; index < system.max_particles / 4; index++)
    {
        system.particles.life[index] = 0.0;
    }
}

static void start_particle_system(ParticleSystemHandle handle, Renderer *renderer)
{
    i32 _internal_handle = renderer->particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer->particles.particle_system_count);
    ParticleSystemInfo &system = renderer->particles.particle_systems[_internal_handle];
    start_particle_system(system);
}

static void stop_particle_system(ParticleSystemHandle handle, Renderer *renderer)
{
    i32 _internal_handle = renderer->particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer->particles.particle_system_count);
    ParticleSystemInfo &system = renderer->particles.particle_systems[_internal_handle];
    system.simulating = false;
}

static void pause_particle_system(ParticleSystemHandle handle, Renderer *renderer, b32 paused)
{
    i32 _internal_handle = renderer->particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer->particles.particle_system_count);
    ParticleSystemInfo &system = renderer->particles.particle_systems[_internal_handle];
    system.paused = paused;
}

static void remove_all_particle_systems(Renderer *renderer)
{
    renderer->particles.particle_system_count = 0;
}

static b32 particle_system_is_running(ParticleSystemHandle handle, Renderer *renderer)
{
    i32 _internal_handle = renderer->particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer->particles.particle_system_count);
    ParticleSystemInfo &system = renderer->particles.particle_systems[_internal_handle];
    return(system.simulating);
}

static void set_rate_over_distance(ParticleSystemInfo &particle_system, r32 rate_over_distance)
{
    particle_system.attributes.emission_module.rate_over_distance = rate_over_distance;
}


static ParticleSystemAttributes get_default_particle_system_attributes()
{
    ParticleSystemAttributes attributes = {};
    attributes.looping = true;
    attributes.duration = 5.0;
    attributes.particle_space = PS_WORLD;
    attributes.start_color = math::Rgba(1.0f);
    attributes.size.constant.start_size = 1.0f;
    attributes.start_size_type = StartParameterType::CONSTANT;
    attributes.direction = math::Vec3(0.0f, 1.0f, 0.0f);
    attributes.start_life_time_type = StartParameterType::CONSTANT;
    attributes.life.constant.life_time = 1.0;
    attributes.start_speed_type = StartParameterType::CONSTANT;
    attributes.speed.constant.start_speed = 1.0f;
    attributes.spread = 1.0f;
    attributes.prewarm = false;
    attributes.particles_per_second = 100;
    attributes.texture_handle.handle = 0;
    attributes.buffer.handle = 0;
    
    attributes.emission_module.rate_over_distance = 0.0f;
    attributes.emission_module.burst_over_lifetime.value_count = 0;
    attributes.emission_module.burst_over_lifetime.current_index = 0;
    attributes.emission_module.burst_over_lifetime.values = nullptr;
    attributes.emission_module.emitter_func_type = EmissionFuncType::CIRCLE;

    return attributes;
}

static void _allocate_particle_system(Renderer *renderer, ParticleSystemInfo& system_info, i32 max_particles)
{
    MemoryArena* memory_arena = &system_info.arena;
    MemoryArena* simd_arena = &system_info.simd_arena;
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

    system_info.active_particles = push_array(memory_arena, max_over_four, EmittedParticles);
    for(i32 i = 0; i < max_over_four; i++)
    {
        system_info.active_particles[i].indices[0] = -1;
        system_info.active_particles[i].indices[1] = -1;
        system_info.active_particles[i].indices[2] = -1;
        system_info.active_particles[i].indices[3] = -1;
    }
    
    system_info.alive0_particle_count = 0;
    system_info.alive1_particle_count = 0;
    
    system_info.particles.position = push_array_simd(simd_arena, max_over_four, Vec3_4x);
    system_info.particles.direction = push_array_simd(simd_arena, max_over_four, Vec3_4x);

    system_info.particles.start_size = push_array_simd(simd_arena, max_over_four, Vec2_4x);
    system_info.particles.start_life = push_array_simd(simd_arena, max_over_four, r64_4x);
    system_info.particles.start_speed = push_array_simd(simd_arena, max_over_four, r32_4x);
    system_info.particles.start_angle = push_array_simd(simd_arena, max_over_four, r32_4x);
    system_info.particles.relative_position = push_array_simd(simd_arena, max_over_four, Vec3_4x);

    system_info.particles.life = push_array_simd(simd_arena, max_over_four, r64_4x);
    push_size(memory_arena, sizeof(rendering::TextureHandle), rendering::TextureHandle);
    
    system_info.color_over_lifetime.value_count = 0;
    system_info.size_over_lifetime.value_count = 0;
    system_info.speed_over_lifetime.value_count = 0;
    system_info.angle_over_lifetime.value_count = 0;
    system_info.color_over_lifetime.values = nullptr;
    system_info.size_over_lifetime.values = nullptr;
    system_info.speed_over_lifetime.values = nullptr;
    system_info.angle_over_lifetime.values = nullptr;
    system_info.color_over_lifetime.keys = nullptr;
    system_info.size_over_lifetime.keys = nullptr;
    system_info.speed_over_lifetime.keys = nullptr;
    system_info.angle_over_lifetime.keys = nullptr;

    system_info.color_buffer_handle = rendering::allocate_instance_buffer(rendering::ValueType::FLOAT4, max_particles, renderer);
    system_info.size_buffer_handle = rendering::allocate_instance_buffer(rendering::ValueType::FLOAT2, max_particles, renderer);
    system_info.offset_buffer_handle = rendering::allocate_instance_buffer(rendering::ValueType::FLOAT3, max_particles, renderer);
    system_info.angle_buffer_handle = rendering::allocate_instance_buffer(rendering::ValueType::FLOAT, max_particles, renderer);

    rendering::Material& material = rendering::get_material_instance(system_info.material_handle, renderer);
    
    for(i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
    {
        rendering::VertexAttributeInstanced& attrib = material.instanced_vertex_attributes[i];
        switch(attrib.mapping_type)
        {
        case rendering::VertexAttributeMappingType::PARTICLE_ANGLE:
        {
            attrib.instance_buffer_handle = system_info.angle_buffer_handle;
        }
        break;
        case rendering::VertexAttributeMappingType::PARTICLE_POSITION:
        {
            attrib.instance_buffer_handle = system_info.offset_buffer_handle;
        }
        break;
        case rendering::VertexAttributeMappingType::PARTICLE_COLOR:
        {
            attrib.instance_buffer_handle = system_info.color_buffer_handle;
        }
        break;
        case rendering::VertexAttributeMappingType::PARTICLE_SIZE:
        {
            attrib.instance_buffer_handle = system_info.size_buffer_handle;
        }
        break;
        default:
        break;
        }
    }
}

i32 _find_unused_particle_system(Renderer *renderer)
{
    for(i32 index = renderer->particles._current_internal_handle; index < renderer->particles._max_particle_system_count; index++)
    {
        if(renderer->particles._internal_handles[index] == -1)
        {
            renderer->particles._current_internal_handle = index;
            return index;
        }
    }
    
    for(i32 index = 0; index < renderer->particles._current_internal_handle; index++)
    {
        if(renderer->particles._internal_handles[index] == -1)
        {
            renderer->particles._current_internal_handle = index;
            return index;
        }
    }
    
    assert(false);
    
    return -1;
}

static ParticleSystemHandle create_particle_system(Renderer *renderer, i32 max_particles, rendering::MaterialHandle material, rendering::BufferHandle buffer = {0})
{
    assert(material.handle != 0);
    
    i32 unused_handle = _find_unused_particle_system(renderer) + 1;
    
    ParticleSystemHandle handle = { unused_handle };
    renderer->particles._internal_handles[unused_handle - 1] = renderer->particles.particle_system_count++;
    
    ParticleSystemInfo &system_info = renderer->particles.particle_systems[renderer->particles._internal_handles[unused_handle - 1]];
    system_info.simulating = false;
    system_info.paused = true;
    
    system_info.attributes = get_default_particle_system_attributes();
    system_info.attributes.buffer = buffer;
    system_info.time_spent = 0.0;
    system_info.transform = {};
    
    system_info.particle_count = 0;
    system_info.last_used_particle = 0;

    system_info.material_handle = rendering::create_material_instance(renderer, material);
    
    _allocate_particle_system(renderer, system_info, max_particles);
    system_info.work_queue = {};

    u32 thread_count = (u32)max_particles / 4;
    system_info.thread_info_count = thread_count;
    system_info.thread_infos = push_array(&system_info.arena, system_info.thread_info_count, ThreadInfo);
    system_info.work_queue = &renderer->particles.work_queues[renderer->particles.active_work_queue_count++];
    platform.make_queue(system_info.work_queue, system_info.thread_info_count, system_info.thread_infos);
    
    return handle;
}

static void remove_particle_system(Renderer *renderer, ParticleSystemHandle &handle)
{
    if(handle.handle == 0)
        return;
    
    i32 removed_handle = handle.handle;
    
    
    if(renderer->particles.particle_system_count == 1)
    {
        rendering::free_instance_buffer(renderer->particles.particle_systems[0].offset_buffer_handle, renderer);
        rendering::free_instance_buffer(renderer->particles.particle_systems[0].size_buffer_handle, renderer);
        rendering::free_instance_buffer(renderer->particles.particle_systems[0].color_buffer_handle, renderer);
        rendering::free_instance_buffer(renderer->particles.particle_systems[0].angle_buffer_handle, renderer);
        rendering::delete_material_instance(renderer, renderer->particles.particle_systems[0].material_handle);
        renderer->particles.particle_system_count = 0;
        renderer->particles._current_internal_handle = 0;
        renderer->particles._internal_handles[removed_handle - 1] = -1;
		clear(&renderer->particles.particle_systems[0].arena);
        renderer->particles.particle_systems[0] = {};
        renderer->particles.particle_systems[0].simulating = false;
    }
    else
    {
        i32 real_handle = renderer->particles._internal_handles[removed_handle - 1];
        ParticleSystemInfo& info = renderer->particles.particle_systems[real_handle];
        rendering::delete_material_instance(renderer, info.material_handle);
        rendering::free_instance_buffer(info.offset_buffer_handle, renderer);
        rendering::free_instance_buffer(info.size_buffer_handle, renderer);
        rendering::free_instance_buffer(info.color_buffer_handle, renderer);
        rendering::free_instance_buffer(info.angle_buffer_handle, renderer);
        
        // Swap system infos
	    clear(&renderer->particles.particle_systems[real_handle].arena);
        
        renderer->particles.particle_systems[real_handle] = renderer->particles.particle_systems[renderer->particles.particle_system_count - 1];

        move_arena(&renderer->particles.particle_systems[renderer->particles.particle_system_count - 1].arena, &renderer->particles.particle_systems[real_handle].arena);

        renderer->particles.particle_systems[renderer->particles.particle_system_count - 1] = {};
        
        renderer->particles.particle_systems[renderer->particles.particle_system_count - 1].simulating = false;
        
        renderer->particles._internal_handles[removed_handle - 1] = -1;
        
        // Find the internal handle corresponding to this particle system
        for(i32 index = 0; index < renderer->particles._max_particle_system_count; index++)
        {
            if(renderer->particles._internal_handles[index] == renderer->particles.particle_system_count - 1)
            {
                renderer->particles._internal_handles[index] = real_handle;
                break;
            }
        }
        
        renderer->particles.particle_system_count--;
    }
}

static void update_particle_system(Renderer *renderer, ParticleSystemHandle handle, i32 max_particles)
{
    i32 _internal_handle = renderer->particles._internal_handles[handle.handle - 1];
    assert(_internal_handle >= 0 && _internal_handle < renderer->particles.particle_system_count);
    ParticleSystemInfo &system_info = renderer->particles.particle_systems[_internal_handle];
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

static i32 add_angle_key(ParticleSystemInfo &particle_system, r64 key_time, r32 value)
{
    if(particle_system.angle_over_lifetime.value_count == 0)
    {
        particle_system.angle_over_lifetime.values = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, r32);
        particle_system.angle_over_lifetime.keys = push_array(&particle_system.arena, MAX_LIFETIME_VALUES, r64);
    }
    
    assert(particle_system.angle_over_lifetime.value_count + 1 < MAX_LIFETIME_VALUES);
    auto &values = particle_system.angle_over_lifetime.values;
    auto &keys = particle_system.angle_over_lifetime.keys;
    
    b32 replaced = false;
    i32 new_index = -1;
    
    for(i32 i = 0; i < particle_system.angle_over_lifetime.value_count; i++)
    {
        if(keys[i] > key_time)
        {
            memmove(&keys[i + 1], &keys[i], sizeof(r64) * (particle_system.angle_over_lifetime.value_count - i));
            memmove(&values[i + 1], &values[i], sizeof(r32) * (particle_system.angle_over_lifetime.value_count - i));
            keys[i] = key_time;
            values[i] = value;
            new_index = i;
            replaced = true;
            break;
        }
    }
    
    if(!replaced)
    {
        values[particle_system.angle_over_lifetime.value_count] = value;
        keys[particle_system.angle_over_lifetime.value_count] = key_time;
    }
    
    particle_system.angle_over_lifetime.value_count++;

    return new_index;
}


static void remove_angle_key(ParticleSystemInfo &particle_system, r64 key_time)
{
    auto& values = particle_system.angle_over_lifetime.values;
    auto& keys = particle_system.angle_over_lifetime.keys;
    
    for(i32 i = 0; i < particle_system.angle_over_lifetime.value_count; i++)
    {
        if(keys[i] == key_time)
        {
            memmove(&keys[i], &keys[i + 1], sizeof(r32) * (particle_system.angle_over_lifetime.value_count - i));
            memmove(&values[i], &values[i + 1], sizeof(r64) * (particle_system.angle_over_lifetime.value_count - i));
            particle_system.angle_over_lifetime.value_count--;
            break;
        }
    }
}

static i32 add_color_key(ParticleSystemInfo &particle_system, r64 key_time, math::Rgba value)
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
    i32 new_index = -1;
    
    for(i32 i = 0; i < particle_system.color_over_lifetime.value_count; i++)
    {
        if(keys[i] > key_time)
        {
            memmove(&keys[i + 1], &keys[i], sizeof(r64) * (particle_system.color_over_lifetime.value_count - i));
            memmove(&values[i + 1], &values[i], sizeof(math::Rgba) * (particle_system.color_over_lifetime.value_count - i));
            keys[i] = key_time;
            values[i] = value;
            new_index = i;
            replaced = true;
            break;
        }
    }
    
    if(!replaced)
    {
        values[particle_system.color_over_lifetime.value_count] = value;
        keys[particle_system.color_over_lifetime.value_count] = key_time;
        // new_index = particle_system.color_over_lifetime.value_count;
    }
    
    particle_system.color_over_lifetime.value_count++;

    return new_index;
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

static i32 add_size_key(ParticleSystemInfo &particle_system, r64 key_time, math::Vec2 value)
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
    i32 new_index = -1;
    
    for(i32 i = 0; i < particle_system.size_over_lifetime.value_count; i++)
    {
        if(keys[i] > key_time)
        {
            memmove(&keys[i + 1], &keys[i], sizeof(r64) * (particle_system.size_over_lifetime.value_count - i));
            memmove(&values[i + 1], &values[i], sizeof(math::Vec2) * (particle_system.size_over_lifetime.value_count - i));
            keys[i] = key_time;
            values[i] = value;
            replaced = true;
            new_index = i;
            break;
        }
    }
    
    if(!replaced)
    {
        values[particle_system.size_over_lifetime.value_count] = value;
        keys[particle_system.size_over_lifetime.value_count] = key_time;
        new_index = particle_system.size_over_lifetime.value_count;
    }
    
    particle_system.size_over_lifetime.value_count++;

    return new_index;
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

static i32 add_speed_key(ParticleSystemInfo &particle_system, r64 key_time, r32 value)
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
    i32 new_index = -1;
    
    for(i32 i = 0; i < particle_system.speed_over_lifetime.value_count; i++)
    {
        if(keys[i] > key_time)
        {
            memmove(&values[i + 1], &values[i], sizeof(r32) * (particle_system.speed_over_lifetime.value_count - i));
            memmove(&keys[i + 1], &keys[i], sizeof(r64) * (particle_system.speed_over_lifetime.value_count - i));
            keys[i] = key_time;
            values[i] = value;
            new_index = i;
            replaced = true;
            break;
        }
    }
    
    if(!replaced)
    {
        values[particle_system.speed_over_lifetime.value_count] = value;
        keys[particle_system.speed_over_lifetime.value_count] = key_time;
        new_index = particle_system.speed_over_lifetime.value_count;;
    }
    
    
    particle_system.speed_over_lifetime.value_count++;

    return new_index;
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

