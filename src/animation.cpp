// QUICK HOW TO GUIDE
// First create an animation_controller with CreateAnimationController
// Then add all needed animation_parameters with AddAnimationControllerParameter
// Then add all animation_nodes with AddAnimationNode
// Then set the links with AddAnimationNodeLink and you're done setting things up
// Just remember to set the correct CurrentNode (node to start on) with SetAnimationControllerCurrentNode

#define LINK_ANIMATION_END true
#define LINK_ALL_FRAMES false

static i32 create_animation_controller(Renderer& renderer, b32 playing = false)
{
    assert(renderer.animation_controller_count + 1 < global_max_animation_controllers);
    auto& controller = renderer.animation_controllers[renderer.animation_controller_count];
    controller.current_frame_index = 0;
    controller.current_node = 0;
    controller.current_time = 0.0;
    controller.playing = playing;
    controller.speed = 1.0f;
    controller.node_count = 0;
    controller.parameter_count = 0;
    return (renderer.animation_controller_count++) + 1;
}

static void clear_animation_controllers(Renderer& renderer)
{
    renderer.animation_controller_count = 0;
}

static void animation_controller_set_playing(Renderer& renderer, i32 controller, b32 playing)
{
    renderer.animation_controllers[controller].playing = playing;
}

static void add_animation_controller_parameter(Renderer& renderer, i32 controller, const char* parameter_name, b32 initial_value)
{
    auto& animation_controller = renderer.animation_controllers[controller];
    assert(animation_controller.parameter_count + 1 < MAX_ANIMATION_PARAMETERS);
    auto& parameter = animation_controller.parameters[animation_controller.parameter_count++];
    strcpy(parameter.name, parameter_name);
    parameter.value = initial_value;
}

static void set_animation_controller_parameter(Renderer& renderer, i32 controller, const char* parameter, b32 value)
{
    auto& animation_controller = renderer.animation_controllers[controller];
    
    for(i32 index = 0; index < animation_controller.parameter_count; index++)
    {
        if(strcmp(animation_controller.parameters[index].name, parameter) == 0)
        {
            animation_controller.parameters[index].value = value;
            break;
        }
    }
}

static i32 add_animation_node(Renderer& renderer, i32 controller, const char* animation_name, b32 loop)
{
    i32 AnimationHandle = -1;
    
    for(i32 animation_index = 0; animation_index < renderer.spritesheet_animation_count; animation_index++)
    {
        if(strcmp(renderer.spritesheet_animations[animation_index].name, animation_name) == 0)
        {
            AnimationHandle = animation_index;
            break;
        }
    }
    
    if(AnimationHandle == -1)
    {
        printf("Animation with name: %s not found\n", animation_name);
    }
    
    assert(AnimationHandle != -1);
    
    auto& animation_controller = renderer.animation_controllers[controller];
    assert(animation_controller.node_count + 1 < MAX_ANIMATION_NODES);
    auto& node = animation_controller.nodes[animation_controller.node_count];
    strcpy(node.name, animation_name);
    node.animation_handle = AnimationHandle;
    node.loop = loop;
    node.link_count = 0;
    node.callback_info_count = 0;
    return animation_controller.node_count++;
}

static void add_callback_to_animation_node(Renderer& renderer, i32 controller, i32 node_handle, void* state, void* data, animation_callback* callback, i32 callback_frame = -1)
{
    auto& node = renderer.animation_controllers[controller].nodes[node_handle];
    assert(node.callback_info_count + 1 < MAX_ANIMATION_CALLBACKS);
    auto& callback_info = node.callback_infos[node.callback_info_count++];
    callback_info.callback = callback;
    callback_info.state = state;
    callback_info.data = data;
    callback_info.frame = callback_frame;
}

static i32 add_animation_node_link(Renderer& renderer, i32 controller, const char* Origin, const char* Destination, b32 after_finished_animation = true)
{
    assert(strcmp(Origin, Destination) != 0);
    
    auto& animation_controller = renderer.animation_controllers[controller];
    i32 OriginNodeHandle = -1;
    i32 DestinationNodeHandle = -1;
    
    for(i32 node_index = 0; node_index < animation_controller.node_count; node_index++)
    {
        if(strcmp(animation_controller.nodes[node_index].name, Origin) == 0)
        {
            OriginNodeHandle = node_index;
        }
        else if(strcmp(animation_controller.nodes[node_index].name, Destination) == 0)
        {
            DestinationNodeHandle = node_index;
        }
        
        if(OriginNodeHandle != -1 && DestinationNodeHandle != -1)
        {
            break;
        }
    }
    
    assert(OriginNodeHandle != -1 && DestinationNodeHandle != -1);
    
    auto& origin_node = animation_controller.nodes[OriginNodeHandle];
    assert(origin_node.link_count + 1 < MAX_ANIMATION_LINKS);
    auto& link = origin_node.links[origin_node.link_count];
    link.origin_node = OriginNodeHandle;
    link.destination_node = DestinationNodeHandle;
    link.condition_count = 0;
    link.after_finished_animation = after_finished_animation;
    return origin_node.link_count++;
}

static void add_animation_link_condition(Renderer& renderer, i32 controller, i32 node, i32 link, const char* parameter_name, b32 expected_value)
{
    auto& animation_controller = renderer.animation_controllers[controller];
    i32 ParameterHandle = -1;
    
    for(i32 parameter_index = 0; parameter_index < animation_controller.parameter_count; parameter_index++)
    {
        if(strcmp(parameter_name, animation_controller.parameters[parameter_index].name) == 0)
        {
            ParameterHandle = parameter_index;
            break;
        }
    }
    
    assert(ParameterHandle != -1);
    auto& node_link = animation_controller.nodes[node].links[link];
    assert(node_link.condition_count + 1 < CONDITION_ARRAY_SIZE);
    auto& condition = node_link.conditions[node_link.condition_count++];
    condition.parameter_handle = ParameterHandle;
    condition.expected_value = expected_value;
}

static void reset_callbacks(AnimationNode& node)
{
    for(i32 callback_index = 0; callback_index < node.callback_info_count; callback_index++)
    {
        node.callback_infos[callback_index].was_called = false;
    }
}

static void tick_animation_controllers(Renderer& renderer, SoundSystem* sound_system, InputController* input_controller, TimerController& timer_controller,  r64 delta_time)
{
    for(i32 index = 0; index < renderer.animation_controller_count; index++)
    {
        auto& animation_controller = renderer.animation_controllers[index];
        
        if(animation_controller.playing)
        {
            auto& current_node = animation_controller.nodes[animation_controller.current_node];
            
            auto& current_animation = renderer.spritesheet_animations[current_node.animation_handle];
            
            b32 reached_end_of_frame = animation_controller.current_time >= current_animation.frames[animation_controller.current_frame_index].duration;
            b32 reached_end = current_animation.frame_count - 1 <= animation_controller.current_frame_index && reached_end_of_frame;
            
            b32 changed_node = false;
            
            // Check for callbacks
            for(i32 callback_index = 0; callback_index < current_node.callback_info_count; callback_index++)
            {
                auto& callback_info = current_node.callback_infos[callback_index];
                
                if(!callback_info.was_called && ((callback_info.frame != -1 && callback_info.frame == animation_controller.current_frame_index) || (callback_info.frame == -1 && reached_end)))
                {
                    if(callback_info.callback)
                    {
                        callback_info.callback(callback_info.state, callback_info.data, renderer, sound_system, input_controller, timer_controller);
                        callback_info.was_called = true;
                        //Debug("Frame for callback %d %s\n", CallbackInfo.Frame, CurrentNode.Name);
                    }
                }
            }
            
            for(i32 link_index = 0; link_index < current_node.link_count; link_index++)
            {
                auto& link = current_node.links[link_index];
                
                // if link is for after the animation has finished we should only check the conditions if we've reached the end of the animation
                if(!link.after_finished_animation || reached_end)
                {
                    b32 conditions_met = true;
                    
                    for(i32 condition_index = 0; condition_index < link.condition_count; condition_index++)
                    {
                        auto& condition = link.conditions[condition_index];
                        conditions_met = condition.expected_value == animation_controller.parameters[condition.parameter_handle].value;
                        
                        if(!conditions_met)
                        {
                            link.condition_met = conditions_met;
                            break;
                        }
                    }
                    
                    link.condition_met = conditions_met;
                    if(conditions_met)
                    {
                        animation_controller.current_node = link.destination_node;
                        changed_node = true;
                        animation_controller.current_frame_index = 0;
                        animation_controller.current_time = 0.0;
                        reset_callbacks(current_node);
                        break;
                    }
                    
                }
            } 
            
            if(reached_end && current_node.loop && !changed_node)
            {
                animation_controller.current_frame_index = 0;
                animation_controller.current_time = 0.0;
                reset_callbacks(current_node);
            }
            else if(reached_end_of_frame && !reached_end)
            {
                animation_controller.current_frame_index++;
                animation_controller.current_time = 0.0;
                reset_callbacks(current_node);
            }
            
            animation_controller.current_time += animation_controller.speed * delta_time;
        }
    }
}

static void set_animation_controller_current_node(Renderer& renderer, i32 controller, const char* node_name)
{
    auto& animation_controller = renderer.animation_controllers[controller];
    for(i32 node_index = 0; node_index < animation_controller.node_count; node_index++)
    {
        if(strcmp(animation_controller.nodes[node_index].name, node_name) == 0)
        {
            renderer.animation_controllers[node_index].current_node = node_index;
            break;
        }
    }
}

static b32 is_controller_playing(Renderer& renderer, i32 controller)
{
    return renderer.animation_controllers[controller].playing;
}
