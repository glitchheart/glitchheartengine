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
    
    GameState->EditorState.ToolbarX = (r32)GameState->RenderState.WindowWidth - 100;
    GameState->EditorState.ToolbarY = 0;
    GameState->EditorState.ToolbarWidth = 100.0f;
    GameState->EditorState.ToolbarHeight = (r32)GameState->RenderState.WindowHeight;
}

inline static r64 GetTime()
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

static b32 ShouldCloseWindow(render_state* RenderState)
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
        
        Font->CharacterInfo[i].AX = (r32)(G->advance.x >> 6);
        Font->CharacterInfo[i].AY = (r32)(G->advance.y >> 6);
        
        Font->CharacterInfo[i].BW = (r32)G->bitmap.width;
        Font->CharacterInfo[i].BH = (r32)G->bitmap.rows;
        
        Font->CharacterInfo[i].BL = (r32)G->bitmap_left;
        Font->CharacterInfo[i].BT = (r32)G->bitmap_top;
        
        Font->CharacterInfo[i].TX = (r32)X / Font->AtlasWidth;
        
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

static void LoadTilemapBuffer(render_state* RenderState, i32 Layer, GLuint* VAO, GLuint* VBO, i32* Size, const tilemap& Tilemap)
{
    GLfloat* VertexBuffer = (GLfloat*)malloc(sizeof(GLfloat) * 16 * Tilemap.Width * Tilemap.Height);
    
    i32 Current = 0;
    
    r32 Width = (r32)Tilemap.RenderEntity.Texture->Width;
    r32 Height = (r32)Tilemap.RenderEntity.Texture->Height;
    
    for(u32 X = 0; X < Tilemap.Width; X++)
    {
        for(u32 Y = 0; Y < Tilemap.Height; Y++)
        {
            tile_data* Tile = &Tilemap.Data[Layer][X][Y];
            
            if(Tile->TypeIndex != 0)
            {
                r32 TexCoordX = (Tile->TextureOffset.x) / Width;
                r32 TexCoordY = (Tile->TextureOffset.y) / Height; 
                r32 TexCoordXHigh = (Tile->TextureOffset.x + Tilemap.TileSize) / Width;
                r32 TexCoordYHigh = (Tile->TextureOffset.y + Tilemap.TileSize) / Height; 
                
                r32 CorrectY = (r32)Y;
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
    
    *Size = Current;
    
    if(*VBO == 0)
        glGenBuffers(1, VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * *Size, VertexBuffer, GL_DYNAMIC_DRAW);
    
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
    free(VertexBuffer);
}


static void LoadEditorTileBuffer(render_state* RenderState, editor_render_info& EditorRenderInfo, const tilemap& Tilemap)
{
    GLfloat* VertexBuffer = (GLfloat*)malloc(sizeof(GLfloat) * 16 * Tilemap.TileCount);
    
    i32 Current = 0;
    
    r32 Width = (r32)Tilemap.RenderEntity.Texture->Width;
    r32 Height = (r32)Tilemap.RenderEntity.Texture->Height;
    
    i32 X = 0;
    i32 Y = 0;
    
    for(u32 Index = 1; Index < Tilemap.TileCount; Index++)
    {
        tile_data* Tile = &Tilemap.Tiles[Index];
        
        r32 TexCoordX = (Tile->TextureOffset.x) / Width;
        r32 TexCoordY = (Tile->TextureOffset.y) / Height; 
        r32 TexCoordXHigh = (Tile->TextureOffset.x + Tilemap.TileSize) / Width;
        r32 TexCoordYHigh = (Tile->TextureOffset.y + Tilemap.TileSize) / Height; 
        
        VertexBuffer[Current++] = (GLfloat)X;
        VertexBuffer[Current++] = (GLfloat)Y + 1.0f;
        VertexBuffer[Current++] = (GLfloat)TexCoordX;
        VertexBuffer[Current++] =  (GLfloat)TexCoordY;
        VertexBuffer[Current++] = (GLfloat)X + 1;
        VertexBuffer[Current++] = (GLfloat)Y + 1;
        VertexBuffer[Current++] = (GLfloat)TexCoordXHigh;
        VertexBuffer[Current++] =  (GLfloat)TexCoordY;
        VertexBuffer[Current++] = (GLfloat)X + 1;
        VertexBuffer[Current++] = (GLfloat)Y;
        VertexBuffer[Current++] = (GLfloat)TexCoordXHigh;
        VertexBuffer[Current++] = (GLfloat)TexCoordYHigh;
        VertexBuffer[Current++] = (GLfloat)X;
        VertexBuffer[Current++] = (GLfloat)Y;
        VertexBuffer[Current++] =(GLfloat)TexCoordX;
        VertexBuffer[Current++] = (GLfloat)TexCoordYHigh;
        
        X++;
        
        if(X == (i32)(Width / Tilemap.TileSize))
        {
            X = 0;
            Y++;
        }
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
    TilemapRenderInfo->VBOS[0] = 0;
    TilemapRenderInfo->VBOS[1] = 0;
    TilemapRenderInfo->Dirty = false;
    
    //tile
    glGenVertexArrays(1, &TilemapRenderInfo->VAOS[0]);
    glBindVertexArray(TilemapRenderInfo->VAOS[0]);
    LoadTilemapBuffer(RenderState, 0, &TilemapRenderInfo->VAOS[0], &TilemapRenderInfo->VBOS[0], &TilemapRenderInfo->VBOSizes[0], Tilemap);
    
    glGenVertexArrays(1, &TilemapRenderInfo->VAOS[1]);
    glBindVertexArray(TilemapRenderInfo->VAOS[1]);
    LoadTilemapBuffer(RenderState, 1, &TilemapRenderInfo->VAOS[1], &TilemapRenderInfo->VBOS[1], &TilemapRenderInfo->VBOSizes[1], Tilemap);
    
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
    
    // Framebuffer
    glGenFramebuffers(1, &RenderState->FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, RenderState->FrameBuffer);
    
    glGenTextures(1, &RenderState->TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, RenderState->TextureColorBuffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, RenderState->WindowWidth, RenderState->WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderState->TextureColorBuffer, 0
        );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    glGenFramebuffers(1, &RenderState->LightingFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, RenderState->LightingFrameBuffer);
    
    glGenTextures(1, &RenderState->LightingTextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, RenderState->LightingTextureColorBuffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, RenderState->WindowWidth, RenderState->WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderState->LightingTextureColorBuffer, 0
        );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    // FrameBuffer VAO
    glGenVertexArrays(1, &RenderState->FrameBufferVAO);
    glBindVertexArray(RenderState->FrameBufferVAO);
    glGenBuffers(1, &RenderState->FrameBufferVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->FrameBufferVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->FrameBufferVertices, GL_DYNAMIC_DRAW);
    
    RenderState->FrameBufferShader.Type = Shader_FrameBuffer;
    
    LoadShader(ShaderPaths[Shader_FrameBuffer], &RenderState->FrameBufferShader);
    
    auto PosLoc = glGetAttribLocation(RenderState->FrameBufferShader.Program, "pos");
    auto TexLoc = glGetAttribLocation(RenderState->FrameBufferShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PosLoc);
    glVertexAttribPointer(PosLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexLoc);
    glVertexAttribPointer(TexLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    TexLoc = glGetUniformLocation(RenderState->FrameBufferShader.Program, "tex");
    glUniform1i(TexLoc, 0);
    TexLoc = glGetUniformLocation(RenderState->FrameBufferShader.Program, "lightingTex");
    glUniform1i(TexLoc, 1);
    
    glBindVertexArray(0);
    
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
    
    // Lines
    glGenBuffers(1, &RenderState->PrimitiveVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->PrimitiveVBO);
    
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
    InitializeFreeTypeFont("../assets/fonts/roboto/Roboto-Regular.ttf", 30, RenderState->FTLibrary, &RenderState->ButtonFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/rubber-biscuit/RUBBBB__.ttf", 50, RenderState->FTLibrary, &RenderState->TitleFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/pixelmix.ttf", 18, RenderState->FTLibrary, &RenderState->DamageFont, &RenderState->StandardFontShader);
    
    // Light sources
    RenderState->LightSourceShader.Type = Shader_LightSource;
    LoadShader(ShaderPaths[Shader_LightSource], &RenderState->LightSourceShader);
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
    
    for(i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
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
        u32 Index = 0;
        
        while(fgets(LineBuffer, 255, File))
        {
            TempNames[Index] = (char*)malloc(sizeof(char) * 20);
            sscanf(LineBuffer, "%s", TempNames[Index++]);
        }
        
        fclose(File);
        
        RenderState->TilesheetCount = Index;
        RenderState->Tilesheets = (tilesheet*)malloc(sizeof(tilesheet) * Index);
        
        for(u32 I = 0; I < Index; I++)
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
    glLineWidth(2.0f);
    
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
}

static void SetFloatUniform(GLuint ShaderHandle, const char* UniformName, r32 Value)
{
    glUniform1f(glGetUniformLocation(ShaderHandle, UniformName), Value);
}

static void SetIntUniform(GLuint ShaderHandle, const char* UniformName, i32 Value)
{
    glUniform1i(glGetUniformLocation(ShaderHandle, UniformName), Value);
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

static void SetVec4ArrayUniform(GLuint ShaderHandle, const char *UniformName, glm::vec4* Value, u32 Length)
{
    glUniform4fv(glGetUniformLocation(ShaderHandle, UniformName), Length, (GLfloat*)&Value[0]);
}


static void SetFloatArrayUniform(GLuint ShaderHandle, const char *UniformName, r32* Value, u32 Length)
{
    glUniform1fv(glGetUniformLocation(ShaderHandle, UniformName), Length, (GLfloat*)&Value[0]);
}

static void RenderLine(render_state& RenderState, glm::vec4 Color, r32 X1, r32 Y1, r32 X2, r32 Y2, b32 IsUI = true, glm::mat4 ProjectionMatrix = glm::mat4(), glm::mat4 ViewMatrix = glm::mat4())
{
    if(IsUI)
    {
        X1 *= RenderState.ScaleX;
        X1 -= 1;
        Y1 *= RenderState.ScaleY;
        Y1 -= 1;
        X2 *= RenderState.ScaleX;
        X2 -= 1;
        Y2 *= RenderState.ScaleY;
        Y2 -= 1;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, RenderState.PrimitiveVBO);
    
    GLfloat Points[4] = { X1, Y1, X2, Y2 };
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat), &Points[0], GL_DYNAMIC_DRAW);
    
    auto& Shader = RenderState.RectShader;
    UseShader(&Shader);
    
    //draw upper part
    glm::mat4 Model(1.0f);
    
    if(!IsUI)
    {
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", ViewMatrix);
    }
    
    SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
    SetMat4Uniform(Shader.Program, "M", Model);
    SetVec4Uniform(Shader.Program, "color", Color);
    
    glDrawArrays(GL_LINES, 0, 4);
}

// NOTE(Niels): Possible future use but buggy
void RenderCircle(render_state& RenderState, glm::vec4 Color, r32 CenterX, r32 CenterY, r32 Radius, b32 IsUI = true, glm::mat4 ProjectionMatrix = glm::mat4(), glm::mat4 ViewMatrix = glm::mat4())
{
    if(IsUI)
    {
        CenterX *= RenderState.ScaleX;
        CenterX -= 1;
        CenterY *= RenderState.ScaleY;
        CenterY -= 1;
        Radius *= RenderState.ScaleX;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, RenderState.PrimitiveVBO);
    
    GLfloat Points[720];
    
    i32 PointIndex = 0;
    
    for(i32 Index = 0; Index < 360; Index++)
    {
        r32 Radians = (Index * PI) / 180.0f;
        Points[PointIndex++] = glm::cos(Radians * Radius);
        Points[PointIndex++] = glm::sin(Radians * Radius);
    }
    
    glBufferData(GL_ARRAY_BUFFER, 720 * sizeof(GLfloat), &Points[0], GL_DYNAMIC_DRAW);
    
    auto& Shader = RenderState.RectShader;
    UseShader(&Shader);
    
    //draw upper part
    glm::mat4 Model(1.0f);
    
    if(!IsUI)
    {
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", ViewMatrix);
    }
    
    SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
    SetMat4Uniform(Shader.Program, "M", Model);
    SetVec4Uniform(Shader.Program, "color", Color);
    
    glDrawArrays(GL_LINE_LOOP, 0, 720);
}

static void RenderRect(Render_Mode Mode, render_state* RenderState, glm::vec4 Color, r32 X, r32 Y, r32 Width, r32 Height, u32 TextureHandle = 0, b32 IsUI = true, glm::mat4 ProjectionMatrix = glm::mat4(), glm::mat4 ViewMatrix = glm::mat4())
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
            
            if(TextureHandle != 0)
            {
                glBindVertexArray(RenderState->TextureRectVAO);
            }
            else
            {
                glBindVertexArray(RenderState->RectVAO);
            }
            
            if(TextureHandle != 0)
            {
                glBindTexture(GL_TEXTURE_2D, TextureHandle);
                Shader = RenderState->TextureRectShader;
                RenderState->BoundTexture = TextureHandle;
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
            
            SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
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
            
            SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
            SetMat4Uniform(Shader.Program, "M", Model);
            SetVec4Uniform(Shader.Program, "color", Color);
            
            glDrawArrays(GL_LINE_STRIP, 0, 8);
        }
        break;
    }
    glBindVertexArray(0);
}

static void MeasureText(const render_font& Font, const char* Text, float* Width, float* Height)
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
        *Width += Font.CharacterInfo[Text[Count]].AX;
        
        if(*Height < Font.CharacterInfo[Text[Count]].BH) 
        {
            *Height = Font.CharacterInfo[Text[Count]].BH;
        }
    }
}

//rendering methods
static void RenderText(render_state* RenderState, const render_font& Font, const glm::vec4& Color, const char* Text, r32 X, r32 Y, r32 Scale, Alignment Alignment = Alignment_Left) 
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
            r32 Width;
            r32 Height;
            
            MeasureText(Font, Text, &Width, &Height);
            
            X -= Width / 2.0f;
            Y -= Height / 2.0f;
        }
        break;
    }
    
    X = (r32)(i32)X;
    Y = (r32)(i32)Y;
    
    if((i32)X % 2 != 0)
        X-= 1.0f;
    if((i32)Y % 2 != 0)
        Y -= 1.0f;
    
    X *= RenderState->ScaleX;
    X -= 1.0f;
    Y *= RenderState->ScaleY;
    Y -= 1.0f;
    
    for(const char *P = Text; *P; P++) 
    { 
        r32 W = Font.CharacterInfo[*P].BW * RenderState->ScaleX;
        r32 H = Font.CharacterInfo[*P].BH * RenderState->ScaleY;
        
        r32 X2 =  X + Font.CharacterInfo[*P ].BL * RenderState->ScaleX;
        r32 Y2 = -Y - Font.CharacterInfo[*P ].BT * RenderState->ScaleY;
        
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
    
    r32 PercentAnimated = 1.0f + 1.0f - (r32)Console->CurrentTime / (r32)Console->TimeToAnimate;
    
    //draw upper part
    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0.0f, 0.4f, 0.3f, 0.6f), 0.0f, (r32)RenderState->WindowHeight * 0.77f * PercentAnimated, (r32)RenderState->WindowWidth, (r32)RenderState->WindowHeight * 0.23f);
    
    //draw lower bar
    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0.0f, 0.2f, 0.2f, 0.6f), 0.0f, (r32)RenderState->WindowHeight * 0.77f * PercentAnimated, (r32)RenderState->WindowWidth, 20);  
    
    GLfloat TimeValue = (r32)glfwGetTime();
    GLfloat AlphaValue = (r32)((sin(TimeValue * 4) / 2) + 0.5f);
    r32 Width;
    r32 Height;
    MeasureText(RenderState->InconsolataFont, &Console->Buffer[0], &Width, &Height);
    
    //draw cursor
    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(AlphaValue, 1, AlphaValue, 1), 5 / 1920.0f * (r32)RenderState->WindowWidth + Width, RenderState->WindowHeight * 0.77f * PercentAnimated, 10, 20);
    
    RenderText(RenderState, RenderState->InconsolataFont, glm::vec4(0, 0.8, 0, 1),  &Console->Buffer[0],  5 / 1920.0f * (r32)RenderState->WindowWidth, (r32)RenderState->WindowHeight * 0.775f * PercentAnimated, 1);
    
    int index = 0;
    
    glm::vec4 Color;
    
    for(int Index = 0; Index < HISTORY_BUFFER_LINES; Index++)
    {
        if(Index % 2 != 0)
            Color = glm::vec4(0, 1, 0, 1);
        else
            Color = glm::vec4(1, 1, 1, 1);
        
        RenderText(RenderState, RenderState->InconsolataFont, Color, &Console->HistoryBuffer[Index][0], 5 / 1920.0f * (r32)RenderState->WindowWidth, (r32)RenderState->WindowHeight * 0.78f * PercentAnimated + (Index + 1) * 20 * PercentAnimated, 1);
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
        
        if(Entity->HasWeapon)
        {
            RenderRect(Render_Outline, RenderState, glm::vec4(0, 0, 1, 1), Entity->Weapon.CollisionAABB.Center.x - Entity->Weapon.CollisionAABB.Extents.x, Entity->Weapon.CollisionAABB.Center.y - Entity->Weapon.CollisionAABB.Extents.y, Entity->Weapon.CollisionAABB.Extents.x * 2, Entity->Weapon.CollisionAABB.Extents.y * 2, 0, false, ProjectionMatrix, View);
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
        for(u32 PathIndex = 0; PathIndex < Path.AStarPathLength; PathIndex++)
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

void RenderUISprite(render_state* RenderState, u32 TextureHandle, glm::vec2 ScreenPosition, glm::vec3 Scale)
{
    r32 X = ScreenPosition.x * RenderState->ScaleX;
    X -= 1;
    r32 Y = ScreenPosition.y * RenderState->ScaleY;
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

static void RenderAnimationPreview(render_state* RenderState, const animation_info& AnimationInfo, const animation& Animation, glm::vec2 ScreenPosition, r32 Scale)
{
    r32 Ratio = Animation.FrameSize.y / Animation.FrameSize.x;
    
    r32 MaxWidth = 350.0f;
    r32 MaxHeight = MaxWidth * Ratio;
    
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
    RenderRect(Render_Fill, RenderState, glm::vec4(0.6, 0, 0, 1), Entity->Position.x + Healthbar.Offset.x, Entity->Position.y + Healthbar.Offset.y, 1.0f
               / (r32)Entity->FullHealth * (r32)Entity->Health , 0.1f, 0, false, ProjectionMatrix, ViewMatrix);
    
    
    if(Entity->HealthLost > 0)
    {
        r32 StartX = Entity->Position.x + Healthbar.Offset.x + 1.0f
            / (r32)Entity->FullHealth * (r32)Entity->Health;
        r32 Width = 1.0f
            / (r32)Entity->FullHealth * (r32)Entity->HealthLost;
        
        RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), StartX, Entity->Position.y + Healthbar.Offset.y, Width, 0.1f, 0, false, ProjectionMatrix, ViewMatrix);
        
        glm::mat4 Model = glm::mat4(1.0f) * ViewMatrix;
        
        glm::vec3 Projected =
            glm::project(glm::vec3(Entity->Position.x, Entity->Position.y, 0), Model, ProjectionMatrix, glm::vec4(RenderState->Viewport[0], RenderState->Viewport[1], RenderState->Viewport[2], RenderState->Viewport[3]));
        
        
        for(i32 Index = 0; Index < 10; Index++)
        {
            auto& HealthCount = Entity->Enemy.HealthCounts[Index];
            if(HealthCount.Visible)
            {
                RenderText(RenderState, RenderState->DamageFont, glm::vec4(1, 0, 0, 1), &HealthCount.Count[0], Projected.x + HealthCount.Position.x, Projected.y + HealthCount.Position.y, 1);
            }
        }
    }
}

static void RenderEntity(game_state *GameState, entity &Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{ 
    render_state* RenderState = &GameState->RenderState;
    
    render_entity* RenderEntity = &RenderState->RenderEntities[Entity.RenderEntityHandle];
    auto Shader = RenderState->Shaders[RenderEntity->ShaderIndex];
    
    if(RenderEntity->Rendered && Entity.Active)
    {
        glm::mat4 Model(1.0f);
        
        if(Entity.CurrentAnimation) 
        {
            r32 WidthInUnits = (r32)Entity.CurrentAnimation->FrameSize.x / (r32)PIXELS_PER_UNIT;
            r32 HeightInUnits = (r32)Entity.CurrentAnimation->FrameSize.y / (r32)PIXELS_PER_UNIT;
            
            glm::vec3 Scale = glm::vec3(WidthInUnits * Entity.Scale, HeightInUnits * Entity.Scale, 1);
            
            r32 RemoveInX = (Entity.IsFlipped ? 1.0f * Scale.x - (2 * Entity.CurrentAnimation->Center.x * Scale.x) : 0) + Entity.CurrentAnimation->Center.x * Scale.x;
            
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
            
            if(Entity.Type == Entity_Enemy && Entity.Enemy.HasLoot && Entity.Dead)
                SetFloatUniform(Shader.Program, "glow", GL_TRUE);
            else
                SetFloatUniform(Shader.Program, "glow", GL_FALSE);
            
            SetFloatUniform(Shader.Program, "time", (r32)GetTime());
            SetVec4Uniform(Shader.Program, "spriteColor", RenderEntity->Color);
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
            
            r32 WidthInUnits = Entity.CurrentAnimation->Texture->Width / (r32)PIXELS_PER_UNIT;
            r32 HeightInUnits = Entity.CurrentAnimation->Texture->Height / (r32)PIXELS_PER_UNIT;
            
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
        
        if(Entity.Type == Entity_Enemy && Entity.Health < Entity.FullHealth && Entity.Health > 0)
        {
            RenderHealthbar(RenderState, &Entity, *Entity.Enemy.Healthbar, ProjectionMatrix, View);
        }
        
        if(GameState->AIDebugModeOn && Entity.Type == Entity_Enemy)
        {
            glm::mat4 Model = glm::mat4(1.0f) * View;
            
            glm::vec3 Projected =
                glm::project(glm::vec3(Entity.Position.x, Entity.Position.y, 0), Model, ProjectionMatrix, glm::vec4(GameState->RenderState.Viewport[0], GameState->RenderState.Viewport[1], GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
            
            char* State = "State MISSING";
            
            switch(Entity.Enemy.AIState)
            {
                case AI_Idle:
                {
                    State = "Idle";
                }
                break;
                case AI_Wandering:
                {
                    State = "Wandering";
                }
                break;
                case AI_Attacking:
                {
                    State = "Attacking";
                }
                break;
                case AI_Defending:
                {
                    State = "Defending";
                }
                break;
                case AI_Dying:
                {
                    State = "Dying";
                }
                break;
                case AI_Charging:
                {
                    State = "Charging";
                }
                break;
                case AI_Following:
                {
                    State = "Following";
                }
                break;
                case AI_Alerted:
                {
                    State = "Alerted";
                }
                break;
                case AI_Hit:
                {
                    State = "Hit";
                }
                break;
            }
            
            RenderText(RenderState, RenderState->ButtonFont, glm::vec4(1, 1, 1, 1), State, Projected.x, Projected.y, 1, Alignment_Center);
        }
    }
    
    if(Entity.Type == Entity_Player && Entity.Player.RenderCrosshair)
    {
        RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), Entity.Position.x + Entity.Player.CrosshairPositionX, Entity.Position.y + Entity.Player.CrosshairPositionY, 1, 1, RenderState->Textures["crosshair"]->TextureHandle, false, ProjectionMatrix, View);
    }
    else if(Entity.RenderButtonHint)
    {
        RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), Entity.Position.x + Entity.RenderButtonOffset.x, Entity.Position.y + Entity.RenderButtonOffset.y, 1, 1, RenderState->Textures["b_button"]->TextureHandle, false, ProjectionMatrix, View);
    }
    
    if(Entity.Type == Entity_Enemy)
    {
        if(Entity.Enemy.IsTargeted)
        {
            RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), Entity.Position.x + Entity.Enemy.TargetingPositionX, Entity.Position.y + Entity.Enemy.TargetingPositionY, 1, 1, RenderState->Textures["red_arrow"]->TextureHandle, false, ProjectionMatrix, View);}
        
        if(GameState->GameMode == Mode_Editor 
           && GameState->EditorState.SelectedEntity 
           && GameState->EditorState.SelectedEntity->EntityIndex == Entity.EntityIndex 
           && Entity.Enemy.WaypointCount > 0)
        {
            for(i32 Index = 1; Index < Entity.Enemy.WaypointCount; Index++)
            {
                auto Point1 = Entity.Enemy.Waypoints[Index - 1];
                auto Point2 = Entity.Enemy.Waypoints[Index];
                
                RenderLine(*RenderState, glm::vec4(1, 1, 1, 1), Point1.X + 0.5f, Point1.Y + 0.5f, Point2.X + 0.5f, Point2.Y + 0.5f, false, ProjectionMatrix, View);
            }
            
            for(i32 Index = 0; Index < Entity.Enemy.WaypointCount; Index++)
            {
                auto Point = Entity.Enemy.Waypoints[Index];
                RenderRect(Render_Fill, RenderState, glm::vec4(0, 1, 0, 0.5), Point.X + 0.25f, Point.Y + 0.25f, 0.5f, 0.5f, RenderState->Textures["circle"]->TextureHandle, false, ProjectionMatrix, View);
            }
        }
    }
    
    if(RenderState->RenderColliders && !Entity.IsKinematic)
        RenderColliderWireframe(RenderState, &Entity, ProjectionMatrix, View);
    
    if(RenderState->RenderPaths && Entity.Type == Entity_Enemy)
        RenderAStarPath(RenderState,&Entity,ProjectionMatrix,View);
}

static void RenderTile(render_state* RenderState, u32 X, u32 Y, u32 TilesheetIndex, glm::vec2 TextureOffset, glm::vec2 SheetSize, glm::vec4 Color,  glm::mat4 ProjectionMatrix, glm::mat4 View)
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
    RenderRect(Render_Fill, RenderState, glm::vec4(0.26, 0.525, 0.95, 1), Button.ScreenPosition.x, Button.ScreenPosition.y, Button.Size.x, Button.Size.y, RenderState->Textures["button"]->TextureHandle);
    RenderText(RenderState, RenderState->ButtonFont, Button.TextColor, Button.Text, Button.ScreenPosition.x + Button.Size.x / 2, Button.ScreenPosition.y + Button.Size.y / 2, 1, Alignment_Center);
}

void RenderTextfield(render_state* RenderState, const textfield& Textfield)
{
    RenderRect(Render_Fill, RenderState, glm::vec4(1, 1, 1, 1), Textfield.ScreenPosition.x, Textfield.ScreenPosition.y, Textfield.Size.x, Textfield.Size.y);
    
    RenderText(RenderState, RenderState->RobotoFont, glm::vec4(1, 1, 1, 1), Textfield.Label, Textfield.ScreenPosition.x, Textfield.ScreenPosition.y + 35, 1);
    RenderText(RenderState, RenderState->RobotoFont, glm::vec4(0, 0, 0, 1), Textfield.Text, Textfield.ScreenPosition.x, Textfield.ScreenPosition.y + 10, 1);
    
    if(Textfield.InFocus)
    {
        RenderRect(Render_Outline, RenderState, glm::vec4(1, 0, 0, 1), Textfield.ScreenPosition.x-3, Textfield.ScreenPosition.y - 3, Textfield.Size.x + 6, Textfield.Size.y + 6);
        
        // Draw cursor
        r32 Width;
        r32 Height;
        MeasureText(RenderState->RobotoFont, Textfield.Text, &Width, &Height);
        RenderRect(Render_Fill, RenderState, glm::vec4(0.5, 0.3, 0.57, 1), Textfield.ScreenPosition.x + Width + 2, Textfield.ScreenPosition.y + 7, 10, 20);
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
    if (RenderState->BoundTexture != Tilemap.RenderEntity.Texture->TextureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, Tilemap.RenderEntity.Texture->TextureHandle);
        RenderState->BoundTexture = Tilemap.RenderEntity.Texture->TextureHandle;
    }
    
    glBindVertexArray(Tilemap.RenderInfo.VAOS[0]);
    
    auto Shader = RenderState->TileShader;
    UseShader(&Shader);
    
    glm::mat4 Model(1.0f);
    Model = glm::scale(Model, glm::vec3(1, 1, 1.0f));
    
    SetFloatUniform(Shader.Program, "isUI", 0);
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    
    glDrawArrays(GL_QUADS, 0, Tilemap.RenderInfo.VBOSizes[0] / 4);
    
    glBindVertexArray(Tilemap.RenderInfo.VAOS[1]);
    
    SetFloatUniform(Shader.Program, "isUI", 0);
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    
    glDrawArrays(GL_QUADS, 0, Tilemap.RenderInfo.VBOSizes[1] / 4);
    
    glBindVertexArray(0);
}


static void EditorRenderTilemap(glm::vec2 ScreenPosition, r32 Size, render_state* RenderState, const tilemap& Tilemap)
{
    r32 X = ScreenPosition.x * RenderState->ScaleX;
    X -= 1;
    r32 Y = ScreenPosition.y * RenderState->ScaleY;
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
    
    for(i32 Index = 0; Index < GameState->RenderState.RenderEntityCount; Index++) 
    {
        render_entity* Render = &GameState->RenderState.RenderEntities[Index];
        // @Incomplete: Only render if in view
        Render->Entity->RenderEntityHandle = Index;
        RenderEntity(GameState, *Render->Entity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    }
}

void RenderGame(game_state* GameState)
{
    if(GameState->GameMode == Mode_InGame || (GameState->GameMode == Mode_Editor && !GameState->EditorState.MenuOpen))
    {
        RenderInGameMode(GameState);
    }
}

void RenderUI(game_state* GameState)
{
    switch(GameState->GameMode)
    {
        case Mode_MainMenu:
        {
            glm::vec4 TextColor;
            
            for(u32 Index = 0; Index < GameState->MainMenu.OptionCount; Index++)
            {
                if((i32)Index == GameState->MainMenu.SelectedIndex)
                {
                    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), (r32)GameState->RenderState.WindowWidth / 2 - 200, (r32)GameState->RenderState.WindowHeight / 2 - 10 - 40 * Index, 400, 40);
                    
                    TextColor = glm::vec4(0, 0, 0, 1);
                }
                else
                {
                    TextColor = glm::vec4(1, 1, 1, 1);
                }
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, TextColor, GameState->MainMenu.Options[Index], (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight / 2 - 40 * Index, 1, Alignment_Center);
                
            }
        }
        break;
        case Mode_InGame:
        {
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            
            if(GameState->Paused)
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.5, 1, 1, 1), "PAUSED", (r32)GameState->RenderState.WindowWidth / 2, 40, 1, Alignment_Center);
            
            if(!GameState->InputController.ControllerPresent)
            {
                RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), (r32)GameState->InputController.MouseX - 20.0f, (r32)GameState->RenderState.WindowHeight - (r32)GameState->InputController.MouseY - 20.0f, 40.0f, 40.0f, 
                           GameState->RenderState.Textures["cross"]->TextureHandle, true, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            }
            
            // Player UI
            auto Player = GameState->Entities[0];
            RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0, 0, 0, 1), 48.0f, GameState->RenderState.WindowHeight - 52.0f, 404.0f, 29.0f);
            RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0.6f, 0, 0, 1), 50.0f, GameState->RenderState.WindowHeight - 50.0f, 400.0f / (r32)Player.FullHealth * (r32)Player.Health, 25.0f);
            
            if(!TimerDone(GameState, Player.HealthDecreaseTimer))
            {
                r32 StartX = 50 +  400.0f / (r32)Player.FullHealth * (r32)Player.Health;
                r32 Width = 400.0f / (r32)Player.FullHealth * Player.HealthLost;
                
                RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), StartX, (r32)(GameState->RenderState.WindowHeight - 50), (r32)Width, 25.0f);
            }
            
            RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0, 0, 0, 1), 48.0f, (r32)(GameState->RenderState.WindowHeight - 92), 404.0f, 29.0f);
            RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 0.5, 1), 50.0f, (r32)(GameState->RenderState.WindowHeight - 90), 400.0f / (r32)Player.Player.FullStamina * (r32)Player.Player.Stamina, 25.0f);
            
            RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0, 0, 0, 1), 48, 10, 80, 80);
            RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), 48 + 40 - 25, 10 + 40 - 25, 50, 50, GameState->RenderState.Textures["health_potion"]->TextureHandle);
            RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), 48 + 40 - 17.5f, 90, 35, 35, GameState->RenderState.Textures["y_button"]->TextureHandle);
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
                        
                        RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), (r32)GameState->RenderState.WindowWidth / 2 - Width / 2 - 5, (r32)GameState->RenderState.WindowHeight / 2 - 30 + Index * 40 - Height + 5, Width + 10, Height + 10);
                        RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0, 0, 0, 1), GameState->EditorState.MenuOptions[Index], (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight / 2 - 30 + Index * 40, 1, Alignment_Center);
                    }
                    else
                    {
                        RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(1, 1, 1, 1), GameState->EditorState.MenuOptions[Index], (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight / 2 - 30 + Index * 40, 1, Alignment_Center);
                    }
                }
            }
            else
            {
                switch(GameState->EditorState.Mode)
                {
                    case Editor_Level:
                    {
                        RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0, 0, 0, 1), 0, (r32)GameState->RenderState.WindowHeight - 155, (r32)GameState->RenderState.WindowWidth - 80, 155);
                        
                        switch(GameState->EditorState.PlacementMode)
                        {
                            case Editor_Placement_Tile:
                            {
                                RenderRect(Render_Outline, &GameState->RenderState, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0, 0, (r32)GameState->CurrentLevel.Tilemap.Width, (r32)GameState->CurrentLevel.Tilemap.Height, 0, false, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                                RenderRect(Render_Outline, &GameState->RenderState, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0, 0, (r32)GameState->CurrentLevel.Tilemap.Width, (r32)GameState->CurrentLevel.Tilemap.Height, 0, false, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                                
                                glm::vec2 TextureOffset = GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType + 1].TextureOffset;
                                
                                if(GameState->EditorState.TileX >= 0 && GameState->EditorState.TileX < GameState->CurrentLevel.Tilemap.Width && GameState->EditorState.TileY > 0 && GameState->EditorState.TileY <= GameState->CurrentLevel.Tilemap.Height)
                                {
                                    const tilesheet& Tilesheet = GameState->RenderState.Tilesheets[GameState->CurrentLevel.TilesheetIndex];
                                    
                                    glm::vec2 SheetSize(Tilesheet.Texture.Width, Tilesheet.Texture.Height);
                                    
                                    for(u32 X = 0;X < GameState->EditorState.TileBrushSize.x && X + GameState->EditorState.TileX < GameState->CurrentLevel.Tilemap.Width; X++)
                                    {
                                        for(u32 Y = 0;Y < GameState->EditorState.TileBrushSize.y && Y + GameState->EditorState.TileY < GameState->CurrentLevel.Tilemap.Height; Y++)
                                        {
                                            RenderTile(&GameState->RenderState, (u32)GameState->EditorState.TileX + X, (u32)GameState->EditorState.TileY + Y, GameState->CurrentLevel.TilesheetIndex, TextureOffset, SheetSize, glm::vec4(1, 1, 1, 1),  GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                                        }
                                    }
                                    RenderTile(&GameState->RenderState, (u32)GameState->EditorState.TileX, (u32)GameState->EditorState.TileY, GameState->CurrentLevel.TilesheetIndex, TextureOffset, SheetSize, glm::vec4(1, 1, 1, 1),  GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
                                }
                                
                                RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(0, 0, 0, 1), GameState->EditorState.ToolbarX, GameState->EditorState.ToolbarY, GameState->EditorState.ToolbarWidth, GameState->EditorState.ToolbarHeight);
                                
                                EditorRenderTilemap(GameState->EditorState.TilemapOffset, GameState->EditorState.RenderedTileSize, &GameState->RenderState, GameState->CurrentLevel.Tilemap);
                                
                                RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 0, 0, 1), GameState->EditorState.ToolbarX + GameState->EditorState.TilemapOffset.x + GameState->EditorState.SelectedTilePosition.x * GameState->EditorState.RenderedTileSize,
                                           GameState->EditorState.ToolbarY + GameState->EditorState.TilemapOffset.y + GameState->EditorState.SelectedTilePosition.y * GameState->EditorState.RenderedTileSize, GameState->EditorState.RenderedTileSize, GameState->EditorState.RenderedTileSize, GameState->RenderState.Textures["selected_tile"]->TextureHandle);
                                
                                char Text[255]; sprintf(Text,"Type index: %d Is solid: %d",GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType].TypeIndex,GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType].IsSolid);
                                
                                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(1, 1, 1, 1), Text, 10, (r32)GameState->RenderState.WindowHeight - 90, 1);
                                
                                char LayerText[20];
                                sprintf(LayerText, "Layer %d", GameState->EditorState.CurrentTilemapLayer);
                                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(1, 1, 1, 1), &LayerText[0], 10, (r32)GameState->RenderState.WindowHeight - 140, 1); 
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
                                
                                RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(1, 1, 1, 1), Text, (r32)GameState->InputController.MouseX, GameState->RenderState.WindowHeight - (r32)GameState->InputController.MouseY + 20, 1, Alignment_Center); 
                                
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
                            RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.6f, 0.6f, 0.6f, 1), "Tile-mode", (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight - 70, 1, Alignment_Center);
                        else
                            RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.6f, 0.6f, 0.6f, 1), "Entity-mode", (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight - 70, 1, Alignment_Center);
                    }
                    break;
                    case Editor_Animation:
                    {
                        RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.6f, 0.6f, 0.6f, 1), "Animation editing", (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight - 70, 1, Alignment_Center);
                        
                        if(!GameState->EditorState.LoadedAnimation)
                        {
                            std::map<char*, animation>::iterator Iterator;
                            
                            i32 Index = 0;
                            
                            for(Iterator = GameState->Animations.begin(); Iterator != GameState->Animations.end(); Iterator++)
                            {
                                if(Index == GameState->EditorState.SelectedAnimation)
                                {
                                    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), 20, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->Animations.size() - Index) * 20 - 150 - 4, 300, 20);
                                    RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(0, 0, 0, 1), Iterator->first, 20, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->Animations.size() - Index++) * 20 - 150, 1);
                                }
                                else
                                    RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(1, 1, 1, 1), Iterator->first, 20, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->Animations.size() - Index++) * 20 - 150, 1);
                            }
                        }
                        else
                        {
                            const texture& Texture = *GameState->EditorState.LoadedAnimation->Texture;
                            r32 Scale = 2.0f;
                            r32 TextureWidth = (r32)Texture.Width * Scale;
                            r32 TextureHeight = (r32)Texture.Height * Scale;
                            
                            for(i32 TextureIndex = 0; TextureIndex < GameState->EditorState.TexturesLength; TextureIndex++)
                            {
                                if(TextureIndex == GameState->EditorState.SelectedTexture)
                                {
                                    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), 20, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->EditorState.TexturesLength - TextureIndex) * 20 - 150 - 4, 300, 20);
                                    RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(0, 0, 0, 1), GameState->EditorState.Textures[TextureIndex], 20, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->EditorState.TexturesLength - TextureIndex) * 20 - 150, 1);
                                }
                                else
                                    RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(1, 1, 1, 1), GameState->EditorState.Textures[TextureIndex], 20, (r32)GameState->RenderState.WindowHeight / 2 + (GameState->EditorState.TexturesLength - TextureIndex) * 20 - 150, 1);
                            }
                            
                            
                            RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), (r32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2, (r32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2, TextureWidth, TextureHeight, Texture.TextureHandle);
                            
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
                                
                                r32 StartX = (r32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2;
                                r32 StartY = (r32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2;
                                
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
                                    
                                    RenderRect(Render_Outline, &GameState->RenderState, glm::vec4(1, 0, 0, 1), StartX + FrameOffsetX + X * Scale, StartY - Y * Scale + TextureHeight - FrameHeight * Scale + FrameOffsetY,(r32)FrameWidth * Scale, (r32)FrameHeight * Scale);
                                }
                                
                            }
                            RenderRect(Render_Outline, &GameState->RenderState, glm::vec4(1, 0, 0, 1), (r32)GameState->RenderState.WindowWidth / 2.0f - TextureWidth / 2, (r32)GameState->RenderState.WindowHeight / 2.0f - TextureHeight / 2, TextureWidth, TextureHeight);
                            
                            if(GameState->EditorState.LoadedAnimation->FrameCount > 0)
                                RenderAnimationPreview(&GameState->RenderState, GameState->EditorState.AnimationInfo, *GameState->EditorState.LoadedAnimation, glm::vec2((r32)GameState->RenderState.WindowWidth - 400, 20), 2);
                        }
                        
                        RenderButton(&GameState->RenderState, *GameState->EditorState.CreateNewAnimationButton);
                    }
                    break;
                }
                
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(1, 1, 1, 1), "EDITOR", (r32)GameState->RenderState.WindowWidth / 2, (r32)GameState->RenderState.WindowHeight - 30, 1, Alignment_Center);
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
        
        RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(Color.x, Color.y, Color.z, GameState->Camera.FadingAlpha), 0, 0, (r32)GameState->RenderState.WindowWidth, (r32)GameState->RenderState.WindowHeight);
    }
    
    if(GameState->PlayerState == Player_Dead)
    {
        RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 0, 0, 0.2f), 0, 0, (r32)GameState->RenderState.WindowWidth, (r32)GameState->RenderState.WindowHeight);
        
        r32 Width = 0;
        r32 Height = 0;
        MeasureText(GameState->RenderState.TitleFont, "YOU DIED", &Width, &Height);
        
        RenderText(&GameState->RenderState, GameState->RenderState.TitleFont, glm::vec4(1, 1, 1, 1), "YOU DIED", (r32)GameState->RenderState.WindowWidth / 2 - Width / 2, (r32)GameState->RenderState.WindowHeight / 2 - Height / 2, 1);
        
        MeasureText(GameState->RenderState.TitleFont, "Press any key to restart. . .", &Width, &Height);
        RenderText(&GameState->RenderState, GameState->RenderState.TitleFont, glm::vec4(1, 1, 1, 1), "Press any key to restart. . .", (r32)GameState->RenderState.WindowWidth / 2 - Width / 2, (r32)GameState->RenderState.WindowHeight / 2 - Height * 2, 1);
    }
}

static void CheckLevelVAO(game_state* GameState)
{
    if(GameState->CurrentLevel.Tilemap.RenderInfo.VAOS[0] == 0 || GameState->CurrentLevel.Tilemap.RenderInfo.VAOS[1] == 0)
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
                   GameState->RenderState.WindowHeight - 20.0f, 1.0f);
        
        i32 X = (i32)glm::floor(Pos.x);
        i32 Y = (i32)glm::floor(Pos.y);
        char MousePos[32];
        sprintf(MousePos,"Mouse: (%d %d)",X,Y);
        RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, 
                   glm::vec4(1, 1, 1, 1), MousePos, GameState->RenderState.WindowWidth / 2.0f - 200, 
                   GameState->RenderState.WindowHeight - 20.0f, 1.0f);
    }
    
}

static void RenderLightSources(game_state* GameState)
{
    glBindVertexArray(GameState->RenderState.FrameBufferVAO);
    glBindFramebuffer(GL_FRAMEBUFFER, GameState->RenderState.LightingFrameBuffer);
    glBindTexture(GL_TEXTURE_2D, GameState->RenderState.LightingTextureColorBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    
    glm::mat4 P = GameState->Camera.ProjectionMatrix;
    glm::mat4 V = GameState->Camera.ViewMatrix;
    
    if(GameState->GameMode == Mode_InGame || GameState->EditorState.Mode == Editor_Level)
    {
        glm::vec4 PointlightPositions[32];
        r32 PointlightRadi[32];
        r32 PointlightIntensities[32];
        r32 PointlightConstantAtt[32];
        r32 PointlightLinearAtt[32];
        r32 PointlightExponentialAtt[32];
        glm::vec4 PointlightColors[32];
        i32 NumOfPointLights = 0;
        
        r32 AmbientIntensity = 0.0f;
        glm::vec4 AmbientColor = glm::vec4(0,0,0,1);
        
        for(u32 Index = 0; Index < GameState->LightSourceCount; Index++)
        {
            auto LightSource = GameState->LightSources[Index];
            switch(LightSource.Type)
            {
                case Light_Pointlight:
                {
                    glm::mat4 Model(1.0f);
                    Model = glm::translate(Model,glm::vec3(LightSource.Pointlight.Position.x + 1.0f,LightSource.Pointlight.Position.y + 1.0f,0));
                    PointlightPositions[NumOfPointLights] = Model * glm::vec4(-0.5,-0.5,0.0f,1.0f);
                    PointlightRadi[NumOfPointLights] = LightSource.Pointlight.Radius;
                    PointlightIntensities[NumOfPointLights] = LightSource.Pointlight.Intensity;
                    PointlightColors[NumOfPointLights] = LightSource.Color;
                    PointlightConstantAtt[NumOfPointLights] = LightSource.Pointlight.ConstantAtten;
                    PointlightLinearAtt[NumOfPointLights] = LightSource.Pointlight.LinearAtten;
                    PointlightExponentialAtt[NumOfPointLights] = LightSource.Pointlight.ExponentialAtten;
                    NumOfPointLights++;
                }
                break;
                case Light_Ambient:
                {
                    AmbientIntensity = LightSource.Ambient.Intensity;
                    AmbientColor = LightSource.Color;
                }
                break;
            }
        }
        
        glm::vec4 Color = glm::vec4(0.5,0.2,0.0,0.6);
        
        auto Shader = GameState->RenderState.LightSourceShader;
        
        UseShader(&Shader);
        
        SetMat4Uniform(Shader.Program, "P", P);
        SetMat4Uniform(Shader.Program, "V", V);
        SetVec4ArrayUniform(Shader.Program, "PointLightColors", PointlightColors,NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightRadius", PointlightRadi, NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightIntensity", PointlightIntensities, NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightConstantAtt", PointlightConstantAtt, NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightLinearAtt", PointlightLinearAtt, NumOfPointLights);
        SetFloatArrayUniform(Shader.Program, "PointLightExpAtt", PointlightExponentialAtt, NumOfPointLights);
        SetIntUniform(Shader.Program, "NUM_POINTLIGHTS", NumOfPointLights);
        SetVec4ArrayUniform(Shader.Program, "PointLightPos",PointlightPositions,NumOfPointLights);
        SetVec2Uniform(Shader.Program, "screenSize", glm::vec2((r32)GameState->RenderState.WindowWidth,(r32)GameState->RenderState.WindowHeight));
        
        SetVec4Uniform(Shader.Program, "AmbientColor", AmbientColor);
        SetFloatUniform(Shader.Program, "AmbientIntensity", AmbientIntensity);
        
        glDrawArrays(GL_QUADS, 0, 4);
        
        glBindVertexArray(0);
    }
}

static void Render(game_state* GameState)
{
    if(GameState->CurrentLevel.Tilemap.RenderInfo.Dirty)
    {
        LoadTilemapBuffer(&GameState->RenderState, 0, &GameState->CurrentLevel.Tilemap.RenderInfo.VAOS[0], &GameState->CurrentLevel.Tilemap.RenderInfo.VBOS[0], &GameState->CurrentLevel.Tilemap.RenderInfo.VBOSizes[0], GameState->CurrentLevel.Tilemap);
        LoadTilemapBuffer(&GameState->RenderState, 1, &GameState->CurrentLevel.Tilemap.RenderInfo.VAOS[1], &GameState->CurrentLevel.Tilemap.RenderInfo.VBOS[1], &GameState->CurrentLevel.Tilemap.RenderInfo.VBOSizes[1], GameState->CurrentLevel.Tilemap);
        GameState->CurrentLevel.Tilemap.RenderInfo.Dirty = false;
    }
    
    GameState->RenderState.ScaleX = 2.0f / GameState->RenderState.WindowWidth;
    GameState->RenderState.ScaleY = 2.0f / GameState->RenderState.WindowHeight;
    
    // First pass
    glBindFramebuffer(GL_FRAMEBUFFER, GameState->RenderState.FrameBuffer);
    glBindTexture(GL_TEXTURE_2D, GameState->RenderState.TextureColorBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1.0f);
    
    // Render scene
    if(GameState->RenderGame)
    {
        RenderGame(GameState);
    }
    
    if(GameState->GameMode == Mode_InGame)
        RenderLightSources(GameState);
    
    // Second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(GameState->RenderState.FrameBufferVAO);
    UseShader(&GameState->RenderState.FrameBufferShader);
    
    auto TexLoc = glGetUniformLocation(GameState->RenderState.FrameBufferShader.Program, "tex");
    glUniform1i(TexLoc, 0);
    TexLoc = glGetUniformLocation(GameState->RenderState.FrameBufferShader.Program, "lightingTex");
    glUniform1i(TexLoc, 1);
    
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GameState->RenderState.TextureColorBuffer);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, GameState->RenderState.LightingTextureColorBuffer);
    
    glDrawArrays(GL_QUADS, 0, 4); 
    
    glActiveTexture(GL_TEXTURE0);
    
    // Render UI
    RenderUI(GameState);
    RenderDebugInfo(GameState);
    glfwSwapBuffers(GameState->RenderState.Window);
    }