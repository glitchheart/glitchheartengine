#ifndef RENDERING_H
#define RENDERING_H

#define PIXELS_PER_UNIT 32
#define MAX_MESHES 60

#define MAX_LIGHTS 150
#define MAX_BONES 50
#define MAX_CHILDREN 30

enum Font_Type
{
    Font_Inconsolata,
    Font_InconsolataSmall,
    Font_Menu,
    Font_Button,
    Font_Roboto,
    Font_Title,
    
    Font_Count
};

struct spotlight
{
    r32 Position[4];
    r32 Direction[4];
    
    r32 CutOff;
    r32 OuterCutOff;
    r32 P; // Padding
    r32 P1; // Padding
    
    r32 Ambient[4];
    r32 Diffuse[4];
    r32 Specular[4];
    
    r32 Constant;
    r32 Linear;
    r32 Quadratic;
    r32 P3; // Padding
};

struct directional_light
{
    r32 Direction[4];
    r32 Ambient[4];
    r32 Diffuse[4];
    r32 Specular[4];
};

struct point_light
{
    r32 Position[4];
    
    r32 Constant;
    r32 Linear;
    r32 Quadratic;
    r32 Padding;
    
    r32 Ambient[4];
    r32 Diffuse[4];
    r32 Specular[4];
};

struct spotlight_data
{
    i32 NumLights; // GLSL: 16, x64: 4: We need 12 bytes of padding
    math::v3 Padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    spotlight Spotlights[MAX_LIGHTS];
};

struct directional_light_data // GLSL: 96, x64: 68 -> 96 - 68 = 24
{
    i32 NumLights; // GLSL: 16, x64: 4: We need 12 bytes of padding
    math::v3 Padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    directional_light DirectionalLights[MAX_LIGHTS];
};

struct point_light_data
{
    i32 NumLights; // GLSL: 16, x64: 4: We need 12 bytes of padding
    math::v3 Padding; // 3 * r32 = 3 * 4 = 12 bytes of padding!
    point_light PointLights[MAX_LIGHTS];
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
    Shader_AStarPath,
    Shader_FrameBuffer,
    Shader_LightSource,
    Shader_SimpleModel,
    Shader_Passthrough,
    
    Shader_Count
};

enum Render_Command_Type
{
    RenderCommand_Line,
    RenderCommand_Text,
    RenderCommand_Sprite,
    RenderCommand_Rect,
    
    RenderCommand_Spotlight,
    RenderCommand_DirectionalLight,
    RenderCommand_PointLight,
    
    RenderCommand_Buffer,
    RenderCommand_Model,
    RenderCommand_WireframeCube,
    RenderCommand_Count
};

enum Alignment
{
    Alignment_Left,
    Alignment_Right,
    Alignment_Center
};

struct shader
{
    Shader_Type Type;
    b32 Loaded;
    u32 Program;
    u32 VertexShader;
    u32 FragmentShader;
};

struct texture_info
{
    b32 HasData;
    char TextureName[50];
    i32 TextureHandle;
};

struct material
{
    texture_info DiffuseTexture;
};

struct vec3_keys
{
    i32 NumKeys;
    r32* TimeStamps;
    math::v3* Values;
};

struct quat_keys
{
    i32 NumKeys;
    r32* TimeStamps;
    math::quat* Values;
};

struct bone_channel
{
    i32 BoneIndex;
    vec3_keys PositionKeys;
    quat_keys RotationKeys;
    vec3_keys ScalingKeys;
};

struct skeletal_animation
{
    char* Name;
    r32 Duration;
    i32 NumBoneChannels;
    bone_channel* BoneChannels;
};

struct skeletal_animation_state
{
    b32 Playing;
    b32 Loop;
    r32 CurrentTime;
};

struct bone
{
    char Name[30];
    i32 ParentId;
    u32 Children[MAX_CHILDREN];
    i32 ChildCount;
    
    math::m4 Transformation;
    math::m4 BoneOffset;
};

struct mesh_data
{
    i32 BaseVertex;
    i32 BaseIndex;
    i32 MaterialIndex;
    i32 NumIndices;
};

enum Model_Type
{
    Model_Static,
    Model_Skinned
};

struct model
{
    Model_Type Type;
    
    i32 BufferHandle;
    
    math::v3 Position;
    math::v3 Scale;
    
    math::quat Orientation;
    
    math::rgba Color;
    
    material Materials[10];
    i32 MaterialCount;
    
    mesh_data Meshes[MAX_MESHES];
    i32 MeshCount;
    
    bone* Bones;
    i32 BoneCount;
    
    skeletal_animation_state AnimationState;
    i32 RunningAnimationIndex;
    math::m4* CurrentPoses;
    skeletal_animation* Animations;
    i32 AnimationCount;
    
    math::m4 GlobalInverseTransform;
};

struct render_command
{
    Render_Command_Type Type;
    Shader_Type ShaderType;
    b32 IsUI;
    
    math::v3 Position;
    math::v3 Scale;
    math::v3 Rotation;
    
    math::quat Orientation;
    
    union
    {
        struct
        {
            math::v3 Point1;
            math::v3 Point2;
            r32 LineWidth;
            math::rgba Color;
        } Line;
        struct
        {
            char Text[256];
            math::v3 Position;
            Font_Type FontType;
            math::rgba Color;
            Alignment Alignment;
        } Text;
        struct
        {
            math::v3 Position;
            math::v3 Scale;
            math::v2 Frame;
            math::v2 TextureOffset;
            char* TextureName;
            math::rgba Color;
        } Sprite;
        struct
        {
            math::v3 Position;
            math::v3 Size;
            math::rgba Color;
            b32 Outlined;
        } Rect;
        struct
        {
            math::rgba Color;
        } WireframeCube;
        struct
        {
            math::v3 Direction;
            r32 CutOff;
            r32 OuterCutOff;
            math::v3 Ambient;
            math::v3 Diffuse;
            math::v3 Specular;
            
            r32 Constant;
            r32 Linear;
            r32 Quadratic;
        } Spotlight;
        struct 
        {
            math::v3 Direction;
            math::v3 Ambient;
            math::v3 Diffuse;
            math::v3 Specular;
        } DirectionalLight;
        struct
        {
            math::v3 Ambient;
            math::v3 Diffuse;
            math::v3 Specular;
            
            r32 Constant;
            r32 Linear;
            r32 Quadratic;
        } PointLight;
        struct
        {
            i32 BufferHandle;
            char* TextureName;
        } Buffer;
        struct
        {
            Model_Type Type;
            i32 BufferHandle;
            mesh_data Meshes[MAX_MESHES];
            i32 MeshCount;
            material Materials[10];
            i32 MaterialCount;
            math::rgba Color;
            math::m4* BoneTransforms;
            i32 BoneCount;
        } Model;
    };
    render_command() {}
};

enum Fading_Mode
{
    Fading_None,
    Fading_In,
    Fading_Out,
    Fading_OutIn
};

struct camera
{
    i32 ViewportWidth;
    i32 ViewportHeight;
    r32 Zoom;
    math::v3 Center;
    math::v3 Position;
    math::quat Orientation;
    math::v3 Target;
    
    r32 FollowSpeed;
    math::m4 ViewMatrix;
    math::m4 ProjectionMatrix;
    
    Fading_Mode FadingMode = Fading_None;
    math::v3 FadingTint;
    
    b32 FadingIn;
    r32 EndAlpha;
    r32 FadingAlpha = 0.0f;
    r32 FadingSpeed;
};

#define RENDER_COMMAND_MAX 400
#define BUFFER_ARRAY_SIZE 400
#define TEXTURE_ARRAY_SIZE 512

struct texture_data
{
    i32 Handle;
    char* Name;
    i32 Width;
    i32 Height;
    unsigned char* ImageData;
};

struct ui_render_info
{
    b32 Rendered = true;
    
    i32 TextureHandle;
    math::v2 TextureOffset;
    math::v2 FrameSize;
    u32 ShaderIndex;
    math::v2 Size = math::v2(1, 1);
    math::v4 Color = math::v4(1, 1, 1, 1);
};

GENERIC_MAP(texture_data, texture_data*, char*, StrCmp, NULL, "%s", STR_ASSIGN);

struct buffer_data
{
    r32* VertexBuffer;
    long VertexBufferSize;
    u32* IndexBuffer;
    i32 IndexBufferCount;
    long IndexBufferSize;
    b32 HasNormals;
    b32 HasUVs;
    b32 Skinned;
    
    Shader_Type ShaderType;
    i32 ExistingHandle = -1;
};

#define MAX_CAMERAS 8

struct renderer
{
    r64 FPS;
    r64 AverageFPS;
    r64 FPSSum;
    u64 CurrentFrame;
    
    memory_arena Commands;
    i32 CommandCount;
    
    memory_arena UICommands;
    i32 UICommandCount;
    
    memory_arena LightCommands;
    i32 LightCommandCount;
    
    buffer_data Buffers[BUFFER_ARRAY_SIZE];
    i32 BufferHandles[BUFFER_ARRAY_SIZE];
    i32 BufferCount;
    
    texture_data TextureData[TEXTURE_ARRAY_SIZE];
    i32 TextureCount;
    
    texture_data_map TextureMap;
    
    camera Cameras[MAX_CAMERAS];
    i32 CurrentCameraHandle;
    
    union
    {
        i32 Viewport[4];
        math::v4i V;
        struct
        {
            i32 ViewportX;
            i32 ViewportY;
            i32 ViewportWidth;
            i32 ViewportHeight;
        };
    };
    
    i32 WindowWidth;
    i32 WindowHeight;
    
    math::rgba ClearColor;
    
    memory_arena AnimationArena;
};

#endif