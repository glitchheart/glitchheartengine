namespace fade
{
    void _enqueue_fade_command(FadeState *fade_state, FadeCommand fade_command)
    {
        fade_command.current_time = 0.0;
        fade_state->fade_commands[fade_state->current_fade_index + fade_state->fade_command_count++] = fade_command;
    }

    void render_fades(FadeState *fade_state, Renderer *renderer)
    {
        if(fade_state->fade_alpha > 0.0f)
        {
            rendering::CreateUICommandInfo info = rendering::create_ui_command_info();

            info.transform.position = math::Vec2(500);
            info.transform.scale = math::Vec2(1000);
            info.color = math::Rgba(0.0f, 0.0f, 0.0f, fade_state->fade_alpha);
            info.z_layer = Z_LAYERS - 1;
            info.scaling_flag = 0;
        
            rendering::push_ui_quad(renderer, info);
        }
    }

    static b32 _check_for_skip(InputController *input_controller, FadeState *fade_state, FadeCommand &command)
    {
        b32 command_done = false;
    
        if(input_controller->any_key_pressed)
        {
            command_done = true;

            if(command.mode == FADE_IN)
            {
                r64 fraction = 1.0f / command.time * command.current_time;
        
                while(true)
                {
                    FadeCommand &next_command = fade_state->fade_commands[fade_state->current_fade_index + 1];
                    if(next_command.mode == FADE_OUT)
                    {
                        next_command.current_time = fraction * next_command.time;
                        break;
                    }
                    else
                    {
                        fade_state->fade_command_count--;
                        fade_state->current_fade_index++;
                    }
                }
            }
        }

        return(command_done);
    }

    void update_fade_commands(FadeState *fade_state, Renderer *renderer, InputController *input_controller, r64 delta_time)
    {
        if(fade_state->fade_command_count > 0)
        {
            fade_state->fading = true;
	
            FadeCommand &command = fade_state->fade_commands[fade_state->current_fade_index];
	
            b32 command_done = false;
        
            switch(command.mode)
            {
            case FADE_OUT:
            command.current_time += delta_time;
            fade_state->fade_alpha = (r32)((r64) 1.0f / command.time * command.current_time);
		
            if(fade_state->fade_alpha >= 1.0f)
            {
                fade_state->fade_alpha = 1.0f;
                command_done = true;
            }
            break;
            case FADE_STAY:
            {
                command.current_time += delta_time;

                if(command.skippable)
                {
                    command_done = _check_for_skip(input_controller, fade_state, command);
                }

                if(command.current_time > command.time)
                {
                    command_done = true;
                }
            }
            break;
            case FADE_IN:
            {
                command.current_time += delta_time;

                r64 fraction = 1.0f / command.time * command.current_time;
        
                fade_state->fade_alpha = 1.0f - (r32)((r64)fraction);

                if(command.skippable)
                {
                    command_done = _check_for_skip(input_controller, fade_state, command);
                }

                if(fade_state->fade_alpha <= 0.0f)
                {
                    fade_state->fade_alpha = 0.0f;
                    command_done = true;
                }
            }
            break;
            default:
            break;
            }

            rendering::CreateUICommandInfo quad_info = rendering::create_ui_command_info();

            quad_info.transform.position = math::Vec2(500);
            quad_info.transform.scale = math::Vec2(1000);
            quad_info.color = math::Rgba(0.0f, 0.0f, 0.0f, fade_state->fade_alpha);
            quad_info.z_layer = Z_LAYERS - 1;
            quad_info.scaling_flag = 0;

            rendering::push_ui_quad(renderer, quad_info);
        
            if(command_done)
            {
                fade_state->fade_command_count--;
            
                if(command.callback)
                {
                    command.callback();
                    command.callback = nullptr;
                }
            
                if(fade_state->fade_command_count > 0)
                {
                    fade_state->current_fade_index++;
                }
                else
                {
                    fade_state->current_fade_index = 0;
                }
            }
        }
        else
            fade_state->fading = false;
    }

}

