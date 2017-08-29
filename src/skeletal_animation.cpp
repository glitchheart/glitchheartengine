static void CalculatePose(bone* Bones, animation_frame* CurrentPose, i32 BoneIndex, i32 ParentBoneIndex)
{
    bone Bone = Bones[BoneIndex];
    math::m4& BoneTransform = CurrentPose->BoneTransforms[BoneIndex].Transformation;
    math::m4& ParentTransform = ParentBoneIndex == -1 ? math::m4(1.0f) : CurrentPose->BoneTransforms[ParentBoneIndex].Transformation;
    
    BoneTransform = ParentTransform * BoneTransform;
    
    for(i32 Index = 0; Index < Bone.ChildCount; Index++)
    {
        CalculatePose(Bones, CurrentPose, Bone.Children[Index], BoneIndex);
    }
    
    BoneTransform = BoneTransform * Bone.BindInverse;
}
