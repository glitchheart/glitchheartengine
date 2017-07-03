#include "opengl_rendering.h"

static void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    game_state *GameState = (game_state *)glfwGetWindowUserPointer(Window);
    
    glfwSetWindowAspectRatio(Window, 16, 9);
    glViewport(0, 0, Width, Height);
    GLint Viewport[4];
    glGetIntegerv(GL_VIEWPORT, Viewport);
    memcpy(GameState->RenderState.Viewport, Viewport, sizeof(GLint) * 4);
    
    GameState->EditorState.ToolbarX = (real32)GameState->RenderState.WindowWidth - 100;
    GameState->EditorState.ToolbarY = 0;
    GameState->EditorState.ToolbarWidth = 100.0f;
    GameState->EditorState.ToolbarHeight = (real32)GameState->RenderState.WindowHeight;
    
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
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
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
    
    for(uint32 X = 0; X < Tilemap.Width; X++)
    {
        for(uint32 Y = 0; Y < Tilemap.Height; Y++)
        {
            tile_data* Tile = &Tilemap.Data[X][Y];
            
            if(Tile->TypeIndex != 0)
            {
                real32 TexCoordX = (Tile->TextureOffset.x) / Width;
                real32 TexCoordY = (Tile->TextureOffset.y) / Height; 
                real32 TexCoordXHigh = (Tile->TextureOffset.x + Tilemap.TileSize) / Width;
                real32 TexCoordYHigh = (Tile->TextureOffset.y + Tilemap.TileSize) / Height; 
                
                real32 CorrectY = (real32)Y;
                VertexBuffer[Current++] = (GLfloat)X;
                VertexBuffer[Current++] = (GLfloat)CorrectY + 1.0f;
                VertexBuffer[Current++] = (GLfloat)TexCoordX;
                VertexBuffer[Current++] =  (GLfloat)TexCoordY;
                VertexBuffer[Current++] = (GLfloat)X + 1;
                VertexBuffer[Current++] = (GLfloat)CorrectY + 1;
                VertexBuffer[Current++] = (GLfloat)TexCoordXHigh;
                VertexBuffer[Current++] =  (GLfloat)TexCoordY;
                VertexBuffer[Current++] = (GLfloat)X + 1;
                VertexBuffer[Current++] = (GLfloat)CorrectY;
                VertexBuffer[Current++] = (GLfloat)TexCoordXHigh;
                VertexBuffer[Current++] = (GLfloat)TexCoordYHigh;
                VertexBuffer[Current++] = (GLfloat)X;
                VertexBuffer[Current++] = (GLfloat)CorrectY;
                VertexBuffer[Current++] =(GLfloat)TexCoordX;
                VertexBuffer[Current++] = (GLfloat)TexCoordYHigh;
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


static void LoadEditorTileBuffer(render_state* RenderState, editor_render_info& EditorRenderInfo, const tilemap& Tilemap)
{
    GLfloat* VertexBuffer = (GLfloat*)malloc(sizeof(GLfloat) * 16 * Tilemap.TileCount);
    
    int32 Current = 0;
    
    real32 Width = (real32)Tilemap.RenderEntity.Texture->Width;
    real32 Height = (real32)Tilemap.RenderEntity.Texture->Height;
    
    for(uint32 Index = 1; Index < Tilemap.TileCount; Index++)
    {
        tile_data* Tile = &Tilemap.Tiles[Index];
        
        real32 TexCoordX = (Tile->TextureOffset.x) / Width;
        real32 TexCoordY = (Tile->TextureOffset.y) / Height; 
        real32 TexCoordXHigh = (Tile->TextureOffset.x + Tilemap.TileSize) / Width;
        real32 TexCoordYHigh = (Tile->TextureOffset.y + Tilemap.TileSize) / Height; 
        
        real32 CorrectY = (real32)Index - 1;
        VertexBuffer[Current++] = (GLfloat)0;
        VertexBuffer[Current++] = (GLfloat)Index - 1 + 1.0f;
        VertexBuffer[Current++] = (GLfloat)TexCoordX;
        VertexBuffer[Current++] =  (GLfloat)TexCoordY;
        VertexBuffer[Current++] = (GLfloat)1;
        VertexBuffer[Current++] = (GLfloat)Index - 1 + 1;
        VertexBuffer[Current++] = (GLfloat)TexCoordXHigh;
        VertexBuffer[Current++] =  (GLfloat)TexCoordY;
        VertexBuffer[Current++] = (GLfloat)1;
        VertexBuffer[Current++] = (GLfloat)Index - 1;
        VertexBuffer[Current++] = (GLfloat)TexCoordXHigh;
        VertexBuffer[Current++] = (GLfloat)TexCoordYHigh;
        VertexBuffer[Current++] = (GLfloat)0;
        VertexBuffer[Current++] = (GLfloat)Index - 1;
        VertexBuffer[Current++] =(GLfloat)TexCoordX;
        VertexBuffer[Current++] = (GLfloat)TexCoordYHigh;
    }
    
    EditorRenderInfo.VBOSize = Current;
    
    if(EditorRenderInfo.VBO == 0)
        glGenBuffers(1, &EditorRenderInfo.VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, EditorRenderInfo.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * EditorRenderInfo.VBOSize, VertexBuffer, GL_STATIC_DRAW);
    
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
    EditorRenderInfo.Dirty = false;
    free(VertexBuffer);
}

static void CreateTilemapVAO(render_state* RenderState, const tilemap& Tilemap, editor_render_info* EditorRenderInfo, tilemap_render_info* TilemapRenderInfo)
{
    //tile
    glGenVertexArrays(1, &TilemapRenderInfo->VAO);
    glBindVertexArray(TilemapRenderInfo->VAO);
    
    TilemapRenderInfo->VBO = 0;
    LoadTilemapBuffer(RenderState, *TilemapRenderInfo, Tilemap);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &EditorRenderInfo->VAO);
    glBindVertexArray(EditorRenderInfo->VAO);
    
    EditorRenderInfo->VBO = 0;
    LoadEditorTileBuffer(RenderState, *EditorRenderInfo, Tilemap);
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
    
    RenderState->TextureShader.Type = Shader_Spritesheet;
    LoadShader(ShaderPaths[Shader_Spritesheet], &RenderState->SpritesheetShader);
    
    PositionLocation = glGetAttribLocation(RenderState->SpritesheetShader.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->SpritesheetShader.Program, "texcoord");
    
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
    
    RenderState->RobotoFont = {};
    InitializeFreeTypeFont("../assets/fonts/roboto/Roboto-Regular.ttf", 20, RenderState->FTLibrary, &RenderState->RobotoFont, &RenderState->StandardFontShader);
    
    RenderState->InconsolataFont = {};
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 18, RenderState->FTLibrary, &RenderState->InconsolataFont, &RenderState->StandardFontShader);
    
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 40, RenderState->FTLibrary, &RenderState->MenuFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/raleway/Raleway-Regular.ttf", 30, RenderState->FTLibrary, &RenderState->ButtonFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/rubber-biscuit/RUBBBB__.ttf", 50, RenderState->FTLibrary, &RenderState->TitleFont, &RenderState->StandardFontShader);
}

static GLuint LoadTexture(const char* FilePath, texture* Texture)
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
                 GL_UNSIGNED_BYTE, (GLvoid*) Image);
    
    Texture->TextureHandle = TextureHandle;
    stbi_image_free(Image);
    
    return GL_TRUE;
}

static void LoadTextures(render_state* RenderState, const char* Directory)
{
    directory_data DirData = {};
    FindFilesWithExtensions(Directory, "png", &DirData);
    
    for(int32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        texture* Texture = &RenderState->TextureArray[RenderState->TextureIndex++];
        Texture->Name = (char*)malloc((strlen(DirData.FileNames[FileIndex]) + 1) * sizeof(char));
        strcpy(Texture->Name, DirData.FileNames[FileIndex]);
        LoadTexture(DirData.FilePaths[FileIndex], Texture);
        RenderState->Textures.insert(std::pair<char*, texture*>(Texture->Name, Texture));
    }
    
    free(DirData.FilePaths);
    free(DirData.FileNames);
}

static void LoadTilesheetTextures(game_state* GameState, render_state* RenderState)
{
    char* TempNames[30];
    
    FILE* File;
    File = fopen("../assets/textures/tilesheets/.tilesheets", "r");
    char LineBuffer[255];
    
    if(File)
    {
        uint32 Index = 0;
        
        while(fgets(LineBuffer, 255, File))
        {
            TempNames[Index] = (char*)malloc(sizeof(char) * 20);
            sscanf(LineBuffer, "%s", TempNames[Index++]);
        }
        
        fclose(File);
        
        RenderState->TilesheetCount = Index;
        RenderState->Tilesheets = (tilesheet*)malloc(sizeof(tilesheet) * Index);
        
        for(uint32 I = 0; I < Index; I++)
        {
            RenderState->Tilesheets[I] = {};
            RenderState->Tilesheets[I].Name = (char*)malloc(sizeof(char) * 20);
            
            strcpy(RenderState->Tilesheets[I].Name, TempNames[I]);
            LoadTexture(Concat(Concat("../assets/textures/tilesheets/", TempNames[I]), ".png"), &RenderState->Tilesheets[I].Texture);
            free(TempNames[I]);
        }
        
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
    glfwSetInputMode(RenderState->Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
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
    }
    
    LoadTextures(RenderState, "../assets/textures/");
    LoadTextures(RenderState, "../assets/textures/spritesheets/");
    RenderSetup(RenderState);
    
    GameState->HealthBar = {};
    GameState->HealthBar.Position = glm::vec2(RenderState->WindowWidth / 2, RenderState->WindowHeight - 50);
    GameState->HealthBar.RenderInfo.Size = glm::vec3(2, 1, 1);
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

static void SetFloatUniform(GLuint ShaderHandle, const char* UniformName, real32 Value)
{
    glUniform1f(glGetUniformLocation(ShaderHandle, UniformName), Value);
}

static void SetVec2Uniform(GLuint ShaderHandle, const char *UniformName, glm::vec2 Value)
{
    glUniform2f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y);
}

void SetVec3Uniform(GLuint ShaderHandle, const char *UniformName, glm::vec3 Value)
{
    glUniform3f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z);
}

static void SetVec4Uniform(GLuint ShaderHandle, const char *UniformName, glm::vec4 Value)
{
    glUniform4f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z, Value.w);
}

static void SetMat4Uniform(GLuint ShaderHandle, const char *UniformName, glm::mat4 Value)
{
    glUniformMatrix4fv(glGetUniformLocation(ShaderHandle, UniformName), 1, GL_FALSE, &Value[0][0]);
}

//TODO(Daniel) there's a weird bug when rendering special characters. The cursor just slowly jumps up for every character pressed
static void RenderRect(Render_Mode Mode, render_state* RenderState, glm::vec4 Color, real32 X, real32 Y, real32 Width, real32 Height, uint32 TextureHandle = 0, bool32 IsUI = true, glm::mat4 ProjectionMatrix = glm::mat4(), glm::mat4 ViewMatrix = glm::mat4())
{
    if(IsUI)
    {
        X *= RenderState->ScaleX;
        X -= 1;
        Y *= RenderState->ScaleY;
        Y -= 1;
        
        Width *= RenderState->ScaleX;
        Height *= RenderState->ScaleY;
    }
    
    switch(Mode)
    {
        case Render_Fill:
        {
            auto Shader = RenderState->RectShader;
            
            if(TextureHandle && RenderState->BoundTexture != TextureHandle)
            {
                glBindVertexArray(RenderState->TextureRectVAO);
                glBindTexture(GL_TEXTURE_2D, TextureHandle);
                Shader = RenderState->TextureRectShader;
                RenderState->BoundTexture = TextureHandle;
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                RenderState->BoundTexture = 0;
                glBindVertexArray(RenderState->RectVAO);
            }
            
            UseShader(&Shader);
            
            //draw upper part
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(X, Y, 0));
            Model = glm::scale(Model, glm::vec3(Width, Height, 1));
            
            if(!IsUI)
            {
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", ViewMatrix);
            }
            
            SetFloatUniform(Shader.Program, "isUI", (real32)IsUI);
            SetMat4Uniform(Shader.Program, "M", Model);
            SetVec4Uniform(Shader.Program, "color", Color);
            
            glDrawArrays(GL_QUADS, 0, 4);
        }
        break;
        case Render_Outline:
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(X, Y, 0));
            Model = glm::scale(Model, glm::vec3(Width, Height, 1));
            
            glBindVertexArray(RenderState->WireframeVAO);
            
            auto Shader = RenderState->RectShader;
            UseShader(&Shader);
            
            if(!IsUI)
            {
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", ViewMatrix);
            }
            
            SetFloatUniform(Shader.Program, "isUI", (real32)IsUI);
            SetMat4Uniform(Shader.Program, "M", Model);
            SetVec4Uniform(Shader.Program, "color", Color);
            
            glDrawArrays(GL_LINE_STRIP, 0, 8);
        }
        break;
    }
    glBindVertexArray(0);
}

static void MeasureText(const render_font* Font, const char* Text, float* Width, float* Height)
{
    int Count;
    
    if (!Text) 
    {
        return;
    }
    
    *Width = 0.0f;
    *Height = 0.0f;
    
    for(Count = 0; Count < strlen(Text); ++Count) 
    {
        *Width += Font->CharacterInfo[Text[Count]].AX;
        
        if(*Height < Font->CharacterInfo[Text[Count]].BH) 
        {
            *Height = Font->CharacterInfo[Text[Count]].BH;
        }
    }
}

//rendering methods
static void RenderText(render_state* RenderState, const render_font& Font, const glm::vec4& Color, const char* Text, real32 X, real32 Y, real32 Scale, Alignment Alignment = Alignment_Left) 
{
    glBindVertexArray(Font.VAO);
    auto Shader = RenderState->Shaders[Shader_StandardFont];
    UseShader(&Shader);
    SetVec4Uniform(Shader.Program, "color", Color);
    SetVec4Uniform(Shader.Program, "alphaColor", Font.AlphaColor);
    
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
            real32 Width = 0;
            real32 Height = 0;
            MeasureText(&Font, Text, &Width, &Height);
            X -= Width / 2.0f;
            Y -= Height / 2.0f;
        }
        break;
    }
    
    X *= RenderState->ScaleX;
    X -= 1.0f;
    Y *= RenderState->ScaleY;
    Y -= 1.0f;
    
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
    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0.0f, 0.4f, 0.3f, 0.6f), 0.0f, (real32)RenderState->WindowHeight * 0.77f * PercentAnimated, (real32)RenderState->WindowWidth, (real32)RenderState->WindowHeight * 0.23f);
    
    //draw lower bar
    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0.0f, 0.2f, 0.2f, 0.6f), 0.0f, (real32)RenderState->WindowHeight * 0.77f * PercentAnimated, (real32)RenderState->WindowWidth, 20);  
    
    GLfloat TimeValue = (real32)glfwGetTime();
    GLfloat AlphaValue = (real32)((sin(TimeValue * 4) / 2) + 0.5f);
    real32 Width;
    real32 Height;
    MeasureText(&RenderState->InconsolataFont, &Console->Buffer[0], &Width, &Height);
    
    //draw cursor
    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(AlphaValue, 1, AlphaValue, 1), 5 / 1920.0f * (real32)RenderState->WindowWidth + Width, RenderState->WindowHeight * 0.77f * PercentAnimated, 10, 20);
    
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
        
        if(Entity->CollisionAABB.IsTrigger)
            color = glm::vec4(0, 0, 1, 1);
        
        SetVec4Uniform(Shader.Program, "color", color);
        
        glDrawArrays(GL_LINE_STRIP, 0, 6);
        glBindVertexArray(0);
        
        if(Entity->HasHitTrigger)
        {
            glm::mat4 Model(1.0f);
            
            Model = glm::translate(Model, glm::vec3(Entity->HitTrigger.Center.x - Entity->HitTrigger.Extents.x, Entity->HitTrigger.Center.y - Entity->HitTrigger.Extents.y, 0.0f)); Model = glm::scale(Model, glm::vec3(Entity->HitTrigger.Extents.x * 2, Entity->HitTrigger.Extents.y * 2,1));
            
            glBindVertexArray(RenderState->WireframeVAO);
            
            auto Shader = RenderState->Shaders[Shader_Wireframe];
            UseShader(&Shader);
            
            SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
            SetMat4Uniform(Shader.Program, "View", View);
            SetMat4Uniform(Shader.Program, "Model", Model);
            glm::vec4 color;
            
            if(Entity->HitTrigger.IsColliding)
            {
                color = glm::vec4(1.0,0.0,0.0,1.0);
            }
            else 
            {
                color = glm::vec4(0.0,1.0,0.0,1.0);
            }
            
            SetVec4Uniform(Shader.Program, "color", color);
            
            glDrawArrays(GL_LINE_STRIP, 0, 6);
            glBindVertexArray(0);
        }
        
        if(Entity->Type == Entity_Player)
        {
            RenderRect(Render_Outline, RenderState, glm::vec4(0, 0, 1, 1), Entity->Weapon.CollisionAABB.Center.x - Entity->Weapon.CollisionAABB.Extents.x, Entity->Weapon.CollisionAABB.Center.y - Entity->Weapon.CollisionAABB.Extents.y, Entity->Weapon.CollisionAABB.Extents.x * 2, Entity->Weapon.CollisionAABB.Extents.y * 2, 0, false, ProjectionMatrix, View);
            //RenderRect(Render_Outline,
        }
    }
}


static void RenderWireframe(render_state* RenderState, entity* Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    if(Entity->Active)
    {
        glm::mat4 Model(1.0f);
        
        Model = glm::translate(Model, glm::vec3(Entity->Position.x -  Entity->Scale / 2, Entity->Position.y, 0.0f));
        Model = glm::scale(Model, glm::vec3(Entity->Scale, Entity->Scale, 1));
        
        glBindVertexArray(RenderState->WireframeVAO);
        
        auto Shader = RenderState->Shaders[Shader_Wireframe];
        UseShader(&Shader);
        
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", View);
        SetMat4Uniform(Shader.Program, "Model", Model);
        glm::vec4 color(0.0,1.0,0.0,1.0);
        
        SetVec4Uniform(Shader.Program, "color", color);
        
        glDrawArrays(GL_LINE_STRIP, 0, 6);
        glBindVertexArray(0);
    }
}

static void RenderAStarPath(render_state* RenderState, entity* Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    astar_path& Path = Entity->Enemy.AStarPath;
    if(Path.AStarPath) 
    {
        glBindVertexArray(RenderState->AStarPathVAO);
        for(uint32 PathIndex = 0; PathIndex < Path.AStarPathLength; PathIndex++)
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(Path.AStarPath[PathIndex].X, Path.AStarPath[PathIndex].Y, 0.0f));
            Model = glm::scale(Model, glm::vec3(1, 1, 1));
            
            auto Shader = RenderState->Shaders[Shader_AStarPath];
            UseShader(&Shader);
            
            SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
            SetMat4Uniform(Shader.Program, "View", View);
            SetMat4Uniform(Shader.Program, "Model", Model);
            glm::vec4 color;
            
            if(Path.PathIndex == PathIndex)
            {
                color = glm::vec4(0.0,1.0,0.0,0.4);
            }
            else 
            {
                color = glm::vec4(0.0, 0.0, 1.0, 0.4);
            }
            
            SetVec4Uniform(Shader.Program, "color", color);
            glDrawArrays(GL_QUADS, 0, 6);
            
        }
        
        glBindVertexArray(0);
    }
}

void RenderUISprite(render_state* RenderState, uint32 TextureHandle, glm::vec2 ScreenPosition, glm::vec3 Scale)
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
        if (RenderState->BoundTexture != TextureHandle) //never bind the same texture if it's already bound
        {
            glBindTexture(GL_TEXTURE_2D, TextureHandle);
            RenderState->BoundTexture = TextureHandle;
        }
        
    }
    
    UseShader(&Shader);
    
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(X, Y, 0));
    Model = glm::scale(Model, Scale);
    
    SetMat4Uniform(Shader.Program, "M", Model);
    SetVec4Uniform(Shader.Program, "color", glm::vec4(1, 1, 1, 1)); 
    
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

static void RenderAnimationPreview(render_state* RenderState, const animation_info& AnimationInfo, const animation& Animation, glm::vec2 ScreenPosition, real32 Scale)
{
    real32 Ratio = Animation.FrameSize.y / Animation.FrameSize.x;
    
    real32 MaxWidth = 350.0f;
    real32 MaxHeight = MaxWidth * Ratio;
    
    ScreenPosition.x *= RenderState->ScaleX;
    ScreenPosition.x -= 1;
    ScreenPosition.y *= RenderState->ScaleY;
    ScreenPosition.y-= 1;
    
    auto Shader = RenderState->SpritesheetShader;
    
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(ScreenPosition.x, ScreenPosition.y, 0.0f));
    Model = glm::scale(Model, glm::vec3(MaxWidth * RenderState->ScaleX, MaxHeight * RenderState->ScaleY, 1));
    
    if (RenderState->BoundTexture != Animation.Texture->TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Animation.Texture->TextureHandle);
        RenderState->BoundTexture = Animation.Texture->TextureHandle;
    }
    
    glBindVertexArray(RenderState->SpriteSheetVAO);
    
    UseShader(&Shader);
    auto Frame = Animation.Frames[AnimationInfo.FrameIndex];
    SetFloatUniform(Shader.Program, "isUI", 1);
    SetVec2Uniform(Shader.Program,"textureOffset", glm::vec2(Frame.X, Frame.Y));
    SetFloatUniform(Shader.Program, "frameWidth", Animation.FrameSize.x);
    SetFloatUniform(Shader.Program, "frameHeight", Animation.FrameSize.y);
    SetVec4Uniform(Shader.Program, "color", glm::vec4(1, 1, 1, 1));
    SetVec2Uniform(Shader.Program,"sheetSize",
                   glm::vec2(Animation.Texture->Width, Animation.Texture->Height));
    
    SetMat4Uniform(Shader.Program, "Model", Model);
    
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

static void RenderHealthbar(render_state* RenderState,
                            entity* Entity, const entity_healthbar& Healthbar, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix)
{
    glm::mat4 Model(1.0f);
    
    Model = glm::translate(Model, glm::vec3(Entity->Position.x + Healthbar.Offset.x, Entity->Position.y + Healthbar.Offset.y, 0.0f));
    Model = glm::scale(Model, glm::vec3(Healthbar.Scale.x, Healthbar.Scale.y, 0));
    
    if (RenderState->BoundTexture != Healthbar.RenderInfo.Texture->TextureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, Healthbar.RenderInfo.Texture->TextureHandle);
        RenderState->BoundTexture = Healthbar.RenderInfo.Texture->TextureHandle;
    }
    
    shader Shader = RenderState->SpritesheetShader;
    
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
    
    SetFloatUniform(Shader.Program, "isUI", 0);
    SetVec2Uniform(Shader.Program,"textureOffset", glm::vec2(Healthbar.RenderInfo.FrameSize.x * Healthbar.CurrentFrame,0));
    SetFloatUniform(Shader.Program, "frameWidth", Healthbar.RenderInfo.FrameSize.x);
    SetFloatUniform(Shader.Program, "frameHeight",Healthbar.RenderInfo.FrameSize.y);
    SetVec4Uniform(Shader.Program, "color", glm::vec4(1, 1, 1, 1));
    SetVec2Uniform(Shader.Program,"sheetSize",
                   glm::vec2(Healthbar.RenderInfo.Texture->Width, Healthbar.RenderInfo.Texture->Height));
    
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", ViewMatrix);
    SetMat4Uniform(Shader.Program, "Model", Model);
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
        
        if(Entity.CurrentAnimation) 
        {
            real32 WidthInUnits = (real32)Entity.CurrentAnimation->FrameSize.x / (real32)PIXELS_PER_UNIT;
            real32 HeightInUnits = (real32)Entity.CurrentAnimation->FrameSize.y / (real32)PIXELS_PER_UNIT;
            
            glm::vec3 Scale = glm::vec3(WidthInUnits * Entity.Scale, HeightInUnits * Entity.Scale, 1);
            
            real32 RemoveInX = (Entity.IsFlipped ? 1.0f * Scale.x - (2 * Entity.CurrentAnimation->Center.x * Scale.x) : 0) + Entity.CurrentAnimation->Center.x * Scale.x;
            
            Model = glm::translate(Model, glm::vec3(Entity.Position.x - RemoveInX, Entity.Position.y - (Entity.IsFlipped ? Entity.CurrentAnimation->Center.y : 0), 0.0f));
            Model = glm::translate(Model, glm::vec3(1, 1, 0.0f));
            Model = glm::rotate(Model, Entity.Rotation.z, glm::vec3(0, 0, 1)); 
            Model = glm::translate(Model, glm::vec3(-1, -1, 0.0f));
            
            if(Entity.IsFlipped)
            {
                Model = glm::translate(Model, glm::vec3(Scale.x, 0, 0));
                Scale = glm::vec3(-Scale.x, Scale.y, 1);
            }
            
            Model = glm::scale(Model, glm::vec3(Scale.x, Scale.y, Scale.z));
            
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
            SetFloatUniform(Shader.Program, "isUI", 0);
            SetVec2Uniform(Shader.Program,"textureOffset", glm::vec2(Frame.X, Frame.Y));
            SetFloatUniform(Shader.Program, "frameWidth", Animation->FrameSize.x);
            SetFloatUniform(Shader.Program, "frameHeight", Animation->FrameSize.y);
            SetVec2Uniform(Shader.Program,"sheetSize",
                           glm::vec2(Animation->Texture->Width, Animation->Texture->Height));
        } 
        else 
        {
            Model = glm::translate(Model, glm::vec3(Entity.Position.x, Entity.Position.y, 0.0f));
            Model = glm::translate(Model, glm::vec3(1, 1, 0.0f));
            Model = glm::rotate(Model, Entity.Rotation.z, glm::vec3(0, 0, 1)); 
            Model = glm::translate(Model, glm::vec3(-1, -1, 0.0f));
            
            real32 WidthInUnits = Entity.CurrentAnimation->Texture->Width / (real32)PIXELS_PER_UNIT;
            real32 HeightInUnits = Entity.CurrentAnimation->Texture->Height / (real32)PIXELS_PER_UNIT;
            
            glm::vec3 Scale = glm::vec3(WidthInUnits * Entity.Scale, HeightInUnits * Entity.Scale, 1);
            
            if(Entity.IsFlipped)
            {
                Scale = glm::vec3(-Scale.x, Scale.y, 1);
                Model = glm::translate(Model, glm::vec3(Entity.Scale, 0, 0));
            }
            
            Model = glm::scale(Model, glm::vec3(Scale.x, Scale.y, Scale.z));
            
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
        SetVec4Uniform(Shader.Program, "Color", RenderEntity->Color);
        
        glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
        glDrawArrays(GL_QUADS, 0, 4);
        glBindVertexArray(0);
        
        if(Entity.Type == Entity_Enemy && Entity.Health < 4 && Entity.Health > 0)
        {
            RenderHealthbar(RenderState, &Entity, *Entity.Enemy.Healthbar, ProjectionMatrix, View);
        }
    }
    
    if(Entity.Type == Entity_Player && Entity.Player.RenderCrosshair)
    {
        RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), Entity.Position.x + Entity.Player.CrosshairPositionX, Entity.Position.y + Entity.Player.CrosshairPositionY, 1, 1, RenderState->Textures["crosshair"]->TextureHandle, false, ProjectionMatrix, View);
    }
    else if(Entity.RenderButtonHint)
    {
        // @Cleanup: This should definitely be done differently to open up for other possible key bindings
        RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), Entity.Position.x + 0.5f, Entity.Position.y + 1.5f, 1, 1, RenderState->Textures["b_button"]->TextureHandle, false, ProjectionMatrix, View);
    }
    
    if(Entity.Type == Entity_Enemy && Entity.Enemy.IsTargeted)
    {
        RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), Entity.Position.x + Entity.Enemy.TargetingPositionX, Entity.Position.y + Entity.Enemy.TargetingPositionY, 1, 1, RenderState->Textures["red_arrow"]->TextureHandle, false, ProjectionMatrix, View);
    }
    
    if(RenderState->RenderColliders && !Entity.IsKinematic)
        RenderColliderWireframe(RenderState, &Entity, ProjectionMatrix, View);
    
    if(RenderState->RenderPaths && Entity.Type == Entity_Enemy)
        RenderAStarPath(RenderState,&Entity,ProjectionMatrix,View);
}

static void RenderTile(render_state* RenderState, uint32 X, uint32 Y, uint32 TilesheetIndex, glm::vec2 TextureOffset, glm::vec2 SheetSize, glm::vec4 Color,  glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    glBindVertexArray(RenderState->SpriteSheetVAO);
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(X, Y - 1, 0.0f));
    Model = glm::scale(Model, glm::vec3(1, 1, 1));
    
    glBindTexture(GL_TEXTURE_2D, RenderState->Tilesheets[TilesheetIndex].Texture.TextureHandle);
    
    shader Shader = RenderState->SpritesheetShader;
    UseShader(&Shader);
    
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    SetVec4Uniform(Shader.Program, "color", Color);
    SetVec2Uniform(Shader.Program, "textureOffset", TextureOffset);
    SetFloatUniform(Shader.Program, "frameWidth", 16.0f);
    SetFloatUniform(Shader.Program, "frameHeight", 16.0f);
    SetVec2Uniform(Shader.Program, "sheetSize", SheetSize);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

void RenderButton(render_state* RenderState, const button& Button)
{
    RenderRect(Render_Fill, RenderState, Button.Color, Button.ScreenPosition.x, Button.ScreenPosition.y, Button.Size.x, Button.Size.y);
    RenderText(RenderState, RenderState->ButtonFont, Button.TextColor, Button.Text, Button.ScreenPosition.x + 4, Button.ScreenPosition.y + 20, 1);
    // @Buggy: Alignment_Center is broken
}

void RenderTextfield(render_state* RenderState, const textfield& Textfield)
{
    RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), Textfield.ScreenPosition.x, Textfield.ScreenPosition.y, Textfield.Size.x, Textfield.Size.y);
    
    RenderText(RenderState, RenderState->RobotoFont, glm::vec4(1, 1, 1, 1), Textfield.Label, Textfield.ScreenPosition.x, Textfield.ScreenPosition.y + 35, 1);
    RenderText(RenderState, RenderState->RobotoFont, glm::vec4(0, 0, 0, 1), Textfield.Text, Textfield.ScreenPosition.x, Textfield.ScreenPosition.y + 10, 1);
    if(Textfield.InFocus)
    {
        RenderRect(Render_Outline, RenderState, glm::vec4(1, 0, 0, 1), Textfield.ScreenPosition.x-3, Textfield.ScreenPosition.y - 3, Textfield.Size.x + 6, Textfield.Size.y + 6);
    }
    
}

void RenderCheckbox(render_state* RenderState, const checkbox& Checkbox)
{
    RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1),Checkbox.ScreenPosition.x, Checkbox.ScreenPosition.y, 25, 25);
    
    RenderText(RenderState, RenderState->RobotoFont, glm::vec4(1, 1, 1, 1), Checkbox.Label, Checkbox.ScreenPosition.x, Checkbox.ScreenPosition.y + 35, 1);
    
    if(Checkbox.Checked)
    {
        RenderRect(Render_Fill, RenderState, glm::vec4(1, 0, 0, 1),Checkbox.ScreenPosition.x + 5, Checkbox.ScreenPosition.y + 5, 15, 15);
    }
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
    
    SetFloatUniform(Shader.Program, "isUI", 0);
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    
    glDrawArrays(GL_QUADS, 0, Tilemap.RenderInfo.VBOSize / 4);
    glBindVertexArray(0);
}


static void EditorRenderTilemap(glm::vec2 ScreenPosition, real32 Size, render_state* RenderState, const tilemap& Tilemap)
{
    real32 X = ScreenPosition.x * RenderState->ScaleX;
    X -= 1;
    real32 Y = ScreenPosition.y * RenderState->ScaleY;
    Y -= 1;
    
    glBindVertexArray(Tilemap.EditorRenderInfo.VAO);
    
    if (RenderState->BoundTexture != Tilemap.RenderEntity.Texture->TextureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, Tilemap.RenderEntity.Texture->TextureHandle);
        RenderState->BoundTexture = Tilemap.RenderEntity.Texture->TextureHandle;
    }
    
    auto Shader = RenderState->TileShader;
    UseShader(&Shader);
    
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(X, Y, 0));
    Model = glm::scale(Model, glm::vec3(Size * RenderState->ScaleX, Size * RenderState->ScaleY, 0.1));
    SetFloatUniform(Shader.Program, "isUI", 1);
    SetMat4Uniform(Shader.Program, "Model", Model);
    
    glDrawArrays(GL_QUADS, 0, Tilemap.EditorRenderInfo.VBOSize / 4);
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
    
    qsort(GameState->RenderState.RenderEntities, GameState->RenderState.RenderEntityCount, sizeof(render_entity), CompareFunction);
    
    for(int32 Index = 0; Index < GameState->RenderState.RenderEntityCount; Index++) 
    {
        render_entity* Render = &GameState->RenderState.RenderEntities[Index];
        // @Incomplete: Only render if in view
        Render->Entity->RenderEntityHandle = Index;
        RenderEntity(&GameState->RenderState, *Render->Entity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    }
}

void RenderGame(game_state* GameState)
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
                    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), (real32)GameState->RenderState.WindowWidth / 2 - 200, (real32)GameState->RenderState.WindowHeight / 2 - 10 - 40 * Index, 400, 40);
                    
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
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            RenderInGameMode(GameState);
            if(GameState->Paused)
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.5, 1, 1, 1), "PAUSED", (real32)GameState->RenderState.WindowWidth / 2, 40, 1, Alignment_Center);
            
            if(!GameState->InputController.ControllerPresent)
            {
                RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), GameState->InputController.MouseX - 20, (real32)GameState->RenderState.WindowHeight - GameState->InputController.MouseY - 20, 40, 40, GameState->RenderState.Textures["cross"]->TextureHandle, true, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            }
        }
        break;
        case Mode_Editor:
        {
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            switch(GameState->EditorState.Mode)
            {
                case Editor_Normal:
                {
                    RenderInGameMode(GameState);
                    RenderRect(Render_Outline, &GameState->RenderState, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0, 0, (real32)GameState->CurrentLevel.Tilemap.Width, (real32)GameState->CurrentLevel.Tilemap.Height, 0, false, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                    
                    
                    if(GameState->EditorState.SelectedEntity)
                        RenderWireframe(&GameState->RenderState, GameState->EditorState.SelectedEntity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                    
                    glm::vec2 TextureOffset = GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType + 1].TextureOffset;
                    
                    if(GameState->EditorState.TileX >= 0 && GameState->EditorState.TileX < GameState->CurrentLevel.Tilemap.Width && GameState->EditorState.TileY > 0 && GameState->EditorState.TileY <= GameState->CurrentLevel.Tilemap.Height)
                    {
                        const tilesheet& Tilesheet = GameState->RenderState.Tilesheets[GameState->CurrentLevel.TilesheetIndex];
                        
                        glm::vec2 SheetSize(Tilesheet.Texture.Width, Tilesheet.Texture.Height);
                        
                        RenderTile(&GameState->RenderState, (uint32)GameState->EditorState.TileX, (uint32)GameState->EditorState.TileY, GameState->CurrentLevel.TilesheetIndex, TextureOffset, SheetSize, glm::vec4(1, 1, 1, 1),  GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                    }
                    
                    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0, 0, 0, 1), GameState->EditorState.ToolbarX, GameState->EditorState.ToolbarY, GameState->EditorState.ToolbarWidth, GameState->EditorState.ToolbarHeight);
                    
                    EditorRenderTilemap(glm::vec2((real32)GameState->RenderState.WindowWidth - 80, 5 + GameState->EditorState.ToolbarScrollOffsetY), 60, &GameState->RenderState, GameState->CurrentLevel.Tilemap);
                    
                    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 0, 0, 1), (real32)GameState->RenderState.WindowWidth - 80,
                               GameState->EditorState.SelectedTileType * 60 + 5 + GameState->EditorState.ToolbarScrollOffsetY, 60, 60, GameState->RenderState.Textures["selected_tile"]->TextureHandle);
                    
                    char Text[255]; sprintf(Text,"Type index: %d Is solid: %d",GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType].TypeIndex,GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType].IsSolid);
                    
                    RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(1, 1, 1, 1), Text, (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight - 200, 1, Alignment_Center);
                    
                    
                    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0, 0, 0, 1), 0, (real32)GameState->RenderState.WindowHeight - 155, (real32)GameState->RenderState.WindowWidth - 80, 155);
                    
                    if(GameState->EditorState.PlacementMode == Editor_Placement_Tile)
                        RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.6f, 0.6f, 0.6f, 1), "Tile-mode", (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight - 70, 1, Alignment_Center);
                    else
                        RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.6f, 0.6f, 0.6f, 1), "Entity-mode", (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight - 70, 1, Alignment_Center);
                }
                break;
                case Editor_Animation:
                {
                    RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.6f, 0.6f, 0.6f, 1), "Animation editing", (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight - 70, 1, Alignment_Center);
                    
                    if(!GameState->EditorState.LoadedAnimation)
                    {
                        std::map<char*, animation>::iterator Iterator;
                        
                        int32 Index = 0;
                        
                        for(Iterator = GameState->Animations.begin(); Iterator != GameState->Animations.end(); Iterator++)
                        {
                            if(Index == GameState->EditorState.SelectedAnimation)
                            {
                                RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), 20, (real32)GameState->RenderState.WindowHeight / 2 + (GameState->Animations.size() - Index) * 20 - 150 - 4, 300, 20);
                                RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(0, 0, 0, 1), Iterator->first, 20, (real32)GameState->RenderState.WindowHeight / 2 + (GameState->Animations.size() - Index++) * 20 - 150, 1);
                            }
                            else
                                RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(1, 1, 1, 1), Iterator->first, 20, (real32)GameState->RenderState.WindowHeight / 2 + (GameState->Animations.size() - Index++) * 20 - 150, 1);
                        }
                    }
                    else
                    {
                        const texture& Texture = *GameState->EditorState.LoadedAnimation->Texture;
                        real32 Scale = 2.0f;
                        real32 TextureWidth = (real32)Texture.Width * Scale;
                        real32 TextureHeight = (real32)Texture.Height * Scale;
                        
                        for(int32 TextureIndex = 0; TextureIndex < GameState->EditorState.TexturesLength; TextureIndex++)
                        {
                            if(TextureIndex == GameState->EditorState.SelectedTexture)
                            {
                                RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), 20, (real32)GameState->RenderState.WindowHeight / 2 + (GameState->EditorState.TexturesLength - TextureIndex) * 20 - 150 - 4, 300, 20);
                                RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(0, 0, 0, 1), GameState->EditorState.Textures[TextureIndex], 20, (real32)GameState->RenderState.WindowHeight / 2 + (GameState->EditorState.TexturesLength - TextureIndex) * 20 - 150, 1);
                            }
                            else
                                RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(1, 1, 1, 1), GameState->EditorState.Textures[TextureIndex], 20, (real32)GameState->RenderState.WindowHeight / 2 + (GameState->EditorState.TexturesLength - TextureIndex) * 20 - 150, 1);
                        }
                        
                        
                        RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), (real32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2, (real32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2, TextureWidth, TextureHeight, Texture.TextureHandle);
                        
                        if(GameState->EditorState.AnimationFrameCountField)
                        {
                            int32 FrameCount = 0;
                            int32 FrameWidth = 0;
                            int32 FrameHeight = 0;
                            int32 FrameOffsetX = 0;
                            int32 FrameOffsetY = 0;
                            
                            sscanf(GameState->EditorState.AnimationFrameCountField->Text, "%d", &FrameCount);
                            sscanf(GameState->EditorState.AnimationFrameWidthField->Text, "%d", &FrameWidth);
                            sscanf(GameState->EditorState.AnimationFrameHeightField->Text, "%d", &FrameHeight);
                            sscanf(GameState->EditorState.AnimationFrameOffsetXField->Text, "%d", &FrameOffsetX);
                            sscanf(GameState->EditorState.AnimationFrameOffsetYField->Text, "%d", &FrameOffsetY);
                            
                            real32 StartX = (real32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2;
                            real32 StartY = (real32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2;
                            
                            real32 X = FrameWidth * FrameOffsetX;
                            real32 Y = FrameHeight * FrameOffsetY;
                            
                            for(int32 Index = 0; Index < FrameCount; Index++)
                            {
                                if(Index > 0)
                                {
                                    if(X + FrameWidth < (real32)Texture.Width)
                                    {
                                        X += FrameWidth;
                                    }
                                    else
                                    {
                                        X = 0;
                                        Y += FrameHeight;
                                    }
                                }
                                
                                RenderRect(Render_Outline, &GameState->RenderState, glm::vec4(1, 0, 0, 1), StartX + FrameOffsetX + X * Scale, StartY - Y * Scale + TextureHeight - FrameHeight * Scale + FrameOffsetY,(real32)FrameWidth * Scale, (real32)FrameHeight * Scale);
                            }
                            
                        }
                        RenderRect(Render_Outline, &GameState->RenderState, glm::vec4(1, 0, 0, 1), (real32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2, (real32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2, TextureWidth, TextureHeight);
                        
                        if(GameState->EditorState.LoadedAnimation->FrameCount > 0)
                            RenderAnimationPreview(&GameState->RenderState, GameState->EditorState.AnimationInfo, *GameState->EditorState.LoadedAnimation, glm::vec2((real32)GameState->RenderState.WindowWidth - 400, 20), 2);
                    }
                    
                    RenderButton(&GameState->RenderState, *GameState->EditorState.CreateNewAnimationButton);
                }
                break;
            }
            
            RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(1, 1, 1, 1), "EDITOR", (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight - 30, 1, Alignment_Center);
            
            
            for(int32 ButtonIndex = 0; ButtonIndex < 10; ButtonIndex++)
            {
                if(GameState->EditorState.Buttons[ButtonIndex].Active)
                    RenderButton(&GameState->RenderState, GameState->EditorState.Buttons[ButtonIndex]);
            }
            
            for(uint32 Index = 0; Index < 20; Index++)
            {
                if(GameState->EditorState.Textfields[Index].Active)
                    RenderTextfield(&GameState->RenderState, GameState->EditorState.Textfields[Index]);
            }
            
            for(uint32 Index = 0; Index < 10; Index++)
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
        
        RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(Color.x, Color.y, Color.z, GameState->Camera.FadingAlpha), 0, 0, GameState->RenderState.WindowWidth, GameState->RenderState.WindowHeight);
    }
    
    if(GameState->PlayerState == Player_Dead)
    {
        RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 0, 0, 0.2f), 0, 0, GameState->RenderState.WindowWidth, GameState->RenderState.WindowHeight);
        
        real32 Width = 0;
        real32 Height = 0;
        MeasureText(&GameState->RenderState.TitleFont, "YOU DIED", &Width, &Height);
        
        RenderText(&GameState->RenderState, GameState->RenderState.TitleFont, glm::vec4(1, 1, 1, 1), "YOU DIED", (real32)GameState->RenderState.WindowWidth / 2 - Width / 2, (real32)GameState->RenderState.WindowHeight / 2 - Height / 2, 1);
        
        MeasureText(&GameState->RenderState.TitleFont, "Press any key to restart. . .", &Width, &Height);
        RenderText(&GameState->RenderState, GameState->RenderState.TitleFont, glm::vec4(1, 1, 1, 1), "Press any key to restart. . .", (real32)GameState->RenderState.WindowWidth / 2 - Width / 2, (real32)GameState->RenderState.WindowHeight / 2 - Height * 2, 1);
    }
}

static void CheckLevelVAO(game_state* GameState)
{
    if(GameState->CurrentLevel.Tilemap.RenderInfo.VAO == 0)
    {
        CreateTilemapVAO(&GameState->RenderState, GameState->CurrentLevel.Tilemap,
                         &GameState->CurrentLevel.Tilemap.EditorRenderInfo, &GameState->CurrentLevel.Tilemap.RenderInfo);
    }
}

static void RenderDebugInfo(game_state* GameState)
{
    auto Pos = glm::unProject(glm::vec3(GameState->InputController.MouseX, GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
                              GameState->Camera.ViewMatrix,
                              GameState->Camera.ProjectionMatrix,
                              glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    
    if(GameState->Console.CurrentTime > 0)
        RenderConsole(GameState, &GameState->Console);
    
    if(GameState->RenderState.RenderFPS)
    {
        char FPS[32];
        sprintf(FPS, "%4.0f",GameState->RenderState.FPS);
        RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, 
                   glm::vec4(1, 1, 1, 1), FPS, GameState->RenderState.WindowWidth / 2.0f, 
                   20.0f, 1.0f);
        
        int32 X = (int32)glm::floor(Pos.x);
        int32 Y = (int32)glm::floor(Pos.y);
        char MousePos[32];
        sprintf(MousePos,"Mouse: (%d %d)",X,Y);
        RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, 
                   glm::vec4(1, 1, 1, 1), MousePos, GameState->RenderState.WindowWidth / 2.0f - 200, 
                   20.0f, 1.0f);
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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    RenderGame(GameState);
    
    RenderDebugInfo(GameState);
    
    glfwSwapBuffers(GameState->RenderState.Window);
}
