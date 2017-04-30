static GLint ShaderCompilationErrorChecking(GLuint Shader)
{
    GLint IsCompiled = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if (!IsCompiled)
    {
        GLint MaxLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &MaxLength);
        
        // The maxLength includes the NULL character
        std::vector<GLchar> ErrorLog(MaxLength);
        glGetShaderInfoLog(Shader, MaxLength, &MaxLength, &ErrorLog[0]);
        
        std::cout << "SHADER Compilation error" << std::endl;
        
        for (std::vector<GLchar>::const_iterator i = ErrorLog.begin(); i != ErrorLog.end(); ++i)
            std::cout << *i;
        
        glDeleteShader(Shader); // Don't leak the shader.
    }
    return IsCompiled;
}

static GLuint LoadShader(const std::string FilePath, shader *Shd)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLchar *VertexText = LoadShaderFromFile(FilePath + std::string(".vert"));
    
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if (!ShaderCompilationErrorChecking(Shd->VertexShader))
        return GL_FALSE;
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar *FragmentText = LoadShaderFromFile(FilePath + std::string(".frag"));
    
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if (!ShaderCompilationErrorChecking(Shd->FragmentShader))
        return GL_FALSE;
    
    Shd->Program = glCreateProgram();
    
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static GLuint LoadVertexShader(const std::string FilePath, shader *Shd)
{
    Shd->Program = glCreateProgram();
    
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLchar *VertexText = LoadShaderFromFile(FilePath + std::string(".vert"));
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if (!ShaderCompilationErrorChecking(Shd->VertexShader))
        return GL_FALSE;
    
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static GLuint LoadFragmentShader(const std::string FilePath, shader *Shd)
{
    Shd->Program = glCreateProgram();
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar *FragmentText = LoadShaderFromFile(FilePath + std::string(".frag"));
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if (!ShaderCompilationErrorChecking(Shd->FragmentShader))
        return GL_FALSE;
    
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static void UseShader(shader *Shader)
{
    glUseProgram(Shader->Program);
}

static void InitializeFreeTypeFont(FT_Library Library, render_font* Font, shader* Shader)
{
    if(FT_New_Face(Library, "../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 0, &Font->Face)) 
    {
        fprintf(stderr, "Could not open font\n");
    }
    
    FT_Set_Pixel_Sizes(Font->Face, 0, 30);
    
    FT_Select_Charmap(Font->Face , ft_encoding_unicode);
    
    //Find the atlas width and height
    FT_GlyphSlot G = Font->Face->glyph;
    
    unsigned int W = 0;
    unsigned int H = 0;
    
    for(int i = 0; i < 255; i++) 
    {
        if(FT_Load_Char(Font->Face, i, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }
        
        W += G->bitmap.width;
        H = std::max(H, G->bitmap.rows);
    }
    
    Font->AtlasWidth = W;
    Font->AtlasHeight = H;
    Font->GlyphWidth = (GLfloat)G->bitmap.width;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &Font->Texture);
    glBindTexture(GL_TEXTURE_2D, Font->Texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, Font->AtlasWidth, Font->AtlasHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    unsigned int X = 0;
    
    for(int i = 0; i < 255; i++) 
    {
        if(FT_Load_Char(Font->Face, i, FT_LOAD_RENDER))
            continue;
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, X, 0, G->bitmap.width, G->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, G->bitmap.buffer);
        
        Font->CharacterInfo[i].AX = (real32)(G->advance.x >> 6);
        Font->CharacterInfo[i].AY = (real32)(G->advance.y >> 6);
        
        Font->CharacterInfo[i].BW = (real32)G->bitmap.width;
        Font->CharacterInfo[i].BH = (real32)G->bitmap.rows;
        
        Font->CharacterInfo[i].BL = (real32)G->bitmap_left;
        Font->CharacterInfo[i].BT = (real32)G->bitmap_top;
        
        Font->CharacterInfo[i].TX = (real32)X / Font->AtlasWidth;
        
        X += G->bitmap.width;
    }
    
    glGenVertexArrays(1, &Font->VAO);
    glBindVertexArray(Font->VAO);
    
    glGenBuffers(1, &Font->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, Font->VBO);
    auto TexcoordLocation = glGetAttribLocation(Shader->Program, "coord");
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

static void RenderSetup(render_state *RenderState)
{
    if(FT_Init_FreeType(&RenderState->FTLibrary)) 
    {
        fprintf(stderr, "Could not init freetype library\n");
    }
    
    //Sprite
    glGenVertexArrays(1, &RenderState->SpriteVAO);
    glBindVertexArray(RenderState->SpriteVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->TextureShader.Type = Shader_Texture;
    LoadShader(ShaderPaths[Shader_Texture], &RenderState->TextureShader);
    
    auto PositionLocation = glGetAttribLocation(RenderState->TextureShader.Program, "pos");
    auto TexcoordLocation = glGetAttribLocation(RenderState->TextureShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //Animation
    //Sprite
    glGenVertexArrays(1, &RenderState->SpriteSheetVAO);
    glBindVertexArray(RenderState->SpriteSheetVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->TextureShader.Type = Shader_SpriteSheetShader;
    LoadShader(ShaderPaths[Shader_SpriteSheetShader], &RenderState->SpriteSheetShader);
    
    PositionLocation = glGetAttribLocation(RenderState->SpriteSheetShader.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->SpriteSheetShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    
    //tile
    glGenVertexArrays(1, &RenderState->TileVAO);
    glBindVertexArray(RenderState->TileVAO);
    glGenBuffers(1, &RenderState->TileQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->TileQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->TileQuadVerticesSize, RenderState->TileQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->TileShader.Type = Shader_Tile;
    LoadShader(ShaderPaths[Shader_Tile], &RenderState->TileShader);
    
    auto PositionLocation2 = glGetAttribLocation(RenderState->TileShader.Program, "pos");
    auto TexcoordLocation2 = glGetAttribLocation(RenderState->TileShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation2);
    glVertexAttribPointer(PositionLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation2);
    glVertexAttribPointer(TexcoordLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //console
    glGenVertexArrays(1, &RenderState->ConsoleVAO);
    glBindVertexArray(RenderState->ConsoleVAO);
    glGenBuffers(1, &RenderState->ConsoleQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->ConsoleQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->ConsoleQuadVerticesSize, RenderState->ConsoleQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->ConsoleShader.Type = Shader_Console;
    LoadShader(ShaderPaths[Shader_Console], &RenderState->ConsoleShader);
    
    auto PositionLocation3 = glGetAttribLocation(RenderState->ConsoleShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    //font
    RenderState->StandardFontShader.Type = Shader_StandardFont;
    LoadShader(ShaderPaths[Shader_StandardFont], &RenderState->StandardFontShader);
    
    RenderState->InconsolataFont = {};
    InitializeFreeTypeFont(RenderState->FTLibrary, &RenderState->InconsolataFont, &RenderState->StandardFontShader);
}

static void ReloadVertexShader(Shader_Type Type, render_state* RenderState)
{
    glDeleteProgram(RenderState->Shaders[Type].Program);
    glDeleteShader(RenderState->Shaders[Type].VertexShader);
    LoadVertexShader(ShaderPaths[Type], &RenderState->Shaders[Type]);
}

static void ReloadFragmentShader(Shader_Type Type, render_state* RenderState)
{
    glDeleteProgram(RenderState->Shaders[Type].Program);
    glDeleteShader(RenderState->Shaders[Type].FragmentShader);
    LoadFragmentShader(ShaderPaths[Type], &RenderState->Shaders[Type]);
}

static GLuint LoadTexture(const char *FilePath)
{
    GLuint TextureHandle;
    
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    //enable alpha for textures
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    int Width, Height;
    unsigned char* Image = stbi_load(FilePath, &Width, &Height, 0, STBI_rgb_alpha);
    
    if (!Image)
        return GL_FALSE;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, Image);
    
    stbi_image_free(Image);
    
    return TextureHandle;
}

static void ReloadAssets(asset_manager *AssetManager, game_state* GameState)
{
    for(int i = 0; i < Shader_Count; i++)
    {
        if(AssetManager->DirtyVertexShaderIndices[i] == 1)
        {
            printf("Reloading vertex shader type: %d\n", i);
            ReloadVertexShader((Shader_Type)i, &GameState->RenderState);
            AssetManager->DirtyVertexShaderIndices[i] = 0;
        }
        
        if(AssetManager->DirtyFragmentShaderIndices[i] == 1)
        {
            printf("Reloading fragment shader type: %d\n", i);
            ReloadFragmentShader((Shader_Type)i, &GameState->RenderState);
            AssetManager->DirtyFragmentShaderIndices[i] = 0;
        }
    }
    
    if(AssetManager->DirtyTileset == 1)
    {
        GameState->Room.RenderEntity.TextureHandle = LoadTexture("./assets/textures/tiles.png");
        AssetManager->DirtyTileset = 0;
    }
}

static void SetVec2Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec2 Value)
{
    glUniform2f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y);
}

static void SetVec3Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec3 Value)
{
    glUniform3f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z);
}

static void SetVec4Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec4 Value)
{
    glUniform4f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z, Value.w);
}

static void SetMat4Uniform(GLuint ShaderHandle, const char *UniformName, glm::mat4 Value)
{
    glUniformMatrix4fv(glGetUniformLocation(ShaderHandle, UniformName), 1, GL_FALSE, &Value[0][0]);
}

struct Point
{
    GLfloat X;
    GLfloat Y;
    GLfloat S;
    GLfloat T;
}; 

//TODO(Daniel) there's a weird bug when rendering special characters. The cursor just slowly jumps up for every character pressed

//rendering methods
static void RenderText(render_state* RenderState, const render_font& Font, const char *Text, real32 X, real32 Y, real32 SX, real32 SY) 
{
    auto Shader = RenderState->Shaders[Shader_StandardFont];
    UseShader(&Shader);
    SetVec4Attribute(Shader.Program, "color", Font.Color);
    
    if (RenderState->BoundTexture != Font.Texture) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Font.Texture);
        RenderState->BoundTexture = Font.Texture;
    }
    
    Point* Coords = new Point[6 * strlen(Text)]; //TODO change this back to the C way (malloc)
    
    int N = 0;
    
    for(const char *P = Text; *P; P++) 
    { 
        float X2 =  X + Font.CharacterInfo[*P ].BL * SX;
        float Y2 = -Y - Font.CharacterInfo[*P ].BT * SY;
        float W = Font.CharacterInfo[*P].BW * SX;
        float H = Font.CharacterInfo[*P].BH * SY;
        
        /* Advance the cursor to the start of the next character */
        X += Font.CharacterInfo[*P].AX * SX;
        Y += Font.CharacterInfo[*P].AY * SY;
        
        /* Skip glyphs that have no pixels */
        if(!W || !H)
            continue;
        
        Coords[N++] = { X2, -Y2, Font.CharacterInfo[*P].TX, 0 };
        Coords[N++] = { X2 + W, -Y2, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth, 0 };
        Coords[N++] = { X2, -Y2 - H, Font.CharacterInfo[*P].TX, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
        Coords[N++] = { X2 + W, -Y2, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth,  0 };
        Coords[N++] = { X2, -Y2 - H, Font.CharacterInfo[*P].TX, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
        Coords[N++] = { X2 + W, -Y2 - H, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
    }
    
    glBindVertexArray(Font.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Font.VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * strlen(Text) * sizeof(Point), Coords, GL_DYNAMIC_DRAW);
    
    glDrawArrays(GL_TRIANGLES, 0, N);
}

static void MeasureText(render_font* Font, const char* Text, float* Width, float* Height)
{
    int Count, C;
    
    if (!Text) 
    {
        return;
    }
    
    if (Width) 
    {
        /* Width of a text rectangle is a sum advances for every glyph in a string */
        *Width = 0.0f;
        
        for(Count = 0; Count < strlen(Text); ++Count) 
        {
            C= Text[Count];
            *Width += Font->CharacterInfo[C].AX;
        }
    }
    
    if (Height)
    {
        *Height = 0.0f;
        
        for(Count = 0; Count < strlen(Text); ++Count) 
        {
            C= Text[Count];
            
            if (*Height < Font->CharacterInfo[C].AY) 
            {
                *Height = Font->CharacterInfo[C].AY;
            }
        }
    }
}

static void RenderConsole(render_state* RenderState, console* Console, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    auto Shader = RenderState->Shaders[Shader_Console];
    
    UseShader(&Shader);
    
    real32 PercentAnimated = 1.0f + 1.0f - Console->CurrentTime / Console->TimeToAnimate;
    
    //draw upper part
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(-1, 0.52f * PercentAnimated, 0));
    Model = glm::scale(Model, glm::vec3(2, 0.5, 1));
    SetMat4Uniform(Shader.Program, "M", Model);
    SetVec4Attribute(Shader.Program, "color", glm::vec4(0, 0.4, 0.3, 0.6));
    
    glBindVertexArray(RenderState->ConsoleVAO); //TODO(Daniel) Create a vertex array buffer + object for console
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->ConsoleQuadVBO);
    glDrawArrays(GL_QUADS, 0, 4);
    
    //draw lower bar
    glm::mat4 SecondModel(1.0f);
    SecondModel = glm::translate(SecondModel, glm::vec3(-1, 0.5f * PercentAnimated, 0));
    SecondModel = glm::scale(SecondModel, glm::vec3(2, 0.08, 1));
    SetMat4Uniform(Shader.Program, "M", SecondModel);
    SetVec4Attribute(Shader.Program, "color", glm::vec4(0, 0.2, 0.2, 0.6));
    
    glDrawArrays(GL_QUADS, 0, 4);
    
    const GLFWvidmode *Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    real32 SX = 2.0f / Mode->width;
    real32 SY = 2.0f / Mode->height;
    
    // auto CursorShader = RenderState->Shaders[Shader_ConsoleCursor];
    glm::mat4 CursorModel(1.0f);
    real32 Width;
    real32 Height;
    MeasureText(&RenderState->InconsolataFont, &Console->Buffer[0], &Width, &Height);
    
    CursorModel = glm::translate(CursorModel, glm::vec3(-0.97 + Width * SX, 0.51 * PercentAnimated, 0));
    CursorModel = glm::scale(CursorModel, glm::vec3(0.015, 0.06, 1));
    
    SetMat4Uniform(Shader.Program, "M", CursorModel);
    
    GLfloat TimeValue = (real32)glfwGetTime();
    GLfloat AlphaValue = (real32)((sin(TimeValue * 6) / 2) + 0.5);
    
    SetVec4Attribute(Shader.Program, "color", glm::vec4(0, 1.0, 0.5, AlphaValue));
    
    glDrawArrays(GL_QUADS, 0, 4);
    
    RenderState->InconsolataFont.Color = glm::vec4(1, 1, 1, 1);
    
    RenderText(RenderState, RenderState->InconsolataFont, ">", -1 + 8 * SX, 0.61f * PercentAnimated - 50 * PercentAnimated * SY, SX, SY);
    RenderText(RenderState, RenderState->InconsolataFont, &Console->Buffer[0], -0.98f + 8 * SX, 0.61f * PercentAnimated - 50 * PercentAnimated * SY, SX, SY); //TODO(Daniel) UNICODE
    
    RenderState->InconsolataFont.Color = glm::vec4(0.8, 0.8, 0.8, 1);
    
    int index = 0;
    
    for(int i = 0; i < HISTORY_BUFFER_LINES; i++)
    {
        RenderText(RenderState, RenderState->InconsolataFont, &Console->HistoryBuffer[i][0], -1 + 8 * SX, 0.69f * PercentAnimated + i * 0.06f - 50 * PercentAnimated * SY, SX, SY);
    }
}

static void RenderEntity(render_state *RenderState, entity &Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{ 
    auto Shader = RenderState->Shaders[Entity.RenderEntity.ShaderIndex];
    UseShader(&Shader);
    
    switch(Entity.Type)
    {
        case Entity_Crosshair:
        case Entity_Player:
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(Entity.Position.x, Entity.Position.y, 0.0f));
            Model = glm::translate(Model, glm::vec3(1, 1, 0.0f)); 
            Model = glm::rotate(Model, Entity.Rotation.z, glm::vec3(0, 0, 1)); //NOTE(Daniel) 1.56 is approximately 90 degrees in radians
            Model = glm::translate(Model, glm::vec3(-1, -1, 0.0f)); 
            Model = glm::scale(Model, Entity.Scale);
            
            if(Entity.CurrentAnimation) 
            {
                auto Animation = Entity.Animations[Entity.CurrentAnimation];
                
                if (RenderState->BoundTexture != Animation.TextureHandle) //never bind the same texture if it's already bound
                {
                    glBindTexture(GL_TEXTURE_2D, Animation.TextureHandle);
                    RenderState->BoundTexture = Animation.TextureHandle;
                }
                
                glBindVertexArray(RenderState->SpriteSheetVAO);
                
                auto Frame = Animation.Frames[Animation.FrameIndex];
                SetVec2Attribute(Shader.Program,"textureOffset", glm::vec2(Frame.X,Frame.Y));
                
                SetVec2Attribute(Shader.Program,"sheetSize",
                                 glm::vec2(Animation.Rows, Animation.Columns));
            } 
            else 
            {
                if (RenderState->BoundTexture != Entity.RenderEntity.TextureHandle) //never bind the same texture if it's already bound
                {
                    glBindTexture(GL_TEXTURE_2D, Entity.RenderEntity.TextureHandle);
                    RenderState->BoundTexture = Entity.RenderEntity.TextureHandle;
                }
                
                glBindVertexArray(RenderState->SpriteVAO);
            }
            glm::mat4 MVP = ProjectionMatrix * View * Model;
            SetMat4Uniform(Shader.Program, "MVP", MVP);
            break;
        }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}


static void RenderRoom(render_state* RenderState, const room& Room,  glm::mat4 ProjectionMatrix, glm::mat4 View, int StartX, int StartY, int EndX, int EndY)
{
    if (RenderState->BoundTexture != Room.RenderEntity.TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Room.RenderEntity.TextureHandle);
        RenderState->BoundTexture = Room.RenderEntity.TextureHandle;
    }
    
    glBindVertexArray(RenderState->TileVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    
    auto Shader = RenderState->Shaders[Room.RenderEntity.ShaderIndex];
    UseShader(&Shader);
    
    real32 Scale = 1.0f;
    
    for (int i = 0; i < ROOM_WIDTH; i++)
    {
        for (int j = 0; j < ROOM_HEIGHT; j++)
        {
            if(Room.Data[i][j].Type != Tile_None)
            {
                glm::mat4 Model(1.0f);
                Model = glm::translate(Model, glm::vec3(i * Scale, j * Scale, 0.0f));
                Model = glm::scale(Model, glm::vec3(Scale, Scale, 1.0f));
                glm::mat4 MVP = ProjectionMatrix * View * Model;
                
                SetVec2Attribute(Shader.Program, "textureOffset", Room.Data[i][j].TextureOffset);
                SetMat4Uniform(Shader.Program, "MVP", MVP);
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
    }
    
    glBindVertexArray(0);
}

static void Render(game_state* GameState)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1.0f);
    
    RenderRoom(&GameState->RenderState, GameState->Room, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix, 0, 0, 0, 0);
    
    RenderEntity(&GameState->RenderState, GameState->Player, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    RenderEntity(&GameState->RenderState, GameState->Crosshair, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    
    if(GameState->Console.CurrentTime > 0)
        RenderConsole(&GameState->RenderState, &GameState->Console, GameState->Camera.ProjectionMatrix,  GameState->Camera.ViewMatrix);
    
    const GLFWvidmode *Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    
    glfwSwapBuffers(GameState->RenderState.Window);
}