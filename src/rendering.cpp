#include "animation.h"

enum Camera_Flags
{
    CFlag_Orthographic = (1 << 0),
    CFlag_Perspective  = (1 << 1),
    CFlag_NoLookAt     = (1 << 2)
};

struct camera_params
{
    u32 ViewFlags;
};

static camera_params DefaultCameraParams()
{
    camera_params Params;
    Params.ViewFlags = CFlag_Orthographic | CFlag_NoLookAt;
    return Params;
}

static camera_params OrthographicCameraParams()
{
    camera_params Params;
    Params.ViewFlags = CFlag_Orthographic;
    return Params;
}

static camera_params PerspectiveCameraParams()
{
    camera_params Params;
    Params.ViewFlags = CFlag_Perspective;
    return Params;
}

// @Incomplete
static inline void CameraTransform(renderer& Renderer, camera& Camera, math::v3 Position = math::v3(), math::quat Orientation = math::quat(), math::v3 Target = math::v3(), r32 Zoom = 1.0f, r32 Near = -1.0f, r32 Far = 1.0f, camera_params Params = DefaultCameraParams())
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
            auto Dist = sqrt(1.0f / 3.0f);
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
        
        auto Dist = sqrt(1.0f / 3.0f);
        
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

static b32 AnimationIsPlaying(i32 InfoHandle, renderer& Renderer)
{
    return Renderer.SpritesheetAnimationInfos[InfoHandle].Playing;
}

static void StartAnimation(i32 InfoHandle, renderer& Renderer, b32 Reset = true)
{
    spritesheet_animation_info& Info = Renderer.SpritesheetAnimationInfos[InfoHandle];
    
    if(Reset)
    {
        Info.CurrentTime = 0.0f;
        Info.FrameIndex = 0;
    }
    
    Info.Playing = true;
}

static void StopAnimation(spritesheet_animation_info& Info)
{
    Info.Playing = false;
}

static void StopAnimation(i32 InfoHandle, renderer& Renderer)
{
    spritesheet_animation_info& Info = Renderer.SpritesheetAnimationInfos[InfoHandle];
    StopAnimation(Info);
}

static void TickAnimations(renderer& Renderer, r64 DeltaTime)
{
    for(i32 Index = 0; Index < Renderer.SpritesheetAnimationInfoCount; Index++)
    {
        spritesheet_animation_info& Info = Renderer.SpritesheetAnimationInfos[Index];
        spritesheet_animation& Animation = Renderer.SpritesheetAnimations[Info.AnimationHandle];
        
        if (Info.Playing)
        {
            Info.CurrentTime += DeltaTime;
            if (Info.CurrentTime >= Animation.Frames[Info.FrameIndex].Duration)
            {
                Info.FrameIndex++;
                Info.CurrentTime = 0.0;
                
                if (Info.FrameIndex >= Animation.FrameCount)
                {
                    if (!Info.FreezeFrame)
                        Info.FrameIndex = 0;
                    else
                        Info.FrameIndex = Animation.FrameCount - 1;
                    
                    if (!Animation.Loop)
                    {
                        StopAnimation(Info);
                    }
                }
            }
        }
    }
}

static void AddAnimation(renderer& Renderer, spritesheet_animation Animation, i32* InfoHandle)
{
    *InfoHandle = Renderer.SpritesheetAnimationInfoCount;
    spritesheet_animation_info& Info = Renderer.SpritesheetAnimationInfos[Renderer.SpritesheetAnimationInfoCount++];
    Info.AnimationHandle = Renderer.SpritesheetAnimationCount;
    Info.FrameIndex = 0;
    Info.Playing = true;
    Info.CurrentTime = 0.0f;
    Info.FreezeFrame = false;
    
    Renderer.SpritesheetAnimations[Renderer.SpritesheetAnimationCount++] = Animation;
    
    Assert(Renderer.SpritesheetAnimationCount < MAX_SPRITESHEET_ANIMATIONS);
    Assert(Renderer.SpritesheetAnimationCount < MAX_SPRITESHEET_ANIMATION_INFOS);
}

static void LoadShader(const char* FullShaderPath, renderer& Renderer, i32* Handle)
{
    shader_data* ShaderData = &Renderer.ShaderData[Renderer.ShaderCount];
    ShaderData->Handle = Renderer.ShaderCount++;
    ShaderData->VertexShaderContent = 0;
    ShaderData->FragmentShaderContent = 0;
    
    int Size = 0;
    FILE* File;
    
    File = fopen(Concat(FullShaderPath, ".vert"), "r");
    
    if(File)
    {
        fseek(File, 0, SEEK_END);
        Size = ftell(File);
        rewind(File);
        
        // @Incomplete: Use built-in memory arena
        ShaderData->VertexShaderContent = (char*) malloc(sizeof(char) * Size);
        fread(ShaderData->VertexShaderContent, 1, Size, File);
        fclose(File);
    }
    else
    {
        printf("Invalid file path: '%s'\n", FullShaderPath);
    }
    
    File = fopen(Concat(FullShaderPath, ".frag"), "r");
    
    if(File)
    {
        fseek(File, 0, SEEK_END);
        Size = ftell(File);
        rewind(File);
        
        // @Incomplete: Use built-in memory arena
        ShaderData->FragmentShaderContent = (char*) malloc(sizeof(char) * Size);
        fread(ShaderData->FragmentShaderContent, 1, Size, File);
        fclose(File);
    }
    else
    {
        printf("Invalid file path: '%s'\n", FullShaderPath);
    }
}

static void LoadTexture(const char* FullTexturePath, renderer& Renderer, memory_arena* PermArena, i32* Handle = 0)
{
    texture_data* TextureData = &Renderer.TextureData[Renderer.TextureCount];
    
    TextureData->Handle = Renderer.TextureCount++;
    
    TextureData->ImageData = stbi_load(FullTexturePath, &TextureData->Width, &TextureData->Height, 0, STBI_rgb_alpha);
    
    if(!TextureData->ImageData)
    {
        printf("Texture cold not be loaded: %s\n", FullTexturePath);
    }
    
    if(Handle)
        *Handle = TextureData->Handle + 1; // We add one to the handle, since we want 0 to be an invalid handle
}

static void LoadTextures(renderer& Renderer, memory_arena* PermArena, const char* Path)
{
    texture_data_Map_Init(&Renderer.TextureMap, HashStringJenkins, 64);
    
    directory_data DirData = {};
    Platform.GetAllFilesWithExtension(Path, "png", &DirData, true);
    
    for (i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        LoadTexture(DirData.FilePaths[FileIndex], Renderer, PermArena);
    }
}

static void LoadTextures(renderer& Renderer, memory_arena* PermArena)
{
    LoadTextures(Renderer, PermArena, "../assets/textures/");
}

static render_command* PushNextCommand(renderer& Renderer, b32 IsUI)
{
    if(IsUI)
    {
        Renderer.UICommandCount++;
        render_command* Command = PushStruct(&Renderer.UICommands, render_command);
        Command->ShaderHandle = -1;
        return Command;
    }
    else
    {
        Renderer.CommandCount++;
        render_command* Command = PushStruct(&Renderer.Commands, render_command);
        Command->ShaderHandle = -1;
        return Command;
    }
}

static void EnableDepthTest(renderer& Renderer)
{
    render_command* RenderCommand = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_DepthTest;
    RenderCommand->DepthTest.On = true;
}

static void DisableDepthTest(renderer& Renderer)
{
    render_command* RenderCommand = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_DepthTest;
    RenderCommand->DepthTest.On = false;
}

static void PushShader(renderer& Renderer, i32 ShaderHandle, shader_attribute* Attributes, i32 AttributeCount)
{
    render_command* RenderCommand = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_ShaderStart;
    RenderCommand->Shader.Handle = ShaderHandle;
    RenderCommand->Shader.Attributes = Attributes;
    RenderCommand->Shader.AttributeCount = AttributeCount;
}

static void EndShader(renderer& Renderer)
{
    render_command* RenderCommand = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_ShaderEnd;
}

static void PushLine(renderer& Renderer, math::v3 Point1, math::v3 Point2, r32 LineWidth, math::rgba Color, b32 IsUI = false)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Line;
    RenderCommand->Line.Point1 = Point1;
    RenderCommand->Line.Point2 = Point2;
    RenderCommand->Line.LineWidth = LineWidth;
    RenderCommand->Line.Color = Color;
    RenderCommand->IsUI = IsUI;
}

// @Incomplete: We still need to do something with fonts!
static void PushText(renderer& Renderer, const char* Text, math::v3 Position, i32 FontHandle, math::rgba Color, Alignment Alignment = Alignment_Left, b32 IsUI = true)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Text;
    
    strcpy(RenderCommand->Text.Text, Text);
    
    RenderCommand->Text.Position = Position;
    //RenderCommand->Text.FontType = FontType;
    RenderCommand->Text.FontHandle = FontHandle;
    RenderCommand->Text.Color = Color;
    RenderCommand->Text.Alignment = Alignment;
    RenderCommand->IsUI = IsUI;
}

static void PushFilledQuad(renderer& Renderer, math::v3 Position, math::v3 Size, math::v3 Rotation, math::rgba Color, i32 TextureHandle = 0, b32 IsUI = true, i32 AnimationInfoHandle = -1,i32 ShaderHandle = -1, shader_attribute* ShaderAttributes = 0, i32 ShaderAttributeCount = 0)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Quad;
    RenderCommand->Position = Position;
    RenderCommand->Rotation = Rotation;
    RenderCommand->Scale = Size;
    RenderCommand->Quad.Color = Color;
    RenderCommand->Quad.Outlined = false;
    RenderCommand->Quad.TextureHandle = TextureHandle - 1;
    
    if(AnimationInfoHandle != -1)
    {
        spritesheet_animation_info& Info = Renderer.SpritesheetAnimationInfos[AnimationInfoHandle];
        spritesheet_animation& Animation = Renderer.SpritesheetAnimations[Info.AnimationHandle];
        spritesheet_frame& Frame = Animation.Frames[Info.FrameIndex];
        RenderCommand->Quad.TextureHandle = Animation.TextureHandle - 1; // @Incomplete: Do we subtract one from this too?
        RenderCommand->Quad.TextureSize = math::v2((r32)Renderer.TextureData[RenderCommand->Quad.TextureHandle].Width, (r32)Renderer.TextureData[RenderCommand->Quad.TextureHandle].Height);
        RenderCommand->Quad.FrameSize = math::v2i(Frame.FrameWidth, Frame.FrameHeight);
        RenderCommand->Quad.TextureOffset = math::v2(Frame.X, Frame.Y);
        RenderCommand->Quad.ForAnimation = true;
    }
    
    RenderCommand->ShaderHandle = ShaderHandle;
    RenderCommand->ShaderAttributes = ShaderAttributes;
    RenderCommand->ShaderAttributeCount = ShaderAttributeCount;
    
    RenderCommand->IsUI = IsUI;
}

static void PushOutlinedQuad(renderer& Renderer, math::v3 Position,  math::v3 Size, math::v3 Rotation, math::rgba Color, b32 IsUI = false, r32 LineWidth = 1.0f)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
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

static void PushWireframeCube(renderer& Renderer, math::v3 Position, math::v3 Scale, math::quat Orientation, math::rgba Color, r32 LineWidth)
{
    render_command* RenderCommand = PushNextCommand(Renderer, false);
    
    RenderCommand->Type = RenderCommand_WireframeCube;
    RenderCommand->WireframeCube.Color = Color;
    RenderCommand->WireframeCube.LineWidth = LineWidth;
    RenderCommand->Position = Position;
    RenderCommand->Scale = Scale;
    RenderCommand->Orientation = Orientation;
    RenderCommand->IsUI = false;
}

static void PushSpotlight(renderer& Renderer, math::v3 Position, math::v3 Direction, r32 CutOff, r32 OuterCutOff, math::v3 Ambient, math::v3 Diffuse, math::v3 Specular, r32 Constant, r32 Linear, r32 Quadratic)
{
    render_command* RenderCommand = PushStruct(&Renderer.LightCommands, render_command);
    Renderer.LightCommandCount++;
    
    RenderCommand->Type = RenderCommand_Spotlight;
    
    RenderCommand->Position = Position;
    
    auto& Spotlight = RenderCommand->Spotlight;
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

static void PushDirectionalLight(renderer& Renderer, math::v3 Direction, math::v3 Ambient, math::v3 Diffuse, math::v3 Specular)
{
    render_command* RenderCommand = PushStruct(&Renderer.LightCommands, render_command);
    Renderer.LightCommandCount++;
    
    RenderCommand->Type = RenderCommand_DirectionalLight;
    
    auto& DirectionalLight = RenderCommand->DirectionalLight;
    DirectionalLight.Direction = Direction;
    DirectionalLight.Ambient = Ambient;
    DirectionalLight.Diffuse = Diffuse;
    DirectionalLight.Specular = Specular;
}

static void PushPointLight(renderer& Renderer, math::v3 Position, math::v3 Ambient, math::v3 Diffuse, math::v3 Specular, r32 Constant, r32 Linear, r32 Quadratic)
{
    render_command* RenderCommand = PushStruct(&Renderer.LightCommands, render_command);
    Renderer.LightCommandCount++;
    
    RenderCommand->Type = RenderCommand_PointLight;
    
    RenderCommand->Position = Position;
    
    auto& PointLight = RenderCommand->PointLight;
    PointLight.Ambient = Ambient;
    PointLight.Diffuse = Diffuse;
    PointLight.Specular = Specular;
    PointLight.Constant = Constant;
    PointLight.Linear = Linear;
    PointLight.Quadratic = Quadratic;
}

static void PushBuffer(renderer& Renderer, i32 BufferHandle, i32 TextureHandle, math::v3 Rotation, b32 IsUI = false)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Buffer;
    RenderCommand->Buffer.BufferHandle = BufferHandle;
    RenderCommand->Buffer.TextureHandle = TextureHandle - 1;
    RenderCommand->Rotation = Rotation;
    RenderCommand->IsUI = IsUI;
    
    RenderCommand->ShaderHandle = -1;
    RenderCommand->ShaderAttributes = 0;
    RenderCommand->ShaderAttributeCount = 0;
}

static void PushModel(renderer& Renderer, model& Model)
{
    render_command* RenderCommand = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_Model;
    RenderCommand->Position = Model.Position;
    RenderCommand->Scale = Model.Scale;
    RenderCommand->Orientation = Model.Orientation;
    RenderCommand->Model.BufferHandle = Model.BufferHandle;
    
    for(i32 Index = 0; Index < Model.MaterialCount; Index++)
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
        
        for(i32 Index = 0; Index < Model.BoneCount; Index++)
        {
            RenderCommand->Model.BoneTransforms[Index] = Model.CurrentPoses[Index];
        }
    }
    
    RenderCommand->Model.Color = math::rgba(1.0f, 1.0f, 1.0f, 1.0f);
    RenderCommand->IsUI = false;
}

static void LoadBuffer(renderer& Renderer, r32* Buffer, i32 BufferSize, i32* BufferHandle)
{
    buffer_data Data = {};
    Data.VertexBuffer = Buffer;
    Data.VertexBufferSize = BufferSize;
    Data.IndexBufferCount = 0;
    Renderer.Buffers[Renderer.BufferCount] = Data;
    
    *BufferHandle = Renderer.BufferCount++;
}

static i32 LoadFont(renderer& Renderer, char* Path, i32 Size, char* Name)
{
    font_data Data = {};
    Data.Path = PushString(&Renderer.FontArena, Path);
    Data.Size = Size;
    Data.Name = PushString(&Renderer.FontArena, Name);
    
    Renderer.Fonts[Renderer.FontCount] = Data;
    return Renderer.FontCount++;
}

static void LoadFont(renderer& Renderer, char* Path, i32 Size, i32* Handle)
{
    font_data Data = {};
    Data.Path = PushString(&Renderer.FontArena, Path);
    Data.Size = Size;
    
    Renderer.Fonts[Renderer.FontCount] = Data;
    *Handle = Renderer.FontCount++;
}

static b32 IsEOF(chunk_format& Format)
{
    return strcmp(Format.Format, "EOF") == 0;
}

static void LoadModel(renderer& Renderer, char* FilePath, model* Model)
{
    model_header Header = {};
    
    FILE *File = fopen(FilePath, "rb");
    if(File)
    {
        fread(&Header,sizeof(model_header), 1, File);
        
        if(strcmp(Header.Version, "1.4") != 0)
        {
            ERR("Wrong file version. Expected version 1.4");
            return;
        }
        
        chunk_format Format = {};
        fread(&Format, sizeof(chunk_format), 1, File);
        
        i32 MeshCount = 0;
        
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


static void LoadGLIMModel(renderer& Renderer, char* FilePath, model* Model)
{
    model_header Header = {};
    
    FILE *File = fopen(FilePath, "rb");
    if(File)
    {
        fread(&Header,sizeof(model_header), 1, File);
        
        if(strcmp(Header.Version, "1.6") != 0)
        {
            ERR("Wrong file version. Expected version 1.6");
            return;
        }
        
        model_data ModelData;
        fread(&ModelData, sizeof(model_data), 1, File);
        fread(Model->Meshes, ModelData.MeshChunkSize, 1, File);
        
        Model->Type = (Model_Type)ModelData.ModelType;
        Model->MeshCount = ModelData.NumMeshes;
        
        r32* VertexBuffer = PushTempSize(ModelData.VertexBufferChunkSize, r32);
        fread(VertexBuffer, ModelData.VertexBufferChunkSize, 1, File);
        u32* IndexBuffer = PushTempSize(ModelData.IndexBufferChunkSize, u32);
        fread(IndexBuffer, ModelData.IndexBufferChunkSize, 1, File);
        
        Model->MaterialCount = ModelData.NumMaterials;
        if(ModelData.NumMaterials > 0)
            fread(&Model->Materials, ModelData.MaterialChunkSize, 1, File);
        
        Model->GlobalInverseTransform = ModelData.GlobalInverseTransform;
        
        Model->BoneCount = ModelData.NumBones;
        if(ModelData.NumBones > 0)
        {
            Model->Bones = PushArray(&Renderer.AnimationArena, ModelData.NumBones, bone);
            Model->CurrentPoses = PushArray(&Renderer.AnimationArena, ModelData.NumBones, math::m4);
            fread(Model->Bones, ModelData.BoneChunkSize, 1, File);
        }
        
        buffer_data Data = {};
        Data.Skinned = Model->BoneCount > 0;
        CopyTemp(Data.VertexBuffer, VertexBuffer, ModelData.VertexBufferChunkSize, r32);
        CopyTemp(Data.IndexBuffer, IndexBuffer, ModelData.IndexBufferChunkSize, u32);
        
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
        animation_header AHeader;
        fread(&AHeader, sizeof(animation_header), 1, File);
        
        Model->AnimationCount = AHeader.NumAnimations;
        Model->Animations = AHeader.NumAnimations > 0 ? PushArray(&Renderer.AnimationArena, AHeader.NumAnimations, skeletal_animation) : 0;
        
        for(i32 Index = 0; Index < Model->AnimationCount; Index++)
        {
            animation_channel_header ACHeader;
            fread(&ACHeader, sizeof(animation_channel_header), 1, File);
            
            skeletal_animation* Animation = &Model->Animations[Index];
            Animation->Duration = ACHeader.Duration;
            Animation->NumBoneChannels = ACHeader.NumBoneChannels;
            
            Animation->BoneChannels = PushArray(&Renderer.AnimationArena, Animation->NumBoneChannels, bone_channel);
            
            for(i32 BoneChannelIndex = 0; BoneChannelIndex < Animation->NumBoneChannels; BoneChannelIndex++)
            {
                bone_animation_header BAHeader;
                fread(&BAHeader, sizeof(bone_animation_header), 1, File);
                
                bone_channel* BoneChannel = &Animation->BoneChannels[BoneChannelIndex];
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