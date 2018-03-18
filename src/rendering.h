#ifndef RENDERING_H
#define RENDERING_H

#define PIXELS_PER_UNIT 32
#define MAX_MESHES 60

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
    math::v3 padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    Spotlight spotlights[MAX_LIGHTS];
};

struct DirectionalLightData // GLSL: 96, x64: 68 -> 96 - 68 = 24
{
    i32 num_lights; // GLSL: 16, x64: 4: We need 12 bytes of padding
    math::v3 padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    DirectionalLight directional_lights[MAX_LIGHTS];
};

struct PointLightData
{
    i32 num_lights; // GLSL: 16, x64: 4: We need 12 bytes of padding
    math::v3 padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    PointLight point_lights[MAX_LIGHTS];
};

enum Shader_Type
{
    Shader_Texture,
    Shader_Tile,
    Shader_Rect,
    Shader_TextureRect,
    Shader_StandardFont,
    Shader_Spritesheet,
    Shader_Wireframe,
    Shader_UISprite,
    Shader_ErrorSprite,
    Shader_ErrorUI,
    Shader_FrameBuffer,
    Shader_LightSource,
    Shader_SimpleModel,
    Shader_Passthrough,
    Shader_Line,
    
    Shader_Count
};

enum RenderCommandType
{
    RenderCommand_Line,
    RenderCommand_Text,
    RenderCommand_Sprite,
    RenderCommand_Quad,
    
    RenderCommand_Spotlight,
    RenderCommand_DirectionalLight,
    RenderCommand_PointLight,
    
    RenderCommand_Buffer,
    RenderCommand_Model,
    RenderCommand_WireframeCube,
    RenderCommand_ShaderStart,
    RenderCommand_ShaderEnd,
    RenderCommand_DepthTest,
    RenderCommand_Count
};

enum Alignment
{
    Alignment_Left,
    Alignment_Right,
    Alignment_Center
};

struct Shader
{
    Shader_Type type;
    b32 loaded;
    u32 program;
    u32 vertex_shader;
    u32 fragment_shader;
};

struct TextureInfo
{
    b32 has_data;
    char texture_name[50];
    i32 texture_handle;
};

struct Material
{
    TextureInfo diffuse_texture;
};

struct Vec3Keys
{
    i32 num_keys;
    r32* time_stamps;
    math::v3* values;
};

struct QuatKeys
{
    i32 num_keys;
    r32* time_stamps;
    math::quat* values;
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
    
    math::m4 transformation;
    math::m4 bone_offset;
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
    Model_Static,
    Model_Skinned
};

struct Model
{
    ModelType type;
    
    i32 buffer_handle;
    
    math::v3 position;
    math::v3 scale;
    math::quat orientation;
    
    math::rgba color;
    
    Material materials[10];
    i32 material_count;
    
    MeshData meshes[MAX_MESHES];
    i32 mesh_count;
    
    Bone* bones;
    i32 bone_count;
    
    SkeletalAnimationState animation_state;
    i32 running_animation_index;
    math::m4* current_poses;
    SkeletalAnimation* animations;
    i32 animation_count;
    
    math::m4 global_inverse_transform;
};

enum ShaderAttributeType
{
    Attribute_Float,
    Attribute_Float2,
    Attribute_Float3,
    Attribute_Float4,
    Attribute_Integer,
    Attribute_Boolean,
    Attribute_Matrix4
};

struct ShaderAttribute
{
    ShaderAttributeType type;
    char* name;
    union
    {
        r32 float_var;
        math::v2 float2_var;
        math::v3 float3_var;
        math::v4 float4_var;
        i32 integer_var;
        b32 boolean_var;
        math::m4 matrix4_var;
    };
};

struct RenderCommand
{
    RenderCommandType type;
    Shader_Type shader_type;
    b32 is_ui;
    
    math::v3 position;
    math::v3 scale;
    math::v3 rotation;
    math::quat orientation;
    b32 with_origin;
    math::v2 origin;
    math::rgba color;
    i32 shader_handle;
    ShaderAttribute* shader_attributes;
    i32 shader_attribute_count;
    
    union
    {
        struct
        {
            math::v3 point1;
            math::v3 point2;
            r32 line_width;
            math::rgba color; // @Cleanup: REMOVE!
        } line;
        struct
        {
            char text[256];
            math::v3 position;
            i32 font_handle;
            math::rgba color; // @Cleanup: REMOVE!
            Alignment alignment;
            r32 scale;
        } text;
        struct
        {
            math::rgba color;
        } sprite;
        struct
        {
            i32 texture_handle;
            b32 flipped;
            math::rgba color; // @Cleanup: REMOVE!
            b32 outlined;
            r32 line_width;
            b32 for_animation;
            math::v2 texture_size;
            math::v2i frame_size;
            math::v2 texture_offset;
        } quad;
        struct
        {
            math::rgba color; // @Cleanup: REMOVE!
            r32 line_width;
        } wireframe_cube;
        struct
        {
            math::v3 direction;
            r32 cut_off;
            r32 outer_cut_off;
            math::v3 ambient;
            math::v3 diffuse;
            math::v3 specular;
            
            r32 constant;
            r32 linear;
            r32 quadratic;
        } spotlight;
        struct 
        {
            math::v3 direction;
            math::v3 ambient;
            math::v3 diffuse;
            math::v3 specular;
        } directional_light;
        struct
        {
            math::v3 ambient;
            math::v3 diffuse;
            math::v3 specular;
            
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
            Material materials[10];
            i32 material_count;
            math::rgba color;
            math::m4* bone_transforms;
            i32 bone_count;
        } model;
        struct
        {
            i32 handle;
            ShaderAttribute* attributes;
            i32 attribute_count;
        } shader;
        struct
        {
            b32 on;
        } depth_test;
    };
    RenderCommand() {}
};

enum FadingMode
{
    Fading_None,
    Fading_In,
    Fading_Out,
    Fading_OutIn
};

struct Camera
{
    i32 viewport_width;
    i32 viewport_height;
    r32 zoom;
    math::v3 center;
    math::v3 position;
    math::quat orientation;
    math::v3 target;
    
    r32 follow_speed;
    math::m4 view_matrix;
    math::m4 projection_matrix;
    
    FadingMode fading_mode = Fading_None;
    math::v3 fading_tint;
    
    b32 fading_in;
    r32 end_alpha;
    r32 fading_alpha = 0.0f;
    r32 fading_speed;
};

#define RENDER_COMMAND_MAX 400
#define BUFFER_ARRAY_SIZE 400
#define TEXTURE_ARRAY_SIZE 512
#define SHADER_ARRAY_SIZE 128

struct texture_data
{
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

struct UiRenderInfo
{
    b32 rendered = true;
    
    i32 texture_handle;
    math::v2 texture_offset;
    math::v2 frame_size;
    u32 shader_index;
    math::v2 size = math::v2(1, 1);
    math::v4 color = math::v4(1, 1, 1, 1);
};

GENERIC_MAP(texture_data, texture_data*, char*, StrCmp, NULL, "%s", STR_ASSIGN, PTR_COPY);

struct BufferData
{
    r32* vertex_buffer;
    long vertex_buffer_size;
    u32* index_buffer;
    i32 index_buffer_count;
    long index_buffer_size;
    b32 has_normals;
    b32 has_u_vs;
    b32 skinned;
    
    Shader_Type shader_type;
    i32 existing_handle = -1;
};

struct Particle
{
    math::v3 center;
};

#define MAX_PARTICLES 256
struct ParticleSystem
{
    i32 particle_texture;
    Particle particles[MAX_PARTICLES];
    i32 particle_count;
    i32 rate; // Particles per second
    r32 particle_speed;
};

#define MAX_CAMERAS 8

#define MAX_ANIMATION_CONTROLLERS 64
#define MAX_RENDER_COMMANDS 2048
#define MAX_UI_COMMANDS 2048 // @Incomplete: This should be defined by the game itself (HARDCODED FOR LEVEL EDITOR RIGHT NOW)
#define MAX_LIGHT_COMMANDS 1024

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
    
    texture_data texture_data[TEXTURE_ARRAY_SIZE];
    i32 texture_count;
    
    i32 texture_handles[TEXTURE_ARRAY_SIZE];
    
    texture_data_map texture_map;
    
    ShaderData shader_data[SHADER_ARRAY_SIZE];
    i32 shader_count;
    
    Camera cameras[MAX_CAMERAS];
    i32 current_camera_handle;
    
    AnimationController* animation_controllers;
    i32 animation_controller_count;
    
    SpritesheetAnimation spritesheet_animations[MAX_SPRITESHEET_ANIMATIONS];
    i32 spritesheet_animation_count;
    
    union
    {
        i32 viewport[4];
        math::v4i v;
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
    
    math::rgba clear_color;
    r32 line_width;
    
    b32 show_mouse_cursor;
    
    FontData fonts[64];
    i32 font_count;
    
    MemoryArena texture_arena;
    MemoryArena animation_arena;
    MemoryArena font_arena;
};

#endif
