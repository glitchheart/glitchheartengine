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
    
    Shader_Count
};

enum Render_Command_Type
{
    RenderCommand_Line,
    RenderCommand_Text,
    RenderCommand_Sprite,
    RenderCommand_Rect,
    RenderCommand_Buffer,
    RenderCommand_Count
};

enum Alignment
{
    Alignment_Left,
    Alignment_Right,
    Alignment_Center
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
            char* Text;
            math::v2 Position;
            i32 FontHandle;
            math::rgba Color;
            Alignment Alignment;
        } Text;
        struct
        {
            math::v2 Position;
            math::v2 Size;
            
            math::v2 TextureCoords;
            i32 TextureHandle;
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
        } Buffer;
    };
};

#define RENDER_COMMAND_MAX 400

struct render_command_buffer
{
    render_command Buffer[RENDER_COMMAND_MAX];
    i32 CommandCount;
};

