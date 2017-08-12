
//@Incomplete: Should be pushed from game/editor...
/*
static void RenderAnimationPreview(render_state& RenderState, renderer& Renderer, const animation_info& AnimationInfo, const animation& Animation, math::v2 ScreenPosition, r32 Scale)
{
    r32 Ratio = Animation.FrameSize.y / Animation.FrameSize.x;
    
    r32 MaxWidth = 350.0f;
    r32 MaxHeight = MaxWidth * Ratio;
    
    RenderRect(Render_Fill, RenderState, math::v4(1, 1, 1, 1), ScreenPosition.x, ScreenPosition.y, MaxWidth, MaxHeight);
    
    ScreenPosition.x *= RenderState.ScaleX;
    ScreenPosition.x -= 1;
    ScreenPosition.y *= RenderState.ScaleY;
    ScreenPosition.y -= 1;
    
    auto Shader = RenderState.SpritesheetShader;
    
    math::m4 Model(1.0f);
    Model = math::Translate(Model, math::v3(ScreenPosition.x, ScreenPosition.y, 0.0f));
    Model = math::Scale(Model, math::v3(MaxWidth * RenderState.ScaleX, MaxHeight * RenderState.ScaleY, 1));
    
    auto TextureData = Renderer.TextureMap[Animation.Texture];
    
    if ((i32)RenderState.BoundTexture != TextureData->Handle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, TextureData->Handle);
        RenderState.BoundTexture = TextureData->Handle;
    }
    
    glBindVertexArray(RenderState.SpriteSheetVAO);
    
    UseShader(&Shader);
    auto Frame = Animation.Frames[AnimationInfo.FrameIndex];
    SetFloatUniform(Shader.Program, "isUI", 1);
    SetVec2Uniform(Shader.Program,"textureOffset", math::v2(Frame.X, Frame.Y));
    SetFloatUniform(Shader.Program, "frameWidth", Animation.FrameSize.x);
    SetFloatUniform(Shader.Program, "frameHeight", Animation.FrameSize.y);
    SetVec4Uniform(Shader.Program, "color", math::v4(1, 1, 1, 1));
    SetVec2Uniform(Shader.Program,"sheetSize",
                   math::v2(TextureData->Width, TextureData->Height));
                   
    SetMat4Uniform(Shader.Program, "Model", Model);
    
    glDrawElements(GL_TRIANGLES, sizeof(RenderState.QuadIndices), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}
*/

//@Incomplete: All this shit that needs to be moved my god
/*
static void RenderHealthbar(render_state& RenderState,
                            entity* Entity, const entity_healthbar& Healthbar, math::m4 ProjectionMatrix, math::m4 ViewMatrix)
{
    auto EntityPosition = ToIsometric(Entity->Position);
    
    RenderRect(Render_Fill, RenderState, math::v4(0.6, 0, 0, 1), EntityPosition.x + Healthbar.Offset.x, EntityPosition.y + Healthbar.Offset.y, 1.0f
               / (r32)Entity->FullHealth * (r32)Entity->Health , 0.05f, 0, false, ProjectionMatrix, ViewMatrix);
               
               
    if(Entity->HealthLost > 0)
    {
        r32 StartX = EntityPosition.x + Healthbar.Offset.x + 1.0f
            / (r32)Entity->FullHealth * (r32)Entity->Health;
        r32 Width = 1.0f
            / (r32)Entity->FullHealth * (r32)Entity->HealthLost;
            
        RenderRect(Render_Fill, RenderState, math::v4(1, 1, 1, 1), StartX, EntityPosition.y + Healthbar.Offset.y, Width, 0.05f, 0, false, ProjectionMatrix, ViewMatrix);
        
        math::m4 Model = math::m4(1.0f) * ViewMatrix;
        
        math::v3 Projected =
            math::Project(math::v3(EntityPosition.x, EntityPosition.y, 0), Model, ProjectionMatrix, math::v4(RenderState.Viewport[0], RenderState.Viewport[1], RenderState.Viewport[2], RenderState.Viewport[3]));
            
        for(i32 Index = 0; Index < 10; Index++)
        {
            auto& HealthCount = Entity->Enemy.HealthCounts[Index];
            if(HealthCount.Visible)
            {
                RenderText(RenderState, RenderState.DamageFont, math::v4(1, 0, 0, 1), HealthCount.Count, Projected.x + HealthCount.Position.x, Projected.y + HealthCount.Position.y);
            }
        }
    }
}

static void RenderEntity(game_state *GameState, render_entity* RenderEntity, math::m4 ProjectionMatrix, math::m4 View)
{ 
    /*render_state* RenderState = &GameState->RenderState;
    
    auto Shader = RenderState->Shaders[RenderEntity->Shader];
    
    b32 Active = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Active : RenderEntity->Object->Active;
    math::v2 Position = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Position : RenderEntity->Object->Position;
    math::v2 Center = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Center : RenderEntity->Object->Center;
    r32 EntityScale = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Scale : RenderEntity->Object->Scale;
    i32 LightSourceHandle = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->LightSourceHandle : RenderEntity->Object->LightSourceHandle;
    
    animation* CurrentAnimation =  RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->CurrentAnimation : RenderEntity->Object->CurrentAnimation;
    animation_info AnimationInfo = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->AnimationInfo : RenderEntity->Object->AnimationInfo;
    
    math::m4 OldModel;
    
    if(RenderEntity->Rendered && Active)
    {
        if(RenderEntity->RenderType == Render_Type_Entity)
        {
            if(RenderEntity->Entity->Type == Entity_Player)
            {
                auto CurrentTilePos = ToIsometric(math::v2(RenderEntity->Entity->CurrentDestination.x, RenderEntity->Entity->CurrentDestination.y));
                RenderIsometricRect(RenderState, math::v4(0.3, 0.3, 0, 0.2), CurrentTilePos.x, CurrentTilePos.y, 1, 0.5f, ProjectionMatrix, View);
            }
        }
        
        math::m4 Model(1.0f);
        
        if(CurrentAnimation)
        {
            r32 WidthInUnits = (r32)CurrentAnimation->FrameSize.x / (r32)PIXELS_PER_UNIT;
            r32 HeightInUnits = (r32)CurrentAnimation->FrameSize.y / (r32)PIXELS_PER_UNIT;
            
            math::v3 Scale = math::v3(WidthInUnits * EntityScale, HeightInUnits * EntityScale, 1);
            
            auto CorrectPos = ToIsometric(Position);
            
            CorrectPos.x -= CurrentAnimation->Center.x * Scale.x;
            CorrectPos.y -= CurrentAnimation->Center.y * Scale.y;
            
            // @Cleanup: Move these to a global variable or similar
            r32 TileWidthHalf = 0.5f;
            r32 TileHeightHalf = 0.25f;
            
            CorrectPos.x += TileWidthHalf; //We want the sprite to be centered in the tile
            CorrectPos.y += TileHeightHalf;
            
            if(LightSourceHandle != -1)
            {
                GameState->LightSources[LightSourceHandle].Pointlight.RenderPosition = CorrectPos + math::v2(CurrentAnimation->Center.x * Scale.x, CurrentAnimation->Center.y * Scale.y);
            }
            
            Model = math::Scale(Model, math::v3(Scale.x, Scale.y, Scale.z));
            Model = math::Translate(Model, math::v3(CorrectPos.x, CorrectPos.y, 0.0f));
            
            animation* Animation = CurrentAnimation;
            
            if (RenderState->BoundTexture != Animation->Texture->TextureHandle) //never bind the same texture if it's already bound
            {
                glBindTexture(GL_TEXTURE_2D, Animation->Texture->TextureHandle);
                RenderState->BoundTexture = Animation->Texture->TextureHandle;
            }
            
            if(Shader.Program == 0)
            {
                Shader = RenderState->ErrorShaderSprite;
                glBindVertexArray(RenderState->SpriteErrorVAO);
            }
            else
            {
                glBindVertexArray(RenderState->SpriteSheetVAO);
            }
            
            UseShader(&Shader);
            auto Frame = Animation->Frames[AnimationInfo.FrameIndex];
            
            SetVec4Uniform(Shader.Program, "spriteColor", RenderEntity->Color);
            SetFloatUniform(Shader.Program, "isUI", 0);
            SetVec2Uniform(Shader.Program,"textureOffset", math::v2(Frame.X, Frame.Y));
            SetFloatUniform(Shader.Program, "frameWidth", Animation->FrameSize.x);
            SetFloatUniform(Shader.Program, "frameHeight", Animation->FrameSize.y);
            SetVec2Uniform(Shader.Program,"textureSize",
                           math::v2(Animation->Texture->Width, Animation->Texture->Height));
        } 
        else 
        {
            auto CorrectPos = ToIsometric(math::v2(Position.x, Position.y));
            
            r32 CorrectX = CorrectPos.x;
            r32 CorrectY = CorrectPos.y;
            
            Model = math::Translate(Model, math::v3(CorrectX, CorrectY, 0.0f));
            
            r32 WidthInUnits = RenderEntity->Texture->Width / (r32)PIXELS_PER_UNIT;
            r32 HeightInUnits = RenderEntity->Texture->Height / (r32)PIXELS_PER_UNIT;
            
            math::v3 Scale = math::v3(WidthInUnits * EntityScale, HeightInUnits * EntityScale, 1);
            
            Model = math::Scale(Model, math::v3(Scale.x, Scale.y, Scale.z));
            
            if (RenderState->BoundTexture != RenderEntity->Texture->TextureHandle) //never bind the same texture if it's already bound
            {
                glBindTexture(GL_TEXTURE_2D, RenderEntity->Texture->TextureHandle);
                RenderState->BoundTexture = RenderEntity->Texture->TextureHandle;
            }
            
            if(Shader.Program == 0)
            {
                Shader = RenderState->ErrorShaderSprite;
                glBindVertexArray(RenderState->SpriteErrorVAO);
            }
            else
            {
                glBindVertexArray(RenderState->SpriteVAO);
            }
            UseShader(&Shader);
            SetVec2Uniform(Shader.Program,"textureSize",
                           math::v2(RenderEntity->Texture->Width, RenderEntity->Texture->Height));
        }
        
        
        if(RenderEntity->RenderType == Render_Type_Entity)
        {
            SetFloatUniform(Shader.Program, "glow", GL_FALSE);
        }
        else
        {
            auto& OEntity = *RenderEntity->Object;
            
            if(OEntity.Type == Object_Loot)
            {
                SetFloatUniform(Shader.Program, "glow", GL_TRUE);
            }
            else
                SetFloatUniform(Shader.Program, "glow", GL_FALSE);
        }
        
        SetFloatUniform(Shader.Program, "time", (r32)GetTime());
        
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", View);
        SetMat4Uniform(Shader.Program, "Model", Model);
        SetVec4Uniform(Shader.Program, "Color", RenderEntity->Color);
        
        glDrawElements(GL_TRIANGLES, sizeof(RenderState->QuadIndices), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
        
        if(RenderEntity->RenderType == Render_Type_Entity)
        {
            auto& Entity = *RenderEntity->Entity;
            
            if(Entity.Type == Entity_Enemy && Entity.Health < Entity.FullHealth && Entity.Health > 0)
            {
                RenderHealthbar(RenderState, &Entity, *Entity.Enemy.Healthbar, ProjectionMatrix, View);
            }
            
            if(GameState->AIDebugModeOn && Entity.Type == Entity_Enemy)
            {
                math::m4 Model = math::m4(1.0f) * View;
                
                math::v3 Projected =
                    math::Project(math::v3(Entity.Position.x, Entity.Position.y, 0), Model, ProjectionMatrix, math::v4(GameState->RenderState.Viewport[0], GameState->RenderState.Viewport[1], GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
                    
                char* State = "State MISSING";
                State = AIEnumToStr(Entity.Enemy.AIState);
                
                RenderText(RenderState, RenderState->ButtonFont, math::v4(1, 1, 1, 1), State, Projected.x, Projected.y, Alignment_Center);
            }
        }
    }
    
    if(RenderEntity->RenderType == Render_Type_Entity)
    {
        auto& Entity = *RenderEntity->Entity;
        
        if(Entity.Type == Entity_Player && Entity.Player.RenderCrosshair)
        {
            RenderRect(Render_Fill, RenderState, math::v4(1, 1, 1, 1), Entity.Position.x + Entity.Player.CrosshairPositionX, Entity.Position.y + Entity.Player.CrosshairPositionY, 1, 1, RenderState->Textures["crosshair"]->TextureHandle, false, ProjectionMatrix, View);
        }
        
        if(RenderState->RenderColliders && (Entity.Type == Entity_Player || Entity.Type == Entity_Enemy))
        {
            r32 CartesianX = (r32)math::Floor(Entity.Position.x - 0.5f);
            r32 CartesianY = (r32)math::Ceil(Entity.Position.y - 0.5f);
            
            math::v2 CorrectPosition = ToIsometric(math::v2(CartesianX, CartesianY));
            r32 CorrectX = CorrectPosition.x;
            r32 CorrectY = CorrectPosition.y;
            
            RenderIsometricOutline(RenderState, math::v4(0, 1, 0, 1), CorrectX, CorrectY, 1, 0.5f, ProjectionMatrix, View);
            
            hit_tile_extents HitExtents = Entity.HitExtents[Entity.LookDirection];
            
            for(i32 X = HitExtents.StartX; X < HitExtents.EndX; X++)
            {
                for(i32 Y = HitExtents.StartY; Y < HitExtents.EndY; Y++)
                {
                    math::v2 Pos = ToIsometric(math::v2(CartesianX + X, CartesianY + Y));
                    RenderIsometricOutline(RenderState, math::v4(0, 0, 1, 1), Pos.x, Pos.y, 1, 0.5f, ProjectionMatrix, View);
                    if(X == 0 && Y == 0)
                        RenderIsometricRect(RenderState, math::v4(1, 0, 0, 1), Pos.x, Pos.y, 1, 0.5f, ProjectionMatrix, View);
                }
            }
        }
        
        if(Entity.Type == Entity_Enemy)
        {
            if(Entity.Enemy.IsTargeted)
            {
                RenderRect(Render_Fill, RenderState, math::v4(1, 1, 1, 1), Entity.Position.x + Entity.Enemy.TargetingPositionX, Entity.Position.y + Entity.Enemy.TargetingPositionY, 1, 1, RenderState->Textures["red_arrow"]->TextureHandle, false, ProjectionMatrix, View);}
                
            if(GameState->GameMode == Mode_Editor 
               && GameState->EditorState.SelectedEntity 
               && GameState->EditorState.SelectedEntity->EntityIndex == Entity.EntityIndex 
               && Entity.Enemy.WaypointCount > 0)
            {
                for(i32 Index = 1; Index < Entity.Enemy.WaypointCount; Index++)
                {
                    auto Point1 = Entity.Enemy.Waypoints[Index - 1];
                    auto Point2 = Entity.Enemy.Waypoints[Index];
                    
                    RenderLine(*RenderState, math::v4(1, 1, 1, 1), Point1.x + 0.5f, Point1.y + 0.5f, Point2.x + 0.5f, Point2.y + 0.5f, false, ProjectionMatrix, View);
                }
                
                for(i32 Index = 0; Index < Entity.Enemy.WaypointCount; Index++)
                {
                    auto Point = Entity.Enemy.Waypoints[Index];
                    RenderRect(Render_Fill, RenderState, math::v4(0, 1, 0, 0.5), Point.x + 0.25f, Point.y + 0.25f, 0.5f, 0.5f, RenderState->Textures["circle"]->TextureHandle, false, ProjectionMatrix, View);
                }
            }
        }
        
        if(RenderState->RenderColliders && !Entity.IsKinematic)
            RenderColliderWireframe(RenderState, &Entity, ProjectionMatrix, View);
            
        if(RenderState->RenderPaths && Entity.Type == Entity_Enemy)
            RenderAStarPath(RenderState,&Entity,ProjectionMatrix,View);
    }
}
*/

//@Incomplete: I guess we don't need this?
/*
static void RenderTile(render_state* RenderState, r32 X, r32 Y, u32 TilesheetIndex, i32 TileWidth, i32 TileHeight, math::v2 TextureOffset, math::v2 SheetSize, math::v4 Color,  math::m4 ProjectionMatrix, math::m4 View)
{
    glBindVertexArray(RenderState->SpriteSheetVAO);
    math::m4 Model(1.0f);
    
    Model = math::Translate(Model, math::v3(X, Y, 0.0f));
    Model = math::Scale(Model, math::v3(1, 0.5f, 1));
    
    glBindTexture(GL_TEXTURE_2D, RenderState->Tilesheets[TilesheetIndex].Texture.TextureHandle);
    
    shader Shader = RenderState->SpritesheetShader;
    UseShader(&Shader);
    
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    SetVec4Uniform(Shader.Program, "color", Color);
    SetVec2Uniform(Shader.Program, "textureOffset", TextureOffset);
    SetFloatUniform(Shader.Program, "frameWidth", (r32)TileWidth);
    SetFloatUniform(Shader.Program, "frameHeight", (r32)TileHeight);
    SetVec2Uniform(Shader.Program, "textureSize", SheetSize);
    
    glDrawElements(GL_TRIANGLES, sizeof(RenderState->QuadIndices), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}
*/

//@Incomplete: This commented out code is getting out of hand
/*
void RenderButton(render_state& RenderState, renderer& Renderer, const button& Button)
{
    RenderRect(Render_Fill, RenderState, math::v4(0.26, 0.525, 0.95, 1), Button.ScreenPosition.x, Button.ScreenPosition.y, Button.Size.x, Button.Size.y, Renderer.TextureMap["button"]->Handle);
    RenderText(RenderState, RenderState.ButtonFont, Button.TextColor, Button.Text, Button.ScreenPosition.x + Button.Size.x / 2, Button.ScreenPosition.y + Button.Size.y / 2, Alignment_Center);
}

void RenderTextfield(render_state& RenderState, const textfield& Textfield)
{
    RenderRect(Render_Fill, RenderState, math::v4(1, 1, 1, 1), Textfield.ScreenPosition.x, Textfield.ScreenPosition.y, Textfield.Size.x, Textfield.Size.y);
    
    RenderText(RenderState, RenderState.RobotoFont, math::v4(1, 1, 1, 1), Textfield.Label, Textfield.ScreenPosition.x, Textfield.ScreenPosition.y + 35);
    RenderText(RenderState, RenderState.RobotoFont, math::v4(0, 0, 0, 1), Textfield.Text, Textfield.ScreenPosition.x, Textfield.ScreenPosition.y + 10);
    
    if(Textfield.InFocus)
    {
        RenderRect(Render_Outline, RenderState, math::v4(1, 0, 0, 1), Textfield.ScreenPosition.x-3, Textfield.ScreenPosition.y - 3, Textfield.Size.x + 6, Textfield.Size.y + 6);
        
        // Draw cursor
        r32 Width;
        r32 Height;
        MeasureText(RenderState.RobotoFont, Textfield.Text, &Width, &Height);
        RenderRect(Render_Fill, RenderState, math::v4(0.5, 0.3, 0.57, 1), Textfield.ScreenPosition.x + Width + 2, Textfield.ScreenPosition.y + 7, 10, 20);
    }
    
}

void RenderCheckbox(render_state& RenderState, const checkbox& Checkbox)
{
    RenderRect(Render_Fill, RenderState, math::v4(1, 1, 1, 1),Checkbox.ScreenPosition.x, Checkbox.ScreenPosition.y, 25, 25);
    
    RenderText(RenderState, RenderState.RobotoFont, math::v4(1, 1, 1, 1), Checkbox.Label, Checkbox.ScreenPosition.x, Checkbox.ScreenPosition.y + 35);
    
    if(Checkbox.Checked)
    {
        RenderRect(Render_Fill, RenderState, math::v4(1, 0, 0, 1),Checkbox.ScreenPosition.x + 5, Checkbox.ScreenPosition.y + 5, 15, 15);
    }
}


// @Incomplete: Is this used????
/*
static void RenderInGameMode(game_state* GameState)
{
    qsort(GameState->RenderEntities, GameState->RenderEntityCount, sizeof(render_entity), CompareFunction);
    
    for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
    {
        if(!GameState->EditorState.RenderAllLayers)
        {
            if(GameState->EditorState.CurrentTilemapLayer == Layer)
                RenderTilemap(Layer, RenderState, GameState->CurrentLevel.Tilemap, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            else
                RenderTilemap(Layer, &GameState->RenderState, GameState->CurrentLevel.Tilemap, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix, math::v4(1, 1, 1, 0.2));
        }
        else
            RenderTilemap(Layer, &GameState->RenderState, GameState->CurrentLevel.Tilemap, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            
        if(Layer == 1)
        {
            for(i32 Index = 0; Index < GameState->RenderEntityCount; Index++)
            {
                auto RenderEntity = GameState->RenderEntities[Index];
                if(RenderEntity.RenderType == Render_Type_Entity)
                {
                    const entity& Entity = *RenderEntity.Entity;
                    if(Entity.ShowAttackTiles &&(Entity.Type == Entity_Player || Entity.Type == Entity_Enemy))
                    {
                        r32 CartesianX = (r32)math::Floor(Entity.Position.x - 0.5f);
                        r32 CartesianY = (r32)math::Ceil(Entity.Position.y - 0.5f);
                        
                        math::v2 CorrectPosition = ToIsometric(math::v2(CartesianX, CartesianY));
                        r32 CorrectX = CorrectPosition.x;
                        r32 CorrectY = CorrectPosition.y;
                        
                        hit_tile_extents HitExtents = Entity.HitExtents[Entity.LookDirection];
                        
                        for(i32 X = HitExtents.StartX; X < HitExtents.EndX; X++)
                        {
                            for(i32 Y = HitExtents.StartY; Y < HitExtents.EndY; Y++)
                            {
                                math::v2 Pos = ToIsometric(math::v2(CartesianX + X, CartesianY + Y));
                                
                                RenderIsometricRect(&GameState->RenderState, math::v4(1, 0, 0, 0.2), Pos.x, Pos.y, 1, 0.5f, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                            }
                        }
                    }
                }
            }
            
            /*for(i32 Index = 0; Index < GameState->RenderState.RenderEntityCount; Index++) 
            {
            render_entity* Render = &GameState->RenderState.RenderEntities[Index];
            
            if(Render->RenderType == Render_Type_Entity)
            Render->Entity->RenderEntityHandle = Index;
            else if(Render->RenderType == Render_Type_Object)
            Render->Object->RenderEntityHandle = Index;
            
            RenderEntity(GameState, Render, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            }
}
}
}
*/
/*
void RenderGame(game_state* GameState)
{
    if(GameState->GameMode == Mode_InGame || (GameState->GameMode == Mode_Editor && !GameState->EditorState.MenuOpen))
    {
        //RenderInGameMode(GameState);
    }
}
*/

//@Incomplete: This was commented out below
/*
void RenderUI(game_state* GameState)
{
    switch(GameState->GameMode)
    {
        case Mode_MainMenu:
        {
            math::v4 TextColor;
            
            for(u32 Index = 0; Index < GameState->MainMenu.OptionCount; Index++)
            {
                if((i32)Index == GameState->MainMenu.SelectedIndex)
                {
                    RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), (r32)GameState->RenderState.WindowWidth / 2 - 200, (r32)GameState->RenderState.WindowHeight / 2.0f - 10 - 40 * Index, 400, 40);
                    
                    TextColor = math::v4(0, 0, 0, 1);
                }
                else
                {
                    TextColor = math::v4(1, 1, 1, 1);
                }
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, TextColor, GameState->MainMenu.Options[Index], (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight / 2 - 40 * Index, Alignment_Center);
                
            }
        }
        break;
        case Mode_InGame:
        {
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            
            if(GameState->Paused)
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(0.5, 1, 1, 1), "PAUSED", (r32)GameState->RenderState.WindowWidth / 2, 40, Alignment_Center);
                
            if(!InputController.ControllerPresent)
            {
                auto Tex = GameState->RenderState.Textures["cross"];
                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), (r32)InputController.MouseX - 20.0f, (r32)GameState->RenderState.WindowHeight - (r32)InputController.MouseY - 20.0f, 40.0f, 40.0f, 
                           GameState->RenderState.Textures["cross"]->TextureHandle, true, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            }
            
            // Player UI
            auto& Player = GameState->Entities[0];
            RenderRect(Render_Fill, &GameState->RenderState, math::v4(0, 0, 0, 1), 48.0f, GameState->RenderState.WindowHeight - 52.0f, 404.0f, 29.0f);
            RenderRect(Render_Fill, &GameState->RenderState, math::v4(0.6f, 0, 0, 1), 50.0f, GameState->RenderState.WindowHeight - 50.0f, 400.0f / (r32)GameState->CharacterData.Health * (r32)Player.Health, 25.0f);
            
            if(!TimerDone(GameState,Player.Player.CheckpointPlacementTimer) && Player.Player.IsChargingCheckpoint)
            {
                r32 TotalWidth = 404.f;
                r32 Height = 25.0f;
                r32 XPos = GameState->RenderState.WindowWidth / 2.0f - TotalWidth/2.0f;
                r32 YPos = GameState->RenderState.WindowHeight - 52.0f;
                
                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1,1,1,1), XPos, YPos, TotalWidth, Height);
                RenderRect(Render_Fill, &GameState->RenderState, math::v4(0.0f,0.5f,0.5f,1.0f), XPos, YPos, TotalWidth * (r32)(1 -  ElapsedTimerFrac(GameState,Player.Player.CheckpointPlacementTimer)),Height);
            }
            
            
            if(!TimerDone(GameState, Player.HealthDecreaseTimer))
            {
                r32 StartX = 50 +  400.0f / (r32)GameState->CharacterData.Health * (r32)Player.Health;
                r32 Width = 400.0f / (r32)GameState->CharacterData.Health * Player.HealthLost;
                
                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), StartX, (r32)(GameState->RenderState.WindowHeight - 50), (r32)Width, 25.0f);
            }
            
            RenderRect(Render_Fill, &GameState->RenderState, math::v4(0, 0, 0, 1), 48.0f, (r32)(GameState->RenderState.WindowHeight - 92), 404.0f, 29.0f);
            RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 0.5, 0, 1), 50.0f, (r32)(GameState->RenderState.WindowHeight - 90), 400.0f / (r32)GameState->CharacterData.Stamina * (r32)Player.Player.Stamina, 25.0f);
            
            RenderRect(Render_Fill, &GameState->RenderState, math::v4(0, 0, 0, 1), 48, 10, 80, 80);
            RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), 48 + 40 - 25, 10 + 40 - 25, 50, 50, GameState->RenderState.Textures["health_potion"]->TextureHandle);
            
            char InventoryText[64];
            sprintf(InventoryText, "%d", GameState->Entities[0].Player.Inventory.HealthPotionCount);
            
            RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), InventoryText, 48 + 40 - 30, 75, Alignment_Center);
            
            if(InputController.ControllerType == Controller_Xbox)
            {
                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), 48 + 40 - 17.5f, 90, 35, 35, GameState->RenderState.Textures["x_button"]->TextureHandle);
            }
            else if (InputController.ControllerType == Controller_PS4) 
            {
                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), 48 + 40 - 17.5f, 90, 35, 35, GameState->RenderState.Textures["square_button"]->TextureHandle);
            }
            
            texture* ButtonTexture = 0;
            if(InputController.ControllerType == Controller_Xbox)
            {
                ButtonTexture = GameState->RenderState.Textures["a_button"];
            }
            else if (InputController.ControllerType == Controller_PS4) 
            {
                ButtonTexture = GameState->RenderState.Textures["cross_button"];
            }
            
            auto ButtonTexWidth = ButtonTexture->Width / 2;
            b32 LootRendered = false;
            
            
            if(GameState->CharacterData.RenderWillButtonHint)
            {
                r32 Width;
                r32 Height;
                MeasureText(GameState->RenderState.RobotoFont, "Regain lost will", &Width, &Height);
                RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1,1,1,1), "Regain lost will", (r32)GameState->RenderState.WindowWidth / 2 - ButtonTexWidth - Width - 10.0f, (r32)GameState->RenderState.WindowHeight - 490, Alignment_Center);
                LootRendered = true;
            }
            
            if(!LootRendered)
            {
                for(i32 Index = 0; Index < GameState->CurrentLootCount; Index++)
                {
                    if(GameState->CurrentLoot[Index].RenderButtonHint)
                    {
                        switch(GameState->CurrentLoot[Index].Type)
                        {
                            case Loot_Health:
                            {
                                r32 Width;
                                r32 Height;
                                MeasureText(GameState->RenderState.RobotoFont,"Health Potion", &Width, &Height);
                                RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1,1,1,1), "Health Potion", (r32)GameState->RenderState.WindowWidth / 2 - ButtonTexWidth - Width - 10.0f, (r32)GameState->RenderState.WindowHeight - 490, Alignment_Center);
                            }
                            break;
                            case Loot_Checkpoint:
                            {
                                r32 Width;
                                r32 Height;
                                MeasureText(GameState->RenderState.RobotoFont,"Checkpoint", &Width, &Height);
                                RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1,1,1,1), "Checkpoint", (r32)GameState->RenderState.WindowWidth / 2 - ButtonTexWidth - Width - 10.0f, (r32)GameState->RenderState.WindowHeight - 490, Alignment_Center);
                            }
                            break;
                            case Loot_LevelItem:
                            {
                            
                                char LootText[64];
                                if(GameState->Entities[0].Player.Will >= GameState->StatData[GameState->CharacterData.Level].WillForLevel)
                                {
                                
                                    sprintf(LootText,"Spend %d will to gain power", GameState->StatData[GameState->CharacterData.Level].WillForLevel);
                                }
                                else
                                {
                                    sprintf(LootText, "Not enough will to gain power");
                                }
                                r32 Width;
                                r32 Height;
                                MeasureText(GameState->RenderState.RobotoFont,LootText, &Width, &Height);
                                RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1,1,1,1), LootText, (r32)GameState->RenderState.WindowWidth / 2 - ButtonTexWidth - Width - 10.0f, (r32)GameState->RenderState.WindowHeight - 490, Alignment_Center);
                            } break;
                        }
                        
                        RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), (r32)GameState->RenderState.WindowWidth / 2 - ButtonTexWidth, (r32)GameState->RenderState.WindowHeight - 500, 35, 35, ButtonTexture->TextureHandle);
                        
                        break;
                    }
                }
            }
            
            char Text[100];
            sprintf(Text, "%d / %d", Player.Health, GameState->CharacterData.Health);
            RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), &Text[0], 48 + 202.0f, GameState->RenderState.WindowHeight - 35.5f, Alignment_Center);
            
            sprintf(Text, "%d / %d", Player.Player.Stamina, GameState->CharacterData.Stamina);
            RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), &Text[0], 48 + 202.0f, GameState->RenderState.WindowHeight - 75.5f, Alignment_Center);
            
            // Level and experience
            sprintf(Text, "Level %d", (GameState->CharacterData.Level + 1));
            RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), &Text[0], 48.0f, (r32)GameState->RenderState.WindowHeight - 115);
            
            i32 CurrentWill = Player.Player.Will;
            
            r32 XPos = 150.0f;
            
            RenderRect(Render_Fill, &GameState->RenderState,math::v4(0, 0, 0, 1), XPos, 50.0f, 300.0f,30.0f);
            
            char Buf[64];
            sprintf(Buf,"%d",CurrentWill);
            
            r32 Width;
            r32 Height;
            
            MeasureText(GameState->RenderState.RobotoFont, Buf, &Width, &Height);
            
            RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), "Will", XPos + 5.0f, 50.0f + Height/2.0f);
            
            RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), Buf, XPos + 5.0f + 300.0f - Width * 2.0f, 50.0f + Height/2.0f);
            
            if(GameState->StatGainModeOn)
            {
                r32 HalfWidth = (r32)GameState->RenderState.WindowWidth / 2.0f;
                r32 HalfHeight = (r32)GameState->RenderState.WindowHeight / 2.0f;
                
                RenderRect(Render_Fill, &GameState->RenderState, math::v4(0, 0, 0, 1), HalfWidth - 150, HalfHeight - 100, 300, 200);
                
                RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), "Choose an upgrade", HalfWidth, HalfHeight + 50, Alignment_Center);
                
                sprintf(Text, "Health +%d", 5);
                RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), &Text[0], HalfWidth, HalfHeight + 20, Alignment_Center);
                
                sprintf(Text, "Stamina +%d", 5);
                RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), &Text[0], HalfWidth, HalfHeight - 10, Alignment_Center);
                
                sprintf(Text, "Strength +%d", 1);
                RenderText(&GameState->RenderState, GameState->RenderState.RobotoFont, math::v4(1, 1, 1, 1), &Text[0], HalfWidth, HalfHeight - 40, Alignment_Center);
                
                r32 YForSelector = HalfHeight + 20 - GameState->SelectedGainIndex * 30.0f;
                
                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), HalfWidth - 70, YForSelector - 7.5f, 15, 15);
            }
        }
        break;
        case Mode_Editor:
        {
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            
            if(GameState->EditorState.MenuOpen)
            {
                for(i32 Index = 0; Index < 3; Index++)
                {
                    if(GameState->EditorState.SelectedMenuOption == Index)
                    {
                        r32 Width = 0;
                        r32 Height = 0;
                        
                        MeasureText(GameState->RenderState.MenuFont, GameState->EditorState.MenuOptions[Index], &Width, &Height);
                        
                        RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), (r32)GameState->RenderState.WindowWidth / 2 - Width / 2 - 5, (r32)GameState->RenderState.WindowHeight / 2 - 30 + Index * 40 - Height + 5, Width + 10, Height + 10);
                        RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(0, 0, 0, 1), GameState->EditorState.MenuOptions[Index], (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight / 2 - 30 + Index * 40, Alignment_Center);
                    }
                    else
                    {
                        RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(1, 1, 1, 1), GameState->EditorState.MenuOptions[Index], (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight / 2 - 30 + Index * 40,Alignment_Center);
                    }
                }
            }
            else
            {
                switch(GameState->EditorState.Mode)
                {
                    case Editor_Level:
                    {
                        RenderRect(Render_Fill, &GameState->RenderState, math::v4(0, 0, 0, 1), 0, (r32)GameState->RenderState.WindowHeight - 155, (r32)GameState->RenderState.WindowWidth - 80, 155);
                        
                        switch(GameState->EditorState.PlacementMode)
                        {
                            case Editor_Placement_Tile:
                            {
                                RenderIsometricOutline(&GameState->RenderState, math::v4(0.7f, 0.4f, 0.0f, 1.0f), -24.5, 0, 50, 25, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                                //RenderRect(Render_Outline, &GameState->RenderState, math::v4(1.0f, 0.0f, 0.0f, 1.0f), 0, 0, (r32)GameState->CurrentLevel.Tilemap.Width, (r32)GameState->CurrentLevel.Tilemap.Height, 0, false, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                                //RenderRect(Render_Outline, &GameState->RenderState, math::v4(1.0f, 0.0f, 0.0f, 1.0f), 0, 0, (r32)GameState->CurrentLevel.Tilemap.Width, (r32)GameState->CurrentLevel.Tilemap.Height, 0, false, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                                
                                math::v2 TextureOffset = GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType + 1].TextureOffset;
                                
                                if(GameState->EditorState.TileX >= 0 && GameState->EditorState.TileX < GameState->CurrentLevel.Tilemap.Width && GameState->EditorState.TileY >= 0 && GameState->EditorState.TileY <= GameState->CurrentLevel.Tilemap.Height)
                                {
                                    const tilesheet& Tilesheet = GameState->RenderState.Tilesheets[GameState->CurrentLevel.TilesheetIndex];
                                    
                                    math::v2 SheetSize(Tilesheet.Texture.Width, Tilesheet.Texture.Height);
                                    
                                    for(i32 X = 0; X < GameState->EditorState.TileBrushSize.x && X + GameState->EditorState.TileX < GameState->CurrentLevel.Tilemap.Width; X++)
                                    {
                                        for(i32 Y = 0;Y < GameState->EditorState.TileBrushSize.y && Y + GameState->EditorState.TileY < GameState->CurrentLevel.Tilemap.Height; Y++)
                                        {
                                            math::v2 CorrectPosition = ToIsometric(math::v2(GameState->EditorState.TileX + X, GameState->EditorState.TileY + Y));
                                            
                                            //printf("Tile x %f y %f\n", GameState->EditorState.TileX, GameState->EditorState.TileY);
                                            RenderTile(&GameState->RenderState, CorrectPosition.x, CorrectPosition.y, GameState->CurrentLevel.TilesheetIndex, GameState->CurrentLevel.Tilemap.TileWidth, GameState->CurrentLevel.Tilemap.TileHeight, TextureOffset, SheetSize, math::v4(1, 1, 1, 1),  GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                                        }
                                    }
                                }
                                
                                RenderRect(Render_Fill, &GameState->RenderState, math::v4(0, 0, 0, 1), GameState->EditorState.ToolbarX, GameState->EditorState.ToolbarY, GameState->EditorState.ToolbarWidth, GameState->EditorState.ToolbarHeight);
                                
                                EditorRenderTilemap(GameState->EditorState.TilemapOffset, GameState->EditorState.RenderedTileSize, &GameState->RenderState, GameState->CurrentLevel.Tilemap);
                                
                                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 0, 0, 1), GameState->EditorState.ToolbarX + GameState->EditorState.TilemapOffset.x + GameState->EditorState.SelectedTilePosition.x * GameState->EditorState.RenderedTileSize,
                                           GameState->EditorState.ToolbarY + GameState->EditorState.TilemapOffset.y + GameState->EditorState.SelectedTilePosition.y * GameState->EditorState.RenderedTileSize, GameState->EditorState.RenderedTileSize, GameState->EditorState.RenderedTileSize, GameState->RenderState.Textures["selected_tile"]->TextureHandle);
                                           
                                char Text[255]; sprintf(Text,"Type index: %d Is solid: %d",GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType].TypeIndex,GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType].IsSolid);
                                
                                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(1, 1, 1, 1), Text, GameState->RenderState.WindowWidth * 0.6f, (r32)GameState->RenderState.WindowHeight - 90);
                                
                                char LayerText[20];
                                sprintf(LayerText, "Layer %d", GameState->EditorState.CurrentTilemapLayer);
                                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(1, 1, 1, 1), &LayerText[0], GameState->RenderState.WindowWidth * 0.6f, (r32)GameState->RenderState.WindowHeight - 140); 
                            }
                            break;
                            case Editor_Placement_PlaceEntity:
                            {
                                char* Text = "";
                                
                                switch(GameState->EditorState.PlacementEntity)
                                {
                                    case Placement_Entity_Skeleton:
                                    {
                                        Text = "Skeleton";
                                    }
                                    break;
                                    case Placement_Entity_Blob:
                                    {
                                        Text = "Blob";
                                    }
                                    break;
                                    case Placement_Entity_Wraith:
                                    {
                                        Text = "Wraith";
                                    }
                                    break;
                                    case Placement_Entity_Minotaur:
                                    {
                                        Text = "Minotaur";
                                    }
                                    break;
                                    case Placement_Entity_Barrel:
                                    {
                                        Text = "Barrel";
                                    }
                                    break;
                                    case Placement_Entity_Bonfire:
                                    {
                                        Text = "Bonfire";
                                    }
                                    break;
                                }
                                
                                RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, math::v4(1, 1, 1, 1), Text, (r32)InputController.MouseX, GameState->RenderState.WindowHeight - (r32)InputController.MouseY + 20, Alignment_Center); 
                                
                                if(GameState->EditorState.SelectedEntity)
                                    RenderWireframe(&GameState->RenderState, GameState->EditorState.SelectedEntity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                            }
                            break;
                            case Editor_Placement_SelectEntity:
                            {
                                if(GameState->EditorState.SelectedEntity)
                                    RenderWireframe(&GameState->RenderState, GameState->EditorState.SelectedEntity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                            }
                            break;
                        }
                        
                        if(GameState->EditorState.PlacementMode == Editor_Placement_Tile)
                            RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(0.6f, 0.6f, 0.6f, 1), "Tile-mode", (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight - 70, Alignment_Center);
                        else
                            RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(0.6f, 0.6f, 0.6f, 1), "Entity-mode", (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight - 70, Alignment_Center);
                    }
                    break;
                    case Editor_Animation:
                    {
                        RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(0.6f, 0.6f, 0.6f, 1), "Animations", (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight - 70, Alignment_Center);
                        
                        if(GameState->EditorState.AnimationMode == Animation_SelectAnimation)
                        {
                            r32 StartingY = 100.0f;
                            
                            for(i32 Index = 0; Index < GameState->AnimationIndex; Index++)
                            {
                                const animation& Animation = GameState->AnimationArray[Index];
                                
                                r32 ExtraX = Index >= 20 ? 120.0f : 0.0f;
                                i32 CalculatedIndex = (Index >= 20 ? Index - 20 : Index);
                                
                                if(Index == GameState->EditorState.SelectedAnimation)
                                {
                                    RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), ExtraX + 20, StartingY + (20 - CalculatedIndex) * 15 - 4, 100, 15);
                                    RenderText(&GameState->RenderState, GameState->RenderState.SmallerInconsolataFont, math::v4(0, 0, 0, 1), Animation.Name, ExtraX + 20, StartingY + (20 - CalculatedIndex) * 15);
                                }
                                else
                                    RenderText(&GameState->RenderState, GameState->RenderState.SmallerInconsolataFont, math::v4(1, 1, 1, 1), Animation.Name, ExtraX + 20, StartingY + (20 - CalculatedIndex) * 15);
                            }
                        }
                        else
                        {
                            if(GameState->EditorState.AnimationMode == Animation_SelectTexture)
                            {
                                for(i32 TextureIndex = 0; TextureIndex < GameState->RenderState.TextureIndex; TextureIndex++)
                                {
                                    if(TextureIndex == GameState->EditorState.SelectedTexture)
                                    {
                                        RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), 15, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->RenderState.TextureIndex - TextureIndex) * 15 - 350 - 4, 300, 20);
                                        RenderText(&GameState->RenderState, GameState->RenderState.SmallerInconsolataFont, math::v4(0, 0, 0, 1), GameState->RenderState.TextureArray[TextureIndex].Name, 20, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->RenderState.TextureIndex - TextureIndex) * 15 - 350);
                                    }
                                    else
                                        RenderText(&GameState->RenderState, GameState->RenderState.SmallerInconsolataFont, math::v4(1, 1, 1, 1), GameState->RenderState.TextureArray[TextureIndex].Name, 20, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->RenderState.TextureIndex - TextureIndex) * 15 - 350);
                                }
                            }
                            
                            if(GameState->EditorState.LoadedAnimation)
                            {
                                const texture& Texture = *GameState->EditorState.LoadedAnimation->Texture;
                                r32 Scale = 1.0f;
                                r32 TextureWidth = (r32)Texture.Width * Scale;
                                r32 TextureHeight = (r32)Texture.Height * Scale;
                                
                                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), (r32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2, (r32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2, TextureWidth, TextureHeight);
                                
                                RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 1, 1, 1), (r32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2, (r32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2, TextureWidth, TextureHeight, Texture.TextureHandle);
                                
                                
                                if(GameState->EditorState.AnimationFrameCountField)
                                {
                                    i32 FrameCount = 0;
                                    i32 FrameWidth = 0;
                                    i32 FrameHeight = 0;
                                    i32 FrameOffsetX = 0;
                                    i32 FrameOffsetY = 0;
                                    
                                    sscanf(GameState->EditorState.AnimationFrameCountField->Text, "%d", &FrameCount);
                                    sscanf(GameState->EditorState.AnimationFrameWidthField->Text, "%d", &FrameWidth);
                                    sscanf(GameState->EditorState.AnimationFrameHeightField->Text, "%d", &FrameHeight);
                                    sscanf(GameState->EditorState.AnimationFrameOffsetXField->Text, "%d", &FrameOffsetX);
                                    sscanf(GameState->EditorState.AnimationFrameOffsetYField->Text, "%d", &FrameOffsetY);
                                    
                                    r32 StartX = (r32)GameState->RenderState.WindowWidth / 2.0f - (r32)TextureWidth / 2.0f;
                                    r32 StartY = (r32)GameState->RenderState.WindowHeight / 2.0f - (r32)TextureHeight / 2.0f;
                                    
                                    r32 X = (r32)(FrameWidth * FrameOffsetX);
                                    r32 Y = (r32)(FrameHeight * FrameOffsetY);
                                    
                                    for(i32 Index = 0; Index < FrameCount; Index++)
                                    {
                                        if(Index > 0)
                                        {
                                            if(X + FrameWidth < (r32)Texture.Width)
                                            {
                                                X += FrameWidth;
                                            }
                                            else
                                            {
                                                X = 0;
                                                Y += FrameHeight;
                                            }
                                        }
                                        
                                        RenderRect(Render_Outline, &GameState->RenderState, math::v4(1, 0, 0, 1), StartX + X * Scale, StartY - Y * Scale + TextureHeight - FrameHeight * Scale,(r32)FrameWidth * Scale, (r32)FrameHeight * Scale);
                                    }
                                    
                                }
                                
                                RenderRect(Render_Outline, &GameState->RenderState, math::v4(1, 0, 0, 1), (r32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2, (r32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2, TextureWidth, TextureHeight);
                                
                                if(GameState->EditorState.LoadedAnimation->FrameCount > 0)
                                    RenderAnimationPreview(&GameState->RenderState, GameState->EditorState.AnimationInfo, *GameState->EditorState.LoadedAnimation, math::v2((r32)GameState->RenderState.WindowWidth - 300, 60), 2);
                            }
                        }
                        
                        RenderButton(&GameState->RenderState, *GameState->EditorState.CreateNewAnimationButton);
                    }
                    break;
                }
                
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, math::v4(1, 1, 1, 1), "EDITOR", (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight - 30, Alignment_Center);
            }
            
            for(i32 ButtonIndex = 0; ButtonIndex < 10; ButtonIndex++)
            {
                if(GameState->EditorState.Buttons[ButtonIndex].Active)
                    RenderButton(&GameState->RenderState, GameState->EditorState.Buttons[ButtonIndex]);
            }
            
            for(u32 Index = 0; Index < 20; Index++)
            {
                if(GameState->EditorState.Textfields[Index].Active)
                    RenderTextfield(&GameState->RenderState, GameState->EditorState.Textfields[Index]);
            }
            
            for(u32 Index = 0; Index < 10; Index++)
            {
                if(GameState->EditorState.Checkboxes[Index].Active)
                    RenderCheckbox(&GameState->RenderState, GameState->EditorState.Checkboxes[Index]);
            }
        }
        break;
    }
    
    if(GameState->Camera.FadingMode != Fading_None)
    {
        auto Color = GameState->Camera.FadingTint;
        
        RenderRect(Render_Fill, &GameState->RenderState, math::v4(Color.x, Color.y, Color.z, GameState->Camera.FadingAlpha), 0, 0, (r32)GameState->RenderState.WindowWidth, (r32)GameState->RenderState.WindowHeight);
    }
    
    if(GameState->PlayerState == Player_Dead)
    {
        RenderRect(Render_Fill, &GameState->RenderState, math::v4(1, 0, 0, 0.2f), 0, 0, (r32)GameState->RenderState.WindowWidth, (r32)GameState->RenderState.WindowHeight);
        
        r32 Width = 0;
        r32 Height = 0;
        MeasureText(GameState->RenderState.TitleFont, "YOU DIED", &Width, &Height);
        
        RenderText(&GameState->RenderState, GameState->RenderState.TitleFont, math::v4(1, 1, 1, 1), "YOU DIED", (r32)GameState->RenderState.WindowWidth / 2 - Width / 2, (r32)GameState->RenderState.WindowHeight / 2 - Height / 2);
        
        MeasureText(GameState->RenderState.TitleFont, "Press any key to restart. . .", &Width, &Height);
        RenderText(&GameState->RenderState, GameState->RenderState.TitleFont, math::v4(1, 1, 1, 1), "Press any key to restart. . .", (r32)GameState->RenderState.WindowWidth / 2 - Width / 2, (r32)GameState->RenderState.WindowHeight / 2 - Height * 2);
    }
}
*/

// @Inefficient: Maybe find a way to update only parts of the tilemap when placing tiles
// @Incomplete: Do it differently?
/*
static void CheckLevelVAO(game_memory* GameMemory)
{
    game_state* GameState = (game_state*)GameMemory->PermanentStorage;
    if(GameState->CurrentLevel.Tilemap.RenderInfo.Dirty)
    {
        if(GameState->CurrentLevel.Tilemap.RenderInfo.DirtyLayer == -1)
        {
            for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
            {
                if(GameState->CurrentLevel.Tilemap.RenderInfo.VAOS[Layer] == 0)
                {
                    CreateTilemapVAO(&GameState->RenderState, GameState->CurrentLevel.Tilemap, GameState->CurrentLevel.Type,
                                     &GameState->CurrentLevel.Tilemap.EditorRenderInfo, &GameState->CurrentLevel.Tilemap.RenderInfo, Layer);
                }
            }
            LoadTilemapWireframeBuffer(GameState->CurrentLevel.Tilemap, &GameState->RenderState, &GameState->CurrentLevel.Tilemap.RenderInfo.WireframeVAO, &GameState->CurrentLevel.Tilemap.RenderInfo.WireframeVBO, &GameState->CurrentLevel.Tilemap.RenderInfo.WireframeVBOSize);
        }
        else
        {
            CreateTilemapVAO(&GameState->RenderState, GameState->CurrentLevel.Tilemap,
                             GameState->CurrentLevel.Type,
                             &GameState->CurrentLevel.Tilemap.EditorRenderInfo, &GameState->CurrentLevel.Tilemap.RenderInfo, GameState->CurrentLevel.Tilemap.RenderInfo.DirtyLayer);
            GameState->CurrentLevel.Tilemap.RenderInfo.DirtyLayer = -1;
            LoadTilemapWireframeBuffer(GameState->CurrentLevel.Tilemap, &GameState->RenderState, &GameState->CurrentLevel.Tilemap.RenderInfo.WireframeVAO, &GameState->CurrentLevel.Tilemap.RenderInfo.WireframeVBO, &GameState->CurrentLevel.Tilemap.RenderInfo.WireframeVBOSize);
        }
    }
}
*/

//@Incomplete: DebugInfo should be pushed just like other rendering from game
/*
static void RenderDebugInfo(game_state* GameState)
{
    auto Pos = math::UnProject(math::v3(InputController.MouseX, GameState->RenderState.Viewport[3] - InputController.MouseY, 0),
                               GameState->Camera.ViewMatrix,
                               GameState->Camera.ProjectionMatrix,
                               math::v4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
                               
    if(GameState->Console.CurrentTime > 0)
        RenderConsole(GameState, &GameState->Console);
        
    if(GameState->RenderState.RenderFPS)
    {
        char FPS[32];
        sprintf(FPS, "%4.0f",GameState->RenderState.FPS);
        RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, 
                   math::v4(1, 1, 1, 1), FPS, GameState->RenderState.WindowWidth / 2.0f, 
                   GameState->RenderState.WindowHeight - 20.0f);
                   
        i32 X = (i32)math::Floor(Pos.x);
        i32 Y = (i32)math::Floor(Pos.y);
        char MousePos[32];
        sprintf(MousePos,"Mouse: (%d %d)",X,Y);
        RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, 
                   math::v4(1, 1, 1, 1), MousePos, GameState->RenderState.WindowWidth / 2.0f - 200, 
                   GameState->RenderState.WindowHeight - 20.0f);
    }
}
*/

//@Incomplete: We need to push light sources and do it differently as well!!!
/*
static void RenderLightSources(game_state* GameState)
{
    glBindVertexArray(GameState->RenderState.FrameBufferVAO);
    glBindFramebuffer(GL_FRAMEBUFFER, GameState->RenderState.LightingFrameBuffer);
    glBindTexture(GL_TEXTURE_2D, GameState->RenderState.LightingTextureColorBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    
    math::m4 P = GameState->Camera.ProjectionMatrix;
    math::m4 V = GameState->Camera.ViewMatrix;
    
    if(GameState->GameMode == Mode_InGame || GameState->EditorState.Mode == Editor_Level)
    {
        math::v4 PointlightPositions[32];
        r32 PointlightIntensities[32];
        r32 PointlightConstantAtt[32];
        r32 PointlightLinearAtt[32];
        r32 PointlightExponentialAtt[32];
        math::v4 PointlightColors[32];
        i32 NumOfPointLights = 0;
        
        for(u32 Index = 0; Index < GameState->LightSourceCount; Index++)
        {
            auto& LightSource = GameState->LightSources[Index];
            if(LightSource.Active)
            {
                switch(LightSource.Type)
                {
                    case Light_Pointlight:
                    {
                        math::m4 Model(1.0f);
                        
                        auto Position = LightSource.Pointlight.RenderPosition + LightSource.Pointlight.Offset;
                        
                        Model = math::Translate(Model, math::v3(Position.x, Position.y, 0));
                        PointlightPositions[NumOfPointLights] = Model * math::v4(0.0f, 0.0f, 0.0f, 1.0f);
                        PointlightIntensities[NumOfPointLights] = LightSource.Pointlight.Intensity;
                        PointlightColors[NumOfPointLights] = LightSource.Color;
                        PointlightConstantAtt[NumOfPointLights] = LightSource.Pointlight.ConstantAtten;
                        PointlightLinearAtt[NumOfPointLights] = LightSource.Pointlight.LinearAtten;
                        PointlightExponentialAtt[NumOfPointLights] = LightSource.Pointlight.ExponentialAtten;
                        NumOfPointLights++;
                    }
                    break;
                }
            }
        }
        
        auto Shader = GameState->RenderState.LightSourceShader;
        
        UseShader(&Shader);
        
        SetMat4Uniform(Shader.Program, "P", P);
        SetMat4Uniform(Shader.Program, "V", V);
        SetVec4ArrayUniform(Shader.Program, "PointLightColors", PointlightColors,NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightIntensity", PointlightIntensities, NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightConstantAtt", PointlightConstantAtt, NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightLinearAtt", PointlightLinearAtt, NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightExpAtt", PointlightExponentialAtt, NumOfPointLights);
        SetIntUniform(Shader.Program, "NUM_POINTLIGHTS", NumOfPointLights);
        SetVec4ArrayUniform(Shader.Program, "PointLightPos",PointlightPositions, NumOfPointLights);
        SetVec2Uniform(Shader.Program, "screenSize", math::v2((r32)GameState->RenderState.WindowWidth,(r32)GameState->RenderState.WindowHeight));
        
        glDrawElements(GL_TRIANGLES, sizeof(GameState->RenderState.QuadIndices), GL_UNSIGNED_INT, (void*)0);
        
        glBindVertexArray(0);
    }
}
*/


