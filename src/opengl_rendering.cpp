static void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glfwSetWindowAspectRatio(Window, 16, 9);
    glViewport(0, 0, Width, Height);
}

inline static real64 GetTime()
{
    return glfwGetTime();
}

inline static void PollEvents()
{
    glfwPollEvents();
}

static void CloseWindow(game_state* GameState)
{
    glfwDestroyWindow(GameState->RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static bool32 ShouldCloseWindow(render_state* RenderState)
{
    return glfwWindowShouldClose(RenderState->Window); 
}

static GLint ShaderCompilationErrorChecking(const char* ShaderName, GLuint Shader)
{
    GLint IsCompiled = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if (!IsCompiled)
    {
        GLint MaxLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &MaxLength);
        
        // The maxLength includes the NULL character
        GLchar* ErrorLog = (GLchar*)malloc(MaxLength);
        glGetShaderInfoLog(Shader, MaxLength, &MaxLength, ErrorLog);
        
        printf("SHADER Compilation error - %s\n", ShaderName);
        printf(ErrorLog);
        
        glDeleteShader(Shader); // Don't leak the shader.
    }
    return IsCompiled;
}

static GLuint LoadShader(const char* FilePath, shader *Shd)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* VertexString = Concat(FilePath,".vert");
    GLchar *VertexText = LoadShaderFromFile(VertexString);
    
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->VertexShader))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* FragmentString = Concat(FilePath,".frag");
    GLchar *FragmentText = LoadShaderFromFile(FragmentString);
    
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->FragmentShader))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    
    Shd->Program = glCreateProgram();
    
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static GLuint LoadVertexShader(const char* FilePath, shader *Shd)
{
    Shd->Program = glCreateProgram();
    
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* VertexString = Concat(FilePath,".vert");
    GLchar *VertexText = LoadShaderFromFile(VertexString);
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->VertexShader))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static GLuint LoadFragmentShader(const char* FilePath, shader *Shd)
{
    Shd->Program = glCreateProgram();
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* FragmentString = Concat(FilePath,".frag");
    GLchar *FragmentText = LoadShaderFromFile(FragmentString);
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->FragmentShader))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static void UseShader(shader *Shader)
{
    glUseProgram(Shader->Program);
}

static void InitializeFreeTypeFont(char* FontPath, int FontSize, FT_Library Library, render_font* Font, shader* Shader)
{
    if(FT_New_Face(Library, FontPath, 0, &Font->Face)) 
    {
        fprintf(stderr, "Could not open font\n");
    }
    
    FT_Set_Pixel_Sizes(Font->Face, 0, FontSize);
    
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
        H = Max(H, G->bitmap.rows);
    }
    
    Font->AtlasWidth = W;
    Font->AtlasHeight = H;
    Font->GlyphWidth = (GLfloat)G->bitmap.width;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &Font->Texture);
    glBindTexture(GL_TEXTURE_2D, Font->Texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, Font->AtlasWidth, Font->AtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    unsigned int X = 0;
    
    for(int i = 0; i < 255; i++) 
    {
        if(FT_Load_Char(Font->Face, i, FT_LOAD_RENDER))
            continue;
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, X, 0, G->bitmap.width, G->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, G->bitmap.buffer);
        
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

static void LoadTilemapBuffer(render_state* RenderState, tilemap_render_info& TilemapRenderInfo, const tilemap& Tilemap)
{
    GLfloat* VertexBuffer = (GLfloat*)malloc(sizeof(GLfloat) * 16 * Tilemap.Width * Tilemap.Height);
    
    int32 Current = 0;
    
    real32 Width = (real32)Tilemap.RenderEntity.Texture->Width;
    real32 Height = (real32)Tilemap.RenderEntity.Texture->Height;
    
    real32 SheetWidth = (real32)Tilemap.TileSize / Width;
    real32 SheetHeight = (real32)Tilemap.TileSize / Height;
    
    for(uint32 X = 0; X < Tilemap.Width; X++)
    {
        for(uint32 Y = 0; Y < Tilemap.Height; Y++)
        {
            tile_data* Tile = &Tilemap.Data[X][Y];
            if(Tile->Type != Tile_None)
            {
                real32 CorrectY = (real32)Tilemap.Height - Y;
                VertexBuffer[Current++] = (GLfloat)X;
                VertexBuffer[Current++] = (GLfloat)CorrectY + 1.0f;
                VertexBuffer[Current++] = (GLfloat)1.0f / Width * Tile->TextureOffset.x + 0.0625f * SheetWidth;
                VertexBuffer[Current++] =  (GLfloat)1.0f / Height * Tile->TextureOffset.y + 0.9375f * SheetHeight;
                VertexBuffer[Current++] = (GLfloat)X + 1;
                VertexBuffer[Current++] = (GLfloat)CorrectY + 1;
                VertexBuffer[Current++] = (GLfloat)1.0f / Width * Tile->TextureOffset.x + 0.9375f * SheetWidth;
                VertexBuffer[Current++] =  (GLfloat)1.0f / Height * Tile->TextureOffset.y + 0.9375f * SheetHeight;
                VertexBuffer[Current++] = (GLfloat)X + 1;
                VertexBuffer[Current++] = (GLfloat)CorrectY;
                VertexBuffer[Current++] = (GLfloat)1.0f / Width * Tile->TextureOffset.x + 0.9375f * SheetWidth;
                VertexBuffer[Current++] = (GLfloat)1.0f / Height * Tile->TextureOffset.y + 0.0625f * SheetHeight;
                VertexBuffer[Current++] = (GLfloat)X;
                VertexBuffer[Current++] = (GLfloat)CorrectY;
                VertexBuffer[Current++] =(GLfloat)1.0f / Width *Tile->TextureOffset.x + 0.0625f * SheetWidth;
                VertexBuffer[Current++] = (GLfloat)1.0f / Height * Tile->TextureOffset.y + 0.0625f * SheetHeight;
            }
        }
    }
    
    TilemapRenderInfo.VBOSize = Current;
    
    if(TilemapRenderInfo.VBO == 0)
        glGenBuffers(1, &TilemapRenderInfo.VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, TilemapRenderInfo.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * TilemapRenderInfo.VBOSize, VertexBuffer, GL_DYNAMIC_DRAW);
    
    if(RenderState->TileShader.Type != Shader_Tile)
    {
        RenderState->TileShader.Type = Shader_Tile;
        LoadShader(ShaderPaths[Shader_Tile], &RenderState->TileShader);
    }
    
    auto PositionLocation = glGetAttribLocation(RenderState->TileShader.Program, "pos");
    auto TexcoordLocation = glGetAttribLocation(RenderState->TileShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    TilemapRenderInfo.Dirty = false;
    free(VertexBuffer);
}

static void CreateTilemapVAO(render_state* RenderState, const tilemap& Tilemap, tilemap_render_info* TilemapRenderInfo)
{
    //tile
    glGenVertexArrays(1, &TilemapRenderInfo->VAO);
    glBindVertexArray(TilemapRenderInfo->VAO);
    
    TilemapRenderInfo->VBO = 0;
    LoadTilemapBuffer(RenderState, *TilemapRenderInfo, Tilemap);
    
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
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    
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
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    
    RenderState->TextureShader.Type = Shader_SpriteSheetShader;
    LoadShader(ShaderPaths[Shader_SpriteSheetShader], &RenderState->SpriteSheetShader);
    
    PositionLocation = glGetAttribLocation(RenderState->SpriteSheetShader.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->SpriteSheetShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //error sprite
    glGenVertexArrays(1, &RenderState->SpriteErrorVAO);
    glBindVertexArray(RenderState->SpriteErrorVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    
    RenderState->ErrorShaderSprite.Type = Shader_ErrorSprite;
    LoadShader(ShaderPaths[Shader_ErrorSprite], &RenderState->ErrorShaderSprite);
    
    PositionLocation = glGetAttribLocation(RenderState->ErrorShaderSprite.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->ErrorShaderSprite.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //ui sprite
    glGenVertexArrays(1, &RenderState->UISpriteVAO);
    glBindVertexArray(RenderState->UISpriteVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    
    RenderState->UISpriteShader.Type = Shader_UISprite;
    LoadShader(ShaderPaths[Shader_UISprite], &RenderState->UISpriteShader);
    
    PositionLocation = glGetAttribLocation(RenderState->UISpriteShader.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->UISpriteShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //ui error shader
    glGenVertexArrays(1, &RenderState->UIErrorVAO);
    glBindVertexArray(RenderState->UIErrorVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    //error shader
    RenderState->ErrorShaderUI.Type = Shader_ErrorUI;
    LoadShader(ShaderPaths[Shader_ErrorUI], &RenderState->ErrorShaderUI);
    
    PositionLocation = glGetAttribLocation(RenderState->ErrorShaderUI.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->ErrorShaderUI.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, RenderState->TileQuadVerticesSize, RenderState->TileQuadVertices, GL_STATIC_DRAW);
    
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
    glGenVertexArrays(1, &RenderState->RectVAO);
    glBindVertexArray(RenderState->RectVAO);
    glGenBuffers(1, &RenderState->NormalQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->NormalQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->NormalQuadVerticesSize, RenderState->NormalQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->RectShader.Type = Shader_Rect;
    LoadShader(ShaderPaths[Shader_Rect], &RenderState->RectShader);
    
    auto PositionLocation3 = glGetAttribLocation(RenderState->RectShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &RenderState->TextureRectVAO);
    glBindVertexArray(RenderState->TextureRectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    
    RenderState->TextureRectShader.Type = Shader_TextureRect;
    LoadShader(ShaderPaths[Shader_TextureRect], &RenderState->TextureRectShader);
    
    PositionLocation2 = glGetAttribLocation(RenderState->TextureRectShader.Program, "pos");
    TexcoordLocation2 = glGetAttribLocation(RenderState->TextureRectShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation2);
    glVertexAttribPointer(PositionLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation2);
    glVertexAttribPointer(TexcoordLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glBindVertexArray(0);
    
    //wireframe
    glGenVertexArrays(1, &RenderState->WireframeVAO);
    glBindVertexArray(RenderState->WireframeVAO);
    glGenBuffers(1, &RenderState->WireframeQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->WireframeQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->WireframeQuadVerticesSize, RenderState->WireframeQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->RectShader.Type = Shader_Wireframe;
    LoadShader(ShaderPaths[Shader_Wireframe], &RenderState->WireframeShader);
    
    PositionLocation3 = glGetAttribLocation(RenderState->WireframeShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    //astar tile
    glGenVertexArrays(1, &RenderState->AStarPathVAO);
    glBindVertexArray(RenderState->AStarPathVAO);
    glGenBuffers(1, &RenderState->AStarPathQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->AStarPathQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->AStarPathQuadVerticesSize,
                 RenderState->AStarPathQuadVertices,GL_DYNAMIC_DRAW);
    
    LoadShader(ShaderPaths[Shader_AStarPath], &RenderState->AStarPathShader);
    
    PositionLocation3 = glGetAttribLocation(RenderState->AStarPathShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    //font
    RenderState->StandardFontShader.Type = Shader_StandardFont;
    LoadShader(ShaderPaths[Shader_StandardFont], &RenderState->StandardFontShader);
    
    RenderState->InconsolataFont = {};
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 18, RenderState->FTLibrary, &RenderState->InconsolataFont, &RenderState->StandardFontShader);
    
    RenderState->MenuFont = {};
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 40, RenderState->FTLibrary, &RenderState->MenuFont, &RenderState->StandardFontShader);
}

static GLuint LoadTexture(const char *FilePath, texture* Texture)
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
    
    unsigned char* Image = stbi_load(FilePath, &Texture->Width, &Texture->Height, 0, STBI_rgb_alpha);
    
    if (!Image)
        return GL_FALSE;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture->Width, Texture->Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, Image);
    Texture->TextureHandle = TextureHandle;
    stbi_image_free(Image);
    return GL_TRUE;
}

static void LoadTextures(render_state* RenderState)
{
    for(uint32 TextureIndex = 0; TextureIndex < Texture_Count; TextureIndex++)
    {
        texture Texture = {};
        LoadTexture(TexturePaths[TextureIndex],&Texture);
        RenderState->Textures[TextureIndex] = Texture;
    }
}

static void InitializeOpenGL(game_state* GameState, render_state* RenderState, config_data* ConfigData)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    RenderState->Window = glfwCreateWindow(ConfigData->ScreenWidth, ConfigData->ScreenHeight, Concat(Concat(ConfigData->Title, " "), ConfigData->Version), ConfigData->Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    
    if (!RenderState->Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    //center window on screen
    const GLFWvidmode *Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int Width, Height;
    
    glfwGetFramebufferSize(RenderState->Window, &Width, &Height);
    glfwSetWindowPos(RenderState->Window, Mode->width / 2 - Width / 2, Mode->height / 2 - Height / 2);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(RenderState->Window, FramebufferSizeCallback);
    
    glfwMakeContextCurrent(RenderState->Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);
    
    glfwGetFramebufferSize(RenderState->Window, &RenderState->WindowWidth, &RenderState->WindowHeight);
    glViewport(0, 0, RenderState->WindowWidth, RenderState->WindowHeight);
    glDisable(GL_DITHER);
    printf("%s\n", glGetString(GL_VERSION));
    
    glfwSetWindowUserPointer(RenderState->Window, GameState);
    glfwSetKeyCallback(RenderState->Window, KeyCallback);
    glfwSetCharCallback(RenderState->Window, CharacterCallback);
    glfwSetCursorPosCallback(RenderState->Window, CursorPositionCallback);
    glfwSetMouseButtonCallback(RenderState->Window, MouseButtonCallback);
    glfwSetScrollCallback(RenderState->Window, ScrollCallback);
    //glfwSetInputMode(RenderState->Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    GLint Viewport[4];
    glGetIntegerv(GL_VIEWPORT, Viewport);
    memcpy(RenderState->Viewport, Viewport, sizeof(GLint) * 4);
    
    int Present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if(Present)
    {
        GameState->InputController.ControllerPresent = true;
        const char* Name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        
        if(strstr(Name, "Xbox") != 0)
        {
            GameState->InputController.ControllerType = Controller_Xbox;
        }
        else if(strstr(Name, "PS4") != 0 || strstr(Name, "Wireless") != 0)
        {
            GameState->InputController.ControllerType = Controller_PS4;
        }
        
        printf("type %d\n", GameState->InputController.ControllerType);
    }
    LoadTextures(RenderState);
    RenderSetup(RenderState);
    
    GameState->HealthBar = {};
    GameState->HealthBar.Position = glm::vec2(RenderState->WindowWidth / 2, RenderState->WindowHeight - 50);
    GameState->HealthBar.RenderInfo.Size = glm::vec3(2, 1, 1);
    GameState->HealthBar.RenderInfo.Texture = &RenderState->Textures[Texture_HealthFull];
    
    // @Incomplete: These values need to be updated when the window size is changed
    GameState->EditorState.ToolbarX = (real32)RenderState->WindowWidth - 100;
    GameState->EditorState.ToolbarY = 0.0f;
    GameState->EditorState.ToolbarWidth = 100.0f;
    GameState->EditorState.ToolbarHeight = (real32)RenderState->WindowHeight;
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
    /*
    if(AssetManager->DirtyTileset == 1)
    {
    GameState->CurrentLevel.Tilemap.RenderEntity.Texture = LoadTexture("../assets/textures/tiles.png");
    AssetManager->DirtyTileset = 0;
    }*/
}

static void SetFloatAttribute(GLuint ShaderHandle, const char* UniformName, real32 Value)
{
    glUniform1f(glGetUniformLocation(ShaderHandle, UniformName), Value);
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

//TODO(Daniel) there's a weird bug when rendering special characters. The cursor just slowly jumps up for every character pressed
static void RenderRect(Render_Mode Mode, game_state* GameState, glm::vec4 Color, real32 X, real32 Y, real32 Width, real32 Height, uint32 TextureHandle = 0)
{
    render_state* RenderState = &GameState->RenderState;
    
    X *= RenderState->ScaleX;
    X -= 1;
    Y *= RenderState->ScaleY;
    Y -= 1;
    Width *= RenderState->ScaleX;
    Height *= RenderState->ScaleY;
    
    switch(Mode)
    {
        case Render_Fill:
        {
            auto Shader = RenderState->RectShader;
            if(TextureHandle)
            {
                glBindVertexArray(RenderState->TextureRectVAO);
                glBindTexture(GL_TEXTURE_2D, TextureHandle);
                Shader = RenderState->TextureRectShader;
            }
            else
            {
                glBindVertexArray(RenderState->RectVAO);
            }
            
            UseShader(&Shader);
            
            //draw upper part
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(X, Y, 0));
            Model = glm::scale(Model, glm::vec3(Width, Height, 1));
            
            SetMat4Uniform(Shader.Program, "M", Model);
            SetVec4Attribute(Shader.Program, "color", Color);
            
            glDrawArrays(GL_QUADS, 0, 4);
        }
        break;
        case Render_Outline:
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(X, Y, 0));
            Model = glm::scale(Model, glm::vec3(Width, Height, 1));
            
            glBindVertexArray(RenderState->WireframeVAO);
            
            auto Shader = RenderState->Shaders[Shader_Wireframe];
            UseShader(&Shader);
            
            SetMat4Uniform(Shader.Program, "Model", Model);
            SetMat4Uniform(Shader.Program, "Projection", GameState->Camera.ProjectionMatrix);
            SetMat4Uniform(Shader.Program, "View", GameState->Camera.ViewMatrix);
            SetVec4Attribute(Shader.Program, "color", Color);
            
            glDrawArrays(GL_LINE_STRIP, 0, 6);
            glBindVertexArray(0);
        }
        break;
    }
}

static void MeasureText(const render_font* Font, const char* Text, float* Width, float* Height)
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
            C = Text[Count];
            
            if (*Height < Font->CharacterInfo[C].AY) 
            {
                *Height = Font->CharacterInfo[C].AY;
            }
        }
    }
}

//rendering methods
static void RenderText(render_state* RenderState, const render_font& Font, const glm::vec4& Color, const char* Text, real32 X, real32 Y, real32 Scale, Alignment Alignment = Alignment_Left) 
{
    X *= RenderState->ScaleX;
    X -= 1;
    Y *= RenderState->ScaleY;
    Y -= 1;
    
    glBindVertexArray(Font.VAO);
    auto Shader = RenderState->Shaders[Shader_StandardFont];
    UseShader(&Shader);
    SetVec4Attribute(Shader.Program, "color", Color);
    SetVec4Attribute(Shader.Program, "alphaColor", Font.AlphaColor);
    
    if (RenderState->BoundTexture != Font.Texture) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Font.Texture);
        RenderState->BoundTexture = Font.Texture;
    }
    
    point* Coords = (point*)malloc(sizeof(point) * 6 * strlen(Text));
    
    int N = 0;
    
    switch(Alignment)
    {
        case Alignment_Left:
        break;
        case Alignment_Right:
        break;
        case Alignment_Center:
        {
            real32 Width;
            real32 Height;
            MeasureText(&Font, Text, &Width, &Height);
            X -= RenderState->ScaleX * Width / 2;
        }
        break;
    }
    
    for(const char *P = Text; *P; P++) 
    { 
        real32 W = Font.CharacterInfo[*P].BW * RenderState->ScaleX;
        real32 H = Font.CharacterInfo[*P].BH * RenderState->ScaleY;
        
        real32 X2 =  X + Font.CharacterInfo[*P ].BL * RenderState->ScaleX;
        real32 Y2 = -Y - Font.CharacterInfo[*P ].BT * RenderState->ScaleY;
        
        /* Advance the cursor to the start of the next character */
        X += Font.CharacterInfo[*P].AX * RenderState->ScaleX;
        Y += Font.CharacterInfo[*P].AY * RenderState->ScaleY;
        
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
    
    glBindBuffer(GL_ARRAY_BUFFER, Font.VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * strlen(Text) * sizeof(point), Coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, N);
    free(Coords);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void RenderConsole(game_state* GameState, console* Console)
{
    render_state* RenderState = &GameState->RenderState;
    
    glBindVertexArray(RenderState->RectVAO);
    
    real32 PercentAnimated = 1.0f + 1.0f - (real32)Console->CurrentTime / (real32)Console->TimeToAnimate;
    
    //draw upper part
    RenderRect(Render_Fill, GameState, glm::vec4(0.0f, 0.4f, 0.3f, 0.6f), 0.0f, (real32)RenderState->WindowHeight * 0.77f * PercentAnimated, (real32)RenderState->WindowWidth, (real32)RenderState->WindowHeight * 0.23f);
    
    //draw lower bar
    RenderRect(Render_Fill, GameState, glm::vec4(0.0f, 0.2f, 0.2f, 0.6f), 0.0f, (real32)RenderState->WindowHeight * 0.77f * PercentAnimated, (real32)RenderState->WindowWidth, 20);  
    
    GLfloat TimeValue = (real32)glfwGetTime();
    GLfloat AlphaValue = (real32)((sin(TimeValue * 4) / 2) + 0.5f);
    real32 Width;
    real32 Height;
    MeasureText(&RenderState->InconsolataFont, &Console->Buffer[0], &Width, &Height);
    
    //draw cursor
    RenderRect(Render_Fill, GameState, glm::vec4(AlphaValue, 1, AlphaValue, 1), 5 / 1920.0f * (real32)RenderState->WindowWidth + Width, RenderState->WindowHeight * 0.77f * PercentAnimated, 10, 20);
    
    RenderText(RenderState, RenderState->InconsolataFont, glm::vec4(0, 0.8, 0, 1),  &Console->Buffer[0], 5 / 1920.0f * (real32)RenderState->WindowWidth, (real32)RenderState->WindowHeight * 0.775f * PercentAnimated, 1);
    
    int index = 0;
    
    glm::vec4 Color;
    
    for(int Index = 0; Index < HISTORY_BUFFER_LINES; Index++)
    {
        if(Index % 2 != 0)
            Color = glm::vec4(0, 1, 0, 1);
        else
            Color = glm::vec4(1, 1, 1, 1);
        
        RenderText(RenderState, RenderState->InconsolataFont, Color, &Console->HistoryBuffer[Index][0], 5 / 1920.0f * (real32)RenderState->WindowWidth, (real32)RenderState->WindowHeight * 0.78f * PercentAnimated + (Index + 1) * 20 * PercentAnimated, 1);
    }
}

static void RenderColliderWireframe(render_state* RenderState, entity* Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    if(Entity->Active)
    {
        glm::mat4 Model(1.0f);
        
        Model = glm::translate(Model, glm::vec3(Entity->CollisionAABB.Center.x - Entity->CollisionAABB.Extents.x, Entity->CollisionAABB.Center.y - Entity->CollisionAABB.Extents.y, 0.0f));
        Model = glm::scale(Model, glm::vec3(Entity->CollisionAABB.Extents.x * 2, Entity->CollisionAABB.Extents.y * 2,1));
        
        glBindVertexArray(RenderState->WireframeVAO);
        
        auto Shader = RenderState->Shaders[Shader_Wireframe];
        UseShader(&Shader);
        
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", View);
        SetMat4Uniform(Shader.Program, "Model", Model);
        glm::vec4 color;
        
        if(Entity->IsColliding)
        {
            color = glm::vec4(1.0,0.0,0.0,1.0);
        }
        else 
        {
            color = glm::vec4(0.0,1.0,0.0,1.0);
        }
        
        SetVec4Attribute(Shader.Program, "color", color);
        
        glDrawArrays(GL_LINE_STRIP, 0, 6);
        glBindVertexArray(0);
        
        if(Entity->HitTrigger)
        {
            glm::mat4 Model(1.0f);
            
            Model = glm::translate(Model, glm::vec3(Entity->Position.x + Entity->Center.x * Entity->Scale.x - Entity->HitTrigger->Extents.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y - Entity->HitTrigger->Extents.y, 0.0f));
            Model = glm::scale(Model, glm::vec3(Entity->HitTrigger->Extents.x * 2, Entity->HitTrigger->Extents.y * 2,1));
            
            glBindVertexArray(RenderState->WireframeVAO);
            
            auto Shader = RenderState->Shaders[Shader_Wireframe];
            UseShader(&Shader);
            
            SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
            SetMat4Uniform(Shader.Program, "View", View);
            SetMat4Uniform(Shader.Program, "Model", Model);
            glm::vec4 color;
            
            if(Entity->HitTrigger->IsColliding)
            {
                color = glm::vec4(1.0,0.0,0.0,1.0);
            }
            else 
            {
                color = glm::vec4(0.0,1.0,0.0,1.0);
            }
            
            SetVec4Attribute(Shader.Program, "color", color);
            
            glDrawArrays(GL_LINE_STRIP, 0, 6);
            glBindVertexArray(0);
        }
    }
}


static void RenderWireframe(render_state* RenderState, entity* Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    if(Entity->Active)
    {
        glm::mat4 Model(1.0f);
        
        Model = glm::translate(Model, glm::vec3(Entity->Position.x, Entity->Position.y, 0.0f));
        Model = glm::scale(Model, glm::vec3(Entity->Scale.x, -Entity->Scale.y, 1));
        
        glBindVertexArray(RenderState->WireframeVAO);
        
        auto Shader = RenderState->Shaders[Shader_Wireframe];
        UseShader(&Shader);
        
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", View);
        SetMat4Uniform(Shader.Program, "Model", Model);
        glm::vec4 color(0.0,1.0,0.0,1.0);
        
        SetVec4Attribute(Shader.Program, "color", color);
        
        glDrawArrays(GL_LINE_STRIP, 0, 6);
        glBindVertexArray(0);
    }
}

static void RenderAStarPath(render_state* RenderState, entity* Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    if(Entity->Type == Entity_Enemy && Entity->Enemy.AStarPath) 
    {
        glBindVertexArray(RenderState->AStarPathVAO);
        for(uint32 PathIndex = 0; PathIndex < Entity->Enemy.AStarPathLength; PathIndex++)
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(Entity->Enemy.AStarPath[PathIndex].x, Entity->Enemy.AStarPath[PathIndex].y, 0.0f));
            Model = glm::scale(Model, glm::vec3(1, 1, 1));
            
            auto Shader = RenderState->Shaders[Shader_AStarPath];
            UseShader(&Shader);
            
            SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
            SetMat4Uniform(Shader.Program, "View", View);
            SetMat4Uniform(Shader.Program, "Model", Model);
            glm::vec4 color;
            
            if(Entity->Enemy.PathIndex == PathIndex)
            {
                color = glm::vec4(0.0,1.0,0.0,0.4);
            }
            else 
            {
                color = glm::vec4(0.0, 0.0, 1.0, 0.4);
            }
            
            SetVec4Attribute(Shader.Program, "color", color);
            glDrawArrays(GL_QUADS, 0, 6);
            
        }
        
        glBindVertexArray(0);
    }
}

static void RenderUISprite(render_state* RenderState, uint32 TextureHandle, glm::vec2 ScreenPosition, glm::vec3 Scale)
{
    real32 X = ScreenPosition.x * RenderState->ScaleX;
    X -= 1;
    real32 Y = ScreenPosition.y * RenderState->ScaleY;
    Y -= 1;
    
    auto Shader = RenderState->UISpriteShader;
    
    if(Shader.Program == 0)
    {
        Shader = RenderState->ErrorShaderUI;
        glBindVertexArray(RenderState->UIErrorVAO);
    }
    else
    {
        glBindVertexArray(RenderState->UISpriteVAO);
        glBindTexture(GL_TEXTURE_2D, TextureHandle);
    }
    
    UseShader(&Shader);
    
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(X, Y, 0));
    Model = glm::scale(Model, Scale);
    
    SetMat4Uniform(Shader.Program, "M", Model);
    SetVec4Attribute(Shader.Program, "color", glm::vec4(1, 1, 1, 1)); 
    
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

static void RenderEntity(render_state *RenderState, entity &Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{ 
    render_entity* RenderEntity = &RenderState->RenderEntities[Entity.RenderEntityHandle];
    auto Shader = RenderState->Shaders[RenderEntity->ShaderIndex];
    
    if(RenderEntity->Rendered && Entity.Active)
    {
        glm::mat4 Model(1.0f);
        Model = glm::translate(Model, glm::vec3(Entity.Position.x, Entity.Position.y, 0.0f));
        Model = glm::translate(Model, glm::vec3(1, 1, 0.0f));
        Model = glm::rotate(Model, Entity.Rotation.z, glm::vec3(0, 0, 1)); 
        Model = glm::translate(Model, glm::vec3(-1, -1, 0.0f));
        
        glm::vec3 Scale;
        
        if(Entity.IsFlipped)
        {
            Scale = glm::vec3(-Entity.Scale.x, Entity.Scale.y, Entity.Scale.z);
            Model = glm::translate(Model, glm::vec3(Entity.Scale.x, 0, 0));
        }
        else
            Scale = Entity.Scale;
        
        Model = glm::scale(Model, glm::vec3(Scale.x, -Scale.y, Scale.z));
        
        if(Entity.Type == Entity_Enemy)
        {
            if(Entity.Enemy.AIState == AI_Hit)
                RenderEntity->Color = glm::vec4(1, 0, 0, 1);
            else if(Entity.Enemy.AIState == AI_Charging)
                RenderEntity->Color = glm::vec4(0, 0, 1, 1);
            else
                RenderEntity->Color = glm::vec4(0, 1, 0, 1); //@Cleanup: This is just placeholder before we get a real enemy sprite that is different from the player
        }
        
        if(Entity.CurrentAnimation) 
        {
            animation* Animation = Entity.CurrentAnimation;
            
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
            auto Frame = Animation->Frames[Entity.AnimationInfo.FrameIndex];
            SetVec2Attribute(Shader.Program,"textureOffset", glm::vec2(Frame.X,Frame.Y));
            SetVec4Attribute(Shader.Program, "color", RenderEntity->Color);
            SetVec2Attribute(Shader.Program,"sheetSize",
                             glm::vec2(Animation->Columns, Animation->Rows));
        } 
        else 
        {
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
        }
        
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", View);
        SetMat4Uniform(Shader.Program, "Model", Model);
        
        glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
        glDrawArrays(GL_QUADS, 0, 4);
        glBindVertexArray(0);
    }
    
    if(RenderState->RenderColliders && !Entity.IsKinematic)
        RenderColliderWireframe(RenderState, &Entity, ProjectionMatrix, View);
    
    if(RenderState->RenderPaths && Entity.Enemy.AStarPath)
        RenderAStarPath(RenderState,&Entity,ProjectionMatrix,View);
}

static void RenderRoom(render_state* RenderState, const room& Room, glm::mat4 ProjectionMatrix, glm::mat4 View, int StartX, int StartY, int EndX, int EndY)
{
    glBindVertexArray(RenderState->TileVAO);
    
    if (RenderState->BoundTexture != Room.RenderEntity.Texture->TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Room.RenderEntity.Texture->TextureHandle);
        RenderState->BoundTexture = Room.RenderEntity.Texture->TextureHandle;
    }
    
    auto Shader = RenderState->Shaders[Room.RenderEntity.ShaderIndex];
    UseShader(&Shader);
    
    real32 Scale = 1.0f;
    
    for (uint32 i = 0; i < Room.Width; i++)
    {
        for (uint32 j = 0; j < Room.Height; j++)
        {
            if(Room.Data[i][j].Type != Tile_None)
            {
                glm::mat4 Model(1.0f);
                Model = glm::translate(Model, glm::vec3(i * Scale, j * Scale, 0.0f));
                Model = glm::scale(Model, glm::vec3(Scale, Scale, 1.0f));
                
                SetVec2Attribute(Shader.Program, "textureOffset", Room.Data[i][j].TextureOffset);
                
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", View);
                SetMat4Uniform(Shader.Program, "Model", Model);
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
    }
    
    glBindVertexArray(0);
}

static void RenderTile(render_state* RenderState, uint32 X, uint32 Y, uint32 TextureHandle, glm::vec4 Color,  glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    glBindVertexArray(RenderState->SpriteVAO);
    auto Shader = RenderState->TextureShader;
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(X, Y, 0.0f));
    Model = glm::scale(Model, glm::vec3(1, 1, 1));
    
    glBindTexture(GL_TEXTURE_2D, TextureHandle);
    
    UseShader(&Shader);
    
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    SetVec4Attribute(Shader.Program, "Color", Color);
    
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

static void NEW_RenderTilemap(render_state* RenderState, const tilemap& Tilemap, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    glBindVertexArray(Tilemap.RenderInfo.VAO);
    
    if (RenderState->BoundTexture != Tilemap.RenderEntity.Texture->TextureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, Tilemap.RenderEntity.Texture->TextureHandle);
        RenderState->BoundTexture = Tilemap.RenderEntity.Texture->TextureHandle;
    }
    
    auto Shader = RenderState->TileShader;
    UseShader(&Shader);
    
    glm::mat4 Model(1.0f);
    Model = glm::scale(Model, glm::vec3(1, 1, 1.0f));
    
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    
    glDrawArrays(GL_QUADS, 0, Tilemap.RenderInfo.VBOSize / 4);
    glBindVertexArray(0);
}

static void RenderTilemap(render_state* RenderState, const tilemap& Tilemap, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    glBindVertexArray(RenderState->TileVAO);
    
    if (RenderState->BoundTexture != Tilemap.RenderEntity.Texture->TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Tilemap.RenderEntity.Texture->TextureHandle);
        RenderState->BoundTexture = Tilemap.RenderEntity.Texture->TextureHandle;
    }
    
    auto Shader = RenderState->Shaders[Tilemap.RenderEntity.ShaderIndex];
    UseShader(&Shader);
    
    real32 Scale = 1.0f;
    
    for (uint32 i = 0; i < Tilemap.Width; i++)
    {
        for (uint32 j = 0; j < Tilemap.Height; j++)
        {
            if(Tilemap.Data[i][j].Type != Tile_None)
            {
                glm::mat4 Model(1.0f);
                Model = glm::translate(Model, glm::vec3(i * Scale, (Tilemap.Height - j) * Scale, 0.0f));
                Model = glm::scale(Model, glm::vec3(Scale, Scale, 1.0f));
                
                SetVec2Attribute(Shader.Program, "textureOffset", Tilemap.Data[i][j].TextureOffset);
                SetVec2Attribute(Shader.Program, "sheetSize", glm::vec2(Tilemap.RenderEntity.Texture->Width, Tilemap.RenderEntity.Texture->Height));
                
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", View);
                SetMat4Uniform(Shader.Program, "Model", Model);
                
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
    }
    
    glBindVertexArray(0);
}

int CompareFunction(const void* a, const void* b)
{
    render_entity APtr = *(render_entity*)a;
    render_entity BPtr = *(render_entity*)b;
    
    collision_AABB BoxA = APtr.Entity->CollisionAABB;
    collision_AABB BoxB = BPtr.Entity->CollisionAABB;
    
    if(BoxA.Center.y - BoxA.Extents.y > BoxB.Center.y - BoxB.Extents.y)
        return -1;
    if(BoxA.Center.y  - BoxA.Extents.y < BoxB.Center.y - BoxB.Extents.y)
        return 1;
    return 0;
}

static void RenderInGameMode(game_state* GameState)
{
    NEW_RenderTilemap(&GameState->RenderState, GameState->CurrentLevel.Tilemap, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    
    //RenderTilemap(&GameState->RenderState, GameState->CurrentLevel.Tilemap, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    
    //@Fix this is buggy
    qsort(GameState->RenderState.RenderEntities, GameState->RenderState.RenderEntityCount, sizeof(render_entity), CompareFunction);
    
    for(uint32 Index = 0; Index < GameState->RenderState.RenderEntityCount; Index++) 
    {
        render_entity* Render = &GameState->RenderState.RenderEntities[Index];
        // @Incomplete: Only render if in view
        Render->Entity->RenderEntityHandle = Index;
        RenderEntity(&GameState->RenderState, *Render->Entity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    }
}

static void RenderGame(game_state* GameState)
{
    switch(GameState->GameMode)
    {
        case Mode_MainMenu:
        {
            glm::vec4 TextColor;
            
            for(uint32 Index = 0; Index < GameState->MainMenu.OptionCount; Index++)
            {
                if((int32)Index == GameState->MainMenu.SelectedIndex)
                {
                    RenderRect(Render_Fill, GameState, glm::vec4(1, 1, 1, 1), (real32)GameState->RenderState.WindowWidth / 2 - 200, (real32)GameState->RenderState.WindowHeight / 2 - 10 - 40 * Index, 400, 40);
                    TextColor = glm::vec4(0, 0, 0, 1);
                }
                else
                {
                    TextColor = glm::vec4(1, 1, 1, 1);
                }
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, TextColor, GameState->MainMenu.Options[Index], (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight / 2 - 40 * Index, 1, Alignment_Center);
                
            }
        }
        break;
        case Mode_InGame:
        {
            RenderInGameMode(GameState);
            if(GameState->Paused)
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.5, 1, 1, 1), "PAUSED", (real32)GameState->RenderState.WindowWidth / 2, 40, 1, Alignment_Center);
        }
        break;
        case Mode_Editor:
        {
            RenderInGameMode(GameState);
            
            RenderRect(Render_Outline, GameState, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0, 0, (real32)GameState->CurrentLevel.Tilemap.Width * 50, (real32)GameState->CurrentLevel.Tilemap.Height * 50);
            
            if(GameState->EditorState.SelectedEntity)
                RenderWireframe(&GameState->RenderState, GameState->EditorState.SelectedEntity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            
            uint32 SelectedTextureHandle = 0;
            
            switch(GameState->EditorState.SelectedTileType)
            {
                case Tile_None:
                SelectedTextureHandle = GameState->RenderState.EmptyTileTexture.TextureHandle;
                break;
                case Tile_Sand:
                SelectedTextureHandle = GameState->RenderState.SandTileTexture.TextureHandle;
                break;
                case Tile_Grass:
                SelectedTextureHandle = GameState->RenderState.GrassTileTexture.TextureHandle;
                break;
                case Tile_Stone:
                SelectedTextureHandle = GameState->RenderState.StoneTileTexture.TextureHandle;
                break;
            }
            
            if(GameState->EditorState.TileX >= 0 && GameState->EditorState.TileX < GameState->CurrentLevel.Tilemap.Width && GameState->EditorState.TileY > 0 && GameState->EditorState.TileY <= GameState->CurrentLevel.Tilemap.Height)
            {
                RenderTile(&GameState->RenderState, (uint32)GameState->EditorState.TileX, (uint32)GameState->EditorState.TileY, SelectedTextureHandle, glm::vec4(1, 1, 1, 1),  GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            }
            
            RenderRect(Render_Fill, GameState, glm::vec4(0, 0, 0, 1), GameState->EditorState.ToolbarX, GameState->EditorState.ToolbarY, GameState->EditorState.ToolbarWidth, GameState->EditorState.ToolbarHeight);
            
            for(uint32 TileIndex = 0; TileIndex < Tile_Count; TileIndex++)
            {
                uint32 TextureHandle = 0;
                
                switch(TileIndex)
                {
                    case Tile_None:
                    TextureHandle = GameState->RenderState.EmptyTileTexture.TextureHandle;
                    break;
                    case Tile_Sand:
                    TextureHandle = GameState->RenderState.SandTileTexture.TextureHandle;
                    break;
                    case Tile_Grass:
                    TextureHandle = GameState->RenderState.GrassTileTexture.TextureHandle;
                    break;
                    //case Tile_DarkGrass: //@Incomplete
                    //break;
                    case Tile_Stone:
                    TextureHandle = GameState->RenderState.StoneTileTexture.TextureHandle;
                    break;
                }
                
                RenderRect(Render_Fill, GameState, glm::vec4(1, 1, 1, 1), (real32)GameState->RenderState.WindowWidth - 80,
                           (real32)GameState->RenderState.WindowHeight - (TileIndex + 1) * 65, 60, 60, TextureHandle);
                if(TileIndex == (uint32)GameState->EditorState.SelectedTileType)
                    RenderRect(Render_Fill, GameState, glm::vec4(1, 0, 0, 1), (real32)GameState->RenderState.WindowWidth - 80,
                               (real32)GameState->RenderState.WindowHeight - (TileIndex + 1) * 65, 60, 60, GameState->RenderState.SelectedTileTexture.TextureHandle);
            }
            
            RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(1, 1, 1, 1), "Editor", (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight - 100, 1, Alignment_Center);
            
            if(GameState->EditorState.PlacementMode == Editor_Placement_Tile)
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.6f, 0.6f, 0.6f, 1), "Tile-mode", (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight - 150, 1, Alignment_Center);
            else
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.6f, 0.6f, 0.6f, 1), "Entity-mode", (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight - 150, 1, Alignment_Center);
        }
        break;
    }
}

static void RenderEditorUI(game_state* GameState, const editor_ui& EditorUI, render_state* RenderState)
{
    switch(GameState->EditorUI.State)
    {
        case State_Selection:
        {
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            
            RenderRect(Render_Outline, GameState, glm::vec4(0.2f, 0.2f, 0.2f, 1), -0.2f, -0.3f, 0.4f, 0.6f);
            
            for(int Index = 0; Index < MENU_OPTIONS_COUNT; Index++)
            {
                RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(1, 1, 1, 1), GameState->EditorUI.MenuOptions[Index], -0.21f, 0 + (MENU_OPTIONS_COUNT - (Index)) * 0.06f, 1);
            }
        }
        break;
        case State_Animations:
        {
        }
        break;
        case State_Collision:
        {
        }
        break;
        case State_EntityList:
        {
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            RenderGame(GameState);
            RenderRect(Render_Fill, 
                       GameState, 
                       glm::vec4(0, 0.4f, 0.3f, 0.6f), 
                       1.0f, 
                       -2.0f + (real32)RenderState->WindowHeight / 2.0f - 20.0f * (1.0f + GameState->EntityCount) / 2.0f, 
                       120.0f, 
                       20.0f * (1.0f + GameState->EntityCount));
            
            for(int i = -1; i < GameState->EntityCount; i++)
            {
                char* EntityName = i == -1 ? "Entities:" : GameState->Entities[i].Name;
                
                if(!EntityName)
                    EntityName = "NO_NAME";
                
                glm::vec4 Color;
                
                if(i == -1 || i == (int32)GameState->EditorUI.SelectedIndex)
                {
                    if(i != -1)
                    {
                        Color = glm::vec4(0, 0, 0, 1);
                        RenderRect(Render_Fill, 
                                   GameState, 
                                   glm::vec4(1, 1, 1, 1), 
                                   1.0f,
                                   -2.0f + (real32)RenderState->WindowHeight / 2.0f - 20.0f * i, 
                                   120.0f, 
                                   20.0f); 
                    }
                    else
                        Color = glm::vec4(1, 1, 1, 1);
                }
                else
                {
                    Color = glm::vec4(1, 1, 1, 1);
                }
                
                RenderText(RenderState, RenderState->InconsolataFont, Color, EntityName, 2.0f, (real32)RenderState->WindowHeight / 2.0f - 20.0f * i, 1.0f);
            }
        }
        break;
    }
}

static void RenderPlayerUI(health_bar* HealthBar, render_state* RenderState)
{
    RenderUISprite(RenderState, HealthBar->RenderInfo.Texture->TextureHandle, HealthBar->Position, glm::vec3(0.1, 0.1f, 1));
}

static void CheckLevelVAO(game_state* GameState)
{
    if(GameState->CurrentLevel.Tilemap.RenderInfo.VAO == 0)
    {
        CreateTilemapVAO(&GameState->RenderState, GameState->CurrentLevel.Tilemap, &GameState->CurrentLevel.Tilemap.RenderInfo);
    }
}

static void Render(game_state* GameState)
{
    if(GameState->CurrentLevel.Tilemap.RenderInfo.Dirty)
    {
        LoadTilemapBuffer(&GameState->RenderState, GameState->CurrentLevel.Tilemap.RenderInfo, GameState->CurrentLevel.Tilemap); 
    }
    
    GameState->RenderState.ScaleX = 2.0f / GameState->RenderState.WindowWidth;
    GameState->RenderState.ScaleY = 2.0f / GameState->RenderState.WindowHeight;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1.0f);
    
    if(GameState->EditorUI.On)
    {
        RenderEditorUI(GameState, GameState->EditorUI, &GameState->RenderState);
    }
    else
    {
        //glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        RenderGame(GameState);
    }
    
    RenderPlayerUI(&GameState->HealthBar, &GameState->RenderState);
    
    if(GameState->Console.CurrentTime > 0)
        RenderConsole(GameState, &GameState->Console);
    
    
    if(GameState->RenderState.RenderFPS)
    {
        char FPS[32];
        sprintf(FPS, "%4.0f",GameState->RenderState.FPS);
        RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(1, 1, 1, 1), FPS, GameState->RenderState.WindowWidth / 2.0f, 20.0f, 1.0f, Alignment_Center);
    }
    
    glfwSwapBuffers(GameState->RenderState.Window);
}
