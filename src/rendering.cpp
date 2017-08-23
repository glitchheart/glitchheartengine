static void LoadTexture(char* TextureName, const char* FullTexturePath, renderer& Renderer, memory_arena* PermArena)
{
    texture_data* TextureData = &Renderer.TextureData[Renderer.TextureCount];
    
    TextureData->Handle = Renderer.TextureCount++;
    
    TextureData->ImageData = stbi_load(FullTexturePath, &TextureData->Width, &TextureData->Height, 0, STBI_rgb_alpha);
    
    TextureData->Name = PushString(PermArena, strlen(TextureName), TextureName);
    
    Renderer.TextureMap[TextureName] = TextureData;
}

static void LoadTextures(renderer& Renderer, memory_arena* TempArena)
{
    texture_data_Map_Init(&Renderer.TextureMap, HashStringJenkins, 64);
    
    directory_data DirData = {};
    Platform.GetAllFilesWithExtension("../assets/textures/", "png", &DirData, true, TempArena);
    
    for (i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        LoadTexture(DirData.FileNames[FileIndex], DirData.FilePaths[FileIndex], Renderer, TempArena);
    }
}

static void PushLine(renderer& Renderer, math::v3 Point1, math::v3 Point2, r32 LineWidth, math::rgba Color, b32 IsUI = false)
{
    render_command* RenderCommand = PushStruct(&Renderer.Buffer, render_command);
    Renderer.CommandCount++;
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
    render_command* RenderCommand = PushStruct(&Renderer.Buffer, render_command);
    Renderer.CommandCount++;
    RenderCommand->Type = RenderCommand_Text;
    
    strcpy(RenderCommand->Text.Text, Text);
    
    RenderCommand->Text.Position = Position;
    RenderCommand->Text.FontHandle = FontHandle;
    RenderCommand->Text.Color = Color;
    RenderCommand->Text.Alignment = Alignment;
    RenderCommand->IsUI = IsUI;
}

static void PushFilledRect(renderer& Renderer, math::v3 Position, math::v3 Size, math::rgba Color, b32 IsUI = true)
{
    render_command* RenderCommand = PushStruct(&Renderer.Buffer, render_command);
    Renderer.CommandCount++;
    
    RenderCommand->Type = RenderCommand_Rect;
    RenderCommand->Rect.Position = Position;
    RenderCommand->Rect.Size = Size;
    RenderCommand->Rect.Color = Color;
    RenderCommand->Rect.Outlined = false;
    RenderCommand->IsUI = IsUI;
}

static void PushOutlinedRect(renderer& Renderer, math::v3 Position, math::v3 Size, math::rgba Color, b32 IsUI = false)
{
    render_command* RenderCommand = PushStruct(&Renderer.Buffer, render_command);
    Renderer.CommandCount++;
    
    RenderCommand->Type = RenderCommand_Rect;
    RenderCommand->Rect.Position = Position;
    RenderCommand->Rect.Size = Size;
    RenderCommand->Rect.Color = Color;
    RenderCommand->Rect.Outlined = true;
    RenderCommand->IsUI = IsUI;
}

static void PushSprite(renderer& Renderer, math::v3 Position, math::v3 Scale, math::v2 Frame, math::v2 TextureOffset, const char* TextureName, math::rgba Color, memory_arena* TempArena, b32 IsUI = false)
{
    render_command* RenderCommand = PushStruct(&Renderer.Buffer, render_command);
    Renderer.CommandCount++;
    
    RenderCommand->Type = RenderCommand_Sprite;
    RenderCommand->Sprite.Position = Position;
    RenderCommand->Sprite.Scale = Scale;
    RenderCommand->Sprite.Frame = Frame;
    RenderCommand->Sprite.TextureOffset = TextureOffset;
    
    RenderCommand->Sprite.TextureName = PushString(TempArena, (u32)strlen(TextureName), TextureName);
    
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

static void PushBuffer(renderer& Renderer, i32 BufferHandle, char* TextureName, math::v3 Rotation)
{
    render_command* RenderCommand = PushStruct(&Renderer.Buffer, render_command);
    Renderer.CommandCount++;
    
    RenderCommand->Type = RenderCommand_Buffer;
    RenderCommand->Buffer.BufferHandle = BufferHandle;
    RenderCommand->Buffer.TextureName = TextureName;
    RenderCommand->Rotation = Rotation;
    RenderCommand->IsUI = false;
}

static void PushModel(renderer& Renderer, model& Model)
{
    render_command* RenderCommand = PushStruct(&Renderer.Buffer, render_command);
    Renderer.CommandCount++;
    RenderCommand->Type = RenderCommand_Model;
    RenderCommand->Position = Model.Position;
    RenderCommand->Scale = Model.Scale;
    RenderCommand->Rotation = Model.Rotation;
    
    for (i32 Index = 0; Index < Model.MeshCount; Index++)
    {
        mesh_render_data RenderInfo;
        RenderInfo.BufferHandle = Model.Meshes[Index].BufferHandle;
        RenderInfo.Material = Model.Meshes[Index].Material;
        RenderCommand->Model.RenderData[Index] = RenderInfo;
    }
    
    RenderCommand->Model.HandleCount = Model.MeshCount;
    
    RenderCommand->Model.Color = math::rgba(1.0f, 1.0f, 1.0f, 1.0f);
    RenderCommand->IsUI = false;
}

static void LoadBuffer(renderer& Renderer, r32* Buffer, i32 BufferSize, i32* BufferHandle)
{
    buffer_data Data = {};
    Data.VertexBuffer = Buffer;
    Data.VertexBufferSize = BufferSize;
    Data.IndexBufferSize = 0;
    Renderer.Buffers[Renderer.BufferCount] = Data;
    
    *BufferHandle = Renderer.BufferCount++;
}


static b32 IsEOF(chunk_format& Format)
{
    return Format.Format[0] == 'E' &&
        Format.Format[1] == 'O' &&
        Format.Format[2] == 'F' && 
        Format.Format[3] == ' ';
}

static void LoadModel(renderer& Renderer, char* FilePath, model* Model, memory_arena* TempArena)
{
    model_header Header = {};
    
    FILE *File = fopen(FilePath, "rb");
    if(File)
    {
        fread(&Header,sizeof(model_header), 1, File);
        
        if(Header.Version[0] != '1' ||
           Header.Version[1] != '2')
        {
            ERR("Wrong file version. Expected version 1.2");
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
               Format.Format[3] == 'H') // Checking version number
            {
                buffer_data Data = {};
                
                mesh_header MHeader;
                
                fread(&MHeader, sizeof(mesh_header), 1, File);
                
                r32* VertexBuffer = PushSize(TempArena, MHeader.VertexChunkSize, r32);
                
                fread(VertexBuffer, MHeader.VertexChunkSize, 1, File);
                
                Data.HasNormals = MHeader.NumNormals > 0;
                Data.HasUVs = MHeader.NumUVs > 0;
                
                i32* IndexBuffer = PushSize(TempArena, MHeader.FacesChunkSize, i32);
                
                fread(IndexBuffer, MHeader.FacesChunkSize, 1, File);
                
                Copy(Data.VertexBuffer, VertexBuffer, MHeader.VertexChunkSize, TempArena, r32);
                
                Data.VertexBufferSize = MHeader.NumVertices * 3 + MHeader.NumNormals * 3 + MHeader.NumUVs * 2;
                
                Copy(Data.IndexBuffer, IndexBuffer, MHeader.FacesChunkSize, TempArena, u32);
                
                Data.IndexBufferSize = MHeader.NumFaces * 3;
                
                Model->Meshes[MeshCount].BufferHandle = Renderer.BufferCount++;
                
                Model->Meshes[MeshCount].Material.HasTexture = MHeader.HasTexture;
                if(Model->Meshes[MeshCount].Material.HasTexture)
                    Model->Meshes[MeshCount].Material.TextureHandle = Renderer.TextureMap[MHeader.TextureFile]->Handle;
                
                Model->Meshes[MeshCount].Material.Color = math::rgba(1, 1, 1, 1);
                MeshCount++;
                
                Assert(MeshCount <= MAX_MESHES);
                
                Renderer.Buffers[Renderer.BufferCount - 1] = Data;
            }
            else
            {
                ERR("Malformed model file");
                break;
            }
            fread(&Format, sizeof(chunk_format), 1, File);
        }
        
        Model->MeshCount = MeshCount;
        
        fclose(File);
    }
}