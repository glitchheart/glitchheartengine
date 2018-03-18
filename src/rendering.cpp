#include "animation.h"

enum CameraFlags
{
    C_FLAG_ORTHOGRAPHIC = (1 << 0),
    C_FLAG_PERSPECTIVE  = (1 << 1),
    C_FLAG_NO_LOOK_AT     = (1 << 2)
};

struct CameraParams
{
    u32 view_flags;
};

static CameraParams default_camera_params()
{
    CameraParams params;
    Params.ViewFlags = CFlag_Orthographic | CFlag_NoLookAt;
    return params;
}

static CameraParams orthographic_camera_params()
{
    CameraParams params;
    Params.ViewFlags = CFlag_Orthographic;
    return params;
}

static CameraParams perspective_camera_params()
{
    CameraParams params;
    Params.ViewFlags = CFlag_Perspective;
    return params;
}

// @Incomplete
static inline void camera_transform(renderer& renderer, camera& camera, math::v3 position = math::v3(), math::quat orientation = math::quat(), math::v3 target = math::v3(), r32 zoom = 1.0f, r32 near = -1.0f, r32 far = 1.0f, CameraParams params = default_camera_params())
{
    Camera.ViewportWidth = Renderer.WindowWidth;
    Camera.ViewportHeight = Renderer.WindowHeight;
    if(Params.ViewFlags & CFlag_Orthographic)
    {
        Camera.ProjectionMatrix = math::Ortho(0.0f, Renderer.Viewport[2] / Zoom, 0.0f, Renderer.Viewport[3] / Zoom, Near, Far);
        Camera.ViewMatrix = math::m4(1.0f);
        
        Camera.Position = Position;
        Camera.Orientation = Orientation;
        Camera.Target = Target;
        
        if(!IsIdentity(Orientation))
        {
            Camera.ViewMatrix = ToMatrix(Orientation) * Camera.ViewMatrix;
        }
        else if(!(Params.ViewFlags & CFlag_NoLookAt))
        {
            auto dist = sqrt(1.0f / 3.0f);
            Camera.ViewMatrix = math::LookAt(math::v3(Dist, Dist, Dist), math::v3(0.0f));
        }
        
        Camera.ViewMatrix = math::Translate(Camera.ViewMatrix, math::v3(-Position.x, -Position.y, Position.z));
        
        //Camera.ViewMatrix = math::Translate(Camera.ViewMatrix, Position);
        Camera.ViewMatrix = math::Translate(Camera.ViewMatrix, math::v3(Renderer.Viewport[2] / Zoom / 2, Renderer.Viewport[3] / Zoom / 2, 0.0f));
        
        
    }
    else if(Params.ViewFlags & CFlag_Perspective)
    {
        Camera.ProjectionMatrix = math::Perspective((r32)Renderer.Viewport[2] / (r32)Renderer.Viewport[3], 0.60f, 0.1f, 100.0f);
        
        Camera.ViewMatrix = math::m4(1.0f);
        
        auto dist = sqrt(1.0f / 3.0f);
        
        Dist = 20.0f;
        
        Camera.ViewMatrix = math::LookAt(math::v3(Dist, Dist, Dist), Target);
        
        if(!IsIdentity(Orientation))
        {
            Camera.ViewMatrix = ToMatrix(Orientation) * Camera.ViewMatrix;
        }
        
        Camera.Position = Position;
        Camera.Orientation = Orientation;
        Camera.Target = Target;
    }
}

// The InfoHandle is used to be able to reference the same animation without having to load the animation again. 
static void add_animation(renderer& renderer, spritesheet_animation animation, const char* animation_name)
{
    strcpy(Animation.Name, AnimationName);
    Renderer.SpritesheetAnimations[Renderer.SpritesheetAnimationCount++] = Animation;
    Assert(Renderer.SpritesheetAnimationCount < MAX_SPRITESHEET_ANIMATIONS);
    Assert(Renderer.SpritesheetAnimationCount < MAX_SPRITESHEET_ANIMATION_INFOS);
}

static void load_shader(const char* full_shader_path, renderer& renderer, i32* handle)
{
    shader_data* shader_data = &Renderer.ShaderData[Renderer.ShaderCount];
    ShaderData->Handle = Renderer.ShaderCount++;
    *Handle = ShaderData->Handle;
    sprintf(ShaderData->Name, "%s", FullShaderPath);
    ShaderData->VertexShaderContent = 0;
    ShaderData->FragmentShaderContent = 0;
    
    u32 size = 0;
    FILE* file;
    
    File = fopen(Concat(FullShaderPath, ".vert"), "rb");
    
    if(File)
    {
        fseek(File, 0, SEEK_END);
        Size = (u32)ftell(File);
        fseek(File, 0, SEEK_SET);
        
        // @Incomplete: Use built-in memory arena
        ShaderData->VertexShaderContent = (char*) malloc(sizeof(char) * Size + 1);
        fread(ShaderData->VertexShaderContent, 1, (size_t)Size, File);
        ShaderData->VertexShaderContent[Size] = '\0';
        fclose(File);
    }
    else
    {
        printf("Invalid file path: '%s'\n", FullShaderPath);
    }
    
    File = fopen(Concat(FullShaderPath, ".frag"), "rb");
    
    if(File)
    {
        fseek(File, 0, SEEK_END);
        Size = (u32)ftell(File);
        fseek(File, 0, SEEK_SET);
        
        // @Incomplete: Use built-in memory arena
        ShaderData->FragmentShaderContent = (char*) malloc(sizeof(char) * Size + 1);
        fread(ShaderData->FragmentShaderContent, 1, (size_t)Size, File);
        ShaderData->FragmentShaderContent[Size] = '\0';
        
        fclose(File);
    }
    else
    {
        printf("Invalid file path: '%s'\n", FullShaderPath);
    }
}

#define GET_TEXTURE_SIZE(Handle) GetTextureSize(Handle, Renderer)
static math::v2i get_texture_size(i32 TextureHandle, renderer Renderer)
{
    if(TextureHandle <= Renderer.TextureCount)
    {
        texture_data data = Renderer.TextureData[TextureHandle - 1];
        return math::v2i(Data.Width, Data.Height);
    }
    return math::v2i();
}

static void load_texture(const char* full_texture_path, renderer& renderer, i32* handle = 0)
{
    texture_data* texture_data = &Renderer.TextureData[Renderer.TextureCount];
    
    TextureData->Handle = Renderer.TextureCount++;
    
    TextureData->ImageData = stbi_load(FullTexturePath, &TextureData->Width, &TextureData->Height, 0, STBI_rgb_alpha);
    
    if(!TextureData->ImageData)
    {
        printf("Texture cold not be loaded: %s\n", FullTexturePath);
    }
    
    if(Handle)
        *Handle = TextureData->Handle + 1; // We add one to the handle, since we want 0 to be an invalid handle
}

static void load_textures(renderer& renderer, const char* path)
{
    texture_data_Map_Init(&Renderer.TextureMap, HashStringJenkins, 64);
    
    directory_data dir_data = {};
    Platform.GetAllFilesWithExtension(Path, "png", &DirData, true);
    
    for (i32 file_index = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        LoadTexture(DirData.FilePaths[FileIndex], Renderer);
    }
}

static void load_textures(renderer& renderer)
{
    LoadTextures(Renderer, "../assets/textures/");
}

static render_command* push_next_command(renderer& Renderer, b32 IsUI)
{
    if(IsUI)
    {
        Renderer.UICommandCount++;
        render_command* command = PushStruct(&Renderer.UICommands, render_command);
        Command->ShaderHandle = -1;
        return Command;
    }
    else
    {
        Renderer.CommandCount++;
        render_command* command = PushStruct(&Renderer.Commands, render_command);
        Command->ShaderHandle = -1;
        return Command;
    }
}

static void enable_depth_test(renderer& renderer)
{
    render_command* render_command = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_DepthTest;
    RenderCommand->DepthTest.On = true;
}

static void disable_depth_test(renderer& renderer)
{
    render_command* render_command = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_DepthTest;
    RenderCommand->DepthTest.On = false;
}

static void push_shader(renderer& renderer, i32 shader_handle, shader_attribute* attributes, i32 attribute_count)
{
    render_command* render_command = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_ShaderStart;
    RenderCommand->Shader.Handle = ShaderHandle;
    RenderCommand->Shader.Attributes = Attributes;
    RenderCommand->Shader.AttributeCount = AttributeCount;
}

static void end_shader(renderer& renderer)
{
    render_command* render_command = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_ShaderEnd;
}

static void push_line(renderer& renderer, math::v3 point1, math::v3 point2, r32 line_width, math::rgba color, b32 is_ui = false)
{
    render_command* render_command = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Line;
    RenderCommand->Line.Point1 = Point1;
    RenderCommand->Line.Point2 = Point2;
    RenderCommand->Line.LineWidth = LineWidth;
    RenderCommand->Line.Color = Color;
    RenderCommand->IsUI = IsUI;
}

#define PUSH_TEXT(Text, Position, Color, FontHandle) PushText(Renderer, Text, Position, 1.0f, FontHandle, Color)
#define PUSH_CENTERED_TEXT(Text, Position, Color, FontHandle) PushText(Renderer, Text, Position, 1.0f, FontHandle, Color, Alignment_Center)
static void push_text(renderer& renderer, const char* text, math::v3 position, r32 scale, i32 font_handle, math::rgba color, Alignment alignment = Alignment_Left, b32 is_ui = true)
{
    render_command* render_command = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Text;
    
    strcpy(RenderCommand->Text.Text, Text);
    
    RenderCommand->Text.Position = Position;
    if(Scale == 0.0f)
        RenderCommand->Text.Scale = 1.0f;
    else
        RenderCommand->Text.Scale = Scale;
    //RenderCommand->Text.FontType = FontType;
    RenderCommand->Text.FontHandle = FontHandle;
    RenderCommand->Text.Color = Color;
    RenderCommand->Text.Alignment = Alignment;
    RenderCommand->IsUI = IsUI;
}

static void push_filled_quad(renderer& renderer, math::v3 position, b32 flipped, math::v3 size, math::v3 rotation = math::v3(), math::rgba color = math::rgba(1.0f, 1.0f, 1.0f, 1.0f), i32 texture_handle = 0, b32 is_ui = true, i32 animation_controller_handle = -1, b32 with_origin = false, math::v2 origin = math::v2(0.0f, 0.0f), i32 shader_handle = -1, shader_attribute* shader_attributes = 0, i32 shader_attribute_count = 0, math::v2 texture_offset = math::v2(-1.0f, -1.0f), math::v2i frame_size = math::v2i(0, 0))
{
    render_command* render_command = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Quad;
    RenderCommand->Position = Position;
    RenderCommand->Rotation = Rotation;
    RenderCommand->WithOrigin = WithOrigin;
    RenderCommand->Origin = Origin;
    RenderCommand->Scale = Size;
    RenderCommand->Quad.Flipped = Flipped;
    RenderCommand->Quad.Color = Color;
    RenderCommand->Quad.Outlined = false;
    RenderCommand->Quad.TextureHandle = TextureHandle - 1;
    
    if(AnimationControllerHandle != -1)
    {
        auto& controller = Renderer.AnimationControllers[AnimationControllerHandle];
        spritesheet_animation& animation = Renderer.SpritesheetAnimations[Controller.Nodes[Controller.CurrentNode].AnimationHandle];
        spritesheet_frame& frame = Animation.Frames[Controller.CurrentFrameIndex];
        RenderCommand->Quad.TextureHandle = Animation.TextureHandle - 1;
        RenderCommand->Quad.TextureSize = math::v2((r32)Renderer.TextureData[RenderCommand->Quad.TextureHandle].Width, (r32)Renderer.TextureData[RenderCommand->Quad.TextureHandle].Height);
        RenderCommand->Quad.FrameSize = math::v2i(Frame.FrameWidth, Frame.FrameHeight);
        RenderCommand->Quad.TextureOffset = math::v2(Frame.X, Frame.Y);
        RenderCommand->Quad.ForAnimation = true;
    }
    else
    {
        RenderCommand->Quad.TextureOffset = TextureOffset;
        RenderCommand->Quad.FrameSize = FrameSize;
        
        if(TextureHandle != -1)
        {
            RenderCommand->Quad.TextureSize = math::v2((r32)Renderer.TextureData[RenderCommand->Quad.TextureHandle].Width, (r32)Renderer.TextureData[RenderCommand->Quad.TextureHandle].Height);
        }
    }
    
    RenderCommand->ShaderHandle = ShaderHandle;
    RenderCommand->ShaderAttributes = ShaderAttributes;
    RenderCommand->ShaderAttributeCount = ShaderAttributeCount;
    
    RenderCommand->IsUI = IsUI;
}

static void push_outlined_quad(renderer& renderer, math::v3 position,  math::v3 size, math::v3 rotation, math::rgba color, b32 is_ui = false, r32 line_width = 1.0f)
{
    render_command* render_command = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Quad;
    RenderCommand->Position = Position;
    RenderCommand->Rotation = Rotation;
    RenderCommand->Scale = Size;
    RenderCommand->Quad.Color = Color;
    RenderCommand->Quad.Outlined = true;
    RenderCommand->Quad.TextureHandle = 0;
    RenderCommand->Quad.LineWidth = LineWidth;
    RenderCommand->IsUI = IsUI;
}

static void push_wireframe_cube(renderer& renderer, math::v3 position, math::v3 scale, math::quat orientation, math::rgba color, r32 line_width)
{
    render_command* render_command = PushNextCommand(Renderer, false);
    
    RenderCommand->Type = RenderCommand_WireframeCube;
    RenderCommand->WireframeCube.Color = Color;
    RenderCommand->WireframeCube.LineWidth = LineWidth;
    RenderCommand->Position = Position;
    RenderCommand->Scale = Scale;
    RenderCommand->Orientation = Orientation;
    RenderCommand->IsUI = false;
}

static void push_spotlight(renderer& renderer, math::v3 position, math::v3 direction, r32 cut_off, r32 outer_cut_off, math::v3 ambient, math::v3 diffuse, math::v3 specular, r32 constant, r32 linear, r32 quadratic)
{
    render_command* render_command = PushStruct(&Renderer.LightCommands, render_command);
    Renderer.LightCommandCount++;
    
    RenderCommand->Type = RenderCommand_Spotlight;
    
    RenderCommand->Position = Position;
    
    auto& spotlight = RenderCommand->Spotlight;
    Spotlight.Direction = Direction;
    Spotlight.CutOff = CutOff;
    Spotlight.OuterCutOff = OuterCutOff;
    Spotlight.Ambient = Ambient;
    Spotlight.Diffuse = Diffuse;
    Spotlight.Specular = Specular;
    Spotlight.Constant = Constant;
    Spotlight.Linear = Linear;
    Spotlight.Quadratic = Quadratic;
}

static void push_directional_light(renderer& renderer, math::v3 direction, math::v3 ambient, math::v3 diffuse, math::v3 specular)
{
    render_command* render_command = PushStruct(&Renderer.LightCommands, render_command);
    Renderer.LightCommandCount++;
    
    RenderCommand->Type = RenderCommand_DirectionalLight;
    
    auto& directional_light = RenderCommand->DirectionalLight;
    DirectionalLight.Direction = Direction;
    DirectionalLight.Ambient = Ambient;
    DirectionalLight.Diffuse = Diffuse;
    DirectionalLight.Specular = Specular;
}

static void push_point_light(renderer& renderer, math::v3 position, math::v3 ambient, math::v3 diffuse, math::v3 specular, r32 constant, r32 linear, r32 quadratic)
{
    render_command* render_command = PushStruct(&Renderer.LightCommands, render_command);
    Renderer.LightCommandCount++;
    
    RenderCommand->Type = RenderCommand_PointLight;
    
    RenderCommand->Position = Position;
    
    auto& point_light = RenderCommand->PointLight;
    PointLight.Ambient = Ambient;
    PointLight.Diffuse = Diffuse;
    PointLight.Specular = Specular;
    PointLight.Constant = Constant;
    PointLight.Linear = Linear;
    PointLight.Quadratic = Quadratic;
}

static void push_buffer(renderer& renderer, i32 buffer_handle, i32 texture_handle, math::v3 rotation = math::v3(), b32 is_ui = false, math::v3 position = math::v3(), math::v3 scale = math::v3(1.0f), math::rgba color = math::rgba(1, 1, 1, 1))
{
    render_command* render_command = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Buffer;
    RenderCommand->Buffer.BufferHandle = BufferHandle;
    RenderCommand->Buffer.TextureHandle = TextureHandle - 1;
    RenderCommand->Rotation = Rotation;
    RenderCommand->Position = Position;
    RenderCommand->Scale = Scale;
    RenderCommand->IsUI = IsUI;
    RenderCommand->Color = Color;
    
    RenderCommand->ShaderHandle = -1;
    RenderCommand->ShaderAttributes = 0;
    RenderCommand->ShaderAttributeCount = 0;
}

static void push_model(renderer& renderer, model& model)
{
    render_command* render_command = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_Model;
    RenderCommand->Position = Model.Position;
    RenderCommand->Scale = Model.Scale;
    RenderCommand->Orientation = Model.Orientation;
    RenderCommand->Model.BufferHandle = Model.BufferHandle;
    
    for(i32 index = 0; Index < Model.MaterialCount; Index++)
    {
        if(Model.Materials[Index].DiffuseTexture.HasData && Model.Materials[Index].DiffuseTexture.TextureHandle == -1 && strlen(Model.Materials[Index].DiffuseTexture.TextureName) > 0)
        {
            Model.Materials[Index].DiffuseTexture.TextureHandle = Renderer.TextureMap[Model.Materials[Index].DiffuseTexture.TextureName]->Handle;
        }
    }
    
    memcpy(&RenderCommand->Model.Meshes, Model.Meshes, sizeof(Model.Meshes));
    memcpy(&RenderCommand->Model.Materials, Model.Materials, sizeof(Model.Materials));
    
    // @Incomplete: Check if the texture handle has been set for the materials
    RenderCommand->Model.Type = Model.Type;
    RenderCommand->Model.MeshCount = Model.MeshCount;
    RenderCommand->Model.MaterialCount = Model.MaterialCount;
    RenderCommand->Model.BoneCount = Model.BoneCount;
    
    if(Model.Type == Model_Skinned)
    {
        RenderCommand->Model.BoneTransforms = PushTempSize(sizeof(math::m4) * Model.BoneCount, math::m4);
        
        for(i32 index = 0; Index < Model.BoneCount; Index++)
        {
            RenderCommand->Model.BoneTransforms[Index] = Model.CurrentPoses[Index];
        }
    }
    
    RenderCommand->Model.Color = math::rgba(1.0f, 1.0f, 1.0f, 1.0f);
    RenderCommand->IsUI = false;
}

static void load_buffer(renderer& renderer, r32* buffer, i32 buffer_size, i32* buffer_handle, b32 dynamic = false)
{
    buffer_data data = {};
    Data.VertexBuffer = Buffer;
    Data.VertexBufferSize = BufferSize;
    Data.IndexBufferCount = 0;
    
    Renderer.Buffers[Renderer.BufferCount] = Data;
    
    *BufferHandle = Renderer.BufferCount++;
}

static void update_buffer(renderer& renderer, r32* buffer, i32 buffer_size, i32 buffer_handle)
{
    buffer_data data = {};
    Data.VertexBuffer = Buffer;
    Data.VertexBufferSize = BufferSize;
    Data.IndexBufferCount = 0;
    Data.ExistingHandle = BufferHandle;
    Renderer.Buffers[BufferHandle] = Data;
    Renderer.UpdatedBufferHandles[Renderer.UpdatedBufferHandleCount++] = BufferHandle;
}

static i32 load_font(renderer& Renderer, char* Path, i32 Size, char* Name)
{
    font_data data = {};
    Data.Path = PushString(&Renderer.FontArena, Path);
    Data.Size = Size;
    Data.Name = PushString(&Renderer.FontArena, Name);
    
    Renderer.Fonts[Renderer.FontCount] = Data;
    return Renderer.FontCount++;
}

static void load_font(renderer& renderer, char* path, i32 size, i32* handle)
{
    font_data data = {};
    Data.Path = PushString(&Renderer.FontArena, Path);
    Data.Size = Size;
    
    Renderer.Fonts[Renderer.FontCount] = Data;
    *Handle = Renderer.FontCount++;
}

static b32 is_eof(chunk_format& Format)
{
    return strcmp(Format.Format, "EOF") == 0;
}

static void load_model(renderer& renderer, char* file_path, model* model)
{
    (void)Renderer;
    model_header header = {};
    
    FILE *file = fopen(FilePath, "rb");
    if(File)
    {
        fread(&Header,sizeof(model_header), 1, File);
        
        if(strcmp(Header.Version, "1.4") != 0)
        {
            ERR("Wrong file version. Expected version 1.4");
            return;
        }
        
        chunk_format format = {};
        fread(&Format, sizeof(chunk_format), 1, File);
        
        i32 mesh_count = 0;
        
        while(!IsEOF(Format))
        {
            if(Format.Format[0] == 'M' &&
               Format.Format[1] == 'E' &&
               Format.Format[2] == 'S' &&
               Format.Format[3] == 'H')
            {
                /*buffer_data Data = {};
                
                mesh_data_info MeshInfo;
                
                fread(&MeshInfo, sizeof(mesh_data_info), 1, File);
                
                u32* IndexBuffer = PushTempSize(MeshInfo.IndexBufferByteLength, unsigned short);
                fread(IndexBuffer, MeshInfo.IndexBufferByteLength, 1, File);
                CopyTemp(Data.IndexBuffer, IndexBuffer, MeshInfo.IndexBufferByteLength, u32);
                
                r32* VertexBuffer = PushTempSize(MeshInfo.VertexBufferByteLength, r32);
                fread(VertexBuffer, MeshInfo.VertexBufferByteLength, 1, File);
                CopyTemp(Data.VertexBuffer, VertexBuffer, MeshInfo.VertexBufferByteLength, r32);
                
                // @Incomplete: Do we really always have normals and uvs?????
                Data.HasNormals = true;
                Data.HasUVs = true;
                
                Data.VertexBufferSize = MeshInfo.VertexBufferByteLength;
                Data.IndexBufferSize = MeshInfo.IndexBufferByteLength;
                Data.IndexBufferCount = MeshInfo.IndexCount;
                
                Model->Meshes[MeshCount].BufferHandle = Renderer.BufferCount++;
                
                // @Incomplete: IMPORTANT
                // @Incomplete: IMPORTANT
                // @Incomplete: IMPORTANT
                // @Incomplete: IMPORTANT
                // @Incomplete: IMPORTANT
                // @Incomplete: IMPORTANT
                Model->Meshes[MeshCount].Material.HasTexture = false;
                
                //if(Model->Meshes[MeshCount].Material.HasTexture)
                //Model->Meshes[MeshCount].Material.TextureHandle = Renderer.TextureMap[MHeader.TextureFile]->Handle;
                
                Model->Meshes[MeshCount].Material.Color = math::rgba(1, 1, 1, 1);
                MeshCount++;
                
                Assert(MeshCount <= MAX_MESHES);
                
                Renderer.Buffers[Renderer.BufferCount - 1] = Data;
                }
                else if(Format.Format[0] == 'S' &&
                Format.Format[1] == 'K' &&
                Format.Format[2] == 'I' &&
                Format.Format[3] == 'N')
                {
                
                }
                else if(Format.Format[0] == 'A' &&
                Format.Format[1] == 'N' &&
                Format.Format[2] == 'I' &&
                Format.Format[3] == 'M')
                {
                
                /*animation_header AHeader;
                fread(&AHeader, sizeof(animation_header), 1, File);
                
                animation_cycle AnimationCycle;
                AnimationCycle.NumFrames = AHeader.NumFrames;
                AnimationCycle.TotalTime = AHeader.TotalTime;
                AnimationCycle.Frames = PushArray(&Renderer.AnimationArena, AnimationCycle.NumFrames, animation_frame);
                
                fread(AnimationCycle.Frames, sizeof(animation_frame) * AnimationCycle.NumFrames, 1, File);
                
                Renderer.AnimationCycles[Renderer.AnimationCycleCount++] = AnimationCycle;*/
            }
            else
            {
                ERR("Malformed model file");
                break;
            }
            fread(&Format, sizeof(chunk_format), 1, File);
        }
        
        Model->MeshCount = MeshCount;
        printf("Mesh count %d\n", MeshCount);
        
        fclose(File);
    }
    else
    {
        printf("Model file not found: %s", FilePath);
    }
}

static void load_glim_model(renderer& renderer, char* file_path, model* model)
{
    model_header header = {};
    
    FILE *file = fopen(FilePath, "rb");
    if(File)
    {
        fread(&Header,sizeof(model_header), 1, File);
        
        if(strcmp(Header.Version, "1.6") != 0)
        {
            ERR("Wrong file version. Expected version 1.6");
            return;
        }
        
        model_data model_data;
        fread(&ModelData, sizeof(model_data), 1, File);
        fread(Model->Meshes, (size_t)ModelData.MeshChunkSize, 1, File);
        
        Model->Type = (Model_Type)ModelData.ModelType;
        Model->MeshCount = ModelData.NumMeshes;
        
        r32* vertex_buffer = PushTempSize(ModelData.VertexBufferChunkSize, r32);
        fread(VertexBuffer, (size_t)ModelData.VertexBufferChunkSize, 1, File);
        u32* index_buffer = PushTempSize(ModelData.IndexBufferChunkSize, u32);
        fread(IndexBuffer, (size_t)ModelData.IndexBufferChunkSize, 1, File);
        
        Model->MaterialCount = ModelData.NumMaterials;
        if(ModelData.NumMaterials > 0)
            fread(&Model->Materials, (size_t)ModelData.MaterialChunkSize, 1, File);
        
        Model->GlobalInverseTransform = ModelData.GlobalInverseTransform;
        
        Model->BoneCount = ModelData.NumBones;
        if(ModelData.NumBones > 0)
        {
            Model->Bones = PushArray(&Renderer.AnimationArena, ModelData.NumBones, bone);
            Model->CurrentPoses = PushArray(&Renderer.AnimationArena, ModelData.NumBones, math::m4);
            fread(Model->Bones, (size_t)ModelData.BoneChunkSize, 1, File);
        }
        
        buffer_data data = {};
        Data.Skinned = Model->BoneCount > 0;
        CopyTemp(Data.VertexBuffer, VertexBuffer, (size_t)ModelData.VertexBufferChunkSize, r32);
        CopyTemp(Data.IndexBuffer, IndexBuffer, (size_t)ModelData.IndexBufferChunkSize, u32);
        
        Data.HasNormals = ModelData.HasNormals;
        Data.HasUVs = ModelData.HasUVs;
        
        Data.VertexBufferSize = ModelData.VertexBufferChunkSize;
        Data.IndexBufferSize = ModelData.IndexBufferChunkSize;
        Data.IndexBufferCount = ModelData.NumIndices;
        
        Model->BufferHandle = Renderer.BufferCount++;
        
        Model->AnimationState.Playing = false;
        Model->AnimationState.Loop = false;
        Model->AnimationState.CurrentTime = 0.0f;
        
        Renderer.Buffers[Renderer.BufferCount - 1] = Data;
        
        // Load animations
        animation_header a_header;
        fread(&AHeader, sizeof(animation_header), 1, File);
        
        Model->AnimationCount = AHeader.NumAnimations;
        Model->Animations = AHeader.NumAnimations > 0 ? PushArray(&Renderer.AnimationArena, AHeader.NumAnimations, skeletal_animation) : 0;
        
        for(i32 index = 0; Index < Model->AnimationCount; Index++)
        {
            animation_channel_header ac_header;
            fread(&ACHeader, sizeof(animation_channel_header), 1, File);
            
            skeletal_animation* animation = &Model->Animations[Index];
            Animation->Duration = ACHeader.Duration;
            Animation->NumBoneChannels = ACHeader.NumBoneChannels;
            
            Animation->BoneChannels = PushArray(&Renderer.AnimationArena, Animation->NumBoneChannels, bone_channel);
            
            for(i32 bone_channel_index = 0; BoneChannelIndex < Animation->NumBoneChannels; BoneChannelIndex++)
            {
                bone_animation_header ba_header;
                fread(&BAHeader, sizeof(bone_animation_header), 1, File);
                
                bone_channel* bone_channel = &Animation->BoneChannels[BoneChannelIndex];
                BoneChannel->BoneIndex = BAHeader.BoneIndex;
                
                BoneChannel->PositionKeys.NumKeys = BAHeader.NumPositionChannels;
                BoneChannel->PositionKeys.TimeStamps = PushArray(&Renderer.AnimationArena, BoneChannel->PositionKeys.NumKeys, r32);
                BoneChannel->PositionKeys.Values = PushArray(&Renderer.AnimationArena, BoneChannel->PositionKeys.NumKeys, math::v3);
                fread(BoneChannel->PositionKeys.TimeStamps, sizeof(r32) * BoneChannel->PositionKeys.NumKeys, 1, File);
                fread(BoneChannel->PositionKeys.Values, sizeof(math::v3) * BoneChannel->PositionKeys.NumKeys, 1, File);
                
                BoneChannel->RotationKeys.NumKeys = BAHeader.NumRotationChannels;
                BoneChannel->RotationKeys.TimeStamps = PushArray(&Renderer.AnimationArena, BoneChannel->RotationKeys.NumKeys, r32);
                BoneChannel->RotationKeys.Values = PushArray(&Renderer.AnimationArena, BoneChannel->RotationKeys.NumKeys, math::quat);
                fread(BoneChannel->RotationKeys.TimeStamps, sizeof(r32) * BoneChannel->RotationKeys.NumKeys, 1, File);
                fread(BoneChannel->RotationKeys.Values, sizeof(math::quat) * BoneChannel->RotationKeys.NumKeys, 1, File);
                
                BoneChannel->ScalingKeys.NumKeys = BAHeader.NumScalingChannels;
                BoneChannel->ScalingKeys.TimeStamps = PushArray(&Renderer.AnimationArena, BoneChannel->ScalingKeys.NumKeys, r32);
                BoneChannel->ScalingKeys.Values = PushArray(&Renderer.AnimationArena, BoneChannel->ScalingKeys.NumKeys, math::v3);
                fread(BoneChannel->ScalingKeys.TimeStamps, sizeof(r32) * BoneChannel->ScalingKeys.NumKeys, 1, File);
                fread(BoneChannel->ScalingKeys.Values, sizeof(math::v3) * BoneChannel->ScalingKeys.NumKeys, 1, File);
            }
        }
        
        fclose(File);
    }
    else
    {
        printf("Model file not found: %s", FilePath);
    }
}

static void add_particle_system(renderer& renderer, math::v3 position, i32 texture_handle, r32 rate, r32 speed, i32* handle)
{
}
static void update_particle_system_position(renderer& renderer, i32 handle, math::v2 new_position)
{
}

static void remove_particle_system(renderer& renderer, i32 handle)
{
}