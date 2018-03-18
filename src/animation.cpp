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
    auto& controller = renderer.AnimationControllers[renderer.AnimationControllerCount];
    controller.CurrentFrameIndex = 0;
    controller.CurrentNode = 0;
    controller.CurrentTime = 0.0;
    controller.Playing = playing;
    controller.Speed = 1.0f;
    controller.NodeCount = 0;
    controller.ParameterCount = 0;
    return renderer.AnimationControllerCount++;
}

static void clear_animation_controllers(Renderer& renderer)
{
    renderer.AnimationControllerCount = 0;
}

static void animation_controller_set_playing(Renderer& renderer, i32 controller, b32 playing)
{
    renderer.AnimationControllers[controller].Playing = playing;
}

static void add_animation_controller_parameter(Renderer& renderer, i32 controller, const char* parameter_name, b32 initial_value)
{
    auto& animation_controller = renderer.AnimationControllers[controller];
    auto& parameter = animation_controller.Parameters[animation_controller.ParameterCount++];
    strcpy(parameter.Name, parameter_name);
    parameter.Value = initial_value;
}

static void set_animation_controller_parameter(Renderer& renderer, i32 controller, const char* parameter, b32 value)
{
    auto& animation_controller = renderer.AnimationControllers[controller];
    
    for(i32 index = 0; index < animation_controller.ParameterCount; index++)
    {
        if(strcmp(animation_controller.Parameters[index].Name, parameter) == 0)
        {
            animation_controller.Parameters[index].Value = value;
            break;
        }
    }
}

static i32 add_animation_node(Renderer& renderer, i32 controller, const char* animation_name, b32 loop)
{
    i32 AnimationHandle = -1;
    
    for(i32 animation_index = 0; animation_index < renderer.SpritesheetAnimationCount; animation_index++)
    {
        if(strcmp(renderer.SpritesheetAnimations[animation_index].Name, animation_name) == 0)
        {
            AnimationHandle = animation_index;
            break;
        }
    }
    
    if(AnimationHandle == -1)
    {
        printf("Animation with name: %s not found\n", animation_name);
    }
    
    Assert(AnimationHandle != -1);
    
    auto& animation_controller = renderer.AnimationControllers[controller];
    auto& node = animation_controller.Nodes[animation_controller.NodeCount];
    strcpy(node.Name, animation_name);
    node.AnimationHandle = AnimationHandle;
    node.Loop = loop;
    node.LinkCount = 0;
    node.CallbackInfoCount = 0;
    return animation_controller.NodeCount++;
}

static void add_callback_to_animation_node(Renderer& renderer, i32 controller, i32 node_handle, void* state, void* data, animation_callback* callback, i32 callback_frame = -1)
{
    auto& node = renderer.AnimationControllers[controller].Nodes[node_handle];
    auto& callback_info = node.CallbackInfos[node.CallbackInfoCount++];
    callback_info.Callback = callback;
    callback_info.State = state;
    callback_info.Data = data;
    callback_info.Frame = callback_frame;
}

static i32 add_animation_node_link(Renderer& renderer, i32 controller, const char* Origin, const char* Destination, b32 after_finished_animation = true)
{
    Assert(strcmp(Origin, Destination) != 0);
    
    auto& animation_controller = renderer.AnimationControllers[controller];
    i32 OriginNodeHandle = -1;
    i32 DestinationNodeHandle = -1;
    
    for(i32 node_index = 0; node_index < animation_controller.NodeCount; node_index++)
    {
        if(strcmp(animation_controller.Nodes[node_index].Name, Origin) == 0)
        {
            OriginNodeHandle = node_index;
        }
        else if(strcmp(animation_controller.Nodes[node_index].Name, Destination) == 0)
        {
            DestinationNodeHandle = node_index;
        }
        
        if(OriginNodeHandle != -1 && DestinationNodeHandle != -1)
        {
            break;
        }
    }
    
    Assert(OriginNodeHandle != -1 && DestinationNodeHandle != -1);
    
    auto& origin_node = animation_controller.Nodes[OriginNodeHandle];
    
    auto& link = origin_node.Links[origin_node.LinkCount];
    link.OriginNode = OriginNodeHandle;
    link.DestinationNode = DestinationNodeHandle;
    link.ConditionCount = 0;
    link.AfterFinishedAnimation = after_finished_animation;
    return origin_node.LinkCount++;
}

static void add_animation_link_condition(Renderer& renderer, i32 controller, i32 node, i32 link, const char* parameter_name, b32 expected_value)
{
    auto& animation_controller = renderer.AnimationControllers[controller];
    i32 ParameterHandle = -1;
    
    for(i32 parameter_index = 0; parameter_index < animation_controller.ParameterCount; parameter_index++)
    {
        if(strcmp(parameter_name, animation_controller.Parameters[parameter_index].Name) == 0)
        {
            ParameterHandle = parameter_index;
            break;
        }
    }
    
    Assert(ParameterHandle != -1);
    auto& node_link = animation_controller.Nodes[node].Links[link];
    auto& condition = node_link.Conditions[node_link.ConditionCount++];
    condition.ParameterHandle = ParameterHandle;
    condition.ExpectedValue = expected_value;
}

static void reset_callbacks(AnimationNode& node)
{
    for(i32 callback_index = 0; callback_index < node.CallbackInfoCount; callback_index++)
    {
        node.CallbackInfos[callback_index].WasCalled = false;
    }
}

static void tick_animation_controllers(Renderer& renderer, sound_commands* sound_commands, InputController* input_controller, timer_controller& timer_controller,  r64 delta_time)
{
    for(i32 index = 0; index < renderer.AnimationControllerCount; index++)
    {
        auto& animation_controller = renderer.AnimationControllers[index];
        
        if(animation_controller.Playing)
        {
            auto& current_node = animation_controller.Nodes[animation_controller.CurrentNode];
            
            auto& current_animation = renderer.SpritesheetAnimations[current_node.AnimationHandle];
            
            b32 reached_end_of_frame = animation_controller.CurrentTime >= current_animation.Frames[animation_controller.CurrentFrameIndex].Duration;
            b32 reached_end = current_animation.FrameCount - 1 <= animation_controller.CurrentFrameIndex && reached_end_of_frame;
            
            b32 changed_node = false;
            
            // Check for callbacks
            for(i32 callback_index = 0; callback_index < current_node.CallbackInfoCount; callback_index++)
            {
                auto& callback_info = current_node.CallbackInfos[callback_index];
                
                if(!callback_info.WasCalled && (callback_info.Frame != -1 && callback_info.Frame == animation_controller.CurrentFrameIndex || callback_info.Frame == -1 && reached_end))
                {
                    if(callback_info.Callback)
                    {
                        callback_info.Callback(callback_info.State, callback_info.Data, Renderer, SoundCommands, InputController, TimerController);
                        callback_info.WasCalled = true;
                        //Debug("Frame for callback %d %s\n", CallbackInfo.Frame, CurrentNode.Name);
                    }
                }
            }
            
            for(i32 link_index = 0; link_index < current_node.LinkCount; link_index++)
            {
                auto& link = current_node.Links[link_index];
                
                // if link is for after the animation has finished we should only check the conditions if we've reached the end of the animation
                if(!link.AfterFinishedAnimation || reached_end)
                {
                    b32 conditions_met = true;
                    
                    for(i32 condition_index = 0; condition_index < link.ConditionCount; condition_index++)
                    {
                        auto& condition = link.Conditions[condition_index];
                        conditions_met = condition.ExpectedValue == animation_controller.Parameters[condition.ParameterHandle].Value;
                        
                        if(!conditions_met)
                        {
                            link.ConditionMet = conditions_met;
                            break;
                        }
                    }
                    
                    link.ConditionMet = conditions_met;
                    if(conditions_met)
                    {
                        animation_controller.CurrentNode = link.DestinationNode;
                        changed_node = true;
                        animation_controller.CurrentFrameIndex = 0;
                        animation_controller.CurrentTime = 0.0;
                        reset_callbacks(current_node);
                        break;
                    }
                    
                }
            } 
            
            if(reached_end && current_node.Loop && !changed_node)
            {
                animation_controller.CurrentFrameIndex = 0;
                animation_controller.CurrentTime = 0.0;
                reset_callbacks(current_node);
            }
            else if(reached_end_of_frame && !reached_end)
            {
                animation_controller.CurrentFrameIndex++;
                animation_controller.CurrentTime = 0.0;
                reset_callbacks(current_node);
            }
            
            animation_controller.CurrentTime += animation_controller.Speed * delta_time;
        }
    }
}

static void set_animation_controller_current_node(Renderer& renderer, i32 controller, const char* node_name)
{
    auto& animation_controller = renderer.AnimationControllers[controller];
    for(i32 node_index = 0; node_index < animation_controller.NodeCount; node_index++)
    {
        if(strcmp(animation_controller.Nodes[node_index].Name, node_name) == 0)
        {
            renderer.AnimationControllers[node_index].CurrentNode = node_index;
            break;
        }
    }
}

static b32 is_controller_playing(Renderer& renderer, i32 controller)
{
    return renderer.AnimationControllers[controller].Playing;
}