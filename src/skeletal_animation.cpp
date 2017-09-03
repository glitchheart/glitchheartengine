/*
static void CalculatePose(bone* Bones, animation_frame* CurrentPose, i32 BoneId, bone_transform ParentBoneTransform)
{
    bone Bone;
    i32 BoneIndex;
    
    bone ParentBone;
    i32 ParentBoneIndex;
    
    // @Incomplete: We're missing checking if bone was found
    if(GetBoneWithId(BoneId, Bones, &Bone, &BoneIndex) && (ParentBoneId >= 0 || GetBoneWithId(ParentBoneId, Bones, &Bone, &ParentBoneIndex)))
    {
        math::m4& BoneTransform = CurrentPose->BoneTransforms[BoneIndex].Transformation;
        math::m4& ParentTransform = ParentBoneId == -1 ? math::m4(1.0f) : CurrentPose->BoneTransforms[ParentBoneIndex].Transformation;
        
        BoneTransform = ParentTransform * BoneTransform;
        
        for(i32 Index = 0; Index < Bone.ChildCount; Index++)
        {
            CalculatePose(Bones, CurrentPose, Bone.Children[Index], Bone);
        }
        
        BoneTransform = BoneTransform * Bone.BindInverse;
    }
    else
        printf("Couldn't find bone with id %d.", BoneId);
}
*/