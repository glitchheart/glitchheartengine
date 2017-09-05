#ifndef MODEL_FORMAT_H
#define MODEL_FORMAT_H

struct model_header
{
    char Format[4]; // M O D L ?
    char Version[4];
};

struct mesh_data
{
    i32 BaseVertex;
    i32 BaseIndex;
    i32 MaterialIndex;
    i32 NumIndices;
};

struct model_data
{
    i32 NumMeshes;
    i32 NumVertices;
    i32 NumIndices;
    i32 NumBones;
    i32 NumMaterials;
    
    long MeshChunkSize;
    long VertexBufferChunkSize;
    long IndexBufferChunkSize;
    long BoneChunkSize;
    
    math::m4 GlobalInverseTransform;
};

struct bone_header
{
    long NumBones;
    long BoneChunkSize;
};

struct animation_header
{
    i32 NumAnimations;
};

struct animation_channel_header
{
    i32 NumBoneChannels;
};

struct bone_animation_header
{
    i32 BoneIndex;
    i32 NumPositionChannels;
    i32 NumRotationChannels;
    i32 NumScalingChannels;
};

struct mesh_header
{
    long NumVertices;
    long VertexChunkSize;
    long NumNormals;
    long NumUVs;
    long NormalsChunkSize;
    long NumFaces;
    long FacesChunkSize;
    bool HasTexture;
    char TextureFile[100];
};

struct vertex_attribute
{
    i32 Position;
    long Type;
    long Stride;
    long Offset;
};

struct mesh_data_info
{
    long IndexBufferTarget; // If it's GL_UNSIGNED_SHORT etc.
    i32 IndexCount;
    long IndexBufferByteLength;
    long VertexBufferByteLength;
    
    b32 HasSkin;
    //i32 VertexAttributeCount;
    // @Incomplete: Skinning and animation
    
    //b32 HasTexture; // @Incomplete: Load these at the same time as the buffers
    //char TextureName[100];
};

struct chunk_format
{
    char Format[4]; // M E S H / E O F / B O N E / A N I M
};

#endif
