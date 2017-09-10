static i32 FindAnimationPosition(const vec3_keys& PositionKeys, r32 AnimationTime)
{
    for (i32 Index = 0; Index < PositionKeys.NumKeys - 1 ; Index++)
    {
        if (AnimationTime < PositionKeys.TimeStamps[Index + 1])
        {
            return Index;
        }
    }
    
    return PositionKeys.NumKeys - 1;;
}

static i32 FindAnimationScaling(const vec3_keys& ScalingKeys, r32 AnimationTime)
{
    for (i32 Index = 0; Index < ScalingKeys.NumKeys - 1 ; Index++)
    {
        if (AnimationTime < ScalingKeys.TimeStamps[Index + 1])
        {
            return Index;
        }
    }
    
    return ScalingKeys.NumKeys - 1;
}

static i32 FindAnimationRotation(const quat_keys& RotationKeys, r32 AnimationTime)
{
    for (i32 Index = 0; Index < RotationKeys.NumKeys - 1 ; Index++)
    {
        if (AnimationTime < RotationKeys.TimeStamps[Index + 1])
        {
            return Index;
        }
    }
    
    return RotationKeys.NumKeys - 1;
}

static void CalculateInterpolatedPosition(const vec3_keys& PositionKeys, math::v3& OutPosition, r32 AnimationTime)
{
    if(PositionKeys.NumKeys == 1)
    {
        OutPosition = PositionKeys.Values[0];
        return;
    }
    
    i32 PositionIndex = FindAnimationPosition(PositionKeys, AnimationTime);
    i32 NextPositionIndex = PositionIndex + 1;
    
    if(NextPositionIndex >= PositionKeys.NumKeys)
        NextPositionIndex = 0;
    
    r32 DeltaTime = PositionKeys.TimeStamps[NextPositionIndex] - PositionKeys.TimeStamps[PositionIndex];
    
    r32 Factor = (AnimationTime - (r32)PositionKeys.TimeStamps[PositionIndex]) / Abs(DeltaTime);
    
    if(Factor > 1.0f)
        Factor = 1.0f;
    
    Assert(Factor >= 0.0f && Factor <= 1.0f);
    
    math::v3& Start = PositionKeys.Values[PositionIndex];
    math::v3& End = PositionKeys.Values[NextPositionIndex];
    math::v3 Delta = End - Start;
    OutPosition = Start + Factor * Delta;
}

static void CalculateInterpolatedScaling(const vec3_keys& ScalingKeys, math::v3& OutScaling, r32 AnimationTime)
{
    if(ScalingKeys.NumKeys == 1)
    {
        OutScaling = ScalingKeys.Values[0];
        return;
    }
    
    i32 Index = FindAnimationPosition(ScalingKeys, AnimationTime);
    i32 NextIndex = Index + 1;
    
    if(NextIndex >= ScalingKeys.NumKeys)
        NextIndex = 0;
    
    r32 DeltaTime = ScalingKeys.TimeStamps[NextIndex] - ScalingKeys.TimeStamps[Index];
    
    r32 Factor = (AnimationTime - (r32)ScalingKeys.TimeStamps[Index]) / Abs(DeltaTime);
    
    if(Factor > 1.0f)
        Factor = 1.0f;
    
    Assert(Factor >= 0.0f && Factor <= 1.0f);
    
    math::v3& Start = ScalingKeys.Values[Index];
    math::v3& End = ScalingKeys.Values[NextIndex];
    math::v3 Delta = End - Start;
    OutScaling = Start + Factor * Delta;
}

static void CalculateInterpolatedRotation(const quat_keys& RotationKeys, math::quat& OutRotation, r32 AnimationTime)
{
    if(RotationKeys.NumKeys == 1)
    {
        OutRotation = RotationKeys.Values[0];
        return;
    }
    
    i32 Index = FindAnimationRotation(RotationKeys, AnimationTime);
    i32 NextIndex = Index + 1;
    
    if(NextIndex >= RotationKeys.NumKeys)
        NextIndex = 0;
    
    r32 DeltaTime = RotationKeys.TimeStamps[NextIndex] - RotationKeys.TimeStamps[Index];
    
    r32 Factor = (AnimationTime - (r32)RotationKeys.TimeStamps[Index]) / Abs(DeltaTime);
    
    if(Factor > 1.0f)
        Factor = 1.0f;
    
    Assert(Factor >= 0.0f && Factor <= 1.0f);
    
    math::quat& Start = RotationKeys.Values[Index];
    math::quat& End = RotationKeys.Values[NextIndex];
    
    OutRotation = Nlerp(Start, End, Factor);
}

static b32 FindBoneChannel(i32 BoneIndex, const skeletal_animation& Animation, bone_channel& Out)
{
    for(i32 ChannelIndex = 0; ChannelIndex < Animation.NumBoneChannels; ChannelIndex++)
    {
        if(Animation.BoneChannels[ChannelIndex].BoneIndex == BoneIndex)
        {
            Out = Animation.BoneChannels[ChannelIndex];
            return true;
        }
    }
    return false;
}

static void CalculateThroughBones(const skeletal_animation& Animation, r32 AnimationTime, bone* Bones, math::m4* Transforms, i32 NumBones, u32 BoneIndex, math::m4 ParentTransform, math::m4 GlobalInverseTransform)
{
    // @Incomplete: Animation stuff!
    bone Bone = Bones[BoneIndex];
    math::m4 BoneTransformation = Bone.Transformation;
    
    bone_channel BoneChannel;
    
    if (FindBoneChannel(BoneIndex, Animation, BoneChannel))
    {
        math::v3 Translation;
        CalculateInterpolatedPosition(BoneChannel.PositionKeys, Translation, AnimationTime);
        math::m4 TranslationMatrix(1.0f);
        TranslationMatrix = math::Translate(TranslationMatrix, Translation);
        
        math::v3 Scaling;
        CalculateInterpolatedScaling(BoneChannel.ScalingKeys, Scaling, AnimationTime);
        math::m4 ScalingMatrix(1.0f);
        ScalingMatrix = math::Scale(ScalingMatrix, Scaling);
        
        math::quat Rotation;
        CalculateInterpolatedRotation(BoneChannel.RotationKeys, Rotation, AnimationTime);
        math::m4 RotationMatrix = ToMatrix(Rotation);
        
        BoneTransformation = TranslationMatrix * RotationMatrix * ScalingMatrix;
    }
    
    math::m4 GlobalTransform = ParentTransform * BoneTransformation;
    Transforms[BoneIndex] = GlobalInverseTransform * GlobalTransform * Bone.BoneOffset;
    
    for(i32 ChildIndex = 0; ChildIndex < Bone.ChildCount; ChildIndex++)
    {
        CalculateThroughBones(Animation, AnimationTime, Bones, Transforms, NumBones, Bone.Children[ChildIndex], GlobalTransform, GlobalInverseTransform);
    }
}

static void CalculateBoneTransformsForAnimation(const skeletal_animation& Animation, r32 AnimationTime, bone* Bones, math::m4* Transforms, i32 NumBones, math::m4 GlobalInverseTransform)
{
    math::m4 Identity(1.0f);
    
    i32 RootIndex = 0; // @Speed: We have to set this when exporting the model in the first place
    for(i32 Index = 0; Index < NumBones; Index++)
    {
        if(Bones[Index].ParentId == -1)
        {
            RootIndex = Index;
            break;
        }
    }
    
    CalculateThroughBones(Animation, AnimationTime, Bones, Transforms, NumBones, RootIndex, Identity, GlobalInverseTransform);
}

static void PlayAnimation(i32 Index, model& Model, b32 Loop = false)
{
    Model.RunningAnimationIndex = Index;
    Model.AnimationState.Playing = true;
    Model.AnimationState.Loop = Loop;
}

static void PlayAnimation(const char* AnimationName, model& Model, b32 Loop = false)
{
    for(i32 AnimationIndex = 0; AnimationIndex < Model.AnimationCount; AnimationIndex++)
    {
        if(strcmp(AnimationName, Model.Animations[AnimationIndex].Name) == 0)
        {
            PlayAnimation(AnimationIndex, Model, Loop);
            break;
        }
    }
}

static void TickAnimation(model& Model, r32 DeltaTime)
{
    skeletal_animation& Animation = Model.Animations[Model.RunningAnimationIndex];
    
    if(Model.AnimationState.Playing)
    {
        Model.CurrentPoses = PushTempSize(sizeof(math::m4) * Model.BoneCount, math::m4);
        CalculateBoneTransformsForAnimation(Animation, Model.AnimationState.CurrentTime, Model.Bones, Model.CurrentPoses, Model.BoneCount, Model.GlobalInverseTransform);
        
        Model.AnimationState.CurrentTime += DeltaTime;
        
        if(Model.AnimationState.CurrentTime >= Animation.Duration)
        {
            Model.AnimationState.CurrentTime = 0.0f;
            
            if(!Model.AnimationState.Loop)
            {
                Model.AnimationState.Playing = false;
            }
        }
    }
}