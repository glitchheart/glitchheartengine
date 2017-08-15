static void LoadTexture(char* TextureName, const char* FullTexturePath, renderer& Renderer)
{
    texture_data* TextureData = &Renderer.TextureData[Renderer.TextureCount];
    
    TextureData->Handle = Renderer.TextureCount++;
    
    TextureData->ImageData = stbi_load(FullTexturePath, &TextureData->Width, &TextureData->Height, 0, STBI_rgb_alpha);
    
    TextureData->Name = (char*)malloc((strlen(TextureName) + 1) * sizeof(char));
    strcpy(TextureData->Name, TextureName);
    
    Renderer.TextureMap[TextureName] = TextureData;
}

static void LoadTextures(renderer& Renderer)
{
    texture_data_Map_Init(&Renderer.TextureMap, HashStringJenkins, 64);
    
    directory_data DirData = {};
    FindFilesWithExtensions("../assets/textures/", "png", &DirData, true);
    
    for (i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        LoadTexture(DirData.FileNames[FileIndex], DirData.FilePaths[FileIndex], Renderer);
    }
    
    free(DirData.FilePaths);
    free(DirData.FileNames);
}

static void PushLine(renderer& Renderer, math::v3 Point1, math::v3 Point2, r32 LineWidth, math::rgba Color, b32 IsUI = false)
{
    render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
    RenderCommand->Type = RenderCommand_Line;
    RenderCommand->Line.Point1 = Point1;
    RenderCommand->Line.Point2 = Point2;
    RenderCommand->Line.LineWidth = LineWidth;
    RenderCommand->Line.Color = Color;
    RenderCommand->IsUI = IsUI;
}

static void PushText(renderer& Renderer, const char* Text, math::v3 Position, i32 FontHandle, math::rgba Color, Alignment Alignment = Alignment_Left, b32 IsUI = true)
{
    render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
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
    render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
    RenderCommand->Type = RenderCommand_Rect;
    RenderCommand->Rect.Position = Position;
    RenderCommand->Rect.Size = Size;
    RenderCommand->Rect.Color = Color;
    RenderCommand->Rect.Outlined = false;
    RenderCommand->IsUI = IsUI;
}

static void PushOutlinedRect(renderer& Renderer, math::v3 Position, math::v3 Size, math::rgba Color, b32 IsUI = false)
{
    render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
    RenderCommand->Type = RenderCommand_Rect;
    RenderCommand->Rect.Position = Position;
    RenderCommand->Rect.Size = Size;
    RenderCommand->Rect.Color = Color;
    RenderCommand->Rect.Outlined = true;
    RenderCommand->IsUI = IsUI;
}

static void PushSprite(renderer& Renderer, math::v3 Position, math::v3 Scale, math::v2 Frame, math::v2 TextureOffset, const char* TextureName, math::rgba Color, b32 IsUI = false)
{
    render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
    RenderCommand->Type = RenderCommand_Sprite;
    RenderCommand->Sprite.Position = Position;
    RenderCommand->Sprite.Scale = Scale;
    RenderCommand->Sprite.Frame = Frame;
    RenderCommand->Sprite.TextureOffset = TextureOffset;
    
    if (RenderCommand->Sprite.TextureName)
    {
        free(RenderCommand->Sprite.TextureName);
    }
    
    RenderCommand->Sprite.TextureName = (char*)malloc(sizeof(char) * strlen(TextureName) + 1);
    strcpy(RenderCommand->Sprite.TextureName, TextureName);
    
    RenderCommand->Sprite.Color = Color;
    RenderCommand->IsUI = IsUI;
}

static void PushBuffer(renderer& Renderer, i32 BufferHandle, char* TextureName, math::v3 Rotation)
{
    render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
    RenderCommand->Type = RenderCommand_Buffer;
    RenderCommand->Buffer.BufferHandle = BufferHandle;
    RenderCommand->Buffer.TextureName = TextureName;
    RenderCommand->Rotation = Rotation;
    RenderCommand->IsUI = false;
}

static void PushModel(renderer& Renderer, model& Model)
{
    render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
    RenderCommand->Type = RenderCommand_Model;
    RenderCommand->Position = Model.Position;
    RenderCommand->Scale = Model.Scale;
    RenderCommand->Rotation = Model.Rotation;
    
    for (i32 Index = 0; Index < Model.MeshCount; Index++)
    {
        RenderCommand->Model.BufferHandles[Index] = Model.Meshes[Index].BufferHandle;
    }
    
    RenderCommand->Model.HandleCount = Model.MeshCount;
    
    RenderCommand->Model.Color = math::rgba(1.0f, 0.0f, 0.0f, 1.0f);
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

static void LoadModel(renderer& Renderer, char* FilePath, model* Model)
{
    model_header Header = {};
    
    FILE *File = fopen(FilePath, "rb");
    if(File)
    {
        fread(&Header,sizeof(model_header), 1, File);
        printf("%c%c%c%c\n", Header.Format[0],Header.Format[1], Header.Format[2], Header.Format[3]);
        
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
                mesh_header MHeader;
                fread(&MHeader, sizeof(mesh_header), 1, File);
                
                printf("%ld\n", MHeader.NumVertices);
                printf("%ld\n", MHeader.VertexChunkSize);
                
                printf("%ld\n", MHeader.NumFaces);
                printf("%ld\n", MHeader.FacesChunkSize);
                
                r32* VertexBuffer = (r32*)malloc(MHeader.VertexChunkSize);
                fread(VertexBuffer, MHeader.VertexChunkSize, 1, File);
                
                i32* IndexBuffer = (i32*)malloc(MHeader.FacesChunkSize);
                fread(IndexBuffer, MHeader.FacesChunkSize, 1, File);
                
                buffer_data Data = {};
                
                Data.VertexBuffer = (r32*)malloc(MHeader.VertexChunkSize);
                memcpy(Data.VertexBuffer, VertexBuffer, MHeader.VertexChunkSize);
                Data.VertexBufferSize = MHeader.NumVertices * 3;
                
                Data.IndexBuffer = (u32*)malloc(MHeader.FacesChunkSize);
                memcpy(Data.IndexBuffer, IndexBuffer, MHeader.FacesChunkSize);
                
                Data.IndexBufferSize = MHeader.NumFaces * 3;
                Model->Meshes[MeshCount++].BufferHandle = Renderer.BufferCount++;
                Renderer.Buffers[Renderer.BufferCount - 1] = Data;
                
                free(VertexBuffer);
                free(IndexBuffer);
            }
            else
            {
                ERR("Malformed model file");
                break;
            }
            fread(&Format, sizeof(chunk_format), 1, File);
        }
        
        printf("MeshCount: %d\n", MeshCount);
        
        Model->MeshCount = MeshCount;
        
        printf("End of file\n");
        
        fclose(File);
    }
}


static void LoadOBJFile(renderer& Renderer, char* FilePath, model* Model)
{
    FILE* File = fopen(FilePath, "r");
    
    char LineBuffer[256];
    
    if (File)
    {
        i32 VertexSize[100] = { 0 };
        i32 IndexSize[100] = { 0 };
        i32 NormalSize[100] = { 0 };
        i32 UVSize[100] = { 0 };
        
        i32 MeshCount = 1;
        b32 FacesReached = false;
        
        i32 LineNumbersForMeshes[100] = { 0 };
        
        while (fgets(LineBuffer, 256, File))
        {
            if (StartsWith(LineBuffer, "v ")) // Vertex
            {
                if (FacesReached)
                {
                    MeshCount++;
                    FacesReached = false;
                }
                
                VertexSize[MeshCount - 1]++;
            }
            else if (StartsWith(LineBuffer, "f ")) // Face
            {
                FacesReached = true;
                IndexSize[MeshCount - 1]++;
            }
            else if (StartsWith(LineBuffer, "vn ")) // Normal
            {
                NormalSize[MeshCount - 1]++;
            }
            else if (StartsWith(LineBuffer, "vt ")) // UV
            {
                UVSize[MeshCount - 1]++;
            }
            
            LineNumbersForMeshes[MeshCount - 1]++;
        }
        
        printf("Mesh count %d\n", MeshCount);
        
        r32** TempVertices = (r32**)calloc(MeshCount, sizeof(r32*));
        r32** TempNormals = (r32**)calloc(MeshCount, sizeof(r32*));
        r32** TempUVs = (r32**)calloc(MeshCount, sizeof(r32*));
        
        u32** VertexIndices = (u32**)malloc(sizeof(u32*) * MeshCount);
        u32** NormalIndices = (u32**)malloc(sizeof(u32*) * MeshCount);
        u32** UVIndices = (u32**)malloc(sizeof(u32*) * MeshCount);
        
        for (i32 I = 0; I < MeshCount; I++)
        {
            TempVertices[I] = (r32*)malloc(sizeof(r32) * VertexSize[I] * 3);
            TempNormals[I] = (r32*)malloc(sizeof(r32) * NormalSize[I] * 3);
            TempUVs[I] = (r32*)malloc(sizeof(r32) * UVSize[I] * 2);
            
            VertexIndices[I] = (u32*)malloc(sizeof(u32) * IndexSize[I] * 3);
            NormalIndices[I] = (u32*)malloc(sizeof(u32) * IndexSize[I] * 3);
            UVIndices[I] = (u32*)malloc(sizeof(u32) * IndexSize[I] * 3);
        }
        
        rewind(File);
        
        Model->MeshCount = MeshCount;
        
        i32 VerticesIndex = 0;
        i32 NormalIndex = 0;
        i32 UVIndex = 0;
        
        i32 IndexCount = 0;
        
        for (i32 MeshIndex = 0; MeshIndex < MeshCount; MeshIndex++)
        {
            buffer_data Data = {};
            
            VerticesIndex = 0;
            NormalIndex = 0;
            UVIndex = 0;
            IndexCount = 0;
            
            Model->Meshes[MeshIndex].BufferHandle = Renderer.BufferCount++;
            printf("BufferCount %d\n", Renderer.BufferCount);
            
            Data.HasNormals = NormalSize[MeshIndex] > 0;
            Data.HasUVs = UVSize[MeshIndex] > 0;
            
            i32 LineNumber = 0;
            
            while (fgets(LineBuffer, 256, File))
            {
                if (StartsWith(LineBuffer, "v "))
                {
                    sscanf(LineBuffer, "v %f %f %f", &TempVertices[MeshIndex][VerticesIndex * 3], &TempVertices[MeshIndex][VerticesIndex * 3 + 1], &TempVertices[MeshIndex][VerticesIndex * 3 + 2]);
                    VerticesIndex++;
                }
                else if (StartsWith(LineBuffer, "vn "))
                {
                    sscanf(LineBuffer, "vn %f %f %f", &TempNormals[MeshIndex][NormalIndex * 3], &TempNormals[MeshIndex][NormalIndex * 3 + 1], &TempNormals[MeshIndex][NormalIndex * 3 + 2]);
                    NormalIndex++;
                }
                else if (StartsWith(LineBuffer, "vt "))
                {
                    sscanf(LineBuffer, "vt %f %f", &TempUVs[MeshIndex][UVIndex * 2], &TempUVs[MeshIndex][UVIndex * 2 + 1]);
                    UVIndex++;
                }
                else if (StartsWith(LineBuffer, "f "))
                {
                    if (NormalSize[MeshIndex] > 0 && UVSize[MeshIndex] > 0)
                    {
                        sscanf(LineBuffer, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                               &VertexIndices[MeshIndex][IndexCount * 3],
                               &UVIndices[MeshIndex][IndexCount * 3],
                               &NormalIndices[MeshIndex][IndexCount * 3],
                               &VertexIndices[MeshIndex][IndexCount * 3 + 1],
                               &UVIndices[MeshIndex][IndexCount * 3 + 1],
                               &NormalIndices[MeshIndex][IndexCount * 3 + 1],
                               &VertexIndices[MeshIndex][IndexCount * 3 + 2],
                               &UVIndices[MeshIndex][IndexCount * 3 + 2],
                               &NormalIndices[MeshIndex][IndexCount * 3 + 2]);
                        
                        VertexIndices[MeshIndex][IndexCount * 3] -= 1;
                        VertexIndices[MeshIndex][IndexCount * 3 + 1] -= 1;
                        VertexIndices[MeshIndex][IndexCount * 3 + 2] -= 1;
                        UVIndices[MeshIndex][IndexCount * 3] -= 1;
                        UVIndices[MeshIndex][IndexCount * 3 + 1] -= 1;
                        UVIndices[MeshIndex][IndexCount * 3 + 2] -= 1;
                        NormalIndices[MeshIndex][IndexCount * 3] -= 1;
                        NormalIndices[MeshIndex][IndexCount * 3 + 1] -= 1;
                        NormalIndices[MeshIndex][IndexCount * 3 + 2] -= 1;
                    }
                    else if (UVSize[MeshIndex] > 0)
                    {
                        sscanf(LineBuffer, "f %d/%d %d/%d %d/%d\n",
                               &VertexIndices[MeshIndex][IndexCount * 3],
                               &UVIndices[MeshIndex][IndexCount * 3],
                               &VertexIndices[MeshIndex][IndexCount * 3 + 1],
                               &UVIndices[MeshIndex][IndexCount * 3 + 1],
                               &VertexIndices[MeshIndex][IndexCount * 3 + 2],
                               &UVIndices[MeshIndex][IndexCount * 3 + 2]);
                        
                        VertexIndices[MeshIndex][IndexCount * 3] -= 1;
                        VertexIndices[MeshIndex][IndexCount * 3 + 1] -= 1;
                        VertexIndices[MeshIndex][IndexCount * 3 + 2] -= 1;
                        UVIndices[MeshIndex][IndexCount * 3] -= 1;
                        UVIndices[MeshIndex][IndexCount * 3 + 1] -= 1;
                        UVIndices[MeshIndex][IndexCount * 3 + 2] -= 1;
                    }
                    else if (NormalSize[MeshIndex] > 0)
                    {
                        sscanf(LineBuffer, "f %d//%d %d//%d %d//%d\n",
                               &VertexIndices[MeshIndex][IndexCount * 3],
                               &NormalIndices[MeshIndex][IndexCount * 3],
                               &VertexIndices[MeshIndex][IndexCount * 3 + 1],
                               &NormalIndices[MeshIndex][IndexCount * 3 + 1],
                               &VertexIndices[MeshIndex][IndexCount * 3 + 2],
                               &NormalIndices[MeshIndex][IndexCount * 3 + 2]);
                        
                        VertexIndices[MeshIndex][IndexCount * 3] -= 1;
                        VertexIndices[MeshIndex][IndexCount * 3 + 1] -= 1;
                        VertexIndices[MeshIndex][IndexCount * 3 + 2] -= 1;
                        NormalIndices[MeshIndex][IndexCount * 3] -= 1;
                        NormalIndices[MeshIndex][IndexCount * 3 + 1] -= 1;
                        NormalIndices[MeshIndex][IndexCount * 3 + 2] -= 1;
                    }
                    else
                    {
                        sscanf(LineBuffer, "f %d %d %d",
                               &VertexIndices[MeshIndex][IndexCount * 3],
                               &VertexIndices[MeshIndex][IndexCount * 3 + 1],
                               &VertexIndices[MeshIndex][IndexCount * 3 + 2]);
                        
                        VertexIndices[MeshIndex][IndexCount * 3] -= 1;
                        VertexIndices[MeshIndex][IndexCount * 3 + 1] -= 1;
                        VertexIndices[MeshIndex][IndexCount * 3 + 2] -= 1;
                    }
                    IndexCount++;
                }
                
                LineNumber++;
                
                if (LineNumbersForMeshes[MeshIndex] == LineNumber)
                {
                    break;
                }
            }
            
            if (NormalSize[MeshIndex] == UVSize[MeshIndex] && (NormalSize[MeshIndex] == VertexSize[MeshIndex] || UVSize[MeshIndex] == VertexSize[MeshIndex] || NormalSize[MeshIndex] == 0 && UVSize[MeshIndex] == 0))
            {
                Data.VertexBuffer = (r32*)malloc(sizeof(r32) * (VertexSize[MeshIndex] * 3 + NormalSize[MeshIndex] * 3 + UVSize[MeshIndex] * 2));
                Data.IndexBuffer = (u32*)malloc(sizeof(u32) * IndexSize[MeshIndex] * 3);
                Data.VertexBufferSize = VertexSize[MeshIndex] * 3 + NormalSize[MeshIndex] * 3 + UVSize[MeshIndex] * 2;
                Data.IndexBufferSize = IndexSize[MeshIndex] * 3;
                
                memcpy(Data.IndexBuffer, VertexIndices[MeshIndex], sizeof(u32) * Data.IndexBufferSize);
                
                for (i32 Index = 0; Index < VertexSize[MeshIndex]; Index++)
                {
                    if (Data.HasNormals && Data.HasUVs)
                    {
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 8];
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 8 + 1];
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 8 + 2];
                        
                        *Data.VertexBuffer++ = TempNormals[MeshIndex][Index * 8];
                        *Data.VertexBuffer++ = TempNormals[MeshIndex][Index * 8 + 1];
                        *Data.VertexBuffer++ = TempNormals[MeshIndex][Index * 8 + 2];
                        
                        *Data.VertexBuffer++ = TempUVs[MeshIndex][Index * 8];
                        *Data.VertexBuffer++ = TempUVs[MeshIndex][Index * 8 + 1];
                    }
                    else if (Data.HasNormals)
                    {
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 6];
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 6 + 1];
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 6 + 2];
                        
                        *Data.VertexBuffer++ = TempNormals[MeshIndex][Index * 6];
                        *Data.VertexBuffer++ = TempNormals[MeshIndex][Index * 6 + 1];
                        *Data.VertexBuffer++ = TempNormals[MeshIndex][Index * 6 + 2];
                    }
                    else if (Data.HasUVs)
                    {
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 5];
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 5 + 1];
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 5 + 2];
                        
                        *Data.VertexBuffer++ = TempUVs[MeshIndex][Index * 5];
                        *Data.VertexBuffer++ = TempUVs[MeshIndex][Index * 5 + 1];
                    }
                    else
                    {
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 3];
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 3 + 1];
                        *Data.VertexBuffer++ = TempVertices[MeshIndex][Index * 3 + 2];
                    }
                }
                
                Data.VertexBuffer -= Data.VertexBufferSize;
            }
            else
            {
                r32* TempVertexBuffer = (r32*)malloc(sizeof(r32) * IndexSize[MeshIndex] * 3 * 8);
                Data.IndexBuffer = (u32*)malloc(sizeof(u32) * IndexSize[MeshIndex] * 3);
                
                u32 ActualVertexCount = 0;
                u32 ActualIndexSize = 0;
                
                for (i32 I = 0; I < IndexSize[MeshIndex] * 3; I++)
                {
                    u32 VIndex = VertexIndices[MeshIndex][I];
                    u32 NIndex = NormalIndices[MeshIndex][I];
                    u32 UVIndex = UVIndices[MeshIndex][I];
                    
                    math::v3 CurrentVertex(TempVertices[MeshIndex][VIndex * 3], TempVertices[MeshIndex][VIndex * 3 + 1], TempVertices[MeshIndex][VIndex * 3 + 2]);
                    math::v3 CurrentNormal(TempNormals[MeshIndex][NIndex * 3], TempNormals[MeshIndex][NIndex * 3 + 1], TempNormals[MeshIndex][NIndex * 3 + 2]);
                    math::v2 CurrentUV(TempUVs[MeshIndex][UVIndex * 3], TempUVs[MeshIndex][UVIndex * 3 + 1]);
                    
                    b32 Found = false;
                    u32 ExistingIndex = 0;
                    
                    for (u32 J = 0; J < ActualIndexSize; J++)
                    {
                        u32 CurrentIndex = Data.IndexBuffer[J];
                        
                        if (Abs(TempVertexBuffer[CurrentIndex * 8] - CurrentVertex.x) < 0.01f
                            && Abs(TempVertexBuffer[CurrentIndex * 8 + 1] - CurrentVertex.y) < 0.01f
                            && Abs(TempVertexBuffer[CurrentIndex * 8 + 2] - CurrentVertex.z) < 0.01f
                            && Abs(TempVertexBuffer[CurrentIndex * 8 + 3] - CurrentNormal.x) < 0.01f
                            && Abs(TempVertexBuffer[CurrentIndex * 8 + 4] - CurrentNormal.y) < 0.01f
                            && Abs(TempVertexBuffer[CurrentIndex * 8 + 5] - CurrentNormal.z) < 0.01f
                            && Abs(TempVertexBuffer[CurrentIndex * 8 + 6] - CurrentUV.x) < 0.01f
                            && Abs(TempVertexBuffer[CurrentIndex * 8 + 7] - CurrentUV.y) < 0.01f)
                        {
                            Found = true;
                            ExistingIndex = CurrentIndex;
                            break;
                        }
                    }
                    
                    if (Found)
                    {
                        Data.IndexBuffer[ActualIndexSize] = ExistingIndex;
                        Found = false;
                    }
                    else
                    {
                        TempVertexBuffer[ActualVertexCount * 8] = CurrentVertex.x;
                        TempVertexBuffer[ActualVertexCount * 8 + 1] = CurrentVertex.y;
                        TempVertexBuffer[ActualVertexCount * 8 + 2] = CurrentVertex.z;
                        TempVertexBuffer[ActualVertexCount * 8 + 3] = CurrentNormal.x;
                        TempVertexBuffer[ActualVertexCount * 8 + 4] = CurrentNormal.y;
                        TempVertexBuffer[ActualVertexCount * 8 + 5] = CurrentNormal.z;
                        TempVertexBuffer[ActualVertexCount * 8 + 6] = CurrentUV.x;
                        TempVertexBuffer[ActualVertexCount * 8 + 7] = CurrentUV.y;
                        
                        Data.IndexBuffer[ActualIndexSize] = ActualVertexCount++;
                    }
                    
                    ActualIndexSize++;
                }
                
                Data.VertexBufferSize = ActualVertexCount * 8;
                Data.VertexBuffer = (r32*)malloc(Data.VertexBufferSize * sizeof(r32));
                memcpy(Data.VertexBuffer, TempVertexBuffer, Data.VertexBufferSize * sizeof(r32));
                Data.IndexBufferSize = ActualIndexSize;
                
                free(TempVertexBuffer);
            }
            
            Renderer.Buffers[Renderer.BufferCount - 1] = Data;
        }
        
        fclose(File);
        
        for (i32 I = 0; I < MeshCount; I++)
        {
            free(TempVertices[I]);
            free(TempNormals[I]);
            free(TempUVs[I]);
            
            free(VertexIndices[I]);
            free(NormalIndices[I]);
            free(UVIndices[I]);
        }
        
        free(TempVertices);
        free(TempNormals);
        free(TempUVs);
        
        free(VertexIndices);
        free(NormalIndices);
        free(UVIndices);
    }
}