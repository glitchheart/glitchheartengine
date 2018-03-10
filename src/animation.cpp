// QUICK HOW TO GUIDE
// First create an animation_controller with CreateAnimationController
// Then add all needed animation_parameters with AddAnimationControllerParameter
// Then add all animation_nodes with AddAnimationNode
// Then set the links with AddAnimationNodeLink and you're done setting things up
// Just remember to set the correct CurrentNode (node to start on) with SetAnimationControllerCurrentNode

#define LINK_ANIMATION_END true
#define LINK_ALL_FRAMES false

static i32 CreateAnimationController(renderer& Renderer, b32 Playing = false)
{
    auto& Controller = Renderer.AnimationControllers[Renderer.AnimationControllerCount];
    Controller.CurrentFrameIndex = 0;
    Controller.CurrentNode = 0;
    Controller.CurrentTime = 0.0;
    Controller.Playing = Playing;
    Controller.Speed = 1.0f;
    Controller.NodeCount = 0;
    Controller.ParameterCount = 0;
    return Renderer.AnimationControllerCount++;
}

static void ClearAnimationControllers(renderer& Renderer)
{
    Renderer.AnimationControllerCount = 0;
}

static void AnimationControllerSetPlaying(renderer& Renderer, i32 Controller, b32 Playing)
{
    Renderer.AnimationControllers[Controller].Playing = Playing;
}

static void AddAnimationControllerParameter(renderer& Renderer, i32 Controller, const char* ParameterName, b32 InitialValue)
{
    auto& AnimationController = Renderer.AnimationControllers[Controller];
    auto& Parameter = AnimationController.Parameters[AnimationController.ParameterCount++];
    strcpy(Parameter.Name, ParameterName);
    Parameter.Value = InitialValue;
}

static void SetAnimationControllerParameter(renderer& Renderer, i32 Controller, const char* Parameter, b32 Value)
{
    auto& AnimationController = Renderer.AnimationControllers[Controller];
    
    for(i32 Index = 0; Index < AnimationController.ParameterCount; Index++)
    {
        if(strcmp(AnimationController.Parameters[Index].Name, Parameter) == 0)
        {
            AnimationController.Parameters[Index].Value = Value;
            break;
        }
    }
}

static i32 AddAnimationNode(renderer& Renderer, i32 Controller, const char* AnimationName, b32 Loop)
{
    i32 AnimationHandle = -1;
    
    for(i32 AnimationIndex = 0; AnimationIndex < Renderer.SpritesheetAnimationCount; AnimationIndex++)
    {
        if(strcmp(Renderer.SpritesheetAnimations[AnimationIndex].Name, AnimationName) == 0)
        {
            AnimationHandle = AnimationIndex;
            break;
        }
    }
    
    if(AnimationHandle == -1)
    {
        printf("Animation with name: %s not found\n", AnimationName);
    }
    
    Assert(AnimationHandle != -1);
    
    auto& AnimationController = Renderer.AnimationControllers[Controller];
    auto& Node = AnimationController.Nodes[AnimationController.NodeCount];
    strcpy(Node.Name, AnimationName);
    Node.AnimationHandle = AnimationHandle;
    Node.Loop = Loop;
    Node.LinkCount = 0;
    Node.CallbackInfoCount = 0;
    return AnimationController.NodeCount++;
}

static void AddCallbackToAnimationNode(renderer& Renderer, i32 Controller, i32 NodeHandle, void* State, void* Data, animation_callback* Callback, i32 CallbackFrame = -1)
{
    auto& Node = Renderer.AnimationControllers[Controller].Nodes[NodeHandle];
    auto& CallbackInfo = Node.CallbackInfos[Node.CallbackInfoCount++];
    CallbackInfo.Callback = Callback;
    CallbackInfo.State = State;
    CallbackInfo.Data = Data;
    CallbackInfo.Frame = CallbackFrame;
}

static i32 AddAnimationNodeLink(renderer& Renderer, i32 Controller, const char* Origin, const char* Destination, b32 AfterFinishedAnimation = true)
{
    Assert(strcmp(Origin, Destination) != 0);
    
    auto& AnimationController = Renderer.AnimationControllers[Controller];
    i32 OriginNodeHandle = -1;
    i32 DestinationNodeHandle = -1;
    
    for(i32 NodeIndex = 0; NodeIndex < AnimationController.NodeCount; NodeIndex++)
    {
        if(strcmp(AnimationController.Nodes[NodeIndex].Name, Origin) == 0)
        {
            OriginNodeHandle = NodeIndex;
        }
        else if(strcmp(AnimationController.Nodes[NodeIndex].Name, Destination) == 0)
        {
            DestinationNodeHandle = NodeIndex;
        }
        
        if(OriginNodeHandle != -1 && DestinationNodeHandle != -1)
        {
            break;
        }
    }
    
    Assert(OriginNodeHandle != -1 && DestinationNodeHandle != -1);
    
    auto& OriginNode = AnimationController.Nodes[OriginNodeHandle];
    
    auto& Link = OriginNode.Links[OriginNode.LinkCount];
    Link.OriginNode = OriginNodeHandle;
    Link.DestinationNode = DestinationNodeHandle;
    Link.ConditionCount = 0;
    Link.AfterFinishedAnimation = AfterFinishedAnimation;
    return OriginNode.LinkCount++;
}

static void AddAnimationLinkCondition(renderer& Renderer, i32 Controller, i32 Node, i32 Link, const char* ParameterName, b32 ExpectedValue)
{
    auto& AnimationController = Renderer.AnimationControllers[Controller];
    i32 ParameterHandle = -1;
    
    for(i32 ParameterIndex = 0; ParameterIndex < AnimationController.ParameterCount; ParameterIndex++)
    {
        if(strcmp(ParameterName, AnimationController.Parameters[ParameterIndex].Name) == 0)
        {
            ParameterHandle = ParameterIndex;
            break;
        }
    }
    
    Assert(ParameterHandle != -1);
    auto& NodeLink = AnimationController.Nodes[Node].Links[Link];
    auto& Condition = NodeLink.Conditions[NodeLink.ConditionCount++];
    Condition.ParameterHandle = ParameterHandle;
    Condition.ExpectedValue = ExpectedValue;
}

static void ResetCallbacks(animation_node& Node)
{
    for(i32 CallbackIndex = 0; CallbackIndex < Node.CallbackInfoCount; CallbackIndex++)
    {
        Node.CallbackInfos[CallbackIndex].WasCalled = false;
    }
}

static void TickAnimationControllers(renderer& Renderer, sound_commands* SoundCommands, input_controller* InputController, timer_controller& TimerController,  r64 DeltaTime)
{
    for(i32 Index = 0; Index < Renderer.AnimationControllerCount; Index++)
    {
        auto& AnimationController = Renderer.AnimationControllers[Index];
        
        if(AnimationController.Playing)
        {
            auto& CurrentNode = AnimationController.Nodes[AnimationController.CurrentNode];
            
            auto& CurrentAnimation = Renderer.SpritesheetAnimations[CurrentNode.AnimationHandle];
            
            b32 ReachedEndOfFrame = AnimationController.CurrentTime >= CurrentAnimation.Frames[AnimationController.CurrentFrameIndex].Duration;
            b32 ReachedEnd = CurrentAnimation.FrameCount - 1 <= AnimationController.CurrentFrameIndex && ReachedEndOfFrame;
            
            b32 ChangedNode = false;
            
            // Check for callbacks
            for(i32 CallbackIndex = 0; CallbackIndex < CurrentNode.CallbackInfoCount; CallbackIndex++)
            {
                auto& CallbackInfo = CurrentNode.CallbackInfos[CallbackIndex];
                
                if(!CallbackInfo.WasCalled && (CallbackInfo.Frame != -1 && CallbackInfo.Frame == AnimationController.CurrentFrameIndex || CallbackInfo.Frame == -1 && ReachedEnd))
                {
                    if(CallbackInfo.Callback)
                    {
                        CallbackInfo.Callback(CallbackInfo.State, CallbackInfo.Data, Renderer, SoundCommands, InputController, TimerController);
                        CallbackInfo.WasCalled = true;
                        //Debug("Frame for callback %d %s\n", CallbackInfo.Frame, CurrentNode.Name);
                    }
                }
            }
            
            for(i32 LinkIndex = 0; LinkIndex < CurrentNode.LinkCount; LinkIndex++)
            {
                auto& Link = CurrentNode.Links[LinkIndex];
                
                // if link is for after the animation has finished we should only check the conditions if we've reached the end of the animation
                if(!Link.AfterFinishedAnimation || ReachedEnd)
                {
                    b32 ConditionsMet = true;
                    
                    for(i32 ConditionIndex = 0; ConditionIndex < Link.ConditionCount; ConditionIndex++)
                    {
                        auto& Condition = Link.Conditions[ConditionIndex];
                        ConditionsMet = Condition.ExpectedValue == AnimationController.Parameters[Condition.ParameterHandle].Value;
                        
                        if(!ConditionsMet)
                        {
                            Link.ConditionMet = ConditionsMet;
                            break;
                        }
                    }
                    
                    Link.ConditionMet = ConditionsMet;
                    if(ConditionsMet)
                    {
                        AnimationController.CurrentNode = Link.DestinationNode;
                        ChangedNode = true;
                        AnimationController.CurrentFrameIndex = 0;
                        AnimationController.CurrentTime = 0.0;
                        ResetCallbacks(CurrentNode);
                        break;
                    }
                    
                }
            } 
            
            if(ReachedEnd && CurrentNode.Loop && !ChangedNode)
            {
                AnimationController.CurrentFrameIndex = 0;
                AnimationController.CurrentTime = 0.0;
                ResetCallbacks(CurrentNode);
            }
            else if(ReachedEndOfFrame && !ReachedEnd)
            {
                AnimationController.CurrentFrameIndex++;
                AnimationController.CurrentTime = 0.0;
                ResetCallbacks(CurrentNode);
            }
            
            AnimationController.CurrentTime += AnimationController.Speed * DeltaTime;
        }
    }
}

static void SetAnimationControllerCurrentNode(renderer& Renderer, i32 Controller, const char* NodeName)
{
    auto& AnimationController = Renderer.AnimationControllers[Controller];
    for(i32 NodeIndex = 0; NodeIndex < AnimationController.NodeCount; NodeIndex++)
    {
        if(strcmp(AnimationController.Nodes[NodeIndex].Name, NodeName) == 0)
        {
            Renderer.AnimationControllers[NodeIndex].CurrentNode = NodeIndex;
            break;
        }
    }
}

static b32 IsControllerPlaying(renderer& Renderer, i32 Controller)
{
    return Renderer.AnimationControllers[Controller].Playing;
}