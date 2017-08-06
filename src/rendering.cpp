                                          static void PushLine(render_command_buffer* CommandBuffer, math::v2 Point1, math::v2 Point2, r32 LineWidth, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &CommandBuffer->Buffer[CommandBuffer->CommandCount++];
                                              RenderCommand->Type = Render_Command_Line;
                                              RenderCommand->Line.Point1 = Point1;
                                              RenderCommand->Line.Point2 = Point2;
                                              RenderCommand->Line.LineWidth = LineWidth;
                                              RenderCommand->Line.Color = Color;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushText(render_command_buffer* CommandBuffer, const char* Text, math::v2 Position, i32 FontHandle, math::rgba Color, Alignment Alignment = Alignment_Left, b32 IsUI = true)
                                          {
                                              render_command* RenderCommand = &CommandBuffer->Buffer[CommandBuffer->CommandCount++];
                                              RenderCommand->Type = Render_Command_Text;
                                              
                                              if(RenderCommand->Text)
                                                  free(Text);
                                              RenderCommand->Text.Text = (char*)malloc(strlen(Text) + 1);
                                              strcpy(RenderCommand->Text.Text, Text);
                                              
                                              RenderCommand->Text.Position = Position;
                                              RenderCommand->Text.FontHandle = FontHandle;
                                              RenderCommand->Text.Color = Color;
                                              RenderCommand->Text.Alignment = Alignment;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushFilledRect(render_command_buffer* CommandBuffer, math::v2 Position, math::v2 Size, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &CommandBuffer->Buffer[CommandBuffer->CommandCount++];
                                              RenderCommand->Type = Render_Command_Rect;
                                              RenderCommand->Rect.Position = Position;
                                              RenderCommand->Rect.Size = Size;
                                              RenderCommand->Rect.Color = Color;
                                              RenderCommand->Rect.Outlined = false;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushOutlinedRect(render_command_buffer* CommandBuffer, math::v2 Position, math::v2 Size, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &CommandBuffer->Buffer[CommandBuffer->CommandCount++];
                                              RenderCommand->Type = Render_Command_Rect;
                                              RenderCommand->Rect.Position = Position;
                                              RenderCommand->Rect.Size = Size;
                                              RenderCommand->Rect.Color = Color;
                                              RenderCommand->Rect.Outlined = true;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushSprite(render_command_buffer* CommandBuffer, math::v2 Position, math::v2 Size, math::v2 TextureCoords, i32 TextureHandle, math::rgba Color, b32 IsUI = false)
                                          {
                                              render_command* RenderCommand = &CommandBuffer->Buffer[CommandBuffer->CommandCount++];
                                              RenderCommand->Type = Render_Command_Sprite;
                                              RenderCommand->Sprite.Position = Position;
                                              RenderCommand->Sprite.Size = Size;
                                              RenderCommand->Sprite.TextureCoords = TextureCoords;
                                              RenderCommand->Sprite.TextureHandle = TextureHandle;
                                              RenderCommand->Sprite.Color = Color;
                                              RenderCommand->IsUI = IsUI;
                                          }
                                          
                                          static void PushBuffer(render_command_buffer* CommandBuffer, i32 BufferHandle)
                                          {
                                              render_command* RenderCommand = &CommandBuffer->Buffer[CommandBuffer->CommandCount++];
                                              RenderCommand->Type = Render_Command_Buffer;
                                              RenderCommand->Buffer.BufferHandle;
                                              RenderCommand->IsUI = IsUI;
                                          }