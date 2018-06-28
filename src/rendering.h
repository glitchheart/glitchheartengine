#ifndef RENDERING_H
#define RENDERING_H

#define PIXELS_PER_UNIT 32
#define MAX_MESHES 64
#define MAX_PARTICLE_SYSTEM 64

#define MAX_LIGHTS 150
#define MAX_BONES 50
#define MAX_CHILDREN 30
#define MAX_SPRITESHEET_ANIMATIONS 128
#define MAX_SPRITESHEET_ANIMATION_INFOS 256

struct FontData
{
    char* path;
    i32 size;
    char* name;
};

struct Spotlight
{
    r32 position[4];
    r32 direction[4];
    
    r32 cut_off;
    r32 outer_cut_off;
    r32 p; // Padding
    r32 p1; // Padding
    
    r32 ambient[4];
    r32 diffuse[4];
    r32 specular[4];
    
    r32 constant;
    r32 linear;
    r32 quadratic;
    r32 p3; // Padding
};

struct DirectionalLight
{
    r32 direction[4];
    r32 ambient[4];
    r32 diffuse[4];
    r32 specular[4];
};

struct PointLight
{
    r32 position[4];
    
    r32 constant;
    r32 linear;
    r32 quadratic;
    r32 padding;
    
    r32 ambient[4];
    r32 diffuse[4];
    r32 specular[4];
};

struct SpotlightData
{
    i32 num_lights; // GLSL: 16, x64: 4: We need 12 bytes of padding
    math::Vec3 padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    Spotlight spotlights[MAX_LIGHTS];
};

struct DirectionalLightData // GLSL: 96, x64: 68 -> 96 - 68 = 24
{
    i32 num_lights; // GLSL: 16, x64: 4: We need 12 bytes of padding
    math::Vec3 padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    DirectionalLight directional_lights[MAX_LIGHTS];
};

struct PointLightData
{
    i32 num_lights; // GLSL: 16, x64: 4: We need 12 bytes of padding
    math::Vec3 padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    PointLight point_lights[MAX_LIGHTS];
};

enum ShaderType
{
    SHADER_MESH,
    SHADER_MESH_INSTANCED,
    SHADER_DEPTH,
    SHADER_DEPTH_INSTANCED,
    SHADER_QUAD,
    SHADER_TEXTURE_QUAD,
    SHADER_STANDARD_FONT,
    SHADER_SPRITESHEET,
    SHADER_FRAME_BUFFER,
    SHADER_SIMPLE_MODEL,
    SHADER_LINE,
    SHADER_PARTICLES,
    SHADER_COUNT
};

enum RenderCommandType
{
    RENDER_COMMAND_LINE,
    RENDER_COMMAND_TEXT,
    RENDER_COMMAND_QUAD,
    
    RENDER_COMMAND_SPOTLIGHT,
    RENDER_COMMAND_DIRECTIONAL_LIGHT,
    RENDER_COMMAND_POINT_LIGHT,
    
    RENDER_COMMAND_BUFFER,
    RENDER_COMMAND_MODEL,
    RENDER_COMMAND_MESH,
    RENDER_COMMAND_MESH_INSTANCED,
    RENDER_COMMAND_SHADER_START,
    RENDER_COMMAND_SHADER_END,
    RENDER_COMMAND_DEPTH_TEST,
    RENDER_COMMAND_PARTICLES,
    RENDER_COMMAND_COUNT
};

enum Alignment
{
    ALIGNMENT_LEFT = (1 << 0),
    ALIGNMENT_RIGHT = (1 << 1),
    ALIGNMENT_CENTER_X = (1 << 2),
    ALIGNMENT_CENTER_Y = (1 << 4)
};

struct Shader
{
    ShaderType type;
    b32 loaded;
    u32 program;
    u32 vertex_shader;
    u32 fragment_shader;
    u32 geometry_shader; // Optional
};

struct VertexInfo
{
    math::Vec3 position;
    math::Vec2 uv;
    math::Vec3 normal;
    math::Rgba color;
};

r32 plane_vertices[] =
{
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f,  1.0f,  
    1.0f, 0.0f,  1.0f,  
    0.0f, 0.0f,  1.0f, 
    0.0f, 0.0f, 0.0f
};

u16 plane_indices[] = 
{
    0, 1, 2,
    3, 4, 5
};

r32 plane_normals[] =
{
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f
};

r32 plane_uvs[] =
{
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f, 
    0.0f, 1.0f, 
    0.0f, 0.0f
};

r32 cube_normals[] =
{
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f
};

r32 cube_vertices[] = {
    0.0f, 0.0f, 0.0f,  
    1.0f, 0.0f, 0.0f,  
    1.0f,  1.0f, 0.0f, 
    1.0f,  1.0f, 0.0f, 
    0.0f,  1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    
    0.0f, 0.0f,  1.0f,
    1.0f, 0.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  
    1.0f,  1.0f,  1.0f, 
    0.0f,  1.0f,  1.0f,
    0.0f, 0.0f,  1.0f, 
    
    0.0f,  1.0f,  1.0f,
    0.0f,  1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f,  1.0f,
    0.0f,  1.0f,  1.0f,
    
    1.0f,  1.0f,  1.0f, 
    1.0f,  1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f,  1.0f,  
    1.0f, 0.0f,  1.0f,  
    0.0f, 0.0f,  1.0f, 
    0.0f, 0.0f, 0.0f, 
    
    0.0f,  1.0f, 0.0f, 
    1.0f,  1.0f, 0.0f, 
    1.0f,  1.0f,  1.0f, 
    1.0f,  1.0f,  1.0f,  
    0.0f,  1.0f,  1.0f, 
    0.0f,  1.0f, 0.0f,  
};

r32 cube_uvs[] =
{
    0.000059f, 1.0f - 0.000004f,
    0.000103f, 1.0f - 0.336048f,
    0.335973f, 1.0f - 0.335903f,
    1.000023f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    0.999958f, 1.0f - 0.336064f,
    
    0.667979f, 1.0f - 0.335851f,
    0.336024f, 1.0f - 0.671877f,
    0.667969f, 1.0f - 0.671889f,
    1.000023f, 1.0f - 0.000013f,
    0.668104f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    
    0.000059f, 1.0f - 0.000004f,
    0.335973f, 1.0f - 0.335903f,
    0.336098f, 1.0f - 0.000071f,
    0.667979f, 1.0f - 0.335851f,
    0.335973f, 1.0f - 0.335903f,
    0.336024f, 1.0f - 0.671877f,
    
    1.000004f, 1.0f - 0.671847f,
    0.999958f, 1.0f - 0.336064f,
    0.667979f, 1.0f - 0.335851f,
    0.668104f, 1.0f - 0.000013f,
    0.335973f, 1.0f - 0.335903f,
    0.667979f, 1.0f - 0.335851f,
    
    0.335973f, 1.0f - 0.335903f,
    0.668104f, 1.0f - 0.000013f,
    0.336098f, 1.0f - 0.000071f,
    0.000103f, 1.0f - 0.336048f,
    0.000004f, 1.0f - 0.671870f,
    0.336024f, 1.0f - 0.671877f,
    
    0.000103f, 1.0f - 0.336048f,
    0.336024f, 1.0f - 0.671877f,
    0.335973f, 1.0f - 0.335903f,
    0.667969f, 1.0f - 0.671889f,
    1.000004f, 1.0f - 0.671847f,
    0.667979f, 1.0f - 0.335851f
};

u16 cube_indices[] = 
{
    0, 1, 2,
    3, 4, 5,
    6, 7, 8,
    9, 10, 11,
    12, 13, 14,
    15, 16, 17,
    
    18, 19, 20,
    21, 22, 23,
    24, 25, 26,
    27, 28, 29,
    30, 31, 32,
    33, 34, 35
};

r32 tetrahedron_vertices[] =
{
    // Bottom
    -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    -0.5f, 0.0f, -1.0f,
    
    //Sides
    -0.5f, 1.0f, -0.5f,
    -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    
    -0.5f, 1.0f, -0.5f,
    0.0f, 0.0f,  0.0f,
    -0.5f, 0.0f, -1.0f,
    
    -0.5f, 1.0f, -0.5f,
    -1.0f, 0.0f, 0.0f,
    -0.5f, 0.0f, -1.0f
};

u16 tetrahedron_indices[] =
{
    0, 1, 2, 
    3, 4, 5,
    6, 7, 8,
    9, 10, 11
};

r32 tetrahedron_normals[] =
{
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    
    -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f
};

struct Vertex
{
    math::Vec3 position;
    math::Vec2 uv;
    math::Vec3 normal;
    math::Rgba color;
};

struct Face
{
    u16 indices[3];
    math::Vec3 normal;
};

enum RenderMaterialType
{
    RM_INVALID,
    RM_COLOR,
    RM_TEXTURED
};

struct Mesh
{
    i32 buffer_handle;
    
    Vertex* vertices;
    i32 vertex_count;
    
    Face* faces;
    i32 face_count;
};

struct TextureInfo
{
    b32 has_data;
    char texture_name[50];
    i32 texture_handle;
};

struct Vec3Keys
{
    i32 num_keys;
    r32* time_stamps;
    math::Vec3* values;
};

struct QuatKeys
{
    i32 num_keys;
    r32* time_stamps;
    math::Quat* values;
};

struct BoneChannel
{
    i32 bone_index;
    Vec3Keys position_keys;
    QuatKeys rotation_keys;
    Vec3Keys scaling_keys;
};

struct SkeletalAnimation
{
    char* name;
    r32 duration;
    i32 num_bone_channels;
    BoneChannel* bone_channels;
};

struct SkeletalAnimationState
{
    b32 playing;
    b32 loop;
    r32 current_time;
};

struct Bone
{
    char name[30];
    i32 parent_id;
    u32 children[MAX_CHILDREN];
    i32 child_count;
    
    math::Mat4 transformation;
    math::Mat4 bone_offset;
};

struct MeshData
{
    i32 base_vertex;
    i32 base_index;
    i32 material_index;
    i32 num_indices;
};

enum ModelType
{
    MODEL_STATIC,
    MODEL_SKINNED
};

struct TransformInfo
{ 
    math::Vec3 position;
    math::Vec3 scale;
    math::Vec3 rotation;
};

enum ShaderAttributeType
{
    ATTRIBUTE_FLOAT,
    ATTRIBUTE_FLOAT2,
    ATTRIBUTE_FLOAT3,
    ATTRIBUTE_FLOAT4,
    ATTRIBUTE_INTEGER,
    ATTRIBUTE_BOOLEAN,
    ATTRIBUTE_MATRIX4
};

struct ShaderAttribute
{
    ShaderAttributeType type;
    char name[32];
    union
    {
        r32 float_var;
        math::Vec2 float2_var;
        math::Vec3 float3_var;
        math::Vec4 float4_var;
        i32 integer_var;
        b32 boolean_var;
        math::Mat4 matrix4_var;
    };
};

struct ShaderInfo
{
    i32 shader_handle;
    ShaderAttribute* shader_attributes;
    i32 shader_attribute_count;
};

struct RenderMaterial
{
    ShaderInfo shader;
    math::Rgba color;
    
    RenderMaterialType type;
    
    union
    {
        i32 diffuse_texture;
    };
};

struct Model
{
    ModelType type;
    
    i32 buffer_handle;
    
    math::Vec3 position;
    math::Vec3 scale;
    math::Quat orientation;
    
    math::Rgba color;
    
    RenderMaterial materials[10];
    i32 material_count;
    
    MeshData meshes[MAX_MESHES];
    i32 mesh_count;
    
    Bone* bones;
    i32 bone_count;
    
    SkeletalAnimationState animation_state;
    i32 running_animation_index;
    math::Mat4* current_poses;
    SkeletalAnimation* animations;
    i32 animation_count;
    
    math::Mat4 global_inverse_transform;
};

enum WireframeType
{
    WT_NONE,
    WT_WITH_MESH,
    WT_WITHOUT_MESH
};

struct MeshInfo
{
    i32 mesh_handle;
    i32 instance_buffer_handle;
    TransformInfo transform;
    RenderMaterial material;
    
    WireframeType wireframe_type;
    math::Rgba wireframe_color;
    
    b32 cast_shadows;
};

struct Particle
{
    math::Vec3 position;
    math::Vec3 speed;
    math::Rgba color;
    r32 size;
    r64 life;
};

struct ParticleSystemInfo
{
    i32 system_handle;
    
    b32 running;
    b32 one_shot;
    b32 emitting;
    math::Vec3 direction;
    r32 spread;
    r32 size;
    math::Rgba color;
    r64 life_time;
    i32 particles_per_second;
    r32 speed_multiplier;
    
    i32 offset_buffer_handle;
    i32 color_buffer_handle;
    i32 size_buffer_handle;
    TransformInfo transform;
    RenderMaterial material;
    
    Particle *particles;
    i32 particle_count;
	i32 total_emitted;
	i32 last_used_particle;
    i32 max_particles;
    
    math::Vec3 *offsets;
    math::Vec4 *colors;
    r32 *sizes;
    
    i32 texture_handle;
};

struct RenderInfo
{
    b32 is_ui;
    
    b32 with_origin;
    math::Vec2 origin;
    math::Rgba color;
};

struct QuadTextureInfo
{
    i32 texture_handle;
    math::Vec2i frame_size;
    math::Vec2 texture_offset;
};

struct QuadInfo
{
    TransformInfo transform_info;
    ShaderInfo shader_info;
    RenderInfo render_info;
    QuadTextureInfo texture_info;
    
    b32 flipped;
    i32 animation_controller_handle;
};

struct TextInfo
{
    math::Vec3 position;
    RenderInfo render_info;
    
    i32 font_handle;
    u64 alignment_flags;
    char* text;
    r32 scale;
};

struct RenderCommand
{
    RenderCommandType type;
    ShaderType shader_type;
    b32 is_ui;
    
    math::Vec3 position;
    math::Vec3 scale;
    math::Vec3 rotation;
    math::Quat orientation;
    b32 with_origin;
    math::Vec2 origin;
    math::Rgba color;
    
    i32 shader_handle;
    ShaderAttribute* shader_attributes;
    i32 shader_attribute_count;
    
    b32 cast_shadows;
    
    union
    {
        struct
        {
            math::Vec3 point1;
            math::Vec3 point2;
            r32 line_width;
            math::Rgba color; // @Cleanup: REMOVE!
        } line;
        struct
        {
            char text[256];
            math::Vec3 position;
            i32 font_handle;
            math::Rgba color; // @Cleanup: REMOVE!
            u64 alignment_flags;
            r32 scale;
        } text;
        struct
        {
            math::Rgba color;
        } sprite;
        struct
        {
            i32 texture_handle;
            b32 flipped;
            math::Rgba color; // @Cleanup: REMOVE!
            b32 outlined;
            r32 line_width;
            b32 for_animation;
            math::Vec2 texture_size;
            math::Vec2i frame_size;
            math::Vec2 texture_offset;
        } quad;
        struct
        {
            math::Vec3 direction;
            r32 cut_off;
            r32 outer_cut_off;
            math::Vec3 ambient;
            math::Vec3 diffuse;
            math::Vec3 specular;
            
            r32 constant;
            r32 linear;
            r32 quadratic;
        } spotlight;
        struct 
        {
            math::Vec3 direction;
            math::Vec3 ambient;
            math::Vec3 diffuse;
            math::Vec3 specular;
        } directional_light;
        struct
        {
            math::Vec3 ambient;
            math::Vec3 diffuse;
            math::Vec3 specular;
            
            r32 constant;
            r32 linear;
            r32 quadratic;
        } point_light;
        struct
        {
            i32 buffer_handle;
            i32 texture_handle;
        } buffer;
        struct
        {
            ModelType type;
            i32 buffer_handle;
            MeshData meshes[MAX_MESHES];
            i32 mesh_count;
            RenderMaterial materials[10];
            i32 material_count;
            math::Rgba color;
            math::Mat4* bone_transforms;
            i32 bone_count;
        } model;
        struct
        {
            i32 buffer_handle;
            RenderMaterialType material_type;
            i32 diffuse_texture;
            WireframeType wireframe_type;
            math::Rgba wireframe_color;
        } mesh;
        struct
        {
            i32 buffer_handle;
            i32 instance_buffer_handle;
            RenderMaterialType material_type;
            i32 diffuse_texture;
            WireframeType wireframe_type;
            math::Rgba wireframe_color;
            math::Vec3 *offsets;
            i32 offset_count;
        } mesh_instanced;
        struct
        {
            b32 on;
        } depth_test;
        struct
        {
            i32 buffer_handle;
            i32 offset_buffer_handle;
            i32 color_buffer_handle;
            i32 size_buffer_handle;
            RenderMaterialType material_type;
            i32 diffuse_texture;
            
            i32 particle_count;
            math::Vec3 *offsets;
            math::Vec4 *colors;
            r32 *sizes;
            i32 texture_handle;
        } particles;
    };
    RenderCommand() {}
};

enum FadingMode
{
    FADING_NONE,
    FADING_IN,
    FADING_OUT,
    FADING_OUT_IN
};

struct Camera
{
    i32 viewport_width;
    i32 viewport_height;
    r32 zoom;
    math::Vec3 center;
    math::Vec3 position;
    math::Quat orientation;
    math::Vec3 target;
    
    r32 follow_speed;
    math::Mat4 view_matrix;
    math::Mat4 projection_matrix;
    
    FadingMode fading_mode = FADING_NONE;
    math::Vec3 fading_tint;
    
    b32 fading_in;
    r32 end_alpha;
    r32 fading_alpha = 0.0f;
    r32 fading_speed;
};

#define RENDER_COMMAND_MAX 400
#define BUFFER_ARRAY_SIZE 400
#define TEXTURE_ARRAY_SIZE 512
#define SHADER_ARRAY_SIZE 128

enum TextureFiltering
{
    LINEAR,
    NEAREST
};

struct TextureData
{
    TextureFiltering filtering;
    i32 handle;
    char* name;
    i32 width;
    i32 height;
    unsigned char* image_data;
};

struct ShaderData
{
    i32 handle;
    char name[512];
    char* vertex_shader_content;
    char* fragment_shader_content;
};

struct UIRenderInfo
{
    b32 rendered = true;
    
    i32 texture_handle;
    math::Vec2 texture_offset;
    math::Vec2 frame_size;
    u32 shader_index;
    math::Vec2 size = math::Vec2(1, 1);
    math::Vec4 color = math::Vec4(1, 1, 1, 1);
};

GENERIC_MAP(TextureData, TextureData*, char*, StrCmp, NULL, "%s", STR_ASSIGN, PTR_COPY);

struct BufferData
{
    r32* vertex_buffer;
    i32 vertex_buffer_size;
    u16* index_buffer;
    i32 index_buffer_count;
    i32 index_buffer_size;
    b32 has_normals;
    b32 has_uvs;
    b32 skinned;
    
    ShaderType shader_type;
    i32 existing_handle = -1;
    
    b32 for_instancing;
    size_t instance_buffer_size;
};

#define MAX_CAMERAS 8

#define MAX_ANIMATION_CONTROLLERS 64
#define MAX_RENDER_COMMANDS 2048
#define MAX_UI_COMMANDS 2048 // @Incomplete: This should be defined by the game itself (HARDCODED FOR LEVEL EDITOR RIGHT NOW)
#define MAX_LIGHT_COMMANDS 1024

struct ShadowMapMatrices
{
    math::Mat4 depth_projection_matrix;
    math::Mat4 depth_model_matrix;
    math::Mat4 depth_view_matrix;
    math::Mat4 depth_bias_matrix;
};

struct Renderer
{
    b32 should_close;
    r64 fps;
    r64 average_fps;
    r64 fps_sum;
    u64 current_frame;
    i32 pixels_per_unit;
    i32 frame_lock;
    
    WindowMode window_mode;
    
    MemoryArena commands;
    i32 command_count;
    
    MemoryArena ui_commands;
    i32 ui_command_count;
    
    MemoryArena light_commands;
    i32 light_command_count;
    
    BufferData buffers[BUFFER_ARRAY_SIZE];
    i32 buffer_handles[BUFFER_ARRAY_SIZE];
    i32 buffer_count;
    
    i32 updated_buffer_handles[BUFFER_ARRAY_SIZE];
    i32 updated_buffer_handle_count;
    
    Mesh meshes[MAX_MESHES];
    i32 mesh_count;
    
    ParticleSystemInfo particle_systems[MAX_PARTICLE_SYSTEM];
    i32 particle_system_count;
    
    TextureData texture_data[TEXTURE_ARRAY_SIZE];
    i32 texture_count;
    
    i32 texture_handles[TEXTURE_ARRAY_SIZE];
    
    TextureData_map texture_map;
    
    ShaderData shader_data[SHADER_ARRAY_SIZE];
    i32 shader_count;
    
    // Shadow map
    ShadowMapMatrices shadow_map_matrices;
    
    math::Mat4 ui_projection_matrix;
    Camera cameras[MAX_CAMERAS];
    i32 current_camera_handle;
    
    AnimationController* animation_controllers;
    i32 animation_controller_count;
    
    SpritesheetAnimation spritesheet_animations[MAX_SPRITESHEET_ANIMATIONS];
    i32 spritesheet_animation_count;
    
    union
    {
        i32 viewport[4];
        math::Vec4i v;
        struct
        {
            i32 viewport_x;
            i32 viewport_y;
            i32 viewport_width;
            i32 viewport_height;
        };
    };
    
    i32 window_width;
    i32 window_height;
    
    r32 scale_x;
    r32 scale_y;
    
    math::Rgba clear_color;
    r32 line_width;
    
    b32 show_mouse_cursor;
    
    FontData fonts[64];
    i32 font_count;
    
    MemoryArena mesh_arena;
    MemoryArena texture_arena;
    MemoryArena animation_arena;
    MemoryArena font_arena;
};

#endif
