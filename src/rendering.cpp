                                          static void PushLine(renderer& Renderer, math::v2 Point1, math::v2 Point2, r32 LineWidth, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
                                              RenderCommand->Type = RenderCommand_Line;
                                              RenderCommand->Line.Point1 = Point1;
                                              RenderCommand->Line.Point2 = Point2;
                                              RenderCommand->Line.LineWidth = LineWidth;
                                              RenderCommand->Line.Color = Color;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushText(renderer& Renderer, const char* Text, math::v2 Position, i32 FontHandle, math::rgba Color, Alignment Alignment = Alignment_Left, b32 IsUI = true)
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
                                          
                                          static void PushFilledRect(renderer& Renderer, math::v2 Position, math::v2 Size, math::rgba Color, b32 IsUI = true)
                                          {
                                              render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
                                              RenderCommand->Type = RenderCommand_Rect;
                                              RenderCommand->Rect.Position = Position;
                                              RenderCommand->Rect.Size = Size;
                                              RenderCommand->Rect.Color = Color;
                                              RenderCommand->Rect.Outlined = false;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushOutlinedRect(renderer& Renderer, math::v2 Position, math::v2 Size, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
                                              RenderCommand->Type = RenderCommand_Rect;
                                              RenderCommand->Rect.Position = Position;
                                              RenderCommand->Rect.Size = Size;
                                              RenderCommand->Rect.Color = Color;
                                              RenderCommand->Rect.Outlined = true;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushSprite(renderer& Renderer, math::v2 Position, math::v3 Scale, math::v2 Frame, math::v2 TextureOffset, const char* TextureName, math::rgba Color, b32 IsUI = false)
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
                                          
                                          static void PushBuffer(renderer& Renderer, i32 BufferHandle, char* TextureName)
                                          {
                                              render_command* RenderCommand = &Renderer.Buffer[Renderer.CommandCount++];
                                              RenderCommand->Type = RenderCommand_Buffer;
                                              RenderCommand->Buffer.BufferHandle;
                                              RenderCommand->Buffer.TextureName = TextureName;
                                              RenderCommand->IsUI = false;
                                          }
                                          
                                          static void LoadBuffer(renderer& Renderer, r32* Buffer, i32 BufferSize, i32* BufferHandle)
                                          {
                                              buffer_data Data = {};
                                              Data.Buffer = Buffer;
                                              Data.Size = BufferSize;
                                              Renderer.Buffers[Renderer.BufferCount] = Data;
                                              
                                              if(*BufferHandle == 0)
                                                  *BufferHandle = Renderer.BufferCount++;
                                          }
                                          
                                          static void LoadOBJFile(char* FilePath, model* Model)
                                          {
                                              FILE* File = fopen(FilePath, "r");
                                              
                                              char LineBuffer[256];
                                              
                                              if(File)
                                              {
                                                  i32 VertexSize = 0;
                                                  i32 IndexSize = 0;
                                                  
                                                  while(fgets(LineBuffer, 256, File))
                                                  {
                                                      if(StartsWith(LineBuffer, "v "))
                                                      {
                                                          VertexSize++;
                                                      }
                                                      else if(StartsWith(LineBuffer, "f "))
                                                      {
                                                          IndexSize++;
                                                      }
                                                  }
                                                  
                                                  r32* VertexBuffer = (r32*)malloc(sizeof(r32) * VertexSize * 3);
                                                  i32* IndexBuffer = (i32*)malloc(sizeof(i32) * IndexSize * 3);
                                                  
                                                  rewind(File);
                                                  
                                                  while(fgets(LineBuffer, 256, File))
                                                  {
                                                      if(StartsWith(LineBuffer, "v "))
                                                      {
                                                          r32 X;
                                                          r32 Y;
                                                          r32 Z;
                                                          
                                                          sscanf(LineBuffer, "v %f %f %f", &X, &Y, &Z);
                                                          *VertexBuffer++ = X;
                                                          *VertexBuffer++ = Y;
                                                          *VertexBuffer++ = Z;
                                                      }
                                                      else if(StartsWith(LineBuffer, "f "))
                                                      {
                                                          i32 First;
                                                          i32 Second;
                                                          i32 Third;
                                                          
                                                          sscanf(LineBuffer, "f %d %d %d", &First, &Second, &Third);
                                                          *IndexBuffer++ = First;
                                                          *IndexBuffer++ = Second;
                                                          *IndexBuffer++ = Third;
                                                      }
                                                      fclose(File);
                                                      
                                                      //RegisterModelBuffers(VertexBuffer, VertexSize, IndexBuffer, IndexSize, &Model->BufferHandle);
                                                  }
                                              }
                                      }