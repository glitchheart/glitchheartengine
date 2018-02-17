static i32 CreateAnimationController(renderer& Renderer)
{
    auto& Controller = Renderer.AnimationControllers[Renderer.AnimationControllerCount];
    Controller.CurrentFrameIndex = 0;
    Controller.CurrentNode = 0;
    Controller.CurrentTime = 0.0;
    Controller.Playing = false;
    Controller.Speed = 1.0f;
    Controller.NodeCount = 0;
    Controller.ParameterCount = 0;
    return Renderer.AnimationControllerCount++;
}

static void AddAnimationControllerParameter(renderer& Renderer, i32 Controller, const char* ParameterName, b32 InitialValue)
{
    auto& Controller = Renderer.AnimationControllers[Controller];
    auto& Parameter = Controller.Parameters[Controller.ParameterCount++];
    strcpy(Parameter.Name, ParameterName);
    Parameter.Value = InitialValue;
}

static void AddAnimationNode(renderer& Renderer, i32 Controller, const char* AnimationName, b32 Loop)
{
    i32 AnimationHandle -1;
    
    for(i32 AnimationIndex = 0; AnimationIndex < Renderer.SpritesheetAnimationCount; AnimationIndex++)
    {
        if(strcmp(Renderer.SpritesheetAnimations[AnimationIndex].Name, AnimationName) == 0)
        {
            AnimationHandle = AnimationIndex;
            break;
        }
    }
    
    Assert(AnimationHandle != -1);
    
    auto& Animation = Renderer.SpritesheetAnimations[AnimationHandle];
    auto& Node = Controller.Nodes[Controller.NodeCount++];
    strcpy(Node.Name, AnimationName);
    Node.AnimationHandle = AnimationHandle;
    Node.FreezeOnLastFrame = FreezeOnLastFrame;
    Node.Loop = Loop;
    Node.LinkCount = 0;
}

static i32 AddAnimationNodeLink(renderer& Renderer, const char* Origin, const char* Destination, b32 AfterFinishedAnimation = true)
{
    auto& Controller = Renderer.AnimationControllers[Controller];
    i32 OriginNodeHandle = -1;
    i32 DestinationNodeHandle = -1;
    
    for(i32 NodeIndex = 0; NodeIndex < Controller.NodeCount; NodeIndex++)
    {
        if(strcmp(Controller.Node.Name, Origin) == 0)
        {
            OriginNodeHandle = NodeIndex;
        }
        else if(strcmp(Controller.Node.Name, Destination) == 0)
        {
            DestinationNodeHandle = NodeIndex;
        }
        
        if(OriginNodeHandle != -1 && DestinationNodeHandle != -1)
        {
            break;
        }
    }
    
    Assert(OriginNodeHandle != -1 && DestinationNodeHandle != -1);
    
    auto& OriginNode = Controller.Nodes[OriginNodeHandle];
    auto& DestinationNode = Controller.Nodes[DestinationNodeHandle];
    
    auto& Link = OriginNode.Links[OriginNode.LinkCount];
    Link.OriginNode = OriginNodeHandle;
    Link.DestinatioNode = DestinationNodeHandle;
    Link.ConditionCount = 0;
    Link.AfterFinishedAnimation = AfterFinishedAnimation;
    return OriginNode.LinkCount++;
}

static void AddAnimationLinkCondition(renderer& Renderer, i32 Controller, i32 Node, i32 Link, const char* ParameterName, b32 ExpectedValue)
{
    i32 ParameterHandle = -1;
    
    for(i32 ParameterIndex = 0; ParameterIndex < Controller.ParameterCount; ParameterIndex++)
    {
        if(strcmp(ParameterName, Controller.Parameters[ParameterIndex]) == 0)
        {
            ParameterHandle = ParameterIndex;
            break;
        }
    }
    
    Assert(ParameterHandle != -1);
    auto& NodeLink = Controller.Node[Node].Links[Link];
    auto& Condition = NodeLink.Conditions[NodeLink.ConditionCount++];
    Condition.ParameterHandle = ParameterHandle;
    Condition.ExpectedValue = ExpectedValue;
}

static void TickAnimationControllers(renderer& Renderer, timer_controller& TimerController, r64 DeltaTime)
{
    for(i32 Index = 0; Index < Renderer.AnimationControllerCount; Index++)
    {
        auto& AnimationController = Renderer.AnimationControllers[Index];
        auto& CurrentNode = AnimationController.Nodes[AnimationController.CurrentNode];
        
        auto& CurrentAnimation = Renderer.SpritesheetAnimations[CurrentNode.AnimationHandle];
        i32 LastFrame = CurrentAnimation.FrameCount - 1;
        
        b32 ReachedEndOfFrame = AnimationController.CurrentTime >= CurrentAnimation.Frames[LastFrame].Duration;
        b32 ReachedEnd = CurrentAnimation.FrameCount == AnimationController.CurrentFrameIndex && ReachedEndOfFrame;
        
        b32 ChangedNode = false;
        
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
                    ConditionsMet = Condition.ExpectedValue == AnimationController.Parameters[Condition.ParameterHandle];
                    
                    if(!ConditionsMet)
                    {
                        break;
                    }
                }
                
                if(ConditionsMet)
                {
                    AnimationController.CurrentNode = Link.DestinationNode;
                    ChangedNode = true;
                    AnimationController.CurrentFrameIndex = 0;
                    AnimationController.CurrentTime = 0.0;
                    break;
                }
            }
        }
        
        if(ReachedEnd && CurrentNode.Loop && !ChangedNode)
        {
            AnimationController.CurrentFrameIndex = 0;
            AnimationController.CurrentTime = 0.0;
        }
        else if(ReachedEndOfFrame)
        {
            AnimationController.CurrentFrameIndex++;
            AnimationController.CurrentTime = 0.0;
        }
        
        AnimationController.CurrentTime += AnimationController.Speed * DeltaTime;
    }
}