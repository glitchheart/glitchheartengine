#ifndef RENDERING_H
#define RENDERING_H

#define PIXELS_PER_UNIT 32
#define MAX_MESHES 60

#define MAX_LIGHTS 150
#define MAX_BONES 50
#define MAX_CHILDREN 30
#define MAX_SPRITESHEET_ANIMATIONS 128
#define MAX_SPRITESHEET_ANIMATION_INFOS 256

struct font_data
{
    char* Path;
    i32 Size;
    char* Name;
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

enum Shader_Attribute_Type
{
    Attribute_Float,
    Attribute_Float2,
    Attribute_Float3,
    Attribute_Float4,
    Attribute_Integer,
    Attribute_Boolean,
    Attribute_Matrix4
};

struct shader_attribute
{
    Shader_Attribute_Type Type;
    char* Name;
    union
    {
        r32 FloatVar;
        math::v2 Float2Var;
        math::v3 Float3Var;
        math::v4 Float4Var;
        i32 IntegerVar;
        b32 BooleanVar;
        math::m4 Matrix4Var;
    };
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
    b32 WithOrigin;
    math::v2 Origin;
    math::rgba Color;
    i32 ShaderHandle;
    shader_attribute* ShaderAttributes;
    i32 ShaderAttributeCount;
    
    union
    {
        struct
        {
            math::v3 Point1;
            math::v3 Point2;
            r32 LineWidth;
            math::rgba Color; // @Cleanup: REMOVE!
        } Line;
        struct
        {
            char Text[256];
            math::v3 Position;
            i32 FontHandle;
            math::rgba Color; // @Cleanup: REMOVE!
            Alignment Alignment;
            r32 Scale;
        } Text;
        struct
        {
            math::rgba Color;
        } Sprite;
        struct
        {
            i32 TextureHandle;
            b32 Flipped;
            math::rgba Color; // @Cleanup: REMOVE!
            b32 Outlined;
            r32 LineWidth;
            b32 ForAnimation;
            math::v2 TextureSize;
            math::v2i FrameSize;
            math::v2 TextureOffset;
        } Quad;
        struct
        {
            math::rgba Color; // @Cleanup: REMOVE!
            r32 LineWidth;
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
            i32 TextureHandle;
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
        struct
        {
            i32 Handle;
            shader_attribute* Attributes;
            i32 AttributeCount;
        } Shader;
        struct
        {
            b32 On;
        } DepthTest;
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
#define SHADER_ARRAY_SIZE 128

struct texture_data
{
    i32 Handle;
    char* Name;
    i32 Width;
    i32 Height;
    unsigned char* ImageData;
};

struct shader_data
{
    i32 Handle;
    char Name[512];
    char* VertexShaderContent;
    char* FragmentShaderContent;
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

GENERIC_MAP(texture_data, texture_data*, char*, StrCmp, NULL, "%s", STR_ASSIGN, PTR_COPY);

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

struct particle
{
    math::v3 Center;
};

#define MAX_PARTICLES 256
struct particle_system
{
    i32 ParticleTexture;
    particle Particles[MAX_PARTICLES];
    i32 ParticleCount;
    i32 Rate; // Particles per second
    r32 ParticleSpeed;
};

#define MAX_CAMERAS 8

#define MAX_ANIMATION_CONTROLLERS 64
#define MAX_RENDER_COMMANDS 2048
#define MAX_UI_COMMANDS 2048 // @Incomplete: This should be defined by the game itself (HARDCODED FOR LEVEL EDITOR RIGHT NOW)
#define MAX_LIGHT_COMMANDS 1024

struct renderer
{
    b32 ShouldClose;
    r64 FPS;
    r64 AverageFPS;
    r64 FPSSum;
    u64 CurrentFrame;
    i32 PixelsPerUnit;
    i32 FrameLock;
    
    Window_Mode WindowMode;
    
    memory_arena Commands;
    i32 CommandCount;
    
    memory_arena UICommands;
    i32 UICommandCount;
    
    memory_arena LightCommands;
    i32 LightCommandCount;
    
    buffer_data Buffers[BUFFER_ARRAY_SIZE];
    i32 BufferHandles[BUFFER_ARRAY_SIZE];
    i32 BufferCount;
    
    i32 UpdatedBufferHandles[BUFFER_ARRAY_SIZE];
    i32 UpdatedBufferHandleCount;
    
    texture_data TextureData[TEXTURE_ARRAY_SIZE];
    i32 TextureCount;
    
    i32 TextureHandles[TEXTURE_ARRAY_SIZE];
    
    texture_data_map TextureMap;
    
    shader_data ShaderData[SHADER_ARRAY_SIZE];
    i32 ShaderCount;
    
    camera Cameras[MAX_CAMERAS];
    i32 CurrentCameraHandle;
    
    animation_controller* AnimationControllers;
    i32 AnimationControllerCount;
    
    spritesheet_animation SpritesheetAnimations[MAX_SPRITESHEET_ANIMATIONS];
    i32 SpritesheetAnimationCount;
    
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
    
    r32 ScaleX;
    r32 ScaleY;
    
    math::rgba ClearColor;
    r32 LineWidth;
    
    b32 ShowMouseCursor;
    
    font_data Fonts[64];
    i32 FontCount;
    
    memory_arena TextureArena;
    memory_arena AnimationArena;
    memory_arena FontArena;
};

#endif
