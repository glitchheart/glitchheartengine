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
                                              
                                              for(i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
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
                                              
                                              if(RenderCommand->Sprite.TextureName)
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
                                              RenderCommand->Model.Position = Model.Position;
                                              RenderCommand->Model.Scale = Model.Scale;
                                              RenderCommand->Rotation = Model.Rotation;
                                              RenderCommand->Model.BufferHandle = Model.BufferHandle;
                                              RenderCommand->Model.Color = math::rgba(1.0f, 0.0f, 0.0f, 1.0f);
                                              //RenderCommand->Model.TextureName = TextureName;
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
                                          
                                          static void LoadOBJFile(renderer& Renderer, char* FilePath, model* Model)
                                          {
                                              FILE* File = fopen(FilePath, "r");
                                              
                                              char LineBuffer[256];
                                              
                                              if(File)
                                              {
                                                  buffer_data Data = {};
                                                  
                                                  Model->BufferHandle = Renderer.BufferCount++;
                                                  
                                                  i32 VertexSize = 0;
                                                  i32 IndexSize = 0;
                                                  i32 NormalSize = 0;
                                                  i32 UVSize = 0;
                                                  
                                                  while(fgets(LineBuffer, 256, File))
                                                  {
                                                      if(StartsWith(LineBuffer, "v ")) // Vertex
                                                      {
                                                          VertexSize++;
                                                      }
                                                      else if(StartsWith(LineBuffer, "f ")) // Face
                                                      {
                                                          IndexSize++;
                                                      }
                                                      else if(StartsWith(LineBuffer, "vn ")) // Normal
                                                      {
                                                          NormalSize++;
                                                      }
                                                      else if(StartsWith(LineBuffer, "vt ")) // UV
                                                      {
                                                          UVSize++;
                                                      }
                                                  }
                                                  
                                                  Data.HasNormals = NormalSize > 0;
                                                  Data.HasUVs = UVSize > 0;
                                                  
                                                  r32* TempVertices = (r32*)malloc(sizeof(r32) * VertexSize * 3);
                                                  r32* TempNormals = (r32*)malloc(sizeof(r32) * NormalSize * 3);
                                                  r32* TempUVs = (r32*)malloc(sizeof(r32) * UVSize * 2);
                                                  
                                                  u32* VertexIndices = (u32*)malloc(sizeof(u32) * IndexSize * 3);
                                                  u32* NormalIndices = (u32*)malloc(sizeof(u32) * IndexSize * 3);
                                                  u32* UVIndices = (u32*)malloc(sizeof(u32) * IndexSize * 3);
                                                  
                                                  rewind(File);
                                                  
                                                  i32 VerticesIndex = 0;
                                                  i32 NormalIndex = 0;
                                                  i32 UVIndex = 0;
                                                  
                                                  i32 IndexCount = 0;
                                                  
                                                  while(fgets(LineBuffer, 256, File))
                                                  {
                                                      if(StartsWith(LineBuffer, "v "))
                                                      {
                                                          sscanf(LineBuffer, "v %f %f %f", &TempVertices[VerticesIndex * 3], &TempVertices[VerticesIndex * 3 + 1], &TempVertices[VerticesIndex * 3 + 2]);
                                                          VerticesIndex++;
                                                      }
                                                      else if(StartsWith(LineBuffer, "vn "))
                                                      {
                                                          sscanf(LineBuffer, "vn %f %f %f", &TempNormals[NormalIndex * 3], &TempNormals[NormalIndex * 3 + 1], &TempNormals[NormalIndex * 3 + 2]);
                                                          NormalIndex++;
                                                      }
                                                      else if(StartsWith(LineBuffer, "vt "))
                                                      {
                                                          sscanf(LineBuffer, "vt %f %f", &TempUVs[UVIndex * 2], &TempUVs[UVIndex * 2 + 1]);
                                                          UVIndex++;
                                                      }
                                                      else if(StartsWith(LineBuffer, "f "))
                                                      {
                                                          if(NormalSize > 0 && UVSize > 0)
                                                          {
                                                              sscanf(LineBuffer, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", 
                                                                     &VertexIndices[IndexCount * 3],
                                                                     &UVIndices[IndexCount * 3],
                                                                     &NormalIndices[IndexCount * 3],
                                                                     &VertexIndices[IndexCount * 3 + 1],
                                                                     &UVIndices[IndexCount * 3 + 1],
                                                                     &NormalIndices[IndexCount * 3 + 1],
                                                                     &VertexIndices[IndexCount * 3 + 2],
                                                                     &UVIndices[IndexCount * 3 + 2],
                                                                     &NormalIndices[IndexCount * 3 + 2]);
                                                              
                                                              VertexIndices[IndexCount * 3] -= 1;
                                                              VertexIndices[IndexCount * 3 + 1] -= 1;
                                                              VertexIndices[IndexCount * 3 + 2] -= 1;
                                                              UVIndices[IndexCount * 3] -= 1;
                                                              UVIndices[IndexCount * 3 + 1] -= 1;
                                                              UVIndices[IndexCount * 3 + 2] -= 1;
                                                              NormalIndices[IndexCount * 3] -= 1;
                                                              NormalIndices[IndexCount * 3 + 1] -= 1;
                                                              NormalIndices[IndexCount * 3 + 2] -= 1;
                                                          }
                                                          else if(UVSize > 0)
                                                          {
                                                              sscanf(LineBuffer, "f %d/%d %d/%d %d/%d\n", 
                                                                     &VertexIndices[IndexCount * 3],
                                                                     &UVIndices[IndexCount * 3],
                                                                     &VertexIndices[IndexCount * 3 + 1],
                                                                     &UVIndices[IndexCount * 3 + 1],
                                                                     &VertexIndices[IndexCount * 3 + 2],
                                                                     &UVIndices[IndexCount * 3 + 2]);
                                                              
                                                              VertexIndices[IndexCount * 3] -= 1;
                                                              VertexIndices[IndexCount * 3 + 1] -= 1;
                                                              VertexIndices[IndexCount * 3 + 2] -= 1;
                                                              UVIndices[IndexCount * 3] -= 1;
                                                              UVIndices[IndexCount * 3 + 1] -= 1;
                                                              UVIndices[IndexCount * 3 + 2] -= 1;
                                                          }
                                                          else if(NormalSize > 0)
                                                          {
                                                              
                                                              sscanf(LineBuffer, "f %d//%d %d//%d %d//%d\n", 
                                                                     &VertexIndices[IndexCount * 3],
                                                                     &NormalIndices[IndexCount * 3],
                                                                     &VertexIndices[IndexCount * 3 + 1],
                                                                     &NormalIndices[IndexCount * 3 + 1],
                                                                     &VertexIndices[IndexCount * 3 + 2],
                                                                     &NormalIndices[IndexCount * 3 + 2]);
                                                              
                                                              VertexIndices[IndexCount * 3] -= 1;
                                                              VertexIndices[IndexCount * 3 + 1] -= 1;
                                                              VertexIndices[IndexCount * 3 + 2] -= 1;
                                                              NormalIndices[IndexCount * 3] -= 1;
                                                              NormalIndices[IndexCount * 3 + 1] -= 1;
                                                              NormalIndices[IndexCount * 3 + 2] -= 1;
                                                          }
                                                          else
                                                          {
                                                              sscanf(LineBuffer, "f %d %d %d", 
                                                                     &VertexIndices[IndexCount * 3],
                                                                     &VertexIndices[IndexCount * 3 + 1],
                                                                     &VertexIndices[IndexCount * 3 + 2]);
                                                              
                                                              VertexIndices[IndexCount * 3] -= 1;
                                                              VertexIndices[IndexCount * 3 + 1] -= 1;
                                                              VertexIndices[IndexCount * 3 + 2] -= 1;
                                                          }
                                                          IndexCount++;
                                                      }
                                                  }
                                                  
                                                  fclose(File);
                                                  
                                                  if(NormalSize == UVSize && (NormalSize == VertexSize || UVSize == VertexSize || NormalSize == 0 && UVSize == 0))
                                                  {
                                                      Data.VertexBuffer = (r32*)malloc(sizeof(r32) * (VertexSize * 3 + NormalSize * 3 + UVSize * 2));
                                                      Data.IndexBuffer = (u32*)malloc(sizeof(u32) * IndexSize * 3);
                                                      Data.VertexBufferSize = VertexSize * 3 + NormalSize * 3 + UVSize * 2;
                                                      Data.IndexBufferSize = IndexSize * 3;
                                                      
                                                      memcpy(Data.IndexBuffer, VertexIndices, sizeof(u32) * Data.IndexBufferSize);
                                                      
                                                      for(i32 Index = 0; Index < VertexSize; Index++)
                                                      {
                                                          if(Data.HasNormals && Data.HasUVs)
                                                          {
                                                              *Data.VertexBuffer++ = TempVertices[Index * 8];
                                                              *Data.VertexBuffer++ = TempVertices[Index * 8 + 1];
                                                              *Data.VertexBuffer++ = TempVertices[Index * 8 + 2];
                                                              
                                                              *Data.VertexBuffer++ = TempNormals[Index * 8];
                                                              *Data.VertexBuffer++ = TempNormals[Index * 8 + 1];
                                                              *Data.VertexBuffer++ = TempNormals[Index * 8 + 2];
                                                              
                                                              *Data.VertexBuffer++ = TempUVs[Index * 8];
                                                              *Data.VertexBuffer++ = TempUVs[Index * 8 + 1];
                                                          }
                                                          else if(Data.HasNormals)
                                                          {
                                                              *Data.VertexBuffer++ = TempVertices[Index * 6];
                                                              *Data.VertexBuffer++ = TempVertices[Index * 6 + 1];
                                                              *Data.VertexBuffer++ = TempVertices[Index * 6 + 2];
                                                              
                                                              *Data.VertexBuffer++ = TempNormals[Index * 6];
                                                              *Data.VertexBuffer++ = TempNormals[Index * 6 + 1];
                                                              *Data.VertexBuffer++ = TempNormals[Index * 6 + 2];
                                                          }
                                                          else if(Data.HasUVs)
                                                          {
                                                              *Data.VertexBuffer++ = TempVertices[Index * 5];
                                                              *Data.VertexBuffer++ = TempVertices[Index * 5 + 1];
                                                              *Data.VertexBuffer++ = TempVertices[Index * 5 + 2];
                                                              
                                                              *Data.VertexBuffer++ = TempUVs[Index * 5];
                                                              *Data.VertexBuffer++ = TempUVs[Index * 5 + 1];
                                                          }
                                                          else
                                                          {
                                                              *Data.VertexBuffer++ = TempVertices[Index * 3];
                                                              *Data.VertexBuffer++ = TempVertices[Index * 3 + 1];
                                                              *Data.VertexBuffer++ = TempVertices[Index * 3 + 2];
                                                          }
                                                      }
                                                      
                                                      Data.VertexBuffer -= Data.VertexBufferSize;
                                                  }
                                                  else
                                                  {
                                                      r32* TempVertexBuffer = (r32*)malloc(sizeof(r32) * IndexSize * 3 * 8);
                                                      Data.IndexBuffer = (u32*)malloc(sizeof(u32) * IndexSize * 3);
                                                      
                                                      u32 ActualVertexCount = 0;
                                                      u32 ActualIndexSize = 0;
                                                      
                                                      for(i32 I = 0; I < IndexSize * 3; I++)
                                                      {
                                                          u32 VIndex = VertexIndices[I];
                                                          u32 NIndex = NormalIndices[I];
                                                          u32 UVIndex = UVIndices[I];
                                                          
                                                          math::v3 CurrentVertex(TempVertices[VIndex * 3], TempVertices[VIndex * 3 + 1], TempVertices[VIndex * 3 + 2]);
                                                          math::v3 CurrentNormal(TempNormals[NIndex* 3], TempNormals[NIndex * 3 + 1], TempNormals[NIndex * 3 + 2]);
                                                          math::v2 CurrentUV(TempUVs[UVIndex* 3], TempUVs[UVIndex * 3 + 1]);
                                                          
                                                          b32 Found = false;
                                                          u32 ExistingIndex = 0;
                                                          
                                                          for(u32 J = 0; J < ActualIndexSize; J++)
                                                          {
                                                              u32 CurrentIndex = Data.IndexBuffer[J];
                                                              
                                                              if(Abs(TempVertexBuffer[CurrentIndex * 8] - CurrentVertex.x) < 0.01f
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
                                                                  printf("Found %d\n", CurrentIndex);
                                                                  break;
                                                              }
                                                          }
                                                          
                                                          if(Found)
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
                                                  
                                                  free(TempVertices);
                                                  free(TempNormals);
                                                  free(TempUVs);
                                                  
                                                  Renderer.Buffers[Renderer.BufferCount - 1] = Data;
                                              }
                                      }