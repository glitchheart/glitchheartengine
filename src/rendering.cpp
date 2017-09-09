enum Camera_Flags
{
    CFlag_Orthographic = (1 << 0),
    CFlag_Perspective = (1 << 1),
};

// @Incomplete
static inline void CameraTransform(renderer& Renderer, camera& Camera, math::v3 Position = math::v3(), math::quat Orientation = math::quat(), math::v3 Target = math::v3(), r32 Zoom = 1.0f, r32 Near = -1.0f, r32 Far = 1.0f, u32 CameraFlags = CFlag_Orthographic)
{
    if(CameraFlags & CFlag_Orthographic)
    {
        Camera.ProjectionMatrix = math::Ortho(0.0f, Renderer.Viewport[2] / Zoom, 0.0f, Renderer.Viewport[3] / Zoom, Near, Far);
        Camera.ViewMatrix = math::m4(1.0f);
        
        if(!IsIdentity(Orientation))
        {
            Camera.ViewMatrix = ToMatrix(Orientation) * Camera.ViewMatrix;
        }
        
        auto Dist = sqrt(1.0f / 3.0f);
        
        Camera.ViewMatrix = math::LookAt(math::v3(Dist, Dist, Dist), math::v3(0.0f));
        
        auto Pos = math::YRotate(-45.0f) * Position;
        
        auto Factor = 1.0f - 35.264f / 90.0f;
        
        Camera.ViewMatrix = math::Translate(Camera.ViewMatrix, math::v3(-Pos.x, Pos.z * Factor, 0.0f));
        
        Camera.ViewMatrix = math::Translate(Camera.ViewMatrix, math::v3(Renderer.Viewport[2] / Zoom / 2, Renderer.Viewport[3] / Zoom / 2, 0.0f));
        
        Camera.Position = Position;
        Camera.Orientation = Orientation;
        Camera.Target = Target;
    }
    else if(CameraFlags & CFlag_Perspective)
    {
        Camera.ProjectionMatrix = math::Perspective((r32)Renderer.Viewport[2] / (r32)Renderer.Viewport[3], 0.60f, Near, Far);
        
        Camera.ViewMatrix = math::m4(1.0f);
        Camera.ViewMatrix = math::Translate(Camera.ViewMatrix, Camera.Center);
        
        Camera.ViewMatrix = math::LookAt(Camera.Center, math::v3(-Target.x, Target.y, -Target.z));
    }
}

static void LoadTexture(char* TextureName, const char* FullTexturePath, renderer& Renderer, memory_arena* PermArena)
{
    texture_data* TextureData = &Renderer.TextureData[Renderer.TextureCount];
    
    TextureData->Handle = Renderer.TextureCount++;
    
    TextureData->ImageData = stbi_load(FullTexturePath, &TextureData->Width, &TextureData->Height, 0, STBI_rgb_alpha);
    
    TextureData->Name = PushString(PermArena, strlen(TextureName), TextureName);
    
    Renderer.TextureMap[TextureName] = TextureData;
}

static void LoadTextures(renderer& Renderer, memory_arena* PermArena)
{
    texture_data_Map_Init(&Renderer.TextureMap, HashStringJenkins, 64);
    
    directory_data DirData = {};
    Platform.GetAllFilesWithExtension("../assets/textures/", "png", &DirData, true);
    
    for (i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        LoadTexture(DirData.FileNames[FileIndex], DirData.FilePaths[FileIndex], Renderer, PermArena);
    }
}

static render_command* PushNextCommand(renderer& Renderer, b32 IsUI)
{
    if(IsUI)
    {
        Renderer.UICommandCount++;
        return PushStruct(&Renderer.UICommands, render_command);
    }
    else
    {
        Renderer.CommandCount++;
        return PushStruct(&Renderer.Commands, render_command);
    }
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
static void PushText(renderer& Renderer, const char* Text, math::v3 Position, Font_Type FontType, math::rgba Color, Alignment Alignment = Alignment_Left, b32 IsUI = true)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Text;
    
    strcpy(RenderCommand->Text.Text, Text);
    
    RenderCommand->Text.Position = Position;
    RenderCommand->Text.FontType = FontType;
    RenderCommand->Text.Color = Color;
    RenderCommand->Text.Alignment = Alignment;
    RenderCommand->IsUI = IsUI;
}

static void PushFilledRect(renderer& Renderer, math::v3 Position, math::v3 Size, math::rgba Color, b32 IsUI = true)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Rect;
    RenderCommand->Rect.Position = Position;
    RenderCommand->Rect.Size = Size;
    RenderCommand->Rect.Color = Color;
    RenderCommand->Rect.Outlined = false;
    RenderCommand->IsUI = IsUI;
}


static void PushFilledRect(renderer& Renderer, rect Rect, math::rgba Color, b32 IsUI = true)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Rect;
    RenderCommand->Rect.Position = math::v3(Rect.X, Rect.Y, 0);
    RenderCommand->Rect.Size = math::v3(Rect.Width, Rect.Height, 0);
    RenderCommand->Rect.Color = Color;
    RenderCommand->Rect.Outlined = false;
    RenderCommand->IsUI = IsUI;
}


static void PushOutlinedRect(renderer& Renderer, math::v3 Position, math::v3 Size, math::rgba Color, b32 IsUI = false)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Rect;
    RenderCommand->Rect.Position = Position;
    RenderCommand->Rect.Size = Size;
    RenderCommand->Rect.Color = Color;
    RenderCommand->Rect.Outlined = true;
    RenderCommand->IsUI = IsUI;
}

static void PushWireframeCube(renderer& Renderer, math::v3 Position, math::v3 Scale, math::quat Orientation, math::rgba Color)
{
    render_command* RenderCommand = PushNextCommand(Renderer, false);
    
    RenderCommand->Type = RenderCommand_WireframeCube;
    RenderCommand->WireframeCube.Color = Color;
    RenderCommand->Position = Position;
    RenderCommand->Scale = Scale;
    RenderCommand->Orientation = Orientation;
    RenderCommand->IsUI = false;
}

static void PushSprite(renderer& Renderer, math::v3 Position, math::v3 Scale, math::v2 Frame, math::v2 TextureOffset, const char* TextureName, math::rgba Color, b32 IsUI = false)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Sprite;
    RenderCommand->Sprite.Position = Position;
    RenderCommand->Sprite.Scale = Scale;
    RenderCommand->Sprite.Frame = Frame;
    RenderCommand->Sprite.TextureOffset = TextureOffset;
    
    RenderCommand->Sprite.TextureName = PushTempString(TextureName);
    
    RenderCommand->Sprite.Color = Color;
    RenderCommand->IsUI = IsUI;
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

static void PushBuffer(renderer& Renderer, i32 BufferHandle, char* TextureName, math::v3 Rotation, b32 IsUI = false)
{
    render_command* RenderCommand = PushNextCommand(Renderer, IsUI);
    
    RenderCommand->Type = RenderCommand_Buffer;
    RenderCommand->Buffer.BufferHandle = BufferHandle;
    RenderCommand->Buffer.TextureName = TextureName;
    RenderCommand->Rotation = Rotation;
    RenderCommand->IsUI = IsUI;
}

static void PushModel(renderer& Renderer, model& Model)
{
    render_command* RenderCommand = PushNextCommand(Renderer, false);
    RenderCommand->Type = RenderCommand_Model;
    RenderCommand->Position = Model.Position;
    RenderCommand->Scale = Model.Scale;
    RenderCommand->Orientation = Model.Orientation;
    
    for (i32 Index = 0; Index < Model.MeshCount; Index++)
    {
        mesh_render_data RenderInfo;
        RenderInfo.BufferHandle = Model.Meshes[Index].BufferHandle;
        RenderInfo.Material = Model.Meshes[Index].Material;
        RenderCommand->Model.RenderData[Index] = RenderInfo;
    }
    
    RenderCommand->Model.HandleCount = Model.MeshCount;
    RenderCommand->Model.BoneCount = Model.BoneCount;
    RenderCommand->Model.BoneTransforms = PushTempSize(sizeof(math::m4) * Model.BoneCount, math::m4);
    
    for(i32 Index = 0; Index < Model.BoneCount; Index++)
    {
        RenderCommand->Model.BoneTransforms[Index] = Model.CurrentPoses[Index];
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
        
        if(strcmp(Header.Version, "1.5") != 0)
        {
            ERR("Wrong file version. Expected version 1.5");
            return;
        }
        
        model_data ModelData;
        fread(&ModelData, sizeof(model_data), 1, File);
        
        mesh_data* MeshData;
        MeshData = PushTempSize(ModelData.MeshChunkSize, mesh_data);
        fread(MeshData, ModelData.MeshChunkSize, 1, File);
        
        r32* VertexBuffer = PushTempSize(ModelData.VertexBufferChunkSize, r32);
        fread(VertexBuffer, ModelData.VertexBufferChunkSize, 1, File);
        u32* IndexBuffer = PushTempSize(ModelData.IndexBufferChunkSize, u32);
        fread(IndexBuffer, ModelData.IndexBufferChunkSize, 1, File);
        
        Model->GlobalInverseTransform = ModelData.GlobalInverseTransform;
        Model->Bones = PushArray(&Renderer.AnimationArena, ModelData.NumBones, bone);
        Model->BoneCount = ModelData.NumBones;
        Model->CurrentPoses = PushArray(&Renderer.AnimationArena, ModelData.NumBones, math::m4);
        fread(Model->Bones, ModelData.BoneChunkSize, 1, File);
        
        buffer_data Data = {};
        
        CopyTemp(Data.VertexBuffer, VertexBuffer, ModelData.VertexBufferChunkSize, r32);
        CopyTemp(Data.IndexBuffer, IndexBuffer, ModelData.IndexBufferChunkSize, u32);
        
        // @Incomplete: Do we really always have normals and uvs?????
        Data.HasNormals = true;
        Data.HasUVs = true;
        
        Data.VertexBufferSize = ModelData.VertexBufferChunkSize;
        Data.IndexBufferSize = ModelData.IndexBufferChunkSize;
        Data.IndexBufferCount = ModelData.NumIndices;
        
        Model->Meshes[0].BufferHandle = Renderer.BufferCount++;
        
        Model->AnimationState.Playing = false;
        Model->AnimationState.Loop = false;
        Model->AnimationState.CurrentTime = 0.0f;
        
        // @Incomplete: IMPORTANT
        // @Incomplete: IMPORTANT
        // @Incomplete: IMPORTANT
        // @Incomplete: IMPORTANT
        // @Incomplete: IMPORTANT
        // @Incomplete: IMPORTANT
        Model->Meshes[0].Material.HasTexture = false;
        
        //if(Model->Meshes[MeshCount].Material.HasTexture)
        //Model->Meshes[MeshCount].Material.TextureHandle = Renderer.TextureMap[MHeader.TextureFile]->Handle;
        
        Model->Meshes[0].Material.Color = math::rgba(1, 1, 1, 1);
        
        //Assert(MeshCount <= MAX_MESHES);
        
        Renderer.Buffers[Renderer.BufferCount - 1] = Data;
        
        Model->MeshCount = 1;
        
        // Load animations
        animation_header AHeader;
        fread(&AHeader, sizeof(animation_header), 1, File);
        
        Model->AnimationCount = AHeader.NumAnimations;
        Model->Animations = PushArray(&Renderer.AnimationArena, AHeader.NumAnimations, skeletal_animation);
        
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