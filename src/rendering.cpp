                                          static void PushLine(renderer* Renderer, math::v2 Point1, math::v2 Point2, r32 LineWidth, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &Renderer->Buffer[Renderer->CommandCount++];
                                              RenderCommand->Type = RenderCommand_Line;
                                              RenderCommand->Line.Point1 = Point1;
                                              RenderCommand->Line.Point2 = Point2;
                                              RenderCommand->Line.LineWidth = LineWidth;
                                              RenderCommand->Line.Color = Color;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushText(renderer* Renderer, const char* Text, math::v2 Position, i32 FontHandle, math::rgba Color, Alignment Alignment = Alignment_Left, b32 IsUI = true)
                                          {
                                              render_command* RenderCommand = &Renderer->Buffer[Renderer->CommandCount++];
                                              RenderCommand->Type = RenderCommand_Text;
                                              
                                              strcpy(RenderCommand->Text.Text, Text);
                                              
                                              RenderCommand->Text.Position = Position;
                                              RenderCommand->Text.FontHandle = FontHandle;
                                              RenderCommand->Text.Color = Color;
                                              RenderCommand->Text.Alignment = Alignment;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushFilledRect(renderer* Renderer, math::v2 Position, math::v2 Size, math::rgba Color, b32 IsUI = true)
                                          {
                                              render_command* RenderCommand = &Renderer->Buffer[Renderer->CommandCount++];
                                              RenderCommand->Type = RenderCommand_Rect;
                                              RenderCommand->Rect.Position = Position;
                                              RenderCommand->Rect.Size = Size;
                                              RenderCommand->Rect.Color = Color;
                                              RenderCommand->Rect.Outlined = false;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushOutlinedRect(renderer* Renderer, math::v2 Position, math::v2 Size, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &Renderer->Buffer[Renderer->CommandCount++];
                                              RenderCommand->Type = RenderCommand_Rect;
                                              RenderCommand->Rect.Position = Position;
                                              RenderCommand->Rect.Size = Size;
                                              RenderCommand->Rect.Color = Color;
                                              RenderCommand->Rect.Outlined = true;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushSprite(renderer* Renderer, math::v2 Position, math::v3 Scale, math::v2 Frame, math::v2 TextureOffset, const char* TextureName, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &Renderer->Buffer[Renderer->CommandCount++];
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
                                          
                                          static void PushBuffer(renderer* Renderer, i32 BufferHandle)
                                          {
                                              render_command* RenderCommand = &Renderer->Buffer[Renderer->CommandCount++];
                                              RenderCommand->Type = RenderCommand_Buffer;
                                              RenderCommand->Buffer.BufferHandle;
                                              RenderCommand->IsUI = false;
                                          }