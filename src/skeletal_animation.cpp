static void CalculateThroughBones(const skeletal_animation& Animation, r32 Time, bone* Bones, math::m4* Transforms, i32 NumBones, i32 BoneIndex, math::m4 ParentTransform, math::m4 GlobalInverseTransform)
{
    // @Incomplete: Animation stuff!
    bone Bone = Bones[BoneIndex];
    math::m4 BoneTransform = Bone.Transformation;
    
    math::m4 GlobalTransform = ParentTransform * BoneTransform;
    Transforms[BoneIndex] = GlobalInverseTransform * GlobalTransform * Bone.BoneOffset;
    
    for(i32 ChildIndex = 0; ChildIndex < Bone.ChildCount; ChildIndex++)
    {
        CalculateThroughBones(Animation, Time, Bones, Transforms, NumBones, Bone.Children[ChildIndex], GlobalTransform, GlobalInverseTransform);
    }
}

static void CalculateBoneTransformsForAnimation(const skeletal_animation& Animation, r32 Time, bone* Bones, math::m4* Transforms, i32 NumBones, math::m4 GlobalInverseTransform)
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
    
    CalculateThroughBones(Animation, 0.0f, Bones, Transforms, NumBones, RootIndex, Identity, GlobalInverseTransform);
}