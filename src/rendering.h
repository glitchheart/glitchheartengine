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

struct model
{
    math::v3 Position;
    math::v3 Scale;
    math::v3 Rotation;
    math::rgba Color;
    i32 BufferHandle;
};

struct render_command
{
    Render_Command_Type Type;
    Shader_Type ShaderType;
    b32 IsUI;
    
    union
    {
        struct
        {
            math::v2 Point1;
            math::v2 Point2;
            r32 LineWidth;
            math::rgba Color;
        } Line;
        struct
        {
            char Text[256];
            math::v2 Position;
            i32 FontHandle;
            math::rgba Color;
            Alignment Alignment;
        } Text;
        struct
        {
            math::v2 Position;
            math::v3 Scale;
            math::v2 Frame;
            math::v2 TextureOffset;
            char* TextureName;
            math::rgba Color;
        } Sprite;
        struct
        {
            math::v2 Position;
            math::v2 Size;
            math::rgba Color;
            b32 Outlined;
        } Rect;
        struct
        {
            i32 BufferHandle;
            char* TextureName;
        } Buffer;
        struct
        {
            math::v3 Position;
            math::v3 Scale;
            math::v3 Rotation;
            i32 BufferHandle;
            math::rgba Color;
        } Model;
    };
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
    math::v2 CenterTarget;
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
#define BUFFER_ARRAY_SIZE 20

struct buffer_data
{
    r32* VertexBuffer;
    i32 VertexBufferSize;
    i32* IndexBuffer;
    i32 IndexBufferSize;
    
    Shader_Type ShaderType;
    i32 ExistingHandle = -1;
};

struct renderer
{
    render_command Buffer[RENDER_COMMAND_MAX];
    i32 CommandCount;
    
    buffer_data Buffers[BUFFER_ARRAY_SIZE];
    i32 BufferHandles[BUFFER_ARRAY_SIZE];
    i32 BufferCount;
    
    camera Camera;
};

