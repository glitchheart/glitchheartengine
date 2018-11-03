i32 find_unused_particle(ParticleSystemInfo &particle_system)
{   
    if(particle_system.dead_particle_count > 0)
    {
        particle_system.dead_particle_count--;
        return particle_system.dead_particles[particle_system.dead_particle_count];
    }
    
	assert(false);
    return -1;
}

r64_4x get_t(r64_4x time_spent, r64_4x start_time, r64_4x end_time)
{
    r64_4x diff = end_time - start_time;
    r64_4x in_this_index = time_spent - start_time;
    
    r64_4x index_over_diff = in_this_index / diff;
    
    r64_4x t = simd_min(1.0, index_over_diff);                      
    
    return t;
}

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
Rgba_4x get_color_by_time(ParticleSystemInfo &particle_system, r64_4x time_spent)
{
    i32 value_count = particle_system.color_over_lifetime.value_count;
    math::Rgba* values = particle_system.color_over_lifetime.values;
    r64* keys = particle_system.color_over_lifetime.keys;
    
    if(equal_epsilon(time_spent, particle_system.attributes.life_time, 0.001))
    {
        return Rgba_4x(values[value_count - 1]);
    }
    
    for(i32 key = 0; key < value_count; key++)
    {
        if(key + 1 == value_count)
        {
            return Rgba_4x(values[key]);
        }
        
        r64 current_key = keys[key] * particle_system.attributes.life_time;
        r64 next_key = keys[key + 1] * particle_system.attributes.life_time;
        if(any_lt_eq(r64_4x(current_key), time_spent) && any_lt_eq(time_spent, next_key))
        {
            Rgba_4x start_color = Rgba_4x(values[key]);
            Rgba_4x end_color = Rgba_4x(values[key + 1]);
            
            r64_4x t_color = get_t(time_spent, current_key, next_key);
            
            return math::lerp(start_color, t_color, end_color);
        }
    }
    return Rgba_4x(values[value_count - 1]);
}

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
Vec2_4x get_size_by_time(ParticleSystemInfo &particle_system, r64_4x time_spent)
{
    i32 value_count = particle_system.size_over_lifetime.value_count;
    math::Vec2* values = particle_system.size_over_lifetime.values;
    r64* keys = particle_system.size_over_lifetime.keys;
    
    if(equal_epsilon(time_spent, particle_system.attributes.life_time, 0.001))
    {
        return Vec2_4x(values[value_count - 1]);
    }
    
    for(i32 key = 0; key < value_count; key++)
    {
        if(key + 1 == value_count)
        {
            return Vec2_4x(values[key]);
        }
        
        r64 current_key = keys[key] * particle_system.attributes.life_time;r64  next_key = keys[key + 1] * particle_system.attributes.life_time;
        if(any_lt_eq(r64_4x(current_key), time_spent) && any_lt_eq(time_spent, next_key))
        {
            Vec2_4x start_size = Vec2_4x(values[key]);
            Vec2_4x end_size = Vec2_4x(values[key + 1]);
            
            r64_4x t_size = get_t(time_spent, current_key, next_key);
            
            return math::lerp(start_size, t_size, end_size);
        }
    }
    return Vec2_4x(values[value_count - 1]);
}

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
r32_4x get_speed_by_time(ParticleSystemInfo &particle_system, r64_4x time_spent)
{
    i32 value_count = particle_system.speed_over_lifetime.value_count;
    r32* values = particle_system.speed_over_lifetime.values;
    r64* keys = particle_system.speed_over_lifetime.keys;
    
    if(equal_epsilon(time_spent, particle_system.attributes.life_time, 0.001))
    {
        return r32_4x(values[value_count - 1]);
    }
    
    for(i32 key = 0; key < value_count; key++)
    {
        if(key + 1 == value_count)
        {
            return r32_4x(values[key]);
        }
        
        r64 current_key = keys[key] * particle_system.attributes.life_time;
        r64 next_key = keys[key + 1] * particle_system.attributes.life_time;
        if(any_lt_eq(r64_4x(current_key), time_spent) && any_lt_eq(time_spent, next_key))
        {
            r32_4x start_speed = r32_4x(values[key]);
            r32_4x end_speed = r32_4x(values[key + 1]);
            
            r64_4x t_speed = get_t(time_spent, current_key, next_key);
            
            r32_4x res = math::lerp(start_speed, t_speed, end_speed);
            
            return res;
        }
    }
    
    return r32_4x(values[value_count - 1]);
}

// @Note:(Niels): Update the particles that have been emitted in previous and in the current frame
// Kills particles in case they have life <= 0.0 and adds them to the dead particle buffer
// Dead particles are used when emitting in the emit procedure.
// Any particle that is NOT dead, will be written into the alive buffer of the next frame
// This ensures that we can clear the current frame buffer, and only ever iterate over
// particles that are actually alive.
void update_particles(Renderer &renderer, ParticleSystemInfo &particle_system, r64 delta_time, i32 *emitted_buf, i32 *emitted_this_frame, i32* next_frame_buf, i32 *next_frame_count)
{
    particle_system.particle_count = 0;
    
    i32 speed_value_count = particle_system.speed_over_lifetime.value_count;
    i32 color_value_count = particle_system.color_over_lifetime.value_count;
    i32 size_value_count = particle_system.size_over_lifetime.value_count;               
    
    for(i32 alive_index = 0; alive_index < *emitted_this_frame; alive_index++)
    {
        i32 main_index = emitted_buf[alive_index];
        
        // @Incomplete(Niels): Used to check where to position initial emission
        // Maybe find a better solution that doesn't require branching on every particle?
        auto start = equal_epsilon(particle_system.particles.life[main_index], particle_system.attributes.life_time, 0.001);
        
        particle_system.particles.life[main_index] -= delta_time;
        
        // @Note:(Niels): Check for alive state of this particle and kill if dead.
        // Seems like a necessary branch here, since we do need to know if a particle is dead.
        // Maybe there is some SIMD magic, that can do this for us (probably not...).
        if(any_lt_eq(particle_system.particles.life[main_index], 0.0))
        {
            particle_system.dead_particles[particle_system.dead_particle_count++] = main_index;
            continue;
        }
        else
        {
            next_frame_buf[(*next_frame_count)++] = main_index;
        }
        
        auto life = particle_system.particles.life[main_index];
        r64_4x time_spent = particle_system.attributes.life_time - life;
        
        particle_system.particles.direction[main_index] += Vec3_4x(math::Vec3(0.0f, -particle_system.attributes.gravity * (r32)delta_time, 0.0f));
        
        // @Note(Niels): This branch will always be true or false for the whole loop so it should be optimized out (hopefully) 
        // We could optimize it specifically by having separate arrays for each of these
        if(size_value_count > 0)
        {
            particle_system.particles.size[main_index] = get_size_by_time(particle_system, time_spent);
        }
        else
        {
            particle_system.particles.size[main_index] = particle_system.attributes.start_size;
        }
        
        if(color_value_count > 0)
        {
            particle_system.particles.color[main_index] = get_color_by_time(particle_system, time_spent);
        }
        else
        {
            particle_system.particles.color[main_index] = particle_system.attributes.start_color;
        }
        
        if(speed_value_count > 0)
        {
            particle_system.particles.position[main_index] += particle_system.particles.direction[main_index] * get_speed_by_time(particle_system, time_spent) * (r32)delta_time;
        }
        else
        {
            particle_system.particles.position[main_index] += particle_system.particles.direction[main_index] * particle_system.attributes.start_speed * (r32)delta_time;				
        }                
        
        Vec3_4x final_pos(0.0f);
        
        if(particle_system.attributes.particle_space == PS_WORLD && !start)
        {
            final_pos = particle_system.particles.position[main_index] + particle_system.particles.relative_position[main_index];
        }
        else
        {
            final_pos = particle_system.particles.position[main_index] + particle_system.transform.position;
            particle_system.particles.relative_position[main_index] = Vec3_4x(particle_system.transform.position);
        }
        
        // @Note(Niels): Now fill the simd vectors into normal vectors that can be drawn
        // Could be moved to another function so we don't mix SIMD too much with non-SIMD
        Rgba_4x color = particle_system.particles.color[main_index];
        Vec2_4x size = particle_system.particles.size[main_index];
        
        float p1[4], p2[4], p3[4], p4[4];
        float s1[4], s2[4], s3[4], s4[4];
        float c1[4], c2[4], c3[4], c4[4];
        
        vec2_4x_to_float4(size, s1, s2, s3, s4);
        vec3_4x_to_float4(final_pos, p1, p2, p3, p4);
        vec4_4x_to_float4(color, c1, c2, c3, c4);
        
        particle_system.offsets[particle_system.particle_count].x = p1[0];
        particle_system.offsets[particle_system.particle_count].y = p1[1];
        particle_system.offsets[particle_system.particle_count].z = p1[2];
        
        particle_system.colors[particle_system.particle_count].r = c1[0];
        particle_system.colors[particle_system.particle_count].g = c1[1];
        particle_system.colors[particle_system.particle_count].b = c1[2];
        particle_system.colors[particle_system.particle_count].a = c1[3];
        
        particle_system.sizes[particle_system.particle_count].x = s1[0];
        particle_system.sizes[particle_system.particle_count].y = s1[1];
        
        particle_system.particle_count++;
        
        particle_system.offsets[particle_system.particle_count].x = p2[0];
        particle_system.offsets[particle_system.particle_count].y = p2[1];
        particle_system.offsets[particle_system.particle_count].z = p2[2];
        
        particle_system.colors[particle_system.particle_count].r = c2[0];
        particle_system.colors[particle_system.particle_count].g = c2[1];
        particle_system.colors[particle_system.particle_count].b = c2[2];
        particle_system.colors[particle_system.particle_count].a = c2[3];
        
        particle_system.sizes[particle_system.particle_count].x = s2[0];
        particle_system.sizes[particle_system.particle_count].y = s2[1];
        
        particle_system.particle_count++;
        
        particle_system.offsets[particle_system.particle_count].x = p3[0];
        particle_system.offsets[particle_system.particle_count].y = p3[1];
        particle_system.offsets[particle_system.particle_count].z = p3[2];
        
        particle_system.colors[particle_system.particle_count].r = c3[0];
        particle_system.colors[particle_system.particle_count].g = c3[1];
        particle_system.colors[particle_system.particle_count].b = c3[2];
        particle_system.colors[particle_system.particle_count].a = c3[3];
        
        particle_system.sizes[particle_system.particle_count].x = s3[0];
        particle_system.sizes[particle_system.particle_count].y = s3[1];
        
        particle_system.particle_count++;
        
        particle_system.offsets[particle_system.particle_count].x = p4[0];
        particle_system.offsets[particle_system.particle_count].y = p4[1];
        particle_system.offsets[particle_system.particle_count].z = p4[2];
        
        particle_system.colors[particle_system.particle_count].r = c4[0];
        particle_system.colors[particle_system.particle_count].g = c4[1];
        particle_system.colors[particle_system.particle_count].b = c4[2];
        particle_system.colors[particle_system.particle_count].a = c4[3];
        
        particle_system.sizes[particle_system.particle_count].x = s4[0];
        particle_system.sizes[particle_system.particle_count].y = s4[1];
        
        particle_system.particle_count++;
    }
    
    *emitted_this_frame = 0;
}

void emit_particle(ParticleSystemInfo &particle_system, i32* alive_buf, i32* count, RandomSeries& entropy)
{
    i32 original_index = find_unused_particle(particle_system);
    
    assert(original_index != -1);
    
    // @Note(Niels): Init particle to the init values saved in the particle system
    particle_system.particles.life[original_index] = r64_4x(particle_system.attributes.life_time);
    particle_system.particles.size[original_index] = Vec2_4x(particle_system.attributes.start_size);
    particle_system.particles.color[original_index] = Rgba_4x(particle_system.attributes.start_color);
    particle_system.particles.relative_position[original_index] = Vec3_4x(0.0f);
    
    assert(particle_system.attributes.emission_module.emitter_func);
    
    /// @Note(Niels): Generate emission info based on the emitter function
    ParticleSpawnInfo spawn_info = particle_system.attributes.emission_module.emitter_func(entropy, particle_system.attributes.emission_module.min, particle_system.attributes.emission_module.max);
    particle_system.particles.position[original_index] = spawn_info.position;
    Vec3_4x new_direction = spawn_info.direction;
    
    // @Note(Niels): Now compute t he direction based on the direction given in the attributes and the randomly geneerated one
    particle_system.particles.direction[original_index] = math::normalize(particle_system.attributes.direction 
                                                                          + new_direction);
    
    // @Note:(Niels): The current buffer gets the particle being emitted
    // It is passed in, so we only need to check once per frame when the system is updated
    alive_buf[(*count)++] = original_index;
    
    // @Note:(Niels): 4 == SIMD? Maybe revisit how we think about particles always in 4's with SIMD...
    // This seems somewhat dumb.
    particle_system.particles_emitted_this_frame += 4;
}

// @Note(Niels): The way we update and choose particles is based on the link below
// https://turanszkij.wordpress.com/2017/11/07/gpu-based-particle-simulation/
void update_particle_systems(Renderer &renderer, r64 delta_time)
{
    for(i32 particle_system_index = 0; particle_system_index < renderer.particles.particle_system_count; particle_system_index++)
    {
        ParticleSystemInfo &particle_system = renderer.particles.particle_systems[particle_system_index];
        
        if (particle_system.running)
        {
            // @Note(Niels): We have one buffer that takes the particles from the previous frame
            // plus the particles that are emitted in the current frame.
            // The first time the buffer is empty and only contains emitted particles
            // For any k > 1 where k is the iteration, the buffer works as explained.
            // The second buffer then gets the particles that are still alive this frame
            // and this is also the buffer that contains the particles that are drawn in
            // a frame.
            i32* emitted_alive_buf = nullptr;
            i32* emitted_alive_count = nullptr;
            i32* write_buf = nullptr;
            i32* write_buf_count = nullptr;
            if(particle_system.alive0_active)
            {
                emitted_alive_buf = particle_system.alive0_particles;
                emitted_alive_count = &particle_system.alive0_particle_count;
                write_buf = particle_system.alive1_particles;
                write_buf_count = &particle_system.alive1_particle_count;
            }
            else
            {
                emitted_alive_buf = particle_system.alive1_particles;
                emitted_alive_count = &particle_system.alive1_particle_count;
                write_buf = particle_system.alive0_particles;
                write_buf_count = &particle_system.alive0_particle_count;
            }
            
            particle_system.alive0_active = !particle_system.alive0_active;
            
            if (particle_system.emitting)
            {
                // @Note(Niels): Needed for over-time values
                particle_system.time_spent += delta_time;
                
                i32 new_particles;
                
                // @Note(Niels): Figure out the burst amount if there is any
                i32 burst_particles = 0;
                
                // @Note(Niels): Burst code for burst emission.
                auto value_count = particle_system.attributes.emission_module.burst_over_lifetime.value_count;
                if(value_count > 0)
                {
                    auto burst_index  = particle_system.attributes.emission_module.burst_over_lifetime.current_index;
                    
                    if(burst_index < value_count)
                    {
                        auto &current_burst = particle_system.attributes.emission_module.burst_over_lifetime.values[burst_index];
                        
                        auto target_time = current_burst.repeat_interval;
                        
                        particle_system.current_emission_time += delta_time;
                        
                        if(particle_system.current_emission_time >= target_time)
                        {
                            burst_particles = current_burst.count;
                            
                            if(current_burst.cycle_count > 0 && particle_system.attributes.emission_module.burst_over_lifetime.current_index >= current_burst.cycle_count)
                            {
                                particle_system.attributes.emission_module.burst_over_lifetime.current_index++;
                            }
                            
                            particle_system.current_emission_time = 0.0;
                        }
                    }
                }
                
                // @Note(Niels): Start figuring out how many particles we need to emit this frame
                r32 per_second = (r32)((r64)particle_system.attributes.particles_per_second * delta_time);
                
                particle_system.particles_cumulative += per_second;
                
                if(particle_system.particles_cumulative >= 1.0f)
                {
                    new_particles = math::round(particle_system.particles_cumulative);
                    particle_system.particles_cumulative = 0.0f;
                }
                else // @Note(Niels): Otherwise just find the round number of particles to emit
                {
                    new_particles = math::round(per_second);
                }
                
                // @Note(Niels): We need to check if we've spent 1 second if per second is lower than 1.0
                if(per_second < 1.0f && per_second > 0.0f && particle_system.time_spent >= 1.0)
                {
                    particle_system.time_spent = 0.0;
                    
                    
                }
                
                // @Note(Niels): If we have a one shot particle system we need to check if we have emitted
                // every particle available. This means that the amount of particles in a one shot system is tightly coupled to the max amount given (should we have two numbers for this?)
                if (particle_system.attributes.one_shot && particle_system.total_emitted + new_particles >= particle_system.max_particles)
                {
                    new_particles = particle_system.max_particles - particle_system.total_emitted;
                    particle_system.emitting = false;
                }
                
                
                // @Incomplete(Niels): Consider if it is even necessary to have these simd values??
                // seems kind of dumb...
                // @Note(Niels): The reason for this is to get a number as a multiple of 4 because of SIMD emission (we always emit in 4's).
                i32 simd_new_particles = math::multiple_of_number(new_particles, 4);
                i32 simd_burst_particles = math::multiple_of_number(burst_particles, 4);
                
                // @Note(Niels): Check if the new amount is below the max and below the amount of dead particles.
                simd_new_particles = MIN(particle_system.max_particles, MIN(simd_new_particles, particle_system.dead_particle_count * 4));
                
                // @Note(Niels): Emit the particles into the current alive buffer
                // The first time around this buffer is empty, but on any subsequent step
                // there should already be particles from the previous frame, which were
                // written into the then next buffer (which is now the alive buffer)
                for (i32 i = 0; i < simd_new_particles / 4; i++)
                {
                    emit_particle(particle_system, emitted_alive_buf, emitted_alive_count, renderer.particles.entropy);
                }
                
                // @Note(Niels): Same goes for burst
                if(particle_system.attributes.one_shot)
                    particle_system.total_emitted += new_particles;
                
                for (i32 i = 0; i < simd_burst_particles / 4; i++)
                {
                    emit_particle(particle_system, emitted_alive_buf, emitted_alive_count, renderer.particles.entropy);
                }
            }
            
            // @Note:(Niels): We now update the particles in the emitted alive buf (which may contain particles from previous frames that are still alive), while passing in the next buffer,
            // which is now our "write" buffer.
            update_particles(renderer, particle_system, delta_time, emitted_alive_buf, emitted_alive_count, write_buf, write_buf_count);
            
            // @Note(Niels): if all particles are dead and the system is one-shot we should stop the particle_system
            if(particle_system.attributes.one_shot && particle_system.total_emitted == particle_system.max_particles)
            {
                particle_system.running = false;
                particle_system.alive0_particle_count = 0;
                particle_system.alive1_particle_count = 0;
            }
        }   
    }
}
// @Move: END


