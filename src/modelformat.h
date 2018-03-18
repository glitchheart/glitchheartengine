#ifndef MODEL_FORMAT_H
#define MODEL_FORMAT_H

struct ModelHeader
{
    char format[4]; // M O D L ?
    char version[4];
};

struct ModelData
{
    i32 model_type;
    
    i32 num_meshes;
    i32 num_vertices;
    i32 num_indices;
    i32 num_bones;
    i32 num_materials;
    
    b32 has_normals;
    b32 has_uvs;
    
    long mesh_chunk_size;
    long vertex_buffer_chunk_size;
    long index_buffer_chunk_size;
    long material_chunk_size;
    long bone_chunk_size;
    
    math::Mat4 global_inverse_transform;
};

struct BoneHeader
{
    long num_bones;
    long bone_chunk_size;
};

struct AnimationHeader
{
    i32 num_animations;
};

struct AnimationChannelHeader
{
    r32 duration;
    i32 num_bone_channels;
};

struct BoneAnimationHeader
{
    i32 bone_index;
    i32 num_position_channels;
    i32 num_rotation_channels;
    i32 num_scaling_channels;
};

struct MeshHeader
{
    long num_vertices;
    long vertex_chunk_size;
    long num_normals;
    long num_u_vs;
    long normals_chunk_size;
    long num_faces;
    long faces_chunk_size;
    bool has_texture;
    char texture_file[100];
};

struct VertexAttribute
{
    i32 position;
    long type;
    long stride;
    long offset;
};

struct MeshDataInfo
{
    long index_buffer_target; // If it's GL_UNSIGNED_SHORT etc.
    i32 index_count;
    long index_buffer_byte_length;
    long vertex_buffer_byte_length;
    
    b32 has_skin;
    //i32 VertexAttributeCount;
    // @Incomplete: Skinning and animation
    
    //b32 HasTexture; // @Incomplete: Load these at the same time as the buffers
    //char TextureName[100];
};

struct ChunkFormat
{
    char format[4]; // M E S H / E O F / B O N E / A N I M
};

#endif
