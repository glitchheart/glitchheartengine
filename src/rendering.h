#ifndef RENDERING_H
#define RENDERING_H

#define PIXELS_PER_UNIT 32
#define MAX_MESHES 60

#define MAX_LIGHTS 20

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

struct material
{
    b32 HasTexture;
    i32 TextureHandle; // This handle is a handle to the render API's array
    math::rgba Color;
};

struct mesh
{
    i32 BufferHandle;
    material Material;
};

struct model
{
    math::v3 Position;
    math::v3 Scale;
    math::v3 Rotation;
    
    //@Incomplete Test this!!!
    math::quat Orientation;
    
    math::rgba Color;
    
    mesh Meshes[MAX_MESHES];
    i32 MeshCount;
};

struct mesh_render_data
{
    i32 BufferHandle;
    material Material;
};

struct render_command
{
    Render_Command_Type Type;
    Shader_Type ShaderType;
    b32 IsUI;
    
    math::v3 Position;
    math::v3 Scale;
    math::v3 Rotation;
    
    //@Incomplete: Test!!!
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
            i32 FontHandle;
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
            mesh_render_data RenderData[MAX_MESHES];
            i32 HandleCount;
            math::rgba Color;
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
    math::v3 CenterTarget;
    r32 FollowSpeed;
    math::m4 ViewMatrix;
    math::m4 ProjectionMatrix;
    
    Fading_Mode FadingMode = Fading_None;
    math::v3 FadingTint;
    
    b32 FadingIn;
    r32 EndAlpha;
    r32 FadingAlpha = 0.0f;
    r32 FadingSpeed;
    
    math::v3 P;
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
    i32 VertexBufferSize;
    u32* IndexBuffer;
    i32 IndexBufferSize;
    
    b32 HasNormals;
    b32 HasUVs;
    
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
    
    i32 Viewport[4];
    i32 WindowWidth;
    i32 WindowHeight;
    
    math::rgba ClearColor;
};

#endif